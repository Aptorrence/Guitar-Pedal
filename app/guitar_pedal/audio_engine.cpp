#include "audio_engine.h"
#include "audio_taper.h"
#include "effects/noise_gate.h"
#include "effects/volume.h"
#include "effects/delay_effect.h"
#include "effects/fuzz.h"
#include "effects/tremolo.h"
#include "ping_pong_buffer.h"
#include "sample_convert.h"
#include <array>

namespace audio_engine
{

    namespace
    {
        /**
         * *******************************************************************
         * Buffer size is chosen here with block frames
         * Since the ping pong buffer buffers 1/2 of buffer at
         * a time, the half size is half of the buffer,
         * but since the I2S interleaves left and right audio
         * the accual block that is proccessed per left/right is 1/2 of the
         * 1/2 thus choosing the block frames gets you your
         * accual proccessed buffer size
         * *******************************************************************
         */
        constexpr size_t BLOCK_FRAMES{64};
        constexpr size_t HALF_SIZE{BLOCK_FRAMES *
                                   2}; // stereo interleaved samples, 24-bit-in-32-bit slots

        /**
            Real sampleing frequency
         */
        constexpr float SAMPLE_RATE_HZ{44117.0f};

        dsp::PingPongBuffer<int32_t, HALF_SIZE> tx_buf;
        dsp::PingPongBuffer<int32_t, HALF_SIZE> rx_buf;

        dsp::Volume volume;

        /**
         * Placeholder tuning.
         * noise_gate(threshold%, attack_ms, release_ms, hold_ms, sample_rate_hz);
         * @param threshold what the noise threshold is to turn off/on sound
         * @param attack_ms time constant for attack of noise gate (sound gated (quiet))
         * @param release_ms time constant for release of noise gate (sound passes)
         */
        dsp::NoiseGate noise_gate{0.05f, 5.0f, 5.0f, 10.0f, SAMPLE_RATE_HZ};

        /**
         * delay Placeholder tuning.
         * float delay_time_ms, float mix, float feedback, float sample_rate_hz
         * @param mix How much the feedbacked sound is mixed with current sound
         * @param feedback decay of the feedback is ex: 1 the feedback never dies .5 it takes a bit
         * @param delay_time_ms how long delay is
         */
        dsp::Delay<41200> delay(500.0f, 0.5f, 0.5f, SAMPLE_RATE_HZ);

        /**
         * fuzz Placeholder tuning.
         * fuzz(threshold, clip, crunch);
         * @param threshold input magnitude below which the curve is linear
         * @param clip output magnitude the signal is clamped to
         * @param crunch extra gain applied inside the linear region
         */
        dsp::Fuzz fuzz(0.2f, 0.9f, 1.0f);

        /**
         * tremolo Placeholder tuning.
         * tremolo(mix, lfo_frequency_hz, sample_rate_hz);
         * @param mix modulation depth 0..1
         * @param lfo_frequency_hz sweep rate of the amplitude LFO
         */
        dsp::Tremolo tremolo(0.5f, 4.0f, SAMPLE_RATE_HZ);

        bool fw1_enabled = false;
        bool fw2_enabled = false;

        /** Last sample written to tx_half, for inspection in a debugger. */
        float debug_out = 0.0f;
        float debug_in = 0.0f;

        /** rx_half converted to float, run through the chain in place, then written to tx_half. */
        std::array<float, HALF_SIZE> block_buf{};
        /**
         * *******************************************************************
         * Main processing function and signal chain:
         *   noise gate -> [fuzz] -> [tremolo] -> volume
         * The bracketed stages are bypassed unless their footswitch is engaged.
         * *******************************************************************
         */

        void processBlock(std::span<int32_t> rx_half, std::span<int32_t> tx_half)
        {
            const size_t frames = rx_half.size();
            const std::span<float> block(block_buf.data(), frames);

            for (size_t i = 0; i < frames; ++i)
            {
                block[i] = dsp::q24_to_float(rx_half[i]);
            }
            debug_in = block[frames - 1];

            noise_gate.processBlock(block);
            if (fw1_enabled)
            {
                fuzz.processBlock(block);
                // delay.processBlock(block);
            }

            if (fw2_enabled)
            {
                tremolo.processBlock(block);
            }
            volume.processBlock(block);

            debug_out = block[frames - 1];
            for (size_t i = 0; i < frames; ++i)
            {
                tx_half[i] = dsp::float_to_q24(block[i]);
            }
        }
    } // namespace

    void start(driver::AudioStream &audio)
    {
        audio.set_process_callback(processBlock);
        audio.start(std::span<int32_t>(tx_buf.data(), tx_buf.TOTAL_SIZE),
                    std::span<int32_t>(rx_buf.data(), rx_buf.TOTAL_SIZE));
    }

    void set_volume(float linear01)
    {
        volume.set_linear(linear01);
    }

    void set_fw1_enabled(bool enabled)
    {
        fw1_enabled = enabled;
    }

    void set_fw2_enabled(bool enabled)
    {
        fw2_enabled = enabled;
    }

    void set_noise_gate_threshold(float linear01)
    {
        constexpr float MAX_THRESHOLD{0.2f};
        noise_gate.set_threshold(linear01 * MAX_THRESHOLD);
    }

    void set_delay_time(float linear01)
    {
        constexpr float MIN_DELAY_MS{20.0f};
        constexpr float MAX_DELAY_MS{900.0f}; // stays under the 41200-sample (~934.7ms) line
        const float delay_time_ms = MIN_DELAY_MS + linear01 * (MAX_DELAY_MS - MIN_DELAY_MS);
        delay.set_delay_time(delay_time_ms, SAMPLE_RATE_HZ);
    }

    void set_delay_mix(float linear01)
    {
        delay.set_mix(linear01);
    }

    void set_delay_feedback(float linear01)
    {
        delay.set_feedback(linear01);
    }

    /**
     * Fuzz floors this at a small positive value
     * since it's the divisor in the small-signal gain.
     */
    void set_fuzz_threshold(float linear01)
    {
        constexpr float MIN_THRESHOLD{0.02f};
        constexpr float MAX_THRESHOLD{0.5f};
        fuzz.set_threshold(MIN_THRESHOLD + linear01 * (MAX_THRESHOLD - MIN_THRESHOLD));
    }

    void set_fuzz_clip(float linear01)
    {
        constexpr float MIN_CLIP{0.1f};
        constexpr float MAX_CLIP{1.0f};
        fuzz.set_clip(MIN_CLIP + linear01 * (MAX_CLIP - MIN_CLIP));
    }

    void set_fuzz_crunch(float linear01)
    {
        constexpr float MIN_CRUNCH{1.0f};
        constexpr float MAX_CRUNCH{15.0f};
        fuzz.set_crunch(MIN_CRUNCH + linear01 * (MAX_CRUNCH - MIN_CRUNCH));
    }

    void set_tremolo_mix(float linear01)
    {
        tremolo.set_mix(linear01);
    }

    void set_tremolo_lfo_frequency(float linear01)
    {
        constexpr float MIN_LFO_HZ{0.5f};
        constexpr float MAX_LFO_HZ{12.0f};
        const float lfo_hz = MIN_LFO_HZ + linear01 * (MAX_LFO_HZ - MIN_LFO_HZ);
        tremolo.set_lfo_frequency(lfo_hz, SAMPLE_RATE_HZ);
    }

} // namespace audio_engine
