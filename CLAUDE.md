# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

C++/CLI DLL implementing Modbus TCP (function code 16 — Write Multiple Registers) for Yaskawa MP2300S motion controllers. Two source files: `MP2300SController.h/.cpp` (library) and `Main.cpp` (interactive console test harness).

## Build

Windows-only. Requires Visual Studio 2017+ with the **C++/CLI support** component, **Desktop development with C++**, and **.NET desktop development** workloads. .NET Framework 4.0+.

```bash
msbuild ModbusTCPMaster.sln /p:Configuration=Debug /p:Platform=Win32
msbuild ModbusTCPMaster.sln /p:Configuration=Release /p:Platform=x64
```

No automated test suite. Manual testing against a Modbus TCP simulator on port 502.

## Conventions

- Use the type aliases (`BYTE`, `WORD`, `INT`, `DINT`) defined in `MP2300SController.h` — not raw C++ types.
- Managed allocations use `gcnew`; do not mix with native `new` for managed types.
- Protocol and frame logic stays in `MP2300SController.cpp`; `Main.cpp` is only the test harness.
- Error conditions throw `System::Exception` subclasses with messages including the Modbus error code in both decimal and hex.
- Conventional commits with `feat/`, `fix/`, `chore/` branch prefixes. See the [Development Guide](https://github.com/rios0rios0/guide/wiki).

<!-- chlog:start -->
## Changelog (chlog) — MANDATORY

If the repository you are working in uses chlog (a `.chlog.yaml` or `.chlog.yml`
config file, or a `.changes/` directory, exists at the project root), the
following is binding and ALWAYS applies: whenever you make ANY change, you MUST
create a changelog fragment as part of the same change — automatically, without
being asked, before committing.

- Do NOT edit CHANGELOG.md directly; it is generated from fragments.
- Create the fragment with:
  `chlog new --kind <Kind> --body '<past-tense description>'`
- Write an apostrophe inside the single-quoted body as `'\''`.
- Valid kinds: Added, Changed, Deprecated, Removed, Fixed, Security
- Choose the kind that best matches the change (e.g., new feature → Added,
  bug fix → Fixed, behavior change → Changed, removal → Removed, security fix → Security).
- If the change is backward-INCOMPATIBLE with the public API (a breaking
  change), you MUST add the `--breaking` flag:
  `chlog new --kind <Kind> --breaking --body '<past-tense description>'`.
  This is the ONLY thing that triggers a major version bump — the kind alone
  never does (per SemVer, major = incompatible change). When unsure whether a
  change breaks compatibility, ask the user instead of guessing.
- Fragments are YAML files in `.changes/unreleased/`; stage them with your commit.
- `chlog check` fails the build when a fragment is missing — never skip it.
<!-- chlog:end -->
