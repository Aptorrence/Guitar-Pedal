#include "hal_audio_stream.h"

namespace
{
    driver::Stmh7::HalAudioStream *g_active_stream = nullptr;
}

namespace driver
{
    namespace Stmh7
    {
        HalAudioStream::HalAudioStream(SAI_HandleTypeDef *tx_handle, SAI_HandleTypeDef *rx_handle)
            : tx_handle_{tx_handle}, rx_handle_{rx_handle}
        {
            g_active_stream = this;
        }

        void HalAudioStream::set_process_callback(ProcessCallback callback)
        {
            callback_ = callback;
        }

        void HalAudioStream::start(std::span<int32_t> tx_buffer, std::span<int32_t> rx_buffer)
        {
            tx_buffer_ = tx_buffer;
            rx_buffer_ = rx_buffer;

            HAL_SAI_Transmit_DMA(tx_handle_, reinterpret_cast<uint8_t *>(tx_buffer_.data()),
                                  static_cast<uint16_t>(tx_buffer_.size()));
            HAL_SAI_Receive_DMA(rx_handle_, reinterpret_cast<uint8_t *>(rx_buffer_.data()),
                                 static_cast<uint16_t>(rx_buffer_.size()));
        }

        void HalAudioStream::notify_half(size_t half_offset)
        {
            if (!callback_)
            {
                return;
            }

            const size_t half_size = rx_buffer_.size() / 2;
            callback_(rx_buffer_.subspan(half_offset, half_size), tx_buffer_.subspan(half_offset, half_size));
        }

        void HalAudioStream::notify_rx_half_complete(SAI_HandleTypeDef *hsai)
        {
            if (hsai == rx_handle_)
            {
                notify_half(0);
            }
        }

        void HalAudioStream::notify_rx_complete(SAI_HandleTypeDef *hsai)
        {
            if (hsai == rx_handle_)
            {
                notify_half(rx_buffer_.size() / 2);
            }
        }
    } // namespace Stmh7
} // namespace driver

extern "C" void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (g_active_stream)
    {
        g_active_stream->notify_rx_half_complete(hsai);
    }
}

extern "C" void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (g_active_stream)
    {
        g_active_stream->notify_rx_complete(hsai);
    }
}
