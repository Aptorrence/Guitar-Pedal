#include "hal_delay.h"

namespace driver
{
    namespace Stmh7
    {
        void HalDelay::delay_ms(uint32_t ms)
        {
            HAL_Delay(ms);
        }
    } // namespace Stmh7
} // namespace driver
