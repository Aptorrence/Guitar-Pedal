/**
 * @file sample_convert.h
 * @brief Conversions between 24-bit-in-32-bit integer audio samples and float.
 */

#pragma once

#include <algorithm>
#include <cstdint>

namespace dsp
{
    constexpr int32_t kQ24SignBit = 0x800000;
    constexpr int32_t kQ24Mask = 0xFFFFFF;
    constexpr float kQ24Max = 8388607.0f; // 2^23 - 1

    /// Converts a 24-bit sample packed in the low bits of a 32-bit word to a float in [-1, 1].
    inline float q24_to_float(int32_t sample)
    {
        sample &= kQ24Mask;
        if (sample & kQ24SignBit)
        {
            sample |= ~kQ24Mask;
        }
        return static_cast<float>(sample) / kQ24Max;
    }

    /// Converts a float to a 24-bit sample packed in the low bits of a 32-bit word, clamping to [-1, 1].
    inline int32_t float_to_q24(float sample)
    {
        const float clamped = std::clamp(sample, -1.0f, 1.0f);
        return static_cast<int32_t>(clamped * kQ24Max) & kQ24Mask;
    }
} // namespace dsp
