/**
 * @file delay.h
 * @brief Blocking delay driver interface.
 */

#pragma once
#include <cstdint>

namespace driver
{
    /**
     * @class Delay
     * @brief Blocking millisecond delay instance.
     *
     * Lets components request timing without depending on a specific HAL.
     */
    class Delay
    {
    public:
        /**
         * @brief Blocks the calling thread for at least ms milliseconds.
         * @param ms milliseconds to wait.
         */
        virtual void delay_ms(uint32_t ms) = 0;

        ~Delay() = default;
    };
} // namespace driver
