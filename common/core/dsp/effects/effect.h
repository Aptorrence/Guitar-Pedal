/**
 * @file effect.h
 * @brief Block-based audio effect interface.
 */

#pragma once

#include <span>

namespace dsp
{
    /**
     * @class Effect
     * @brief One stage in an audio processing chain.
     *
     * Implementations hold their own state (envelope followers, delay lines,
     * LFO phase, etc.) and process a block of samples in place, so a chain can
     * be built as a plain ordered list of Effect* without any branching on
     * which combination of effects is active.
     */
    class Effect
    {
    public:
        /**
         * @brief Processes one block of samples in place.
         * @param block Mono samples in [-1, 1]; overwritten with this stage's output.
         */
        virtual void processBlock(std::span<float> block) = 0;

        virtual ~Effect() = default;
    };
} // namespace dsp
