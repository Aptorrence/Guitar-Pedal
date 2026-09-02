/**
 * @file arm_backend.h
 * @brief Namespaced wrappers over the CMSIS-DSP kernels used by the effects.
 *
 */

#pragma once

#include <span>

namespace dsp
{
    /**
     * @brief Multiplies every sample in @p block by @p gain, in place.
     */
    void scale(std::span<float> block, float gain);
} // namespace dsp
