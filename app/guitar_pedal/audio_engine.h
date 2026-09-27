/**
 * @file audio_engine.h
 * @brief Audio processing chain for the guitar pedal app.
 *
 * Every set_* taking linear01 expects a normalized (0..1) knob reading and maps it
 * onto that parameter's useful range internally.
 */

#pragma once

#include "audio_stream.h"

namespace audio_engine
{
    /**
     * @brief Owns the audio buffers, wires up the processing callback, and starts streaming.
     * @param audio Audio stream to drive.
     */
    void start(driver::AudioStream &audio);

    void set_volume(float linear01);
    void set_noise_gate_threshold(float linear01);

    void set_delay_time(float linear01);
    void set_delay_mix(float linear01);
    /** Feedback = regeneration. */
    void set_delay_feedback(float linear01);

    /** Width of the linear region. */
    void set_fuzz_threshold(float linear01);
    /** Output ceiling. */
    void set_fuzz_clip(float linear01);
    /** Small-signal gain. */
    void set_fuzz_crunch(float linear01);

    /** Modulation depth. */
    void set_tremolo_mix(float linear01);
    void set_tremolo_lfo_frequency(float linear01);

    /**
     * @brief Engages (true) or bypasses (false) the effect on footswitch 1 / 2.
     */
    void set_fw1_enabled(bool enabled);
    void set_fw2_enabled(bool enabled);
} // namespace audio_engine
