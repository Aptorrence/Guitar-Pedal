/**
 * @file cs4270.h
 * @brief Hardware-agnostic driver for the Cirrus Logic CS4270 stereo audio codec.
 */

#pragma once

#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include <array>
#include <cstdint>

namespace componets
{
    /**
     * @class Cs4270
     * @brief CS4270 codec instance.
     *
     * Talks to the codec over an injected driver::I2c bus, toggles reset via an
     * injected driver::Gpio, and times the reset pulse via an injected driver::Delay,
     * so the same driver runs on any platform that provides those interfaces.
     */
    class Cs4270
    {
    public:
        static constexpr uint8_t kI2cAddress = 0x48;
        static constexpr uint8_t kDeviceId = 0xC0;

        enum Register : uint8_t
        {
            kRegDeviceId = 0x01,
            kRegPowerControl = 0x02,
            kRegModeControl = 0x03,
            kRegAdcDacControl = 0x04,
            kRegTransitionControl = 0x05,
            kRegMuteControl = 0x06,
            kRegDacAVolControl = 0x07,
            kRegDacBVolControl = 0x08,
        };

        enum class Error : uint8_t
        {
            kOk = 0,
            kPowerDownFailed,
            kDeviceIdMismatch,
            kConfigWriteFailed,
            kConfigReadbackFailed,
        };

        Cs4270(driver::I2c &i2c, driver::Gpio &nrst_gpio, driver::Delay &delay);

        /// Resets the codec and writes/verifies the default configuration.
        Error init();

        /// Pulses the hardware reset line.
        void reset();

        bool reg_write(uint8_t reg_addr, uint8_t data);
        bool reg_read(uint8_t reg_addr, uint8_t &data);

    private:
        // Power-up defaults for kRegPowerControl..kRegDacBVolControl, in that order.
        static constexpr std::array<uint8_t, 7> kDefaultConfig = {
            0x00, // Power up
            0x31, // Mode (slave, normal speed, pop guard enabled)
            0x09, // ADC/DAC (disable HPF, I2S)
            0x00, // Transition (independent volume controls)
            0x00, // No Mute right channel (we stayin stero)
            0x00, // DAC A vol (0dB)
            0x00, // DAC B vol (0dB)
        };

        driver::I2c &i2c_;
        driver::Gpio &nrst_gpio_;
        driver::Delay &delay_;
    };
} // namespace componets
