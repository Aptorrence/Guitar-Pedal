/**
 * @file delay.h
 * @brief simplest implimentation of a delay
 * @author Adam Torrence
 * @date 8/12/2026
 */
#pragma once

#include "effect.h"
#include <algorithm>
#include <array>
#include <cstddef>

namespace dsp
{
    /**
     * @class Delay
     * @brief Feedback delay line with dry/wet mix.
     *
     * @param mix How much the feedbacked sound is mixed with current sound
     * @param feedback what the decay of feedback sound is ie: 1 never dies less it decays
     * @param delay_time_ms how long delay is
     */
    template <size_t MAX_LINE_LENGTH> class Delay : public Effect
    {
    public:
        Delay(float delay_time_ms, float mix, float feedback, float sample_rate_hz)
        {
            set_delay_time(delay_time_ms, sample_rate_hz);
            set_mix(mix);
            set_feedback(feedback);
        }

        void set_delay_time(float delay_time_ms, float sample_rate_hz)
        {
            const size_t requested = static_cast<size_t>(0.001f * delay_time_ms * sample_rate_hz);
            line_length_ = std::min(requested, MAX_LINE_LENGTH);
            line_index_ = 0;
        }

        void set_mix(float mix)
        {
            mix_ = std::clamp(mix, 0.0f, 1.0f);
        }

        void set_feedback(float feedback)
        {
            feedback_ = std::clamp(feedback, 0.0f, 1.0f);
        }

        float process(float sample) override
        {
            const float delay_out = line_[line_index_];
            const float delay_in = sample + feedback_ * delay_out;

            line_[line_index_] = delay_in;

            line_index_++;
            if (line_index_ >= line_length_)
            {
                line_index_ = 0;
            }

            const float out = (1.0f - mix_) * sample + mix_ * delay_out;
            return std::clamp(out, -1.0f, 1.0f);
        }

    private:
        std::array<float, MAX_LINE_LENGTH> line_{};
        size_t line_index_ = 0;
        size_t line_length_ = MAX_LINE_LENGTH;

        float mix_ = 0.0f;
        float feedback_ = 0.0f;
    };
} // namespace dsp
