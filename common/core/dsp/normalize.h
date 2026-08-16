/**
 * @file normalize.h
 * @brief Linear scaling of a raw integer reading into a clamped [0, 1] float.
 */

#pragma once

#include <algorithm>
#include <cstdint>

namespace dsp
{
    /**
     * @brief Maps raw from [min, max] to [0, 1], clamping if raw falls outside that range
     *        (readings taken right at a pot's mechanical end-stops commonly do).
     * @param raw Raw reading to normalize.
     * @param min Value that maps to 0.
     * @param max Value that maps to 1.
     * @return Normalized value clamped to [0, 1].
     */
    inline float normalize01(uint16_t raw, uint16_t min, uint16_t max)
    {
        const float span = static_cast<float>(max) - static_cast<float>(min);
        const float scaled = (static_cast<float>(raw) - static_cast<float>(min)) / span;
        return std::clamp(scaled, 0.0f, 1.0f);
    }
} // namespace dsp
