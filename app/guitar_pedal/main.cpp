#include "audio_engine.h"
#include "bsp.h"
#include "knob_filter.h"
#include "normalize.h"
#include <array>

namespace
{
    constexpr uint32_t CONTROL_INTERVAL_MS{100};
    constexpr float CONTROL_RATE_HZ{1000.0f / static_cast<float>(CONTROL_INTERVAL_MS)};
    constexpr float KNOB_SMOOTH_MS{0.0f};
    constexpr float KNOB_DEADBAND{0.002f};

    std::array<controls::KnobFilter, bsp::potChannelCount> knob_filters;

    struct KnobBinding
    {
        bsp::PotChannel channel;
        void (*set)(float linear01);
        driver::Stmh7::HwGpio &led;
    };

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

    /** Which pot drives which engine setter; swap or uncomment rows to remap. */
    const std::array knobs{
        KnobBinding{bsp::pot0, audio_engine::set_volume, board.pled0},
        KnobBinding{bsp::pot1, audio_engine::set_noise_gate_threshold, board.pled1},
        KnobBinding{bsp::pot2, audio_engine::set_fuzz_clip, board.pled2},
        KnobBinding{bsp::pot3, audio_engine::set_fuzz_crunch, board.pled3},
        KnobBinding{bsp::pot4, audio_engine::set_fuzz_threshold, board.pled4},
        KnobBinding{bsp::pot5, audio_engine::set_tremolo_mix, board.pled5},
        // KnobBinding{bsp::pot5, audio_engine::set_delay_time, board.pled5},
        KnobBinding{bsp::pot6, audio_engine::set_tremolo_lfo_frequency, board.pled6},
        // KnobBinding{bsp::pot6, audio_engine::set_delay_mix, board.pled6},
        // KnobBinding{bsp::pot7, audio_engine::set_delay_feedback, board.pled7},
    };

    uint32_t last_control = board.clock.millis();

    while (true)
    {
        if (board.clock.millis() - last_control >= CONTROL_INTERVAL_MS)
        {
            last_control = board.clock.millis();

            for (const KnobBinding &knob : knobs)
            {
                knob.set(read_knob(board, knob.channel));
                knob.led.set(true);
            }

            audio_engine::set_fw1_enabled(board.ft_sw1.read());
            audio_engine::set_fw2_enabled(board.ft_sw2.read());
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
