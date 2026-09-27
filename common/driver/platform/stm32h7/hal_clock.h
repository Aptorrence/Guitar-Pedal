/**
 * @file hal_clock.h
 * @brief Millisecond uptime hal wrapper for STM32H743xx
 */
#pragma once

#include "stm32h7xx_hal.h"

namespace driver
{
    namespace Stmh7
    {
        class HalClock
        {
        public:
            uint32_t millis();
        };
    } // namespace Stmh7
} // namespace driver
