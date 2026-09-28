/**
 * @file delay_effect.h
 * @brief simplest implimentation of a delay
 * @author Adam Torrence
 * @date 8/12/2026
 */
#pragma once

#include "effect.h"
#include "single_pole_lp_filter.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <span>

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
            mix_filter_.set_time(PARAM_SMOOTH_MS, sample_rate_hz);
            feedback_filter_.set_time(PARAM_SMOOTH_MS, sample_rate_hz);
            length_filter_.set_time(TIME_SMOOTH_MS, sample_rate_hz);

            /**
             * Route through the normal setters so the clamping logic stays in one place,
             * then snap each filter straight to that target -- no 80ms ramp-in at startup.
             */
            set_delay_time(delay_time_ms, sample_rate_hz);
            length_filter_.reset(length_target_);
            set_mix(mix);
            mix_filter_.reset(mix_target_);
            set_feedback(feedback);
            feedback_filter_.reset(feedback_target_);
        }

        void set_delay_time(float delay_time_ms, float sample_rate_hz)
        {
            const float requested = 0.001f * delay_time_ms * sample_rate_hz;
            length_target_ = std::clamp(requested, 1.0f, static_cast<float>(MAX_LINE_LENGTH));
        }

        void set_mix(float mix)
        {
            mix_target_ = std::clamp(mix, 0.0f, 1.0f);
        }

        void set_feedback(float feedback)
        {
            /**
             * Capped below 1.0: at feedback == 1 the line never decays, so a sustained
             * input turns it into an integrator that can ramp away from +-1 forever.
             */
            feedback_target_ = std::clamp(feedback, 0.0f, MAX_FEEDBACK);
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
            const float mix = mix_filter_.process(mix_target_);
            const float feedback = feedback_filter_.process(feedback_target_);
            const size_t line_length =
                static_cast<size_t>(std::lround(length_filter_.process(length_target_)));

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

        /** Setters write the targets; each sample ramps its filter toward them. */
        float mix_target_ = 0.0f;
        float feedback_target_ = 0.0f;
        float length_target_ = 1.0f;

        SinglePoleLpFilter mix_filter_;
        SinglePoleLpFilter feedback_filter_;
        SinglePoleLpFilter length_filter_;
    };
} // namespace dsp
