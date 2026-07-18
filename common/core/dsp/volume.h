/**
 * @file volume.h
 * @brief Audio-taper volume control effect.
 */

#pragma once

#include "audio_taper.h"
#include "effect.h"

namespace dsp
{
    class Volume : public Effect
    {
    public:
        /// Sets the gain from a normalized (0..1) control reading.
        void set_linear(float linear01) { gain_ = audio_taper(linear01); }

        float process(float sample) override { return sample * gain_; }

    private:
        float gain_ = 1.0f;
    };
} // namespace dsp
