/**
 * @file single_pole_lp_filter.h
 * @brief Single-pole (one-pole) low-pass filter and its time-constant math.
 */
#pragma once

#include <cmath>

namespace dsp
{
    /**
     * @brief Single-pole coefficient for a given 10%-90% rise time.
     * @param time_ms Rise time in ms; <= 0 gives 0 (output follows input instantly).
     * @param rate_hz How often the filter is stepped (sample rate, poll rate, ...).
     */
    inline float single_pole_coeff(float time_ms, float rate_hz)
    {
        /**
         * 1000*ln(9): converts a 10%-90% rise time in ms into a one-pole coefficient.
         */
        constexpr float TIME_CONST_90_10{2197.22457734f};
        return (time_ms <= 0.0f) ? 0.0f : std::exp(-TIME_CONST_90_10 / (rate_hz * time_ms));
    }

    /**
     * @class SinglePoleLpFilter
     * @brief y[n] = coeff * y[n-1] + (1 - coeff) * x[n].
     */
    class SinglePoleLpFilter
    {
    public:
        void set_time(float time_ms, float rate_hz)
        {
            coeff_ = single_pole_coeff(time_ms, rate_hz);
        }

        /**
         * @brief Jumps the output straight to value with no ramp.
         */
        void reset(float value)
        {
            value_ = value;
        }

        /**
         * @brief Feeds one input and returns the new output.
         */
        float process(float input)
        {
            value_ = coeff_ * value_ + (1.0f - coeff_) * input;
            return value_;
        }

    private:
        float coeff_ = 0.0f;
        float value_ = 0.0f;
    };
} // namespace dsp
