# Copilot Instructions

## Project Overview

**Modbus TCP Master** is a C++/CLI dynamic link library (DLL) that implements the Modbus TCP protocol for communicating with Yaskawa MP2300S motion controllers. It writes multiple holding registers (function code 16) over TCP/IP on port 502. The repository also includes a console test application (`Main.cpp`) that exercises the library interactively.

## Repository Structure

```
modbus-tcpmaster/
├── .github/
│   ├── copilot-instructions.md   # This file
│   ├── skills/
│   │   └── code-review/
│   │       └── SKILL.md          # Copilot code-review skill for this repo
│   └── workflows/
│       ├── checks.yaml           # PR gate — rebase and changelog checks (reusable workflow)
│       ├── release.yaml          # Automated release pipeline (reusable workflow)
│       ├── claude-review.yaml    # Claude automated PR review (reusable workflow)
│       └── claude-mention.yaml   # @claude mention responder (reusable workflow)
├── .changes/
│   └── unreleased/               # chlog fragments awaiting the next release
├── .gitignore
├── CLAUDE.md                     # Guidance for Claude Code sessions
├── MP2300SController.h           # Public API: MP2300SController ref class declaration
├── MP2300SController.cpp         # Modbus TCP protocol implementation
├── Main.cpp                      # Interactive console test harness
├── stdafx.h                      # Precompiled header
├── stdafx.cpp                    # Precompiled header source
├── AssemblyInfo.cpp              # .NET assembly metadata (version 1.0.*)
├── resource.h                    # Resource definitions
├── app.rc                        # Application resource script
├── app.ico                       # Application icon
├── app.aps                       # Visual Studio resource editor cache
├── ModbusTCPMaster.sln           # Visual Studio solution file
├── ModbusTCPMaster.vcxproj       # Visual Studio project file (C++/CLI DLL)
├── ModbusTCPMaster.vcxproj.filters  # Source file filters for Solution Explorer
├── ModbusTCPMaster.vcxproj.user     # Per-user project settings
├── CHANGELOG.md                  # Generated from chlog fragments — never hand-edited
├── CONTRIBUTING.md
├── LICENSE
└── README.md
```

## Technology Stack

| Layer | Technology |
|---|---|
| Language | C++/CLI (managed Visual C++ with .NET extensions) |
| Runtime | .NET Framework 4.0+ |
| Networking | `System::Net::Sockets::Socket` (TCP) |
| Build system | MSBuild / Visual Studio 2017+ |
| Protocol | Modbus TCP (MBAP framing, function code 16) |
| Target device | Yaskawa MP2300S motion controller |

## Architecture and Design Patterns

