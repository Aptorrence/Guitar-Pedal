#pragma once

#include "cs4270.h"
#include "hal_adc_input.h"
#include "hal_audio_stream.h"
#include "hal_gpio.h"
#include "hal_i2c.h"

namespace bsp
{
    // Index order matches the ADC1 DMA scan (rank) order set up in MX_ADC1_Init.
    enum PotChannel : size_t
    {
        kPot0 = 0,
        kPot1,
        kPot2,
        kPot3,
        kPot4,
        kPot5,
        kPot6,
        kPot7,
        kExpPedal,
        kPotChannelCount,
    };

    // Measured end-stops of a pot's raw reading — this board's wiring never
    // actually reaches 0 at the low end. Pass these to dsp::normalize01().
    constexpr uint16_t kPotRawMin = 1799;
    constexpr uint16_t kPotRawMax = 65535;

    struct Board
    {
        driver::Stmh7::HalGpio &led;
        driver::Stmh7::HalGpio &pled0;
        driver::Stmh7::HalGpio &pled1;
        driver::Stmh7::HalGpio &pled2;
        driver::Stmh7::HalGpio &pled3;
        driver::Stmh7::HalGpio &pled4;
        driver::Stmh7::HalGpio &pled5;
        driver::Stmh7::HalGpio &pled6;
        driver::Stmh7::HalGpio &pled7;
        driver::Stmh7::HalGpio &kFtSw1;
        driver::Stmh7::HalGpio &kFtSw2;
        driver::Stmh7::HalI2c &i2c1;
        componets::Cs4270 &codec;
        driver::Stmh7::HalAudioStream &audio;
        driver::Stmh7::HalAdcInputArray<kPotChannelCount> &pots;
    };

    void board_init();
    Board &get_board();

} // namespace bsp
