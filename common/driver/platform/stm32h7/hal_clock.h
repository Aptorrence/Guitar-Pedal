/**
 * @file hal_clock.h
 * @brief Uptime hal wrapper for STM32H743xx
 */
#pragma once

#include "stm32h7xx_hal.h"
#include "uptime.h"

namespace driver
{
    namespace Stmh7
    {
        class HalClock : public Uptime
        {
        public:
            uint32_t millis() override;
        };
    } // namespace Stmh7
} // namespace driver
