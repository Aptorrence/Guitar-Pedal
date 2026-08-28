/**
 * @file st_i2c.h
 * @brief I2c driver specifics for the stm32h7.
 */

#pragma once

#include <cstdint>
#include <span>
#include "i2c.h"
#include "stm32h7xx.h"

namespace driver
{
    namespace Stmh7
    {
        /**
         * @brief Construction parameters for HwI2c.
         */
        struct StI2cParams
        {
            /** I2C peripheral instance, e.g. I2C1. */
            I2C_TypeDef *base_addr;
            /** Raw I2C_TIMINGR value for the selected kernel clock (from CubeMX). */
            uint32_t timingr;
            /**
             * Poll-loop iterations to spin on each status flag before giving up.
             * Bounds a stuck bus -- this is an iteration count, not milliseconds.
             */
            uint32_t timeout_spins = 100000;
        };

        /**
         * @class HwI2c
         * @brief Register-level I2C master (polled, 7-bit addressing, AUTOEND).
         */
        class HwI2c : public I2c
        {
        public:
            explicit HwI2c(const StI2cParams &params);

            /**
             * @brief Sets TIMINGR and 7-bit addressing, then enables the peripheral.
             *        The I2C kernel clock and SCL/SDA pin muxing must already be set up.
             * @return true on success, false if base_addr is null.
             */
            bool init(void);

            /** @param dev_addr 7-bit device address. */
            bool mem_read(std::span<uint8_t> data, uint8_t dev_addr) override;
            bool mem_write(std::span<const uint8_t> data, uint8_t dev_addr) override;

        private:
            /** @return true once the peripheral is enabled (PE set). */
            bool is_enabled(void);

            /** @return true when the peripheral is enabled and the bus is idle. */
            bool bus_free(void);

            /**
             * @brief Spins until @p flag is set in ISR, an error flag appears, or the
             *        spin budget runs out.
             * @return true on timeout or bus error (error flags are cleared first),
             *         false once @p flag was observed.
             */
            bool timed_out(uint32_t flag);

            I2C_TypeDef *const base_addr_;
            const uint32_t timingr_;
            const uint32_t timeout_spins_;
        };

    } // namespace Stmh7
} // namespace driver
