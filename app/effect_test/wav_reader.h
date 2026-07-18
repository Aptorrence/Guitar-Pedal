/**
 * @file wav_reader.h
 * @brief Minimal 16-bit PCM WAV file reader, for native effect testing.
 */

#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>

namespace test
{
    struct WavData
    {
        uint32_t sample_rate_hz;
        std::vector<float> samples; // downmixed to mono, -1..1
    };

    /// Reads a 16-bit PCM WAV file (mono or stereo; stereo is downmixed to mono
    /// by averaging channels). Returns std::nullopt on any I/O or format error.
    inline std::optional<WavData> read_wav_mono16(const char *path)
    {
        FILE *file = std::fopen(path, "rb");
        if (!file)
        {
            return std::nullopt;
        }

        char riff_id[4];
        uint32_t riff_size = 0;
        char wave_id[4];
        if (std::fread(riff_id, 1, 4, file) != 4 || std::fread(&riff_size, 4, 1, file) != 1 ||
            std::fread(wave_id, 1, 4, file) != 4 || std::memcmp(riff_id, "RIFF", 4) != 0 ||
            std::memcmp(wave_id, "WAVE", 4) != 0)
        {
            std::fclose(file);
            return std::nullopt;
        }

        uint16_t audio_format = 0;
        uint16_t num_channels = 0;
        uint32_t sample_rate = 0;
        uint16_t bits_per_sample = 0;
        bool have_fmt = false;
        std::vector<int16_t> pcm;

        char chunk_id[4];
        uint32_t chunk_size = 0;
        while (std::fread(chunk_id, 1, 4, file) == 4 && std::fread(&chunk_size, 4, 1, file) == 1)
        {
            const long next_chunk =
                std::ftell(file) + static_cast<long>(chunk_size) + (chunk_size % 2);

            if (std::memcmp(chunk_id, "fmt ", 4) == 0)
            {
                std::fread(&audio_format, 2, 1, file);
                std::fread(&num_channels, 2, 1, file);
                std::fread(&sample_rate, 4, 1, file);
                std::fseek(file, 6, SEEK_CUR); // skip byte rate + block align
                std::fread(&bits_per_sample, 2, 1, file);
                have_fmt = true;
            }
            else if (std::memcmp(chunk_id, "data", 4) == 0)
            {
                if (!have_fmt || audio_format != 1 || bits_per_sample != 16 || num_channels == 0)
                {
                    std::fclose(file);
                    return std::nullopt; // unsupported format
                }
                pcm.resize(chunk_size / sizeof(int16_t));
                std::fread(pcm.data(), sizeof(int16_t), pcm.size(), file);
            }

            std::fseek(file, next_chunk, SEEK_SET);
        }

        std::fclose(file);

        if (pcm.empty() || num_channels == 0)
        {
            return std::nullopt;
        }

        WavData result;
        result.sample_rate_hz = sample_rate;
        result.samples.resize(pcm.size() / num_channels);

        for (size_t frame = 0; frame < result.samples.size(); ++frame)
        {
            int32_t sum = 0;
            for (uint16_t ch = 0; ch < num_channels; ++ch)
            {
                sum += pcm[frame * num_channels + ch];
            }
            result.samples[frame] = (static_cast<float>(sum) / num_channels) / 32768.0f;
        }

        return result;
    }
} // namespace test
