/**
 * @file audio_engine.h
 * @brief Audio processing chain for the guitar pedal app.
 */

#pragma once

#include "audio_stream.h"

namespace audio_engine
{
    /// Owns the audio buffers, wires up the processing callback, and starts streaming.
    void start(driver::AudioStream &audio);

    /// Sets the output volume from a normalized (0..1) knob reading.
    void set_volume(float linear01);

    /// Sets the noise gate threshold from a normalized (0..1) knob reading.
    void set_noise_gate_threshold(float linear01);
} // namespace audio_engine
