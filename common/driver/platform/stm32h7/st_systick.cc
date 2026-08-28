#include "st_systick.h"

#include "stm32h7xx.h"

namespace
{
    /**
     * Milliseconds since the SysTick interrupt came alive. Written only by
     * st_systick_inc() from SysTick_Handler; a lone 32-bit aligned load is
     * atomic on Cortex-M7, so readers need no interrupt masking.
     */
    volatile uint32_t g_ms_ticks{0};
} // namespace

extern "C" void st_systick_inc(void)
{
    g_ms_ticks = g_ms_ticks + 1u;
}

namespace driver
{
    namespace Stmh7
    {
        bool HwSysTick::init(void)
        {
            const uint32_t armed = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
            return (SysTick->CTRL & armed) == armed;
        }

        uint32_t HwSysTick::millis()
        {
            return g_ms_ticks;
        }

        void HwSysTick::delay_ms(uint32_t ms)
        {
            const uint32_t start = g_ms_ticks;
            /**
             * start is captured partway through the current millisecond, so wait one
             * extra tick to honour the Delay contract of "at least ms". Unsigned
             * subtraction keeps this correct across the counter's 49-day wrap.
             */
            while ((g_ms_ticks - start) <= ms)
            {
            }
        }

    } // namespace Stmh7
} // namespace driver
