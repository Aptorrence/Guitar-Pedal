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
        return i2c_.mem_write(buf, kI2cAddress);
    }

    bool Cs4270::reg_read(uint8_t reg_addr, uint8_t &data)
    {
        std::array<uint8_t, 1> addr_buf{reg_addr};
        if (!i2c_.mem_write(addr_buf, kI2cAddress))
        {
            return false;
        }

        std::array<uint8_t, 1> data_buf{};
        if (!i2c_.mem_read(data_buf, kI2cAddress))
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
        if (!reg_write(kRegPowerControl, 0xA3))
        {
            return Error::kPowerDownFailed;
        }

        uint8_t device_id = 0x00;
        if (!reg_read(kRegDeviceId, device_id) || ((device_id & 0xF0) != kDeviceId))
        {
            return Error::kDeviceIdMismatch;
        }

        for (int reg_addr = kRegDacBVolControl; reg_addr >= kRegPowerControl; --reg_addr)
        {
            const uint8_t expected = kDefaultConfig[static_cast<size_t>(reg_addr - kRegPowerControl)];

            if (!reg_write(static_cast<uint8_t>(reg_addr), expected))
            {
                return Error::kConfigWriteFailed;
            }

            uint8_t readback = 0x00;
            if (!reg_read(static_cast<uint8_t>(reg_addr), readback) || (readback != expected))
            {
                return Error::kConfigReadbackFailed;
            }
        }

        return Error::kOk;
    }
} // namespace componets
