#include "audio_engine.h"
#include "bsp.h"
#include "normalize.h"

namespace
{
    float read_knob(bsp::Board &board, bsp::PotChannel channel)
    {
        return dsp::normalize01(board.pots.read(channel), bsp::POT_RAW_MIN, bsp::POT_RAW_MAX);
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

    uint32_t last_blink = HAL_GetTick();
    const uint32_t blink_interval = codec_ok ? 500 : 100;

    uint32_t last_control = HAL_GetTick();
    constexpr uint32_t CONTROL_INTERVAL_MS{100};

    while (true)
    {
        if (HAL_GetTick() - last_control >= CONTROL_INTERVAL_MS)
        {
            last_control = HAL_GetTick();

            float knob0 = read_knob(board, bsp::pot0);
            audio_engine::set_volume(knob0);

            float knob1 = read_knob(board, bsp::pot1);
            audio_engine::set_noise_gate_threshold(knob1);

            float knob2 = read_knob(board, bsp::pot2);
            float knob3 = read_knob(board, bsp::pot3);
            float knob4 = read_knob(board, bsp::pot4);
            float knob5 = read_knob(board, bsp::pot5);
            float knob6 = read_knob(board, bsp::pot6);
            float knob7 = read_knob(board, bsp::pot7);
            bool foot_Switch1 = board.ft_sw1.read();
            bool foot_Switch2 = board.ft_sw2.read();
        }

        if (HAL_GetTick() - last_blink >= blink_interval)
        {
            last_blink = HAL_GetTick();
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
