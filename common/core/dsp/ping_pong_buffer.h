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
    template <typename T, size_t HALF_SIZE> class PingPongBuffer
    {
    public:
        static constexpr size_t TOTAL_SIZE{HALF_SIZE * 2};

        T *data()
        {
            return buffer_.data();
        }
        const T *data() const
        {
            return buffer_.data();
        }

        std::span<T, HALF_SIZE> first_half()
        {
            return std::span<T, HALF_SIZE>(buffer_.data(), HALF_SIZE);
        }
        std::span<T, HALF_SIZE> second_half()
        {
            return std::span<T, HALF_SIZE>(buffer_.data() + HALF_SIZE, HALF_SIZE);
        }

    private:
        std::array<T, TOTAL_SIZE> buffer_{};
    };
} // namespace dsp
