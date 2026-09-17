/**
 * @file tremolo.h
 * @brief Amplitude-modulation tremolo driven by a triangle-wave LFO.
 * @author Alejandro Esparza (original C), ported to C++
 */
#pragma once

#include "effect.h"
#include <algorithm>
#include <span>

namespace dsp
{
    /**
     * @class Tremolo
     * @brief Multiplies the input by a slowly varying gain to produce a pulsing
     *        volume effect.
     *
     * @param mix Modulation depth, 0..1.
     * @param lfo_frequency_hz Sweep rate of the LFO.
     * @param sample_rate_hz Audio sample rate.
     *
     * The LFO is a triangle wave built from an up/down counter: lfo_count_ walks
     * between +-lfo_count_limit_ one step per sample, so a full period is
     * 4 * lfo_count_limit_ samples and the limit is a quarter period,
     * 0.25 * (sample_rate_hz / lfo_frequency_hz).
     *
     * Normalised to +-1, the LFO feeds gain = (1 - mix) + mix * lfo. At mix == 0
     * the gain is a constant 1 (bypass); as mix approaches 1 the gain swings
     * through 0 and can go briefly negative, which inverts the signal at the
     * troughs -- a quirk carried over from the original C implementation.
     */
    class Tremolo : public Effect
    {
    public:
        Tremolo(float mix, float lfo_frequency_hz, float sample_rate_hz)
        {
            set_mix(mix);
            set_lfo_frequency(lfo_frequency_hz, sample_rate_hz);
        }

        void set_mix(float mix)
        {
            mix_ = std::clamp(mix, 0.0f, 1.0f);
        }

        void set_lfo_frequency(float lfo_frequency_hz, float sample_rate_hz)
        {
            /**
             * A non-positive request falls back to 1 Hz; the ceiling is Nyquist so
             * lfo_count_limit_ can't drop below 0.5 and stays a safe divisor.
             */
            if (lfo_frequency_hz <= 0.0f)
            {
                lfo_frequency_hz = 1.0f;
            }
            lfo_frequency_hz = std::min(lfo_frequency_hz, 0.5f * sample_rate_hz);

            lfo_count_limit_ = 0.25f * (sample_rate_hz / lfo_frequency_hz);

            /**
             * A shorter period can leave the counter outside the new range; pull it
             * back to the edge so the first samples after a rate change stay bounded.
             */
            lfo_count_ = std::clamp(lfo_count_, -lfo_count_limit_, lfo_count_limit_);
        }

        void processBlock(std::span<float> block) override
        {
            for (float &sample : block)
            {
                sample = process_sample(sample);
            }
        }

    private:
        float process_sample(float sample)
        {
            const float lfo = lfo_count_ / lfo_count_limit_;
            const float gain = (1.0f - mix_) + mix_ * lfo;

            /**
             * Flip direction once a rail is reached, then take the step, matching the
             * original ordering (output uses the pre-step counter).
             */
            if (lfo_count_ >= lfo_count_limit_)
            {
                lfo_dir_ = -1.0f;
            }
            else if (lfo_count_ <= -lfo_count_limit_)
            {
                lfo_dir_ = 1.0f;
            }
            lfo_count_ += lfo_dir_;

            return sample * gain;
        }

        float mix_ = 0.0f;
        float lfo_dir_ = 1.0f;
        float lfo_count_ = 0.0f;
        float lfo_count_limit_ = 1.0f;
    };
} // namespace dsp
