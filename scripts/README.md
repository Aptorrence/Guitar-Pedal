# Live-plotting a variable (e.g. audio signal levels)

`scripts/plot_debug_out.py` polls a variable's value straight out of MCU RAM over OpenOCD's telnet interface and live-plots it with matplotlib. Handy for watching signal levels while you play, without needing a full GDB debug session.


1. Build normally (see Building above).
2. One-time setup: `python -m pip install matplotlib`
3. Start OpenOCD claiming the probe (leave this running in its own terminal):
   ```
   openocd -f mcu_support/stm32/stlink-dap.cfg -f mcu_support/stm32/h7xx/stm32h7x.cfg
   ```
   This exposes a telnet console on port 4444 (used for polling and flashing) and a GDB port on 3333 (unused here, but harmless).
4. In another terminal, run the plotter:
   ```
   python scripts/plot_debug_out.py
   ```
   A window pops up graphing the live values. Ctrl+C or close the window to stop.

To flash new firmware while OpenOCD is already running (no VSCode needed), connect to its telnet console and program directly:
```
telnet localhost 4444
> program build/dsp_pedal/app/guitar_pedal/guitar_pedal.elf verify reset
```
(On Windows, if `telnet` isn't available, any TCP client — e.g. PuTTY in raw mode to `localhost:4444` — works the same way.)

Notes:
- This is a **polling** read, not a real trace capture — fine for watching envelope/level trends, not exact waveform shape. Sampling (reading the MCU) and rendering (redrawing the plot) run on separate threads so one doesn't bottleneck the other:
  - `--poll-hz` (default 500) caps how fast the background thread reads the MCU. The link itself measures well under 1ms per read, so this can go quite high; the plot's bottom-right corner shows the actual achieved sampling rate live.
  - `--fps` (default 30) sets how often the plot redraws. Pushing this much higher rarely helps — matplotlib's own redraw becomes the limit, not the data.
  - `--window-seconds` sets how much history is visible (default 1s).
- Watch a different or additional variable with `--symbol <name>` (repeatable), as long as it's linked into the ELF (the default `-Og -O0` build keeps debug variables around, so this just works).
- If OpenOCD reports the target as `halted` (e.g. left over from a previous debug session) and nothing seems to move, resume it from the telnet console with `reset run`.
- We tried doing this "properly" via SWO/ITM trace + a VSCode graph first, but hit persistent ITM packet framing issues specific to this STM32H7 + OpenOCD combo that we couldn't fully root-cause. The polling approach above is simpler and reliably works, at the cost of resolution.
