#include "hal_i2c.h"

namespace driver
{
    namespace Stmh7
    {
        HalI2c::HalI2c(I2C_HandleTypeDef *handle) : handle_{handle} {}

        bool HalI2c::mem_read(std::span<uint8_t> data, uint8_t dev_addr)
        {
            return HAL_I2C_Master_Receive(handle_, static_cast<uint16_t>(dev_addr << 1u),
                                          data.data(), static_cast<uint16_t>(data.size()),
                                          HAL_MAX_DELAY) == HAL_OK;
        }

        bool HalI2c::mem_write(std::span<const uint8_t> data, uint8_t dev_addr)
        {
            // HAL_I2C_Master_Transmit takes a non-const pointer; data is not modified
            return HAL_I2C_Master_Transmit(handle_, static_cast<uint16_t>(dev_addr << 1u),
                                           const_cast<uint8_t *>(data.data()),
                                           static_cast<uint16_t>(data.size()),
                                           HAL_MAX_DELAY) == HAL_OK;
        }
    } // namespace Stmh7
} // namespace driver
