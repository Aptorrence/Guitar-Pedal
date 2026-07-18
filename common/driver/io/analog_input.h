/**
 * @file analog_input.h
 * @brief Multi-channel analog input array interface.
 */

#pragma once
#include <cstddef>
#include <cstdint>

namespace driver
{
    /**
     * @class AnalogInputArray
     * @brief Continuously-scanned bank of analog inputs (e.g. potentiometers).
     *
     * Backed by a free-running conversion (DMA round-robin scan, etc.) — read()
     * just returns the most recent value for a channel, no blocking involved.
     */
    class AnalogInputArray
    {
    public:
        /// Number of channels this array scans.
        virtual size_t channel_count() const = 0;

        /// Most recent reading for the given channel, scaled 0..UINT16_MAX
        /// regardless of the underlying ADC's native resolution.
        virtual uint16_t read(size_t channel) const = 0;

        ~AnalogInputArray() = default;
    };
} // namespace driver
