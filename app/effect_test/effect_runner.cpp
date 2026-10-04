/**
 * @file effect_runner.cpp
 * @brief Command-line effect chain runner, driven by the effect UI (scripts/effect_ui).
 *
 * Usage:
 *   effect_runner --list
 *       Prints every available effect and its parameters as JSON.
 *   effect_runner <in.wav> <out.wav> [<effect> <param>...]...
 *       Runs the chain in the order given. Each effect name is followed by exactly
 *       as many numbers as it has parameters, in the order --list reports them.
 *       e.g. effect_runner in.wav out.wav fuzz 0.2 0.9 1 delay 300 0.5 0.5
 */
#include "effects/delay_effect.h"
#include "effects/fuzz.h"
#include "effects/noise_gate.h"
#include "effects/tremolo.h"
#include "effects/volume.h"
#include "wav_reader.h"
#include "wav_writer.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace
{
    /** Big enough for 2 s of delay at 48 kHz. Heap allocated, so size is no concern here. */
    constexpr size_t MAX_DELAY_SAMPLES{96000};
    constexpr size_t BLOCK_SIZE{64};

    struct ParamSpec
    {
        const char *name;
        float min;
        float max;
        float default_value;
    };

    using Factory = std::function<std::unique_ptr<dsp::Effect>(const std::vector<float> &p,
                                                               float sample_rate_hz)>;

    struct EffectSpec
    {
        const char *name;
        std::vector<ParamSpec> params;
        Factory make;
    };

    /**
     * The single place effects are registered. The UI builds its knobs from this list,
     * so adding an effect here is all it takes for it to show up there.
     */
    const std::vector<EffectSpec> &registry()
    {
        static const std::vector<EffectSpec> effects{
            {"noise_gate",
             {{"threshold", 0.0f, 0.5f, 0.05f},
              {"attack_ms", 0.1f, 100.0f, 5.0f},
              {"release_ms", 0.1f, 500.0f, 5.0f},
              {"hold_ms", 0.0f, 500.0f, 10.0f}},
             [](const std::vector<float> &p, float sr)
             { return std::make_unique<dsp::NoiseGate>(p[0], p[1], p[2], p[3], sr); }},
            {"fuzz",
             {{"threshold", 0.01f, 1.0f, 0.2f},
              {"clip", 0.0f, 1.0f, 0.9f},
              {"crunch", 0.0f, 10.0f, 1.0f}},
             [](const std::vector<float> &p, float)
             { return std::make_unique<dsp::Fuzz>(p[0], p[1], p[2]); }},
            {"delay",
             {{"time_ms", 1.0f, 2000.0f, 300.0f},
              {"mix", 0.0f, 1.0f, 0.5f},
              {"feedback", 0.0f, 0.97f, 0.5f}},
             [](const std::vector<float> &p, float sr)
             { return std::make_unique<dsp::Delay<MAX_DELAY_SAMPLES>>(p[0], p[1], p[2], sr); }},
            {"tremolo",
             {{"mix", 0.0f, 1.0f, 0.5f}, {"rate_hz", 0.1f, 20.0f, 4.0f}},
             [](const std::vector<float> &p, float sr)
             { return std::make_unique<dsp::Tremolo>(p[0], p[1], sr); }},
            {"volume",
             {{"level", 0.0f, 1.0f, 1.0f}},
             [](const std::vector<float> &p, float)
             {
                 auto volume = std::make_unique<dsp::Volume>();
                 volume->set_linear(p[0]);
                 return volume;
             }},
        };
        return effects;
    }

    const EffectSpec *find_effect(const char *name)
    {
        for (const auto &spec : registry())
        {
            if (std::strcmp(spec.name, name) == 0)
            {
                return &spec;
            }
        }
        return nullptr;
    }

    void print_registry_json()
    {
        printf("[");
        for (size_t e = 0; e < registry().size(); ++e)
        {
            const auto &spec = registry()[e];
            printf("%s{\"name\":\"%s\",\"params\":[", e ? "," : "", spec.name);
            for (size_t i = 0; i < spec.params.size(); ++i)
            {
                const auto &param = spec.params[i];
                printf("%s{\"name\":\"%s\",\"min\":%g,\"max\":%g,\"default\":%g}", i ? "," : "",
                       param.name, param.min, param.max, param.default_value);
            }
            printf("]}");
        }
        printf("]\n");
    }
} // namespace

int main(int argc, char **argv)
{
    if (argc == 2 && std::strcmp(argv[1], "--list") == 0)
    {
        print_registry_json();
        return 0;
    }
    if (argc < 3)
    {
        fprintf(stderr,
                "usage: effect_runner --list | <in.wav> <out.wav> [<effect> <param>...]...\n");
        return 1;
    }

    auto wav = test::read_wav_mono16(argv[1]);
    if (!wav)
    {
        fprintf(stderr, "Failed to read '%s' (must be 16-bit PCM WAV)\n", argv[1]);
        return 1;
    }
    const float sample_rate_hz = static_cast<float>(wav->sample_rate_hz);

    std::vector<std::unique_ptr<dsp::Effect>> chain;
    for (int arg = 3; arg < argc;)
    {
        const EffectSpec *spec = find_effect(argv[arg]);
        if (!spec)
        {
            fprintf(stderr, "Unknown effect '%s'\n", argv[arg]);
            return 1;
        }
        ++arg;

        std::vector<float> params;
        for (const auto &param : spec->params)
        {
            if (arg >= argc)
            {
                fprintf(stderr, "'%s' is missing parameter '%s'\n", spec->name, param.name);
                return 1;
            }
            params.push_back(std::clamp(std::strtof(argv[arg++], nullptr), param.min, param.max));
        }
        chain.push_back(spec->make(params, sample_rate_hz));
    }

    /** Same block-at-a-time path as the real engine, see effect_test. */
    std::vector<float> &samples = wav->samples;
    for (size_t start = 0; start < samples.size(); start += BLOCK_SIZE)
    {
        const std::span<float> block(samples.data() + start,
                                     std::min(BLOCK_SIZE, samples.size() - start));
        for (auto &effect : chain)
        {
            effect->processBlock(block);
        }
    }

    if (!test::write_wav_mono16(argv[2], samples, wav->sample_rate_hz))
    {
        fprintf(stderr, "Failed to write '%s'\n", argv[2]);
        return 1;
    }
    printf("Processed %zu samples @ %u Hz through %zu effect(s)\n", samples.size(),
           wav->sample_rate_hz, chain.size());
    return 0;
}
