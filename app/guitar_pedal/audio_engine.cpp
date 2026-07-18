#include "audio_engine.h"
#include "noise_gate.h"
#include "ping_pong_buffer.h"
#include "sample_convert.h"
#include "volume.h"

namespace audio_engine
{
    // Non-static on purpose: lets a debugger (STM32CubeMonitor, CubeIDE Live
    // Expressions / SWV trace) watch these by symbol name with no extra firmware
    // wiring — just the exact -1..1 sample values already flowing through process().
    volatile float g_last_input_sample = 0.0f;
    volatile float g_last_output_sample = 0.0f;

    namespace
    {
        constexpr size_t kBlockFrames = 64;
        constexpr size_t kHalfSize =
            kBlockFrames * 2; // stereo interleaved samples, 24-bit-in-32-bit slots

        constexpr float kSampleRateHz = 44100.0f; // matches SAI_AUDIO_FREQUENCY_44K in bsp_h743.cpp

        dsp::PingPongBuffer<int32_t, kHalfSize> tx_buf;
        dsp::PingPongBuffer<int32_t, kHalfSize> rx_buf;

        dsp::Volume volume;
        // Placeholder tuning: 5% threshold, 5ms attack, 50ms release, 10ms hold.
        dsp::NoiseGate noise_gate{0.05f, 5.0f, 5.0f, 10.0f, kSampleRateHz};

        // Signal chain (fixed for now): noise gate then volume. TODO : Footswitch-toggled
        // effects will turn this into an ordered list of active effects later.
        void process(std::span<int32_t> rx_half, std::span<int32_t> tx_half)
        {
            for (size_t i = 0; i < rx_half.size(); ++i)
            {
                const float sample = dsp::q24_to_float(rx_half[i]);
                const float gated = noise_gate.process(sample);
                const float out = volume.process(gated);
                tx_half[i] = dsp::float_to_q24(out);

                g_last_input_sample = sample;
                g_last_output_sample = out;
            }
        }
    } // namespace

    void start(driver::AudioStream &audio)
    {
        audio.set_process_callback(process);
        audio.start(std::span<int32_t>(tx_buf.data(), tx_buf.kTotalSize),
                    std::span<int32_t>(rx_buf.data(), rx_buf.kTotalSize));
    }

    void set_volume(float linear01)
    {
        volume.set_linear(linear01);
    }

    void set_noise_gate_threshold(float linear01)
    {
        constexpr float kMaxThreshold = 0.2f; // guitar signals rarely need more than this
        noise_gate.set_threshold(linear01 * kMaxThreshold);
    }
} // namespace audio_engine
