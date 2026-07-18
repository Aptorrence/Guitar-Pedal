#include "hal_gpio.h"

namespace driver
{
    namespace Stmh7
    {
        HalGpio::HalGpio(GPIO_TypeDef *port, uint16_t pin) : port_{port}, pin_{pin} {}

        bool HalGpio::toggle()
        {
            HAL_GPIO_TogglePin(port_, pin_);
            return true;
        }

        bool HalGpio::set(bool active)
        {
            HAL_GPIO_WritePin(port_, pin_, active ? GPIO_PIN_SET : GPIO_PIN_RESET);
            return true;
        }

        bool HalGpio::read()
        {
            return HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET;
        }
    } // namespace Stmh7
} // namespace driver
