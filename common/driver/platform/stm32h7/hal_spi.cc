#include "hal_spi.h"

namespace driver
{
    namespace Stmh7
    {
        HalSpi::HalSpi(SPI_HandleTypeDef *handle) : handle_{handle} {}

        bool HalSpi::Read(std::span<uint8_t> rx_data)
        {
            return HAL_SPI_Receive(handle_, rx_data.data(), static_cast<uint16_t>(rx_data.size()),
                                   HAL_MAX_DELAY) == HAL_OK;
        }

        bool HalSpi::Write(std::span<uint8_t> tx_data)
        {
            return HAL_SPI_Transmit(handle_, tx_data.data(), static_cast<uint16_t>(tx_data.size()),
                                    HAL_MAX_DELAY) == HAL_OK;
        }

        bool HalSpi::Transfer(std::span<uint8_t> tx_data, std::span<uint8_t> rx_data)
        {
            return HAL_SPI_TransmitReceive(handle_, tx_data.data(), rx_data.data(),
                                           static_cast<uint16_t>(tx_data.size()),
                                           HAL_MAX_DELAY) == HAL_OK;
        }
    } // namespace Stmh7
} // namespace driver
