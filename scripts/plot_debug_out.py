#!/usr/bin/env python3
"""
Live-plots a float variable read out of the MCU's RAM over OpenOCD's telnet
interface, by polling with `mdw` and reinterpreting the returned word as an
IEEE-754 float. Requires a standalone `openocd` instance already running
against the board (its telnet port defaults to 4444) -- this does not need
a GDB session or VSCode running at the same time.

Sampling and rendering are decoupled: a background thread polls the MCU as
fast as the link allows (--poll-hz), while the plot redraws at a steady,
smooth rate (--fps) using blitting so the display stays responsive even at
high sample rates.

Usage:
    openocd -f mcu_support/stm32/stlink-dap.cfg -f mcu_support/stm32/h7xx/stm32h7x.cfg
    python scripts/plot_debug_out.py
"""

import argparse
import re
import socket
import struct
import subprocess
import sys
import threading
import time
from collections import deque

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

DEFAULT_ELF = "build/dsp_pedal/app/guitar_pedal/guitar_pedal.elf"
DEFAULT_SYMBOLS = ["debug_in", "debug_out"]
NM_TOOL = "arm-none-eabi-nm"
COLORS = ["#4287f5", "#f57542", "#42f554", "#f542e0"]


def find_symbol_address(elf_path: str, symbol: str) -> int:
    output = subprocess.check_output([NM_TOOL, "-C", elf_path], text=True)
    for line in output.splitlines():
        if symbol in line:
            address_str = line.split()[0]
            return int(address_str, 16)
    raise RuntimeError(f"Symbol '{symbol}' not found in {elf_path}")


class OpenOcdTelnet:
    def __init__(self, host: str, port: int):
        self.sock = socket.create_connection((host, port), timeout=2)
        self.sock.settimeout(2)
        self._read_until_prompt()

    def _read_until_prompt(self) -> str:
        buf = b""
        while not buf.endswith(b"> "):
            chunk = self.sock.recv(4096)
            if not chunk:
                break
            buf += chunk
        return buf.decode(errors="replace")

    def read_word(self, address: int) -> int:
        self.sock.sendall(f"mdw 0x{address:08x} 1\n".encode())
        response = self._read_until_prompt()
        match = re.search(r"0x[0-9a-fA-F]+:\s*([0-9a-fA-F]+)", response)
        if not match:
            raise RuntimeError(f"Unexpected mdw response: {response!r}")
        return int(match.group(1), 16)

    def read_float(self, address: int) -> float:
        word = self.read_word(address)
        return struct.unpack("<f", struct.pack("<I", word))[0]


class Sampler(threading.Thread):
    """Polls the MCU as fast as the link allows, on its own thread, decoupled
    from the plot's redraw rate."""

    def __init__(self, telnet: OpenOcdTelnet, addresses: dict, max_samples: int, max_hz: float):
        super().__init__(daemon=True)
        self.telnet = telnet
        self.addresses = addresses
        self.min_period = 1.0 / max_hz if max_hz > 0 else 0.0
        self.lock = threading.Lock()
        self.series = {symbol: deque(maxlen=max_samples) for symbol in addresses}
        self.sample_count = 0
        self.stop_event = threading.Event()
        self.last_error = None

    def run(self) -> None:
        while not self.stop_event.is_set():
            t0 = time.perf_counter()
            try:
                values = {
                    symbol: self.telnet.read_float(address)
                    for symbol, address in self.addresses.items()
                }
            except (RuntimeError, OSError) as exc:
                if str(exc) != str(self.last_error):
                    print(f"read failed: {exc}", file=sys.stderr)
                self.last_error = exc
                continue

            with self.lock:
                for symbol, value in values.items():
                    self.series[symbol].append(value)
                self.sample_count += 1

            elapsed = time.perf_counter() - t0
            remaining = self.min_period - elapsed
            if remaining > 0:
                time.sleep(remaining)

    def snapshot(self):
        with self.lock:
            return self.sample_count, {symbol: list(d) for symbol, d in self.series.items()}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", default=DEFAULT_ELF)
    parser.add_argument("--symbol", dest="symbols", action="append")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=4444)
    parser.add_argument("--window-seconds", type=float, default=0.1)
    parser.add_argument("--poll-hz", type=float, default=500.0, help="background sampling rate")
    parser.add_argument("--fps", type=float, default=30.0, help="plot redraw rate")
    args = parser.parse_args()

    symbols = args.symbols or DEFAULT_SYMBOLS
    addresses = {symbol: find_symbol_address(args.elf, symbol) for symbol in symbols}
    for symbol, address in addresses.items():
        print(f"{symbol} @ 0x{address:08x}")

    telnet = OpenOcdTelnet(args.host, args.port)

    max_samples = max(2, int(args.window_seconds * args.poll_hz))
    sampler = Sampler(telnet, addresses, max_samples, args.poll_hz)
    sampler.start()

    fig, ax = plt.subplots()
    x_axis = list(range(max_samples))
    lines = {
        symbol: ax.plot(x_axis, [0.0] * max_samples, label=symbol, color=COLORS[i % len(COLORS)])[0]
        for i, symbol in enumerate(symbols)
    }
    ax.set_ylim(-1.05, 1.05)
    ax.set_xlim(0, max_samples - 1)
    ax.set_xlabel(f"last {args.window_seconds:g}s ({max_samples} samples)")
    ax.set_ylabel("value")
    ax.set_title("Live: " + ", ".join(symbols))
    ax.legend(loc="upper right")
    fps_text = ax.text(0.99, 0.02, "", transform=ax.transAxes, ha="right", color="gray", fontsize=8)

    last_frame_time = [time.perf_counter()]
    last_sample_count = [0]

    def update(_frame):
        now = time.perf_counter()
        sample_count, series = sampler.snapshot()

        actual_hz = (sample_count - last_sample_count[0]) / max(now - last_frame_time[0], 1e-6)
        last_frame_time[0] = now
        last_sample_count[0] = sample_count
        fps_text.set_text(f"sampling ~{actual_hz:.0f} Hz")

        for symbol, line in lines.items():
            values = series[symbol]
            if not values:
                continue
            # Right-align the most recent sample against the fixed x-axis window.
            padded = [None] * (max_samples - len(values)) + values
            line.set_ydata([v if v is not None else 0.0 for v in padded])

        return tuple(lines.values()) + (fps_text,)

    interval_ms = 1000.0 / args.fps
    _anim = FuncAnimation(fig, update, interval=interval_ms, blit=True, cache_frame_data=False)

    def on_close(_event):
        sampler.stop_event.set()

    fig.canvas.mpl_connect("close_event", on_close)
    plt.show()


if __name__ == "__main__":
    main()
