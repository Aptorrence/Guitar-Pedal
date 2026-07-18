#include "bsp.h"

int main()
{
    bsp::board_init();
    bsp::Board &board = bsp::get_board();

    while (true)
    {
        board.led.toggle();
        HAL_Delay(500);
    }
}
