#include "cs4270.h"

namespace componets
{
    Cs4270::Cs4270(driver::I2c &i2c, driver::Gpio &nrst_gpio, driver::Delay &delay)
        : i2c_{i2c}, nrst_gpio_{nrst_gpio}, delay_{delay}
    {
    }

    void Cs4270::reset()
    {
        nrst_gpio_.set(false);
        delay_.delay_ms(25);

        nrst_gpio_.set(true);
        delay_.delay_ms(25);
    }

    bool Cs4270::reg_write(uint8_t reg_addr, uint8_t data)
    {
        std::array<uint8_t, 2> buf{reg_addr, data};
        return i2c_.mem_write(buf, I2C_ADDRESS);
    }

    bool Cs4270::reg_read(uint8_t reg_addr, uint8_t &data)
    {
        std::array<uint8_t, 1> addr_buf{reg_addr};
        if (!i2c_.mem_write(addr_buf, I2C_ADDRESS))
        {
            return false;
        }

        std::array<uint8_t, 1> data_buf{};
        if (!i2c_.mem_read(data_buf, I2C_ADDRESS))
        {
            return false;
        }

        data = data_buf[0];
        return true;
    }

    Cs4270::Error Cs4270::init()
    {
        reset();

        // Freeze bit + power down ADC/DAC/control port before touching configuration
        if (!reg_write(regPowerControl, 0xA3))
        {
            return Error::powerDownFailed;
        }

        uint8_t device_id = 0x00;
        if (!reg_read(regDeviceId, device_id) || ((device_id & 0xF0) != DEVICE_ID))
        {
            return Error::deviceIdMismatch;
        }

        for (int reg_addr = regDacBVolControl; reg_addr >= regPowerControl; --reg_addr)
        {
            const uint8_t expected =
                DEFAULT_CONFIG[static_cast<size_t>(reg_addr - regPowerControl)];

            if (!reg_write(static_cast<uint8_t>(reg_addr), expected))
            {
                return Error::configWriteFailed;
            }

            uint8_t readback = 0x00;
            if (!reg_read(static_cast<uint8_t>(reg_addr), readback) || (readback != expected))
            {
                return Error::configReadbackFailed;
            }
        }

        return Error::ok;
    }
} // namespace componets
