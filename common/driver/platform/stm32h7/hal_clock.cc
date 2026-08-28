#include "hal_clock.h"

namespace driver
{
    namespace Stmh7
    {
        uint32_t HalClock::millis()
        {
            return HAL_GetTick();
        }
    } // namespace Stmh7
} // namespace driver
