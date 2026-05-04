# Trace Viewer

A simple local web viewer for reconstructed trace output.

## How to use

1. Run `demo.exe` to generate `trace.log`.
2. Run `reconstruction.exe reconstruction.json` from the project root to produce `reconstruction.json`.
3. Open `viewer/index.html` in a browser.
4. Use the file picker to choose `reconstruction.json`, or paste the reconstructed JSON into the textarea.
5. Click **Parse Trace Data**.

## What it shows

- Reconstructed span tree built by the C++ reconstruction engine
- Span duration, `startTs`, and `endTs`
- Async child spans when child thread differs from parent thread
- Log statements with file, function, and line number
- Log level filtering for INFO / WARN / ERROR
- Compact call flow view for direct calls vs async thread calls
- Overall span/log summary

## Project flow

1. `demo.exe` generates raw trace events in `trace.log`.
2. `reconstruction.exe` reads `trace.log` and builds the span hierarchy.
3. `reconstruction.exe` writes structured JSON output to `reconstruction.json`.
4. `viewer/index.html` loads `reconstruction.json` and displays the flow graphically.

This keeps the reconstruction intelligence in C++, while the browser provides easy visual debugging.
