#include "audio_engine.h"
#include "bsp.h"
#include "knob_filter.h"
#include "normalize.h"
#include <array>

namespace
{
    constexpr uint32_t CONTROL_INTERVAL_MS{100};
    constexpr float CONTROL_RATE_HZ{1000.0f / static_cast<float>(CONTROL_INTERVAL_MS)};
    constexpr float KNOB_SMOOTH_MS{150.0f};
    constexpr float KNOB_DEADBAND{0.002f};

    std::array<controls::KnobFilter, bsp::potChannelCount> knob_filters;

    float read_knob(bsp::Board &board, bsp::PotChannel channel)
    {
        const float raw =
            controls::normalize01(board.pots.read(channel), bsp::POT_RAW_MIN, bsp::POT_RAW_MAX);
        return knob_filters[channel].update(raw);
    }
} // namespace

int main()
{
    bsp::board_init();
    bsp::Board &board = bsp::get_board();

    const bool codec_ok = board.codec.init() == componets::Cs4270::Error::ok;

    if (codec_ok)
    {
        audio_engine::start(board.audio);
    }

    for (size_t channel = 0; channel < bsp::potChannelCount; ++channel)
    {
        knob_filters[channel].configure(KNOB_SMOOTH_MS, CONTROL_RATE_HZ, KNOB_DEADBAND);
        const float raw =
            controls::normalize01(board.pots.read(channel), bsp::POT_RAW_MIN, bsp::POT_RAW_MAX);
        knob_filters[channel].prime(raw);
    }

    uint32_t last_blink = board.clock.millis();
    const uint32_t blink_interval = codec_ok ? 500 : 100;

    uint32_t last_control = board.clock.millis();

    while (true)
    {
        if (board.clock.millis() - last_control >= CONTROL_INTERVAL_MS)
        {
            last_control = board.clock.millis();

            float knob0 = read_knob(board, bsp::pot0);
            audio_engine::set_volume(knob0);
            board.pled0.set(true);

            float knob1 = read_knob(board, bsp::pot1);
            audio_engine::set_noise_gate_threshold(knob1);
            board.pled1.set(true);

            float knob2 = read_knob(board, bsp::pot2);
            audio_engine::set_fuzz_clip(knob2);
            board.pled2.set(true);

            float knob3 = read_knob(board, bsp::pot3);
            audio_engine::set_fuzz_crunch(knob3);
            board.pled3.set(true);

            float knob4 = read_knob(board, bsp::pot4);
            audio_engine::set_fuzz_threshold(knob4);
            board.pled4.set(true);

            float knob5 = read_knob(board, bsp::pot5);
            audio_engine::set_tremolo_mix(knob5);
            // audio_engine::set_delay_time(knob5);
            board.pled5.set(true);

            float knob6 = read_knob(board, bsp::pot6);
            audio_engine::set_tremolo_lfo_frequency(knob6);
            // audio_engine::set_delay_mix(knob6);
            board.pled6.set(true);

            float knob7 = read_knob(board, bsp::pot7);
            // audio_engine::set_fuzz_clip(knob2);
            // board.pled2.set(true);
            // audio_engine::set_delay_feedback(knob7);

            const bool fw1 = board.ft_sw1.read();
            const bool fw2 = board.ft_sw2.read();
            audio_engine::set_fw1_enabled(fw1);
            audio_engine::set_fw2_enabled(fw2);
        }

        if (board.clock.millis() - last_blink >= blink_interval)
        {
            last_blink = board.clock.millis();
            board.led.toggle();
            // board.pled0.toggle();
            // board.pled1.toggle();
            // board.pled2.toggle();
            // board.pled3.toggle();
            // board.pled4.toggle();
            // board.pled5.toggle();
            // board.pled6.toggle();
            // board.pled7.toggle();
        }
    }
}
