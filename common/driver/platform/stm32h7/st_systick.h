/**
 * @file st_systick.h
 * @brief SysTick millisecond timebase for the stm32h7.
 */

#pragma once

#include <cstdint>
#include "delay.h"
#include "uptime.h"

/**
 * @brief Advances the millisecond counter. SysTick_Handler must call this once
 *        per interrupt (see boards/.../stm32h7xx_it.c). Declared with C linkage
 *        so the C interrupt file can forward-declare and call it.
 */
extern "C" void st_systick_inc(void);

namespace driver
{
    namespace Stmh7
    {
        /**
         * @class HwSysTick
         * @brief Free-running 1 kHz tick backing both Uptime and Delay.
         *
         * Coexists with the HAL timebase: HAL_InitTick() (via HAL_Init) programs the
         * Cortex-M SysTick for a 1 ms period and enables its interrupt, and
         * SysTick_Handler calls both HAL_IncTick() and st_systick_inc(). This class
         * only reads the counter st_systick_inc() maintains, so the application gets
         * a millisecond timebase with no HAL dependency.
         *
         * There is no params struct because coexist mode configures no hardware; the
         * SysTick registers stay owned by the HAL init path.
         */
        class HwSysTick : public Uptime, public Delay
        {
        public:
            HwSysTick() = default;

            /**
             * @brief Confirms the SysTick interrupt is live (i.e. HAL_Init() has run),
             *        so a missing init surfaces here instead of as a frozen clock.
             * @return true if SysTick is enabled with its interrupt armed.
             */
            bool init(void);

            uint32_t millis() override;

            void delay_ms(uint32_t ms) override;
        };

    } // namespace Stmh7
} // namespace driver
