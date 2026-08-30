#include "st_i2c.h"

namespace driver
{
    namespace Stmh7
    {
        namespace
        {
            /** A 7-bit device address sits in SADD[7:1]; SADD[0] is a don't-care. */
            constexpr uint32_t ADDR_SHIFT{1};
            /** NBYTES is an 8-bit field, so a single transfer moves at most 255 bytes. */
            constexpr std::size_t MAX_TRANSFER{255};

            void start_transfer(I2C_TypeDef *i2c, uint8_t dev_addr, std::size_t nbytes,
                                uint32_t direction)
            {
                i2c->CR2 &=
                    ~(I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND);
                i2c->CR2 |= ((static_cast<uint32_t>(dev_addr) << (I2C_CR2_SADD_Pos + ADDR_SHIFT)) |
                             direction | I2C_CR2_START | I2C_CR2_AUTOEND |
                             (static_cast<uint32_t>(nbytes) << I2C_CR2_NBYTES_Pos));
            }
        } // namespace

        HwI2c::HwI2c(const StI2cParams &params)
            : base_addr_{params.base_addr}, timingr_{params.timingr},
              timeout_spins_{params.timeout_spins} {};

        bool HwI2c::init(void)
        {
            if (base_addr_ == nullptr)
            {
                return false;
            }

            base_addr_->CR1 &= ~I2C_CR1_PE;    // disable so config registers unlock
            base_addr_->TIMINGR = timingr_;    // SCL timing for the kernel clock
            base_addr_->CR2 &= ~I2C_CR2_ADD10; // 7-bit addressing
            base_addr_->CR1 |= I2C_CR1_PE;     // Enable peripheral

            return true;
        }

        bool HwI2c::is_enabled(void)
        {
            return (base_addr_ != nullptr) && (base_addr_->CR1 & I2C_CR1_PE);
        }

        bool HwI2c::bus_free(void)
        {
            return is_enabled() && !(base_addr_->ISR & I2C_ISR_BUSY);
        }

        bool HwI2c::timed_out(uint32_t flag)
        {
            uint32_t spins = timeout_spins_;
            while (--spins)
            {
                if (base_addr_->ISR & (I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO))
                {
                    base_addr_->ICR |= I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF;
                    return true;
                }

                if (base_addr_->ISR & flag)
                {
                    return false;
                }
            }
            return true;
        }

        bool HwI2c::mem_write(std::span<const uint8_t> data, uint8_t dev_addr)
        {
            if (data.size() > MAX_TRANSFER || !bus_free())
            {
                return false;
            }

            /** Drop any STOPF/NACKF left set by an earlier aborted transfer. */
            base_addr_->ICR |= I2C_ICR_STOPCF | I2C_ICR_NACKCF;

            start_transfer(base_addr_, dev_addr, data.size(), 0u /* write */);

            for (const uint8_t byte : data)
            {
                if (timed_out(I2C_ISR_TXIS))
                {
                    return false;
                }
                base_addr_->TXDR = byte;
            }

            if (timed_out(I2C_ISR_STOPF))
            {
                return false;
            }
            base_addr_->ICR |= I2C_ICR_STOPCF;

            return true;
        }

        bool HwI2c::mem_read(std::span<uint8_t> data, uint8_t dev_addr)
        {
            if (data.size() > MAX_TRANSFER || !bus_free())
            {
                return false;
            }

            /** Drop any STOPF/NACKF left set by an earlier aborted transfer. */
            base_addr_->ICR |= I2C_ICR_STOPCF | I2C_ICR_NACKCF;

            start_transfer(base_addr_, dev_addr, data.size(), I2C_CR2_RD_WRN);

            for (uint8_t &byte : data)
            {
                if (timed_out(I2C_ISR_RXNE))
                {
                    return false;
                }
                byte = static_cast<uint8_t>(base_addr_->RXDR);
            }

            if (timed_out(I2C_ISR_STOPF))
            {
                return false;
            }
            base_addr_->ICR |= I2C_ICR_STOPCF;

            return true;
        }

    } // namespace Stmh7
} // namespace driver
