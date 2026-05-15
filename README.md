# Lysa – Tutorial 1

A minimal Lysa Engine project that:

1. **Loads** a `.assets` file from disk using `lysa::AssetsPack::load`.
2. **Displays** it with a deferred renderer (ACES tone mapping, SMAA, GTAO,
   bloom) and two lights (directional + omni).
3. **Rotates** the asset continuously around the Y-axis via `float4x4::rotation_y`
   applied every process tick.

---

## Prerequisites

| Tool | Version |
|---|---|
| Vulkan SDK | 1.3.x+ |
| MSVC (Windows) | 19+ (VS 2022) |
| LLVM / MinGW (Windows alt.) | 21+ |
| CMake | 3.29+ |
| Ninja (or another generator) | any recent |

See the [Lysa build documentation](https://henrimichelon.github.io/Lysa/) for
platform-specific instructions.

---

## Setup

```sh
# 1. Clone the Lysa engine & Vireo RHI
git clone https://github.com/LysaEngine/lysa_engine
git clone https://github.com/HenriMichelon/vireo_rhi

# 2. Clone this project
git clone <this-repo> lysa_tutorial_01
cd lysa_tutorial_01

# 3. Point the project at the engine
cp .env.cmake.example .env.cmake
#    → Edit .env.cmake and set LYSA_ENGINE_PROJECT_DIR and VIREO_RHI_PROJECT_DIR

# 4. Configure & build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target myapp
```

---

## Running

**Always run from the project root** (not from `build/`) so the `app://` virtual
filesystem resolves `app://shaders` and `app://res` correctly:

```sh
# Linux
./build/myapp

# Windows
build\myapp.exe
```

---

## Controls (after clicking the window)

| Key / Input | Action |
|---|---|
| **W / A / S / D** | Move camera forward / left / back / right |
| **Q / Z** | Move camera up / down |
| **Mouse** | Look around |
| **Arrow keys** | Look around (keyboard alternative) |
| **Escape** | Release mouse cursor |
| **Click** | Re-capture mouse cursor |

