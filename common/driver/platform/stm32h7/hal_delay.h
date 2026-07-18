/**
 * @file hal_delay.h
 * @brief Delay hal wrapper for STM32H743xx
 */
#pragma once

#include "delay.h"
#include "stm32h7xx_hal.h"

namespace driver
{
    namespace Stmh7
    {
        class HalDelay : public Delay
        {
        public:
            void delay_ms(uint32_t ms) override;
        };
    } // namespace Stmh7
} // namespace driver
