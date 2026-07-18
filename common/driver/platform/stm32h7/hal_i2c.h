/**
 * @file hal_i2c.h
 * @brief I2c hal wrapper for STM32H743xx
 * @author Adam Torrence
 * @date 06/04/2026
 */
#pragma once

#include "i2c.h"
#include "stm32h7xx_hal.h"
#include <cstdint>
#include <span>

namespace driver
{
    namespace Stmh7
    {
        class HalI2c : public I2c
        {
        public:
            explicit HalI2c(I2C_HandleTypeDef *handle);

            // dev_addr is the 7-bit address; shifted to 8-bit internally for HAL
            bool mem_read(std::span<uint8_t> data, uint8_t dev_addr) override;
            bool mem_write(std::span<const uint8_t> data, uint8_t dev_addr) override;

        private:
            I2C_HandleTypeDef *handle_;
        };
    } // namespace Stmh7
} // namespace driver
