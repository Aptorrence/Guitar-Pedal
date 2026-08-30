/**
 * @file fuzz.h
 * @brief Piecewise-linear hard-clipping fuzz distortion.
 * @author Alejandro Esparza (original C), ported to C++
 */
#pragma once

#include "effect.h"
#include <algorithm>

namespace dsp
{
    /**
     * @class Fuzz
     * @brief Boosts low-amplitude signal with a steep linear gain and hard-clips
     *        anything past a threshold, giving an aggressive square-ish fuzz.
     *
     * @param threshold Input magnitude below which the transfer curve is linear.
     * @param clip Output magnitude the signal is clamped to.
     * @param crunch Extra gain applied inside the linear region.
     *
     * Inside +-threshold the transfer curve is (clip / threshold) * crunch * x, so the
     * small-signal slope is clip/threshold scaled by crunch. Outside that band the
     * output jumps straight to +-clip, and a final clamp keeps the linear segment from
     * overshooting +-clip when crunch > 1. output_level scales the result afterward and
     * is independent of threshold/clip/crunch, so it's a pure loudness trim rather than
     * part of the curve shape.
     */
    class Fuzz : public Effect
    {
    public:
        Fuzz(float threshold, float clip, float crunch, float output_level = 1.0f)
        {
            set_threshold(threshold);
            set_clip(clip);
            set_crunch(crunch);
        }

        void set_threshold(float threshold)
        {
            /**
             * Floored at a small positive value: threshold is the divisor in the
             * small-signal gain, so zero would blow up the transfer curve.
             */
            threshold_ = std::max(threshold, MIN_THRESHOLD);
        }

        void set_clip(float clip)
        {
            clip_ = std::max(clip, 0.0f);
        }

        void set_crunch(float crunch)
        {
            crunch_ = std::max(crunch, 0.0f);
        }


        float processBlock(float sample) override
        {
            float output;
            if (sample > -threshold_ && sample < threshold_)
            {
                const float gradient = clip_ / threshold_;
                output = gradient * crunch_ * sample;
            }
            else if (sample < 0.0f)
            {
                output = -clip_;
            }
            else
            {
                output = clip_;
            }

            return std::clamp(output, -clip_, clip_) * std::pow(10.0f, OUTPUT_ATTIUATION / 20.0f);
        }

    private:
        static constexpr float MIN_THRESHOLD{1e-6f};
        static constexpr float OUTPUT_ATTIUATION{-24.0f};

        float threshold_ = MIN_THRESHOLD;
        float clip_ = 0.0f;
        float crunch_ = 0.0f;
    };
} // namespace dsp
