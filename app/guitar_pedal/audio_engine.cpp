#include "audio_engine.h"
#include "noise_gate.h"
#include "ping_pong_buffer.h"
#include "sample_convert.h"
#include "volume.h"

namespace audio_engine
{

    namespace
    {
        constexpr size_t BLOCK_FRAMES{64};
        constexpr size_t HALF_SIZE{BLOCK_FRAMES *
                                   2}; // stereo interleaved samples, 24-bit-in-32-bit slots

        constexpr float SAMPLE_RATE_HZ{44100.0f}; // matches SAI_AUDIO_FREQUENCY_44K in bsp_h743.cpp

        dsp::PingPongBuffer<int32_t, HALF_SIZE> tx_buf;
        dsp::PingPongBuffer<int32_t, HALF_SIZE> rx_buf;

        dsp::Volume volume;
        /**
         * Placeholder tuning.
         * noise_gate(threshold%, attack_ms, release_ms, hold_ms, sample_rate_hz);
         */
        dsp::NoiseGate noise_gate{0.05f, 5.0f, 5.0f, 10.0f, SAMPLE_RATE_HZ};

        /**
         * *******************************************************************
         * Main processing function and Signal chain
         * (fixed for now): noise gate then volume. TODO : Footswitch-toggled
         * *******************************************************************
         */

        void process(std::span<int32_t> rx_half, std::span<int32_t> tx_half)
        {
            for (size_t i = 0; i < rx_half.size(); ++i)
            {
                const float sample = dsp::q24_to_float(rx_half[i]);
                const float gated = noise_gate.process(sample);
                const float out = volume.process(gated);
                tx_half[i] = dsp::float_to_q24(out);
            }
        }
    } // namespace

    void start(driver::AudioStream &audio)
    {
        audio.set_process_callback(process);
        audio.start(std::span<int32_t>(tx_buf.data(), tx_buf.TOTAL_SIZE),
                    std::span<int32_t>(rx_buf.data(), rx_buf.TOTAL_SIZE));
    }

    void set_volume(float linear01)
    {
        volume.set_linear(linear01);
    }

    /**
     * Sets the noise gate threshold max is 20% of max volume
     */
    void set_noise_gate_threshold(float linear01)
    {
        constexpr float MAX_THRESHOLD{0.2f};
        noise_gate.set_threshold(linear01 * MAX_THRESHOLD);
    }
} // namespace audio_engine
