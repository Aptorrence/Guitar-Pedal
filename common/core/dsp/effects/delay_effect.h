/**
 * @file delay_effect.h
 * @brief simplest implimentation of a delay
 * @author Adam Torrence
 * @date 8/12/2026
 */
#pragma once

#include "effect.h"
#include "smoother.h"
#include <algorithm>
#include <array>
#include <cmath>
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
     *
     * mix, feedback, and delay_time are all ramped internally (see PARAM_SMOOTH_MS /
     * TIME_SMOOTH_MS below) so calling the setters from a slow control-rate loop (e.g. a
     * pot poll) never causes an audible click or a hard jump in the read/write cursor.
     */
    template <size_t MAX_LINE_LENGTH> class Delay : public Effect
    {
    public:
        Delay(float delay_time_ms, float mix, float feedback, float sample_rate_hz)
        {
            mix_smoother_.set_time(PARAM_SMOOTH_MS, sample_rate_hz);
            feedback_smoother_.set_time(PARAM_SMOOTH_MS, sample_rate_hz);
            length_smoother_.set_time(TIME_SMOOTH_MS, sample_rate_hz);

            /**
             * Route through the normal setters so the clamping logic stays in one place,
             * then snap each smoother straight to that target -- no 80ms ramp-in at startup.
             */
            set_delay_time(delay_time_ms, sample_rate_hz);
            length_smoother_.reset(length_smoother_.target());
            set_mix(mix);
            mix_smoother_.reset(mix_smoother_.target());
            set_feedback(feedback);
            feedback_smoother_.reset(feedback_smoother_.target());
        }

        void set_delay_time(float delay_time_ms, float sample_rate_hz)
        {
            const float requested = 0.001f * delay_time_ms * sample_rate_hz;
            const float clamped = std::clamp(requested, 1.0f, static_cast<float>(MAX_LINE_LENGTH));
            length_smoother_.set_target(clamped);
        }

        void set_mix(float mix)
        {
            mix_smoother_.set_target(std::clamp(mix, 0.0f, 1.0f));
        }

        void set_feedback(float feedback)
        {
            /**
             * Capped below 1.0: at feedback == 1 the line never decays, so a sustained
             * input turns it into an integrator that can ramp away from +-1 forever.
             */
            feedback_smoother_.set_target(std::clamp(feedback, 0.0f, MAX_FEEDBACK));
        }

        float process(float sample) override
        {
            const float mix = mix_smoother_.next();
            const float feedback = feedback_smoother_.next();
            const size_t line_length = static_cast<size_t>(std::lround(length_smoother_.next()));

            const float delay_out = line_[line_index_];
            const float delay_in = std::clamp(sample + feedback * delay_out, -4.0f, 4.0f);

            line_[line_index_] = delay_in;

            line_index_++;
            if (line_index_ >= line_length)
            {
                line_index_ = 0;
            }

            const float out = (1.0f - mix) * sample + mix * delay_out;
            return std::clamp(out, -1.0f, 1.0f);
        }

    private:
        /**
         * Time constants for the internal ramps. Mix/feedback are quick since they're a
         * plain crossfade; delay time is slower since changing it re-times the echo and
         * sounds best as a gentle tape-style pitch bend rather than a fast slide.
         */
        static constexpr float PARAM_SMOOTH_MS{20.0f};
        static constexpr float TIME_SMOOTH_MS{80.0f};
        static constexpr float MAX_FEEDBACK{0.97f};

        std::array<float, MAX_LINE_LENGTH> line_{};
        size_t line_index_ = 0;

        Smoother mix_smoother_;
        Smoother feedback_smoother_;
        Smoother length_smoother_;
    };
} // namespace dsp
