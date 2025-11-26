# HelloWorld (OpenCV + fmt + CMake)

Small starter app to experiment with OpenCV.

## Prerequisites

Ubuntu/Debian:

- `sudo apt-get update`
- `sudo apt-get install -y build-essential cmake pkg-config libopencv-dev libfmt-dev`
- Optional tooling: `sudo apt-get install -y clang-format clang-tidy`

Other distros (rough equivalents):

- Fedora/RHEL: `sudo dnf install @development-tools cmake opencv-devel fmt-devel`
- Arch: `sudo pacman -S base-devel cmake opencv fmt`
- macOS (Homebrew): `brew install cmake opencv fmt`

## Build

From this folder (`helloworld`):

- `rm -rf .build`  (optional clean)
- `cmake --preset=default`
- `cmake --build .build -j`

Notes:

- Preset uses the "Unix Makefiles" generator and system packages (no vcpkg toolchain).
- `compile_commands.json` is generated in `.build/` for tooling.

## Run

- `./.build/HelloWorld assets/lena_img.png`

Behavior:

- If a GUI is available (`DISPLAY`/`WAYLAND_DISPLAY`), shows a resizable window (drag to resize).
- Otherwise, saves `output.png` in this folder.

## Code Layout

- `helloworld.cpp` — main entry
- `src/logger.h` / `src/logger.cpp` — colored logger with timestamps, levels
- `src/cv_util.h` — header-only helpers: `cvutil::load`, `cvutil::quickDisplay`
- `assets/` — sample images

## Logger

- Construct: `logger::Logger log{logger::Level::DEBUG, "[cv-demo] {}"};`
- Usage: `log.info("loaded {}x{}", img.cols, img.rows);`
- Output format: `[LEVEL HH:MM:SS.mmm] <pattern-applied-text>`

## Formatting and Linting

- Format all sources (needs `clang-format`):
  - `cmake --build .build --target format`
- Clang-Tidy (if installed) runs automatically during the build.
  - Manual: `run-clang-tidy -p .build` (requires `run-clang-tidy` helper)

## Adding Code

- Header-only (in `src/`): just `#include "your.hpp"` — already on the include path.
- New `.cpp` files: add to the target in `CMakeLists.txt`, e.g.:
  - `target_sources(HelloWorld PRIVATE src/your.cpp)`

## Optional: Ninja generator

- Install: `sudo apt-get install -y ninja-build`
- Edit `CMakePresets.json` and set `"generator": "Ninja"`, then reconfigure/build.

## Optional: Using vcpkg

This project is configured to use system packages. If you want vcpkg:

- Set a toolchain in `CMakePresets.json`:
  - `"CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"`
- Add dependencies to `vcpkg.json` (e.g., `"opencv4"`, `"fmt"`).
- Reconfigure: `cmake --preset=default`

Be aware OpenCV via vcpkg may pull many desktop deps on Linux (dbus, systemd, gtk). System packages are simpler for getting started.
