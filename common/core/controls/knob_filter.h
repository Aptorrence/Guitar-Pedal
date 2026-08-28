/**
 * @file knob_filter.h
 * @brief Deadband + smoothing filter for noisy potentiometer/ADC readings.
 */
#pragma once

#include <cmath>

namespace controls
{
    /**
     * @class KnobFilter
     * @brief Turns a raw, ADC-noise-jittery 0..1 knob reading into a stable value.
     *
     * A one-pole low-pass knocks down per-poll ADC noise, and a deadband stops whatever
     * noise survives the low-pass from dithering the reported value back and forth.
     * Without this, a knob sitting still can still cause a downstream parameter (e.g.
     * delay time) to be re-targeted every control-loop tick, which is audible as clicking
     * -- worse the more sensitive the parameter is to small changes (e.g. a short delay
     * line, where a couple of samples of jitter is a much bigger fraction of the line
     * length than at a long delay time).
     */
    class KnobFilter
    {
    public:
        /**
         * @param smooth_ms Low-pass time constant.
         * @param poll_rate_hz How often update() is called.
         * @param deadband Minimum change (0..1 scale) required before the reported
         *                 value moves; smaller residual noise is held at its last value.
         */
        void configure(float smooth_ms, float poll_rate_hz, float deadband)
        {
            /**
             * 1000*ln(9): converts a 10%-90% rise time in ms into a one-pole coefficient.
             */
            constexpr float TIME_CONST_90_10{2197.22457734f};
            coeff_ = (smooth_ms <= 0.0f) ? 0.0f
                                         : std::exp(-TIME_CONST_90_10 / (poll_rate_hz * smooth_ms));
            deadband_ = deadband;
        }

        /**
         * @brief Jumps straight to raw with no ramp-in. Call once at startup so the
         *        filter doesn't ramp up from 0 the first time each pot is read.
         */
        void prime(float raw)
        {
            smoothed_ = raw;
            reported_ = raw;
        }

        /**
         * @brief Feeds one new raw reading and returns the filtered value.
         */
        float update(float raw)
        {
            smoothed_ = coeff_ * smoothed_ + (1.0f - coeff_) * raw;
            if (std::fabs(smoothed_ - reported_) > deadband_)
            {
                reported_ = smoothed_;
            }
            return reported_;
        }

    private:
        float coeff_ = 0.0f;
        float deadband_ = 0.0f;
        float smoothed_ = 0.0f;
        float reported_ = 0.0f;
    };
} // namespace controls
