/**
 * @file smoother.h
 * @brief One-pole exponential smoother for ramping a control value toward a target.
 */
#pragma once

#include <cmath>

namespace dsp
{
    /**
     * @class Smoother
     * @brief Ramps a value toward a target by a fixed fraction each sample, instead of
     *        jumping to it immediately. Used to turn a stepped knob/pot reading into a
     *        continuous per-sample control signal so parameter changes don't click or zipper.
     */
    class Smoother
    {
    public:
        /**
         * @brief Sets how quickly the smoother tracks its target.
         * @param time_ms Approx. time to settle to a new target (~99.97% of the way there).
         * @param sample_rate_hz Audio sample rate.
         */
        void set_time(float time_ms, float sample_rate_hz)
        {
            /**
             * 1000*ln(9): converts a 10%-90% rise time in ms into a one-pole coefficient.
             */
            constexpr float TIME_CONST_90_10{2197.22457734f};
            coeff_ =
                (time_ms <= 0.0f) ? 0.0f : std::exp(-TIME_CONST_90_10 / (sample_rate_hz * time_ms));
        }

        /**
         * @brief Jumps the current value to target with no ramp (e.g. at startup).
         */
        void reset(float value)
        {
            value_ = value;
            target_ = value;
        }

        /**
         * @brief Sets the value to ramp toward; does not change the current value.
         */
        void set_target(float target)
        {
            target_ = target;
        }

        /**
         * @brief Advances the ramp by one sample and returns the new current value.
         */
        float next()
        {
            value_ = coeff_ * value_ + (1.0f - coeff_) * target_;
            return value_;
        }

        float value() const
        {
            return value_;
        }

        float target() const
        {
            return target_;
        }

    private:
        float coeff_ = 0.0f;
        float value_ = 0.0f;
        float target_ = 0.0f;
    };
} // namespace dsp
