/**
 * @file audio_stream.h
 * @brief Duplex, buffered audio streaming interface.
 */

#pragma once
#include <cstdint>
#include <span>

namespace driver
{
    /**
     * @class AudioStream
     * @brief Continuous duplex audio stream instance.
     *
     * Wraps whatever peripheral moves audio samples in the background (SAI+DMA,
     * I2S, etc.) behind a buffer-half callback, so audio processing code never
     * touches the underlying HAL/DMA types.
     */
    class AudioStream
    {
    public:
        /**
         * @brief Called once per buffer-half boundary, pairing the half of the
         *        receive buffer that was just captured with the half of the
         *        transmit buffer that needs filling next.
         */
        using ProcessCallback = void (*)(std::span<int32_t> rx_half, std::span<int32_t> tx_half);

        /**
         * @brief Registers the callback invoked at each buffer-half boundary.
         */
        virtual void set_process_callback(ProcessCallback callback) = 0;

        /**
         * @brief Starts continuous duplex streaming between tx_buffer and rx_buffer.
         * @param tx_buffer contiguous buffer streamed out, split into two equal halves.
         * @param rx_buffer contiguous buffer streamed into, split into two equal halves.
         */
        virtual void start(std::span<int32_t> tx_buffer, std::span<int32_t> rx_buffer) = 0;

        ~AudioStream() = default;
    };
} // namespace driver
