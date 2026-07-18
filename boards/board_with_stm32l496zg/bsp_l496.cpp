#include "bsp.h"
#include "stm32l4xx_hal.h"

// ---------------------------------------------------------------------------
// Paste SystemClock_Config() from CubeIDE here
// ---------------------------------------------------------------------------
static void SystemClock_Config()
{
    // TODO: paste from CubeIDE
}

// ---------------------------------------------------------------------------
// Board objects
// ---------------------------------------------------------------------------
static driver::Stml4::HwGpio led(driver::Stml4::StGpioParams{
    .settings = {
        .mode = driver::Stml4::GpioMode::GPOUT,
        .otype = driver::Stml4::GpioOtype::PUSH_PULL,
        .ospeed = driver::Stml4::GpioOspeed::LOW,
        .pupd = driver::Stml4::GpioPupd::NO_PULL,
        .af = 0,
    },
    .pin_num = 0, // PG0
    .base_addr = GPIOG,
});

static bsp::Board board{.led = led};

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------
void bsp::board_init()
{
    SystemClock_Config();

    // Enable peripheral clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;

    led.init();
}

bsp::Board &bsp::get_board()
{
    return board;
}
