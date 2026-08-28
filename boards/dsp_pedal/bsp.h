#pragma once

#include "cs4270.h"
#include "hal_adc_input.h"
#include "hal_audio_stream.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_systick.h"

namespace bsp
{
    // Index order matches the ADC1 DMA scan (rank) order set up in MX_ADC1_Init.
    enum PotChannel : size_t
    {
        pot0 = 0,
        pot1,
        pot2,
        pot3,
        pot4,
        pot5,
        pot6,
        pot7,
        expPedal,
        potChannelCount,
    };

    // Measured end-stops of a pot's raw reading — this board's wiring never
    // actually reaches 0 at the low end. Pass these to controls::normalize01().
    constexpr uint16_t POT_RAW_MIN{1799};
    constexpr uint16_t POT_RAW_MAX{65535};

    struct Board
    {
        driver::Stmh7::HwGpio &led;
        driver::Stmh7::HwGpio &pled0;
        driver::Stmh7::HwGpio &pled1;
        driver::Stmh7::HwGpio &pled2;
        driver::Stmh7::HwGpio &pled3;
        driver::Stmh7::HwGpio &pled4;
        driver::Stmh7::HwGpio &pled5;
        driver::Stmh7::HwGpio &pled6;
        driver::Stmh7::HwGpio &pled7;
        driver::Stmh7::HwGpio &ft_sw1;
        driver::Stmh7::HwGpio &ft_sw2;
        driver::Stmh7::HwI2c &i2c1;
        componets::Cs4270 &codec;
        driver::Stmh7::HalAudioStream &audio;
        driver::Stmh7::HalAdcInputArray<potChannelCount> &pots;
        driver::Stmh7::HwSysTick &clock;
    };

    void board_init();
    Board &get_board();

} // namespace bsp
