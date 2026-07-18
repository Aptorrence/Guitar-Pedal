# Generic CMake

Welcome to our CMake build system for Embedded SW! This repo currently targets one real board — **`dsp_pedal`**, an STM32H743-based guitar pedal with a CS4270 stereo audio codec — plus a `native` (host PC) target for fast DSP iteration without any hardware attached.

## Installing Prerequisites
First, install the arm toolchain. If you are on a Unix system, simply use your package installer, for example in Ubuntu: ```sudo apt-get install arm-none-eabi-gcc```.
On Windows, you can download it from online, try [here](https://developer.arm.com/downloads/-/gnu-rm) - you may have to add it to your path. Make sure it worked: ```arm-none-eabi-gcc --version```

If you want to cross-compile for 32-bit RPI, also install the arm32 toolchain: ```gcc-arm-linux-gnueabihf```.
On Windows, try [here](https://developer.arm.com/downloads/-/gnu-a). Make sure it worked: ```arm-linux-gnueabihf-gcc --version```


Next, install Cmake (3.27+). ```sudo apt-get install cmake```
On Windows, try [here](https://cmake.org/download/). Again, may have to add to path.
Make sure it worked: ```cmake --version```

Also, install Ninja ```sudo apt-get install ninja-build```
On Windows, try [here](https://github.com/ninja-build/ninja/releases). Same with adding to path.
Make sure it worked: ```ninja --version```


Lastly, pull in external dependencies:
```git submodule update --init --recursive```


## Building
CMake presets. Configure once per target, then build:

```
cmake --preset dsp_pedal
cmake --build build/dsp_pedal
```

Available presets (see `CMakePresets.json`): `native`, `dsp_pedal`, `board_with_stm32l496zg`. Each preset configures into its own `build/<preset>` directory, so you can keep several targets configured side by side.

By default all apps for the target board are built. To build only one app, pass `TARGET_APP` at configure time:
```
cmake --preset dsp_pedal -DTARGET_APP=guitar_pedal
cmake --build build/dsp_pedal
```

For a clean build, delete the corresponding `build/<preset>` directory and re-run `cmake --preset`.

Build type defaults to whatever CMake picks for a single-config Ninja generator (unset = no optimization flags added). Pass `-DCMAKE_BUILD_TYPE=Release` at configure time if you need an optimized build.

On the `dsp_pedal` board, building `guitar_pedal` also produces `guitar_pedal.hex` and `guitar_pedal.bin` alongside the `.elf` (see `app/guitar_pedal/CMakeLists.txt`).

## Debugging
To debug, make sure you have openocd installed ```sudo apt-get install openocd```
On Windows, try [here](https://openocd.org/pages/getting-openocd.html). Also may have to add to path.
Additionally, grab the cortex-debug extension for VSCode.

`.vscode/launch.json` already has working debug configs for `blink` and `guitar_pedal` on the H743VI — just build the target first (see above), then launch the matching config from the Run and Debug panel.

## Developing DSP on this board

The DSP-relevant pieces live in a few places:

- **`common/core/dsp/`** — platform-agnostic effect building blocks: `Effect` (the one-sample-in/one-sample-out interface everything implements), `Volume`, `NoiseGate`, plus helpers (`sample_convert.h` for Q24 <-> float, `normalize.h`, `audio_taper.h`, `ping_pong_buffer.h`). This code doesn't touch hardware and builds for `native` too.
- **`app/guitar_pedal/`** — the real firmware app. `audio_engine.cpp` owns the ping-pong DMA buffers and the fixed signal chain (noise gate -> volume today), wired to the board's `driver::AudioStream` via a process callback invoked once per DMA half-buffer. `main.cpp` polls pots/footswitches and pushes their normalized values into the audio engine.
- **`app/effect_test/`** — a **host-native** sandbox for iterating on effects without touching hardware. It runs an effect chain over either a generated test tone or a 16-bit mono WAV file you pass on the command line, and writes `input.wav`/`output.wav` so you can listen to before/after. Build and run it with the `native` preset:
  ```
  cmake --preset native
  cmake --build build/native
  ./build/native/app/effect_test/effect_test.exe [path/to/input.wav]
  ```
  This is the fastest way to prototype/tune a new `dsp::Effect` — write it in `common/core/dsp/`, wire it into `effect_test/main.cpp`, and listen to the result before ever flashing hardware.
- **`boards/dsp_pedal/`** — the board support package: `bsp.h`/`bsp_h743.cpp` wire up the concrete CS4270 codec, SAI/DMA audio stream, ADC-scanned pots, and GPIOs behind the hardware-agnostic interfaces above. `board_pins.h` is generated from CubeIDE — don't hand-edit it, regenerate it there instead.
- **`common/componets/cs4270/`** — the CS4270 codec driver (I2C register config + reset sequencing), hardware-agnostic over injected `driver::I2c`/`driver::Gpio`/`driver::Delay`.

Sample rate is fixed at 44.1kHz (`kSampleRateHz` in `audio_engine.cpp`, must match `SAI_AUDIO_FREQUENCY_44K` in `bsp_h743.cpp`) and audio samples are moved as 24-bit-in-32-bit stereo-interleaved words — use `dsp::q24_to_float`/`dsp::float_to_q24` (`sample_convert.h`) at the edges of your effect code so the effects themselves just work in `-1..1` float.

`audio_engine.cpp` also exposes `g_last_input_sample`/`g_last_output_sample` as non-static volatile globals specifically so you can watch them live by symbol name from a debugger (cortex-debug Live Expressions, etc.).

## Code style
Install clang-format to auto-format your code - on Windows, try ```<python> -m pip install clang-format```. On Linux, try ```sudo apt install clang-format```. In VSCode, you can go to settings > Text Editor > Formatting > Format On Save to enable auto-formatting on save. `format.sh`/`format_check.sh` at the repo root run clang-format across the whole tree.
