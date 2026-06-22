---
name: cmake-build
description: Build, configure, and test the AmuseEngine CMake project through CMakePresets.json. Use when Codex needs to run a local build check, configure CMake, run CTest, diagnose compile errors, or verify C++ changes in this repository.
---

# CMake Build

## Overview

Use the repository's `CMakePresets.json` as the single source of truth for Codex builds. Do not introduce ad hoc build directories or direct `cmake -S . -B ...` commands unless the preset file is missing or broken and the user explicitly asks for a workaround.

## Standard Workflow

Run commands from the repository root.

1. Inspect available presets when needed:

```powershell
cmake --list-presets
cmake --build --list-presets
ctest --list-presets
```

2. Configure with the Codex preset:

```powershell
cmake --preset codex-x64-debug
```

3. Build with the matching build preset:

```powershell
cmake --build --preset codex-x64-debug
```

4. Run tests with the matching test preset:

```powershell
ctest --preset codex-x64-debug --output-on-failure
```

## Repository Preset

The expected default preset is `codex-x64-debug`.

- Configure preset: `codex-x64-debug`
- Build preset: `codex-x64-debug`
- Test preset: `codex-x64-debug`
- Binary directory: `Engine/Build/Codex/x64-Debug`
- Install directory: `Engine/Install/Codex/x64-Debug`
- Generator: `Ninja`
- Build type: `Debug`

If `CMakePresets.json` changes, re-read it before running build commands and prefer its current preset names over the defaults listed here.

## Failure Handling

- If configure fails because `Ninja` or a compiler is missing, report the missing tool and do not switch generators unless the user asks.
- If build fails, quote the first relevant compiler or linker error and inspect the referenced source files before editing.
- If tests fail, use `--output-on-failure` output and inspect the failing test source before editing.
- After any C++ change, run at least `cmake --build --preset codex-x64-debug`. Run `ctest --preset codex-x64-debug --output-on-failure` when test targets are affected or when the user requests full verification.

## Working Directory Hygiene

- Treat `Engine/Build/Codex/x64-Debug` and generated dependency folders as build output.
- Do not delete existing build directories without explicit user approval.
- Ignore unrelated untracked or modified files unless they directly affect configure, build, or test results.
