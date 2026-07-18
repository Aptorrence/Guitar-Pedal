/**
 * @file bsp.h
 * @brief bsp initialization and functions
 * @author Adam Torrence
 * @date 06/03/2026
 */

#pragma once

#include "st_gpio.h"

namespace bsp
{

    struct Board
    {
        driver::Stml4::HwGpio &led;
    };

    void board_init();
    Board &get_board();

} // namespace Bsp
