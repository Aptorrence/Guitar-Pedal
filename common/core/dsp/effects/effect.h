/**
 * @file effect.h
 * @brief Single-sample audio effect interface.
 */

#pragma once

namespace dsp
{
    /**
     * @class Effect
     * @brief One stage in an audio processing chain.
     *
     * Implementations hold their own state (envelope followers, delay lines,
     * LFO phase, etc.) and process one sample at a time, so a chain can be
     * built as a plain ordered list of Effect* without any branching on which
     * combination of effects is active.
     */
    class Effect
    {
    public:
        virtual float process(float sample) = 0;

        virtual ~Effect() = default;
    };
} // namespace dsp
