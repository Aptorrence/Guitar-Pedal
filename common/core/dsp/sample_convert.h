/**
 * @file sample_convert.h
 * @brief Conversions between 24-bit-in-32-bit integer audio samples and float.
 */

#pragma once

#include <algorithm>
#include <cstdint>

namespace dsp
{
    constexpr int32_t Q24_SIGN_BIT{0x800000};
    constexpr int32_t Q24_MASK{0xFFFFFF};
    constexpr float Q24_MAX{8388607.0f}; // 2^23 - 1

    /**
     * @brief Converts a 24-bit sample packed in the low bits of a 32-bit word to a float in [-1, 1].
     * @param sample Packed 24-bit sample in the low bits of a 32-bit word.
     * @return Float sample in [-1, 1].
     */
    inline float q24_to_float(int32_t sample)
    {
        sample &= Q24_MASK;
        if (sample & Q24_SIGN_BIT)
        {
            sample |= ~Q24_MASK;
        }
        return static_cast<float>(sample) / Q24_MAX;
    }

    /**
     * @brief Converts a float to a 24-bit sample packed in the low bits of a 32-bit word,
     *        clamping to [-1, 1].
     * @param sample Float sample to convert.
     * @return Packed 24-bit sample in the low bits of a 32-bit word.
     */
    inline int32_t float_to_q24(float sample)
    {
        const float clamped = std::clamp(sample, -1.0f, 1.0f);
        return static_cast<int32_t>(clamped * Q24_MAX) & Q24_MASK;
    }
} // namespace dsp
