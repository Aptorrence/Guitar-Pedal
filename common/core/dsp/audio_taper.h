/**
 * @file audio_taper.h
 * @brief Linear-to-logarithmic ("audio taper") gain curve for volume-style controls.
 */

#pragma once

#include <cmath>

namespace dsp
{
    /// Maps a linear 0..1 control value to a linear gain factor along a dB-linear
    /// curve: 0 -> min_db (near silence), 1 -> 0dB (unity). Matches how a volume
    /// knob is expected to feel — turning it maps to perceived loudness, not raw
    /// amplitude, unlike a plain linear multiply.
    inline float audio_taper(float linear01, float min_db = -60.0f)
    {
        const float db = min_db * (1.0f - linear01);
        return std::pow(10.0f, db / 20.0f);
    }
} // namespace dsp
