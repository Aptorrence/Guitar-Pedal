/**
 * @file hal_adc_input.h
 * @brief ADC+DMA analog input array wrapper for STM32H743xx
 */
#pragma once

#include "analog_input.h"
#include "stm32h7xx_hal.h"
#include <array>

namespace driver
{
    namespace Stmh7
    {
        /**
         * @class HalAdcInputArray
         * @brief Wraps a free-running, DMA-scanned ADC channel bank.
         *
         * CHANNEL_COUNT must match the ADC's configured NbrOfConversion — the
         * scan order (and so which array index maps to which physical channel)
         * is whatever rank order MX_ADC1_Init assigned.
         */
        template <size_t CHANNEL_COUNT> class HalAdcInputArray : public AnalogInputArray
        {
        public:
            explicit HalAdcInputArray(ADC_HandleTypeDef *handle) : handle_{handle} {}

            /**
             * @brief Kicks off the free-running DMA scan. Call once, after ADC/DMA HAL init.
             * @return Returns true if success.
             */
            bool start()
            {
                return HAL_ADC_Start_DMA(handle_, reinterpret_cast<uint32_t *>(raw_.data()),
                                         CHANNEL_COUNT) == HAL_OK;
            }

            size_t channel_count() const override
            {
                return CHANNEL_COUNT;
            }

            uint16_t read(size_t channel) const override
            {
                const uint8_t raw = raw_[channel];
                return static_cast<uint16_t>((raw << 8) | raw);
            }

        private:
            ADC_HandleTypeDef *handle_;
            std::array<uint8_t, CHANNEL_COUNT> raw_{};
        };
    } // namespace Stmh7
} // namespace driver