- **`MP2300SController`** – a single `public ref class` (managed C++ class) that encapsulates the full Modbus TCP lifecycle: connect → write registers → validate response → disconnect.
- **Frame construction** – `WriteMultipleRegistersMsg()` builds a complete MBAP + PDU byte array from scratch (transaction ID, protocol ID, length, unit ID, function code, start address, register count, byte count, data).
- **Byte-order conversion** – `ToByteArray(DINT)` swaps bytes from little-endian host order to the big-endian Modbus wire format for 32-bit DINT values split across two 16-bit registers.
- **Error handling** – `CheckValidate()` parses Modbus exception responses (function code OR'd with 0x80) and throws `System::NotImplementedException` with a descriptive message for codes 0x01–0x0B.
- **Type aliases** defined in `MP2300SController.h`:
  - `BYTE` = `unsigned char`
  - `WORD` = `unsigned short`
  - `INT`  = `signed short`
  - `DINT` = `signed int`

## Key Constants

| Constant | Value | Meaning |
|---|---|---|
| `READ_BUFFER_SIZE` | 2048 | Receive buffer size in bytes |
| `WRITE_BUFFER_SIZE` | 2048 | Send buffer size in bytes |
| Default port | 502 | Standard Modbus TCP port |
| Slave address | 1 | Hard-coded unit identifier |
| Function code | 16 (0x10) | Write Multiple Registers |

## CI / Release

A `release.yaml` workflow runs on every push to `main` and delegates to a reusable pipeline in `rios0rios0/pipelines`. A `checks.yaml` workflow runs on every pull request targeting `main` and delegates to the shared `checks.yaml` in `rios0rios0/pipelines` — the rebase and changelog-fragment gate. Two event-driven workflows — `claude-review.yaml` (automated pull-request review) and `claude-mention.yaml` (`@claude` mention responder) — call the matching reusable workflows in `rios0rios0/pipelines` and authenticate with the `CLAUDE_CODE_OAUTH_TOKEN` secret. There is no build or compile CI — builds are performed locally with Visual Studio or MSBuild on Windows.

## Build Commands

### Visual Studio (recommended)

1. Open `ModbusTCPMaster.sln` in Visual Studio 2017 or later.
2. Select **Debug** or **Release** configuration and **Win32** or **x64** platform.
3. Press `Ctrl+Shift+B` (or **Build › Build Solution**).
4. Output artifacts are placed in `Debug\` or `Release\` (gitignored).

### MSBuild (command line)

```bash
# Debug build (Win32)
msbuild ModbusTCPMaster.sln /p:Configuration=Debug /p:Platform=Win32

# Release build (x64)
msbuild ModbusTCPMaster.sln /p:Configuration=Release /p:Platform=x64
```

Typical build time: **< 10 seconds** on a modern machine.

### Prerequisites

- Visual Studio 2017 or later with:
  - **Desktop development with C++** workload
  - **C++/CLI support** individual component
  - **.NET desktop development** workload
- .NET Framework 4.0+ SDK

## Running the Console Test Application

```bash
Debug\ModbusTCPMaster.exe
```

The application connects to `127.0.0.1:502`, then loops prompting for four 32-bit integer values and writes them to registers 0–7 on the slave device. Requires a live Modbus TCP slave or a simulator (e.g., Modbus Slave by Witte Software).

## Testing

There is no automated test suite. Validation is done manually:

1. Start a Modbus TCP simulator listening on port 502.
2. Run the console application and enter test values.
3. Verify the simulator shows the expected register values written.
4. Verify exception responses (simulate slave errors) cause the expected `NotImplementedException` messages.

## Coding Conventions

- Use the type aliases (`BYTE`, `WORD`, `INT`, `DINT`) consistently — do **not** use raw `unsigned char` etc. in new code.
- Managed heap allocations use `gcnew`; avoid mixing with native `new` for managed types.
- Keep frame construction and protocol logic inside `MP2300SController.cpp`; `Main.cpp` is only a test harness.
- Follow the commit and branching conventions from the [Development Guide](https://github.com/rios0rios0/guide/wiki): use `feat/`, `fix/`, `chore/` prefixes and open PRs against `main`.
- Error conditions must throw a managed exception (`System::Exception` subclass) with a human-readable message including the Modbus error code in both decimal and hex.

## Development Workflow

1. Fork the repository and create a feature branch:
   ```bash
   git checkout -b feat/my-change
   ```
2. Open `ModbusTCPMaster.sln` in Visual Studio and make your changes.
3. Build and test manually (see above).
4. Commit with a conventional commit message, e.g. `feat: add read multiple registers support`.
5. Open a pull request against `main`.

## Common Tasks

| Task | How |
|---|---|
| Add a new Modbus function code | Add a method to `MP2300SController` following the pattern of `WriteML`/`WriteMultipleRegistersMsg` |
| Support a new target device | Subclass or wrap `MP2300SController`; override `Connect` parameters (IP, port, slave address) |
| Change buffer size | Update `READ_BUFFER_SIZE` / `WRITE_BUFFER_SIZE` constants in `MP2300SController.h` |
| Handle a new Modbus exception code | Add a `case` to the `CheckValidate` switch statement |
| Build for x64 | Use `Platform=x64` in MSBuild or select the x64 platform in Visual Studio |

## Troubleshooting

- **`SocketException: Connection refused`** – No Modbus TCP slave is running on the target IP/port. Start a simulator or check the device IP.
- **`NotImplementedException` with error code** – The slave returned a Modbus exception response. Check the register address, data length, and slave configuration.
- **Build error: `error C4950` or managed/unmanaged mismatch** – Ensure the **C++/CLI support** component is installed and the project is configured as `/clr` (check project properties › General › Common Language Runtime Support).
- **Garbled characters in source comments** – The source files use Windows-1252 encoding; open them with the correct encoding in your editor to preserve the original comments.

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
