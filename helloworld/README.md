# HelloWorld (OpenCV + fmt + CMake)

Small starter CV playground with a simple runner (engine) for examples, a tiny
argument parser, and a colored timestamped logger.

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
- `compile_commands.json` is generated in `.build/` and copied next to sources for editors.

## Run

List examples:

- `./.build/HelloWorld --list`

Run all examples:

- `./.build/HelloWorld`
- `./.build/HelloWorld --example all`

Run one example and pass arguments (everything after `--args` is forwarded to the example):

- `./.build/HelloWorld --example show --args assets/lena_img.png`
- `./.build/HelloWorld --example edges --args assets/lena_img.png --t1 50 --t2 150 --blur 3`

Show example-specific help:

- `./.build/HelloWorld --example edges --args --help`

Behavior:

- Examples that open windows use a resizable window if a GUI is available (`DISPLAY`/`WAYLAND_DISPLAY`).
- In headless environments, examples typically write `output.png`.

## Code Layout

- `main.cpp` — bootstrap runner with `--list`, `--example`, `--args`
- `src/examples/registry.h` / `src/examples/registry.cpp` — example registry and macro
- `src/examples/show.cpp` — example: display an image
- `src/examples/edges.cpp` — example: Canny edge detection
- `src/cli/argparse.h` — tiny header-only arg parser used by examples
- `src/logger.h` / `src/logger.cpp` — colored logger with timestamps, levels, names
- `src/cv_util.h` — header-only helpers: `cv_util::load`, `cv_util::quickDisplay`
- `assets/` — sample images

## Examples

- `show [path]`
  - Displays an image (defaults to `assets/lena_img.png`).
  - Help: `./.build/HelloWorld --example show --args --help`
- `edges [--t1 N] [--t2 N] [--blur K] [path]`
  - Canny edges with optional Gaussian blur; overlays edges in red.
  - Defaults: `--t1 100 --t2 200 --blur 3`, `path=assets/lena_img.png`.
  - Help: `./.build/HelloWorld --example edges --args --help`

## Logger

- Construct (named): `logger::Logger log{"cv-demo", logger::Level::DEBUG};`
- Usage: `log.info("loaded {}x{}", img.cols, img.rows);`
- Output format: `[LEVEL name] [HH:MM:SS.mmm] <pattern-applied-text>`

## Formatting and Linting

- Format all sources (needs `clang-format`):
  - `cmake --build .build --target format`
- Clang-Tidy (if installed) runs automatically during the build.
  - Manual: `run-clang-tidy -p .build` (requires `run-clang-tidy` helper)

## Adding Code / New Examples

- Header-only (in `src/`): just `#include "your.hpp"` — already on the include path.
- New `.cpp` files: add to target via `target_sources` in `CMakeLists.txt` if outside `src/examples`.
- New example:
  1) Create `src/examples/<name>.cpp` with function `static int <name>_example(int argc, char** argv)`.
  2) Register it at the end: `REGISTER_EXAMPLE("<name>", <name>_example, "short help")`.
  3) Parse args with the tiny parser:

```cpp
#include "cli/argparse.h"
static int my_example(int argc, char** argv) {
  logger::Logger log{"my", logger::Level::INFO};
  cli::ArgParser ap{"my"};
  ap.add_option("k", 'k', "some int option", "42");
  ap.add_positional("path", "image path");
  if (!ap.parse(argc, argv) || ap.help()) { log.info("\n{}", ap.usage()); return ap.help() ? 0 : 2; }
  int k = ap.get_int("k", 42);
  std::string path = ap.positionals().empty() ? "assets/lena_img.png" : ap.positionals().front();
  // ... do work ...
  return 0;
}
REGISTER_EXAMPLE("my", my_example, "demo example");
```

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

## Documentation (Doxygen + PlantUML)

Install tools (Ubuntu/Debian):

- `sudo apt-get install -y doxygen graphviz plantuml default-jre`

Build docs:

- `cmake --build .build --target docs`

Output:

- HTML in `.build/docs/html/index.html`

Notes:

- PlantUML jar is auto-detected at `/usr/share/plantuml/plantuml.jar`. If it
  lives elsewhere, pass `-DPLANTUML_JAR=/path/to/plantuml.jar` when configuring
  CMake.
- Docs include the engine registry/runner, the tiny CLI, the logger, and all
  examples. Example and engine pages live in `docs/`.

## CI/CD (GitHub Actions)

Workflows live under `.github/workflows/`:

- `ci.yml` — builds on Linux, runs a smoke test, uploads `output_edges.png`.
- `docs.yml` — builds Doxygen and deploys to GitHub Pages.
- `release.yml` — on tag push `v*`, builds and attaches a Linux tarball to the release.

Enable Pages:

- Repo Settings → Pages → Build and deployment → Source: “GitHub Actions”.

Cut a release:

- `git tag v0.1.0 && git push origin v0.1.0`
- The workflow builds `HelloWorld` and uploads `helloworld-<tag>-linux-x64.tar.gz`.
