#!/usr/bin/env python3
"""Capture labeled EMG samples from the Nano 33 BLE and write them to CSV.

The firmware listens for:
  BEGIN_CAPTURE
  END_CAPTURE

While capture is active, it streams CSV rows as:
  timestamp_ms,a3,a4

This script adds the gesture label and writes:
  label,timestamp_ms,a3,a4
"""

from __future__ import annotations

import argparse
import csv
import os
import select
import sys
import termios
import time
from pathlib import Path
from typing import Iterable, Optional

DEFAULT_LABELS = ["rest", "open", "fist", "point", "like", "middle", "pinch"]
BAUD_CONSTANTS = {
    9600: termios.B9600,
    19200: termios.B19200,
    38400: termios.B38400,
    57600: termios.B57600,
    115200: termios.B115200,
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Record a labeled EMG dataset from Arduino serial output.")
    parser.add_argument("--port", required=True, help="Serial port, for example /dev/cu.usbmodemXXXX")
    parser.add_argument(
        "--output",
        default=str(Path.home() / "Desktop" / "emg_dataset.csv"),
        help="Output CSV file path.",
    )
    parser.add_argument(
        "--labels",
        nargs="*",
        default=DEFAULT_LABELS,
        help="Gesture labels to record in order.",
    )
    parser.add_argument(
        "--samples-per-label",
        type=int,
        default=200,
        help="Number of valid samples to save per gesture label.",
    )
    parser.add_argument(
        "--baud",
        type=int,
        default=9600,
        choices=sorted(BAUD_CONSTANTS),
        help="Serial baud rate used by the firmware.",
    )
    parser.add_argument(
        "--settle-seconds",
        type=float,
        default=2.0,
        help="Pause after each label prompt before capture starts.",
    )
    return parser.parse_args()


def open_serial(port: str, baud: int) -> int:
    fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_SYNC)
    attrs = termios.tcgetattr(fd)
    speed = BAUD_CONSTANTS[baud]

    attrs[0] = 0
    attrs[1] = 0
    attrs[2] |= termios.CLOCAL | termios.CREAD
    attrs[3] = 0
    termios.cfsetispeed(attrs, speed)
    termios.cfsetospeed(attrs, speed)
    attrs[6][termios.VMIN] = 0
    attrs[6][termios.VTIME] = 1
    termios.tcsetattr(fd, termios.TCSANOW, attrs)
    return fd


def write_line(fd: int, text: str) -> None:
    os.write(fd, f"{text}\n".encode("utf-8"))


def read_line(fd: int, timeout: float = 1.0) -> Optional[str]:
    deadline = time.monotonic() + timeout
    buffer = bytearray()

    while True:
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            break

        ready, _, _ = select.select([fd], [], [], remaining)
        if not ready:
            break

        chunk = os.read(fd, 1)
        if not chunk:
            continue

        if chunk == b"\n":
            return buffer.decode("utf-8", errors="replace").strip()
        if chunk != b"\r":
            buffer.extend(chunk)

    if buffer:
        return buffer.decode("utf-8", errors="replace").strip()
    return None


def drain_serial(fd: int, seconds: float = 1.0) -> None:
    end_time = time.monotonic() + seconds
    while time.monotonic() < end_time:
        line = read_line(fd, timeout=0.1)
        if line is None:
            continue


def wait_for_capture_begin(fd: int) -> None:
    while True:
        line = read_line(fd, timeout=2.0)
        if line is None:
            continue
        if line == "CAPTURE_BEGIN":
            return


def capture_label(
    fd: int,
    writer: csv.writer,
    label: str,
    samples_per_label: int,
    settle_seconds: float,
) -> None:
    print(f"Prepare gesture '{label}'. You have {settle_seconds:.1f} seconds.")
    input("Press Enter when you are ready to record this label...")
    time.sleep(settle_seconds)

    write_line(fd, "BEGIN_CAPTURE")
    wait_for_capture_begin(fd)

    collected = 0
    while collected < samples_per_label:
        line = read_line(fd, timeout=5.0)
        if line is None:
            raise RuntimeError(f"Timed out while collecting '{label}' samples.")

        if line in {"CAPTURE_BEGIN", "CAPTURE_END", "nano active"}:
            continue

        parts = line.split(",")
        if len(parts) != 3:
            continue

        try:
            timestamp_ms = int(parts[0])
            a3 = int(parts[1])
            a4 = int(parts[2])
        except ValueError:
            continue

        writer.writerow([label, timestamp_ms, a3, a4])
        collected += 1

    write_line(fd, "END_CAPTURE")
    while True:
        line = read_line(fd, timeout=2.0)
        if line == "CAPTURE_END":
            break

    print(f"Saved {collected} samples for '{label}'.")


def main() -> int:
    args = parse_args()
    output_path = Path(args.output).expanduser()
    output_path.parent.mkdir(parents=True, exist_ok=True)

    if not args.labels:
        print("No labels supplied.", file=sys.stderr)
        return 2

    fd = open_serial(args.port, args.baud)
    try:
        print(f"Connected to {args.port} at {args.baud} baud.")
        print(f"Writing dataset to {output_path}")
        time.sleep(2.0)
        drain_serial(fd, seconds=1.0)

        with output_path.open("w", newline="", encoding="utf-8") as csv_file:
            writer = csv.writer(csv_file)
            writer.writerow(["label", "timestamp_ms", "a3", "a4"])

            for label in args.labels:
                capture_label(fd, writer, label, args.samples_per_label, args.settle_seconds)

        print("Dataset capture complete.")
        return 0
    finally:
        os.close(fd)


if __name__ == "__main__":
    raise SystemExit(main())
