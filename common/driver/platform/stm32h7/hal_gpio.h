/**
 * @file hal_gpio.h
 * @brief GPIO hal wrapper for STM32H743xx
 * @author Adam Torrence
 * @date 06/04/2026
 */
#pragma once

#include "gpio.h"
#include "stm32h7xx_hal.h"

namespace driver
{
    namespace Stmh7
    {
        class HalGpio : public Gpio
        {
        public:
            HalGpio(GPIO_TypeDef *port, uint16_t pin);

            bool toggle() override;
            bool set(bool active) override;
            bool read() override;

        private:
            GPIO_TypeDef *port_;
            uint16_t pin_;
        };
    } // namespace Stmh7
} // namespace driver
