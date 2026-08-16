/**
 * @file wav_writer.h
 * @brief Minimal mono 16-bit PCM WAV file writer, for native effect testing/visualization.
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <span>

/**
 * @brief Writes a 16-bit mono PCM WAV file.
 * @param path Path to the WAV file to write.
 */
namespace test
{
    inline bool write_wav_mono16(const char *path, std::span<const float> samples,
                                 uint32_t sample_rate_hz)
    {
        FILE *file = std::fopen(path, "wb");
        if (!file)
        {
            return false;
        }

        const uint32_t data_bytes = static_cast<uint32_t>(samples.size() * sizeof(int16_t));
        const uint16_t num_channels = 1;
        const uint16_t bits_per_sample = 16;
        const uint32_t byte_rate = sample_rate_hz * num_channels * bits_per_sample / 8;
        const uint16_t block_align = static_cast<uint16_t>(num_channels * bits_per_sample / 8);
        const uint32_t riff_size = 36 + data_bytes;
        const uint32_t fmt_size = 16;
        const uint16_t audio_format = 1; // PCM

        std::fwrite("RIFF", 1, 4, file);
        std::fwrite(&riff_size, 4, 1, file);
        std::fwrite("WAVE", 1, 4, file);

        std::fwrite("fmt ", 1, 4, file);
        std::fwrite(&fmt_size, 4, 1, file);
        std::fwrite(&audio_format, 2, 1, file);
        std::fwrite(&num_channels, 2, 1, file);
        std::fwrite(&sample_rate_hz, 4, 1, file);
        std::fwrite(&byte_rate, 4, 1, file);
        std::fwrite(&block_align, 2, 1, file);
        std::fwrite(&bits_per_sample, 2, 1, file);

        std::fwrite("data", 1, 4, file);
        std::fwrite(&data_bytes, 4, 1, file);

        for (float sample : samples)
        {
            const float clamped = std::clamp(sample, -1.0f, 1.0f);
            const int16_t pcm = static_cast<int16_t>(clamped * 32767.0f);
            std::fwrite(&pcm, sizeof(pcm), 1, file);
        }

        std::fclose(file);
        return true;
    }
} // namespace test
