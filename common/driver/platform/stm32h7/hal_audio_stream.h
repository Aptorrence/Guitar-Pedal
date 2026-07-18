/**
 * @file hal_audio_stream.h
 * @brief SAI+DMA audio stream wrapper for STM32H743xx
 */
#pragma once

#include "audio_stream.h"
#include "stm32h7xx_hal.h"

namespace driver
{
    namespace Stmh7
    {
        class HalAudioStream : public AudioStream
        {
        public:
            HalAudioStream(SAI_HandleTypeDef *tx_handle, SAI_HandleTypeDef *rx_handle);

            void set_process_callback(ProcessCallback callback) override;
            void start(std::span<int32_t> tx_buffer, std::span<int32_t> rx_buffer) override;

            // Called from the HAL_SAI_Rx*CpltCallback glue in hal_audio_stream.cc — not for app
            // use.
            void notify_rx_half_complete(SAI_HandleTypeDef *hsai);
            void notify_rx_complete(SAI_HandleTypeDef *hsai);

        private:
            void notify_half(size_t half_offset);

            SAI_HandleTypeDef *tx_handle_;
            SAI_HandleTypeDef *rx_handle_;
            std::span<int32_t> tx_buffer_;
            std::span<int32_t> rx_buffer_;
            ProcessCallback callback_ = nullptr;
        };
    } // namespace Stmh7
} // namespace driver
