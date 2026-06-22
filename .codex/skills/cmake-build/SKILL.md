---
name: cmake-build
description: Build, configure, and test the AmuseEngine CMake project with direct Ninja commands under the Visual Studio MSVC x64 environment. Use when Codex needs to run a local build check, configure CMake, run CTest, diagnose compile errors, or verify C++ changes in this repository.
---

# CMake Build

## Overview

Use direct CMake commands with the Ninja generator. This repository must be configured and built from the Visual Studio x64 developer environment so CMake selects MSVC `cl.exe`, matching `CMakeSettings.json` / Visual Studio behavior.

## Codex Permission Requirement

In Codex, run configure, build, and test commands with escalated permissions from the start. Without escalation, CMake/Ninja can appear to freeze or fail with `Permission denied` while touching generated files under `Engine/Build`, especially `_deps/*-subbuild` Ninja files.

Do not first try the non-escalated command for this repository. Request escalation immediately for:

- `cmake -G "Ninja" -S . -B Engine/Build`
- `cmake --build Engine/Build`
- `ctest --test-dir Engine/Build --output-on-failure`

## Standard Workflow

Run commands from the repository root.

1. Enter the Visual Studio x64 developer environment:

```cmd
"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
```

If that path does not exist, locate Visual Studio with:

```powershell
& "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
```

2. Configure:

```cmd
cmake -G "Ninja" -S . -B Engine/Build
```

3. Build:

```cmd
cmake --build Engine/Build
```

4. Run tests when needed:

```cmd
ctest --test-dir Engine/Build --output-on-failure
```

For one-shot automation from PowerShell/Codex, run commands through `cmd.exe` so the `VsDevCmd.bat` environment applies to the CMake process:

```powershell
cmd.exe /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"" -arch=x64 -host_arch=x64 && cmake -G ""Ninja"" -S . -B Engine/Build"
cmd.exe /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"" -arch=x64 -host_arch=x64 && cmake --build Engine/Build"
cmd.exe /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"" -arch=x64 -host_arch=x64 && ctest --test-dir Engine/Build --output-on-failure"
```

## Failure Handling

- If configure fails because `Ninja` or a compiler is missing, report the missing tool and do not switch generators unless the user asks.
- If a non-escalated Codex run hangs, stop any leftover `cmake` / `ninja` processes from that run before retrying with escalation.
- If CMake selects `C:/Program Files/LLVM/bin/clang++.exe`, the shell is not equivalent to Visual Studio. Reconfigure from `VsDevCmd.bat` so `cl.exe` is selected.
- If `Engine/Build/CMakeCache.txt` already contains LLVM/Clang compilers, remove the stale generated cache/build tree only with user approval, then configure again from `VsDevCmd.bat`.
- If build fails, quote the first relevant compiler or linker error and inspect the referenced source files before editing.
- If tests fail, use `--output-on-failure` output and inspect the failing test source before editing.
- After any C++ change, run at least `cmake --build Engine/Build`. Run `ctest --test-dir Engine/Build --output-on-failure` when test targets are affected or when the user requests full verification.

## Working Directory Hygiene

- Treat `Engine/Build` and generated dependency folders as build output.
- Do not delete existing build directories without explicit user approval.
- Ignore unrelated untracked or modified files unless they directly affect configure, build, or test results.
