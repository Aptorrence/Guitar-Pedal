/**
 * @file audio_engine.h
 * @brief Audio processing chain for the guitar pedal app.
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

    /**
     * @brief Sets the output volume from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_volume(float linear01);

    /**
     * @brief Sets the noise gate threshold from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_noise_gate_threshold(float linear01);

    /**
     * @brief Sets the delay time from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_delay_time(float linear01);

    /**
     * @brief Sets the delay dry/wet mix from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_delay_mix(float linear01);

    /**
     * @brief Sets the delay feedback (regeneration) from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_delay_feedback(float linear01);

    /**
     * @brief Sets the fuzz threshold (linear-region width) from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_fuzz_threshold(float linear01);

    /**
     * @brief Sets the fuzz clip (output ceiling) from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_fuzz_clip(float linear01);

    /**
     * @brief Sets the fuzz crunch (small-signal gain) from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_fuzz_crunch(float linear01);

    /**
     * @brief Sets the tremolo depth/mix from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_tremolo_mix(float linear01);

    /**
     * @brief Sets the tremolo LFO frequency from a normalized (0..1) knob reading.
     * @param linear01 Normalized (0..1) knob reading.
     */
    void set_tremolo_lfo_frequency(float linear01);

    /**
     * @brief Engages or bypasses effect attached to footswitch 1.
     * @param enabled True to run the effect, false to pass the signal through untouched.
     */
    void set_fw1_enabled(bool enabled);

    /**
     * @brief Engages or bypasses effect attached to footswitch 2.
     * @param enabled True to run the effect, false to pass the signal through untouched.
     */
    void set_fw2_enabled(bool enabled);
} // namespace audio_engine
