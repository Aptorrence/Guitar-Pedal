#include "noise_gate.h"
#include "volume.h"
#include "delay.h"
#include "wav_reader.h"
#include "wav_writer.h"
#include <cmath>
#include <cstdio>
#include <vector>

namespace
{
    constexpr float DEFAULT_SAMPLE_RATE_HZ{44100.0f};
    constexpr float DURATION_S{3.0f};
    constexpr float PI{3.14159265f};

    /**
     * gerated test tone
     * Loud for the first half, quiet for the second -- exercises the noise gate
     * opening and closing, with the volume taper on top.
     */
    std::vector<float> generate_test_tone(float sample_rate_hz)
    {
        const size_t num_samples = static_cast<size_t>(sample_rate_hz * DURATION_S);
        std::vector<float> signal(num_samples);
        for (size_t i = 0; i < num_samples; ++i)
        {
            const float t = static_cast<float>(i) / sample_rate_hz;
            const float amplitude = (t < DURATION_S / 2.0f) ? 0.8f : 0.02f;
            signal[i] = amplitude * std::sin(2.0f * PI * 220.0f * t);
        }
        return signal;
    }
} // namespace

int main(int argc, char **argv)
{
    std::vector<float> input;
    float sample_rate_hz = DEFAULT_SAMPLE_RATE_HZ;

    if (argc > 1)
    {
        auto wav = test::read_wav_mono16(argv[1]);
        if (!wav)
        {
            printf("Failed to read '%s' (must be 16-bit PCM WAV)\n", argv[1]);
            return 1;
        }
        input = std::move(wav->samples);
        sample_rate_hz = static_cast<float>(wav->sample_rate_hz);
        printf("Loaded %s: %zu samples @ %.0f Hz\n", argv[1], input.size(), sample_rate_hz);
    }
    else
    {
        input = generate_test_tone(DEFAULT_SAMPLE_RATE_HZ);
        printf("No input file given, generated a test tone (%zu samples @ %.0f Hz)\n", input.size(),
               DEFAULT_SAMPLE_RATE_HZ);
    }

    /**
     * noise gate Placeholder tuning.
     * noise_gate(threshold%, attack_ms, release_ms, hold_ms, sample_rate_hz);
     */
    dsp::NoiseGate noise_gate(0.05f, 5.0f, 5.0f, 10.0f, sample_rate_hz);

    /**
     * delay Placeholder tuning.
     * float delay_time_ms, float mix, float feedback, float sample_rate_hz
     * @param mix How much the feedbacked sound is mixed with current sound
     * @param feedback what the decay of the feedback is ex: 1 the feedback never dies .5 it takes a
     * bit
     * @param delay_time_ms how long delay is
     */
    dsp::Delay<41200> delay(500.0f, 0.5f, 0.5f, sample_rate_hz);

    /**
     * volume Placeholder tuning 100%.
     */
    dsp::Volume volume;
    volume.set_linear(1.0f);

    std::vector<float> output(input.size());
    for (size_t i = 0; i < input.size(); ++i)
    {
        const float gated = noise_gate.process(input[i]);
        const float delayed = delay.process(gated);
        output[i] = volume.process(delayed);
    }

    test::write_wav_mono16("input.wav", input, static_cast<uint32_t>(sample_rate_hz));
    test::write_wav_mono16("output.wav", output, static_cast<uint32_t>(sample_rate_hz));

    printf("Wrote input.wav and output.wav\n");
    return 0;
}
