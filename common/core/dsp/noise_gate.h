/**
 * @file noise_gate.h
 * @brief Threshold noise gate effect with smoothed attack/hold/release.
 */

#pragma once

#include "effect.h"
#include <algorithm>
#include <cmath>

namespace dsp
{
    class NoiseGate : public Effect
    {
    public:
        NoiseGate(float threshold, float attack_ms, float release_ms, float hold_ms,
                  float sample_rate_hz)
            : hold_time_s_{0.001f * hold_ms}, sample_time_s_{1.0f / sample_rate_hz}
        {
            set_threshold(threshold);
            set_attack_release_time(attack_ms, release_ms, sample_rate_hz);
        }

        void set_threshold(float threshold)
        {
            threshold_ = std::clamp(threshold, 0.0f, 1.0f);
        }

        void set_attack_release_time(float attack_ms, float release_ms, float sample_rate_hz)
        {
            attack_coeff_ = std::exp(-2197.22457734f / (sample_rate_hz * attack_ms));
            release_coeff_ = std::exp(-2197.22457734f / (sample_rate_hz * release_ms));
        }

        float process(float sample) override
        {
            const float input_abs = std::fabs(sample);
            const float gain = (input_abs < threshold_) ? 0.0f : 1.0f;

            if (gain <= smoothed_gain_)
            {
                // Attack (decrease gain -> reducing output as the gate engages)
                if (attack_counter_ > hold_time_s_)
                {
                    smoothed_gain_ = attack_coeff_ * smoothed_gain_ + (1.0f - attack_coeff_) * gain;
                }
                else
                {
                    attack_counter_ += sample_time_s_;
                }
            }
            else
            {
                // Release (increase gain -> letting signal back through as the gate disengages)
                smoothed_gain_ = release_coeff_ * smoothed_gain_ + (1.0f - release_coeff_) * gain;
                attack_counter_ = 0.0f;
            }

            return sample * smoothed_gain_;
        }

    private:
        float threshold_ = 0.0f;
        float hold_time_s_;
        float sample_time_s_;

        float attack_coeff_ = 0.0f;
        float release_coeff_ = 0.0f;

        float attack_counter_ = 0.0f;
        float smoothed_gain_ = 0.0f;
    };
} // namespace dsp
