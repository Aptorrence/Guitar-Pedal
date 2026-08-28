# Generic CMake

Welcome to our CMake build system for Embedded SW! This repo currently targets one real board — **`dsp_pedal`**, an STM32H743-based guitar pedal with a CS4270 stereo audio codec — plus a `native` (host PC) target for fast DSP iteration without any hardware attached.

## Installing Prerequisites
First, install the arm toolchain. If you are on a Unix system, simply use your package installer, for example in Ubuntu: ```sudo apt-get install arm-none-eabi-gcc```.
On Windows, you can download it from online, try [here](https://developer.arm.com/downloads/-/gnu-rm) - you may have to add it to your path. Make sure it worked: ```arm-none-eabi-gcc --version```

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

## Debugging
To debug, make sure you have openocd installed ```sudo apt-get install openocd```
On Windows, try [here](https://openocd.org/pages/getting-openocd.html). Also may have to add to path.
Additionally, grab the cortex-debug extension for VSCode.

`.vscode/launch.json` already has working debug configs for `blink` and `guitar_pedal` on the H743VI — just build the target first (see above), then launch the matching config from the Run and Debug panel.

## Developing DSP on this board

Relevant pieces live in a few places:

- **`common/core/dsp/`** — platform-agnostic audio-signal primitives that don't touch hardware and build for `native` too.
- **`app/guitar_pedal/`** — app for the main board. `audio_engine.cpp` owns the the signal chain ex: (noise gate -> volume),`main.cpp` sets everything up and polls user knobs n stuff.
- **`app/effect_test/`** — a **host-native** sandbox for iterating on effects without touching hardware. It runs an effect chain over either a generated test tone or a 16-bit mono WAV file you pass on the command line, and writes `input.wav`/`output.wav` so you can listen to before/after. Build and run it with the `native` preset:
  ```
  cmake --preset native
  cmake --build build/native
  ./build/native/app/effect_test/effect_test.exe "C:\path\to\input.wav"

  ```

 Great way to prototype/tune a new `dsp::Effect` — write it in `common/core/dsp/effects/`, wire it into `effect_test/main.cpp`, and listen to the result before ever flashing hardware (this is really cool :D). There is a example guitar noise in `\common\core\dsp\audio_test_clips\` that you can copy the path to or add your own.

- **`boards/`** — the board support packages : for our current board `dsp_pedal bsp.h`/`bsp_h743.cpp` theres allot of cube IDE functions that are wrapped here but one could also write their own low levele drivers in `common/driver/platform/stm32h7/` to replace them eventually
- **`common/componets/`** — holds the hardware-agnostic componet drivers injected with `driver::I2c`/`driver::Gpio`/`driver::Delay`.

## Code style
Install clang-format to auto-format your code - on Windows, try ```<python> -m pip install clang-format```. On Linux, try ```sudo apt install clang-format```. In VSCode, you can go to settings > Text Editor > Formatting > Format On Save to enable auto-formatting on save. `format.sh`/`format_check.sh` at the repo root run clang-format across the whole tree.
