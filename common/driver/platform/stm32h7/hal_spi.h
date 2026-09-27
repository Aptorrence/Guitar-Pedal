/**
 * @file hal_spi.h
 * @brief SPI hal wrapper for STM32H743xx
 * @author Adam Torrence
 * @date 06/04/2026
 */

#pragma once

#include "stm32h7xx_hal.h"
#include <cstdint>
#include <span>

namespace driver
{
    namespace Stmh7
    {
        class HalSpi
        {
        public:
            explicit HalSpi(SPI_HandleTypeDef *handle);

            bool Read(std::span<uint8_t> rx_data);
            bool Write(std::span<uint8_t> tx_data);
            bool Transfer(std::span<uint8_t> tx_data, std::span<uint8_t> rx_data);

        private:
            SPI_HandleTypeDef *handle_;
        };
    } // namespace Stmh7
} // namespace driver
