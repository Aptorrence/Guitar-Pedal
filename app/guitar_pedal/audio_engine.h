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
} // namespace audio_engine
