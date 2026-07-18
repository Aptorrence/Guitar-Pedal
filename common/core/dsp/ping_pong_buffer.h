/**
 * @file ping_pong_buffer.h
 * @brief Fixed-size double buffer for continuous streaming I/O (e.g. circular DMA).
 */

#pragma once

#include <array>
#include <cstddef>
#include <span>

namespace dsp
{
    /**
     * @class PingPongBuffer
     * @brief Single contiguous buffer split into two equal halves.
     *
     * Hands a raw contiguous pointer to whatever moves the data (DMA, a codec
     * driver, etc.) while exposing each half as a span for the producer/consumer
     * side to fill or drain. Which half is safe to touch is implied by which
     * half/complete event just fired — the buffer itself holds no state.
     */
    template <typename T, size_t kHalfSize>
    class PingPongBuffer
    {
    public:
        static constexpr size_t kTotalSize = kHalfSize * 2;

        T *data() { return buffer_.data(); }
        const T *data() const { return buffer_.data(); }

        std::span<T, kHalfSize> first_half() { return std::span<T, kHalfSize>(buffer_.data(), kHalfSize); }
        std::span<T, kHalfSize> second_half()
        {
            return std::span<T, kHalfSize>(buffer_.data() + kHalfSize, kHalfSize);
        }

    private:
        std::array<T, kTotalSize> buffer_{};
    };
} // namespace dsp
