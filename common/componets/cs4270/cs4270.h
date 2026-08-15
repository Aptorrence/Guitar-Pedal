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
        static constexpr uint8_t I2C_ADDRESS{0x48};
        static constexpr uint8_t DEVICE_ID{0xC0};

        enum Register : uint8_t
        {
            regDeviceId = 0x01,
            regPowerControl = 0x02,
            regModeControl = 0x03,
            regAdcDacControl = 0x04,
            regTransitionControl = 0x05,
            regMuteControl = 0x06,
            regDacAVolControl = 0x07,
            regDacBVolControl = 0x08,
        };

        enum class Error : uint8_t
        {
            ok = 0,
            powerDownFailed,
            deviceIdMismatch,
            configWriteFailed,
            configReadbackFailed,
        };

        Cs4270(driver::I2c &i2c, driver::Gpio &nrst_gpio, driver::Delay &delay);

        /**
         * @brief Resets the codec and writes/verifies the default configuration.
         * @return Error::ok on success, or the failure reason.
         */
        Error init();

        /**
         * @brief Pulses the hardware reset line.
         */
        void reset();

        bool reg_write(uint8_t reg_addr, uint8_t data);
        bool reg_read(uint8_t reg_addr, uint8_t &data);

    private:
        // Power-up defaults for regPowerControl..regDacBVolControl, in that order.
        static constexpr std::array<uint8_t, 7> DEFAULT_CONFIG{
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
