/**
 * @file uptime.h
 * @brief Monotonic millisecond uptime interface.
 */

#pragma once
#include <cstdint>

namespace driver
{
    /**
     * @class Uptime
     * @brief Free-running millisecond time source.
     *
     * Lets code measure elapsed time without depending on a specific HAL.
     * Backed by the platform tick counter (SysTick on Cortex-M).
     */
    class Uptime
    {
    public:
        /**
         * @brief Milliseconds elapsed since the tick counter started.
         * @return Monotonic count in milliseconds; wraps at UINT32_MAX.
         */
        virtual uint32_t millis() = 0;

        ~Uptime() = default;
    };
} // namespace driver
