/**
 * @file volume.h
 * @brief Audio-taper volume control effect.
 */

#pragma once

#include "arm_backend.h"
#include "audio_taper.h"
#include "effect.h"
#include <span>

namespace dsp
{
    class Volume : public Effect
    {
    public:
        /**
         * @brief Sets the gain from a normalized (0..1) control reading.
         * @param linear01 Normalized (0..1) control reading.
         */
        void set_linear(float linear01)
        {
            gain_ = audio_taper(linear01);
        }

        void processBlock(std::span<float> block) override
        {
            dsp::scale(block, gain_);
        }

    private:
        float gain_ = 1.0f;
    };
} // namespace dsp
