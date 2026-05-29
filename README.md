# theflexprotocol

Top-level CMake wraps PlatformIO projects into organized build targets.

## Structure

- `source/`: primary firmware projects (`esp`, `nano`, `pico`)
- `week2examples/`: optional week-2 firmware snapshots
- `cmake/`: shared CMake helpers for PlatformIO target generation
- `build/`: out-of-source CMake build trees (generated)

## Requirements

- CMake 3.20+
- PlatformIO CLI (`pio`) installed and in `PATH`

## Configure

```bash
cmake --preset default
```

To include `week2examples` targets too:

```bash
cmake --preset with-week2
```

## Build Targets

Examples (after configure):

```bash
cmake --build --preset default --target source_nano_build
cmake --build --preset default --target source_nano_upload
cmake --build --preset default --target source_nano_monitor
cmake --build --preset default --target source_nano_test
```

Other available targets follow the same naming pattern:

- `source_esp_build`, `source_esp_upload`, `source_esp_monitor`, `source_esp_clean`, `source_esp_test`
- `source_pico_build`, `source_pico_upload`, `source_pico_monitor`, `source_pico_clean`

If `with-week2` is configured, additional targets are available:

- `week2_esp_build`, `week2_nano_build`, `week2_pico_build`

## EMG Dataset Capture

The Nano firmware now supports a capture mode that streams raw EMG samples from A3 and A4 over Serial. On your Mac, use the helper script to save labeled samples into a CSV file.

Example:

```bash
python3 tools/capture_emg_dataset.py \
	--port /dev/cu.usbmodemXXXX \
	--output ~/Desktop/emg_dataset.csv
```

Default labels are:

- `rest`
- `open`
- `fist`
- `point`
- `like`
- `middle`
- `pinch`

You can override them with `--labels` and change the sample count with `--samples-per-label`.

## Notes

- Existing PlatformIO folder layout is preserved.
- CMake is used as an orchestrator, while board-specific compile/upload behavior remains in each `platformio.ini`.
