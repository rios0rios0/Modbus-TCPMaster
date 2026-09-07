---
name: code-review
description: "Review pull requests and diffs in modbus-tcpmaster — the C++/CLI Modbus TCP master for MP2300S controllers — against the rios0rios0/guide standards, with extra weight on frame construction, the fixed type aliases, managed/native memory boundaries, and exception reporting. Use when reviewing a PR, a branch, or staged changes here."
---

# Code review — `modbus-tcpmaster`

`modbus-tcpmaster` is a Visual Studio C++/CLI project implementing a Modbus TCP master that writes 32-bit values to registers on an MP2300S industrial controller. `MP2300SController.cpp` holds the protocol; `Main.cpp` is only a console test harness.

## When to use this skill

Use it whenever you are asked to review a pull request, a diff, a branch, or staged changes
in this repository — and before opening a pull request of your own, as a self-check. It is a
**review** skill: it produces findings, not commits.

## Source of truth

The canonical engineering standards live in the
**[rios0rios0/guide wiki](https://github.com/rios0rios0/guide/wiki)**. This file is a
repo-tailored index into that guide plus the rules that only apply here. Precedence, highest
first:

1. This repository's `.github/copilot-instructions.md`, `CLAUDE.md`, and `CONTRIBUTING.md` —
   they describe *this* codebase and its load-bearing invariants.
2. The **rios0rios0/guide** wiki — the shared standard.
3. General language idiom.

When the guide and a general convention disagree, the guide wins. When this file and the
guide disagree, the guide wins and this file should be corrected in the same pull request.

### Guide pages that apply here

| Topic | Page |
|-------|------|
| Mapper Design Pattern — replacing `switch`/`case` | [Mapper-Design-Pattern](https://github.com/rios0rios0/guide/wiki/Mapper-Design-Pattern) |
| Git Flow — branches, commits, SemVer, breaking changes | [Git-Flow](https://github.com/rios0rios0/guide/wiki/Git-Flow) |
| Documentation & Change Control — changelog and docs discipline | [Documentation-&-Change-Control](https://github.com/rios0rios0/guide/wiki/Documentation-&-Change-Control) |
| CHANGELOG Formatting — capitalisation and backticks | [CHANGELOG-Formatting](https://github.com/rios0rios0/guide/wiki/CHANGELOG-Formatting) |
| Security — OWASP checklist, secret hygiene, SAST | [Security](https://github.com/rios0rios0/guide/wiki/Security) |
| CI & CD — pipeline stages and the local quality gates | [CI-&-CD](https://github.com/rios0rios0/guide/wiki/CI-&-CD) |
| Code Style — baseline naming and the operations vocabulary | [Code-Style](https://github.com/rios0rios0/guide/wiki/Code-Style) |

## How to run the review

1. **Establish the range.** Resolve the default branch with
   `git symbolic-ref refs/remotes/origin/HEAD` (strip `refs/remotes/origin/`; fall back to `main`),
   then read the diff with `git diff <default>...HEAD` and the file list with
   `git diff <default>...HEAD --name-only`.
2. **Read whole files, not just hunks.** A hunk cannot show a layering violation, a missing
   test, or a duplicated helper. Open every changed file in full, plus the files it imports
   from the layer below.
3. **Check the change set as a unit** — not only the code. A change that alters behaviour,
   configuration, or architecture is incomplete without its changelog entry and its
   documentation update, and that omission is a finding in its own right.
4. **Map every finding to a rule.** Each finding must name the rule it breaks and link the
   guide page (or the repository file) that states it. A comment that cannot be traced to a
   rule is a suggestion, not a defect — label it as such.
5. **Report, do not rewrite.** Produce the review in the output format below. Only edit files
   when the request explicitly asks for fixes.

## What matters most in `modbus-tcpmaster`

These are the checks that catch real defects in this repository. Work through
them before the generic ones.

- **Protocol logic stays in `MP2300SController.cpp`.** `Main.cpp` is a test harness only — frame construction, transaction identifiers, and response parsing that leak into it will not be reachable from the DLL's real consumers.
- **This drives industrial equipment.** A malformed frame or an off-by-one register address writes to the wrong holding register on a live controller. Byte order (Modbus is big-endian on the wire), the register-count field, and the 32-bit value-splitting across two registers all need checking by hand. **Critical.**
- **Use the type aliases consistently** — `BYTE`, `WORD`, `INT`, `DINT` — and not raw `unsigned char` or `int` in new code. The aliases carry the controller's semantics.
- **Managed and native allocation must not be mixed.** Managed heap allocations use `gcnew`; a native `new` for a managed type, or the reverse, is a correctness defect in C++/CLI, not a style preference.
- **Error conditions throw a managed exception** (`System::Exception` subclass) with a human-readable message including the Modbus error code in **both decimal and hex**. A silent return, or a `NotImplementedException` that hides a real protocol error, leaves the operator with no diagnosis.
- **Socket lifetime and timeouts matter.** A blocking read without a timeout hangs the caller when the slave does not answer; a socket left open exhausts the connection slot on the controller.
- **The default endpoint is `127.0.0.1:502`** in the console harness. A real device address committed as a default points the tool at someone's plant equipment.
- **Prerequisites are specific**: Visual Studio 2017+ with the C++ desktop workload, C++/CLI support, and .NET Framework 4.0+. A project-file change must keep Debug and Release, Win32 and x64 all building.
- **Build output (`Debug/`, `Release/`) is gitignored** and must not be committed.

### Commands a reviewer should be able to quote

```bash
msbuild ModbusTCPMaster.sln /p:Configuration=Debug /p:Platform=Win32
msbuild ModbusTCPMaster.sln /p:Configuration=Release /p:Platform=x64
Debug\ModbusTCPMaster.exe        # console harness; needs a Modbus slave on :502
```

### Dispatch tables over `switch`

See [Mapper Design Pattern](https://github.com/rios0rios0/guide/wiki/Mapper-Design-Pattern). Two or three stable cases may stay a
`switch`. Four or more, or a set that grows with features, becomes a map from key to handler
so that adding a case is a new entry rather than an edit to the dispatcher. Flag new
`switch`/`if-else` chains that dispatch on a string or enum key.

## Tests

There is no automated test suite. Verification is building both configurations and running the console harness against a Modbus TCP simulator: enter test values, confirm the registers hold what was written, and simulate a slave error to confirm the exception message carries the code in decimal and hex.

## Documentation and change control

See [Documentation & Change Control](https://github.com/rios0rios0/guide/wiki/Documentation-&-Change-Control) and
[CHANGELOG Formatting](https://github.com/rios0rios0/guide/wiki/CHANGELOG-Formatting).

This repository uses **chlog fragments**. `CHANGELOG.md` is generated and is never edited by
hand.

- Every change ships a fragment created with `chlog new --kind <Kind> --body "…"`, staged in
  the **same commit** as the code. Kinds: `Added`, `Changed`, `Deprecated`, `Removed`,
  `Fixed`, `Security`.
- A backward-incompatible change to the public interface additionally carries `--breaking`.
  The kind alone never triggers a major bump.
- A hand-edited `CHANGELOG.md`, or a code change with no fragment under
  `.changes/unreleased/`, is a **Critical** finding — `chlog check` fails the build for it.
- Fragment bodies start with a lowercase verb in simple past tense, capitalise proper nouns
  (GitHub, Go, Docker), and wrap code identifiers and versions in backticks.
- `README.md` is updated whenever usage, setup, configuration, or architecture changes;
  `.github/copilot-instructions.md` and `CLAUDE.md` whenever the workflow, commands, or
  structure changes. Documentation and code ship in one commit.

## Git Flow and pull-request hygiene

See [Git Flow](https://github.com/rios0rios0/guide/wiki/Git-Flow) and [Merge Guide](https://github.com/rios0rios0/guide/wiki/Merge-Guide).

- Branch names are `feat/`, `fix/`, `refactor/`, `chore/`, `test/`, or `docs/` followed by a
  ticket ID or a short slug — `feat/TICKET-000`, `fix/input-mask`.
- Commit subjects are `type(SCOPE): message`: simple past tense (`added`, `fixed`, `changed`,
  `removed`), lowercase first word, no trailing period, code identifiers in backticks.
- Branches are synchronised with `git rebase`, never `git merge`. A merge commit from the
  default branch inside a feature branch is a finding.
- Breaking changes are flagged in **three** places: the commit footer
  (`**BREAKING CHANGE:** …`), the changelog, and the pull-request description. One or two of
  the three is not enough.
- Versions follow [SemVer](https://semver.org/): MAJOR for incompatible changes, MINOR for
  features, PATCH for fixes.

## Security

See [Security](https://github.com/rios0rios0/guide/wiki/Security).

- **No hard-coded secrets.** API keys, tokens, passwords, and private keys belong in
  environment variables or a secret manager — never in source, tests, fixtures, or the
  changelog. A secret that reaches a commit must be rotated, not merely deleted.
- **Never write a PEM header sentinel or a realistic key shape into a fixture**
  (GitHub `ghp_` prefixes, OpenAI `sk-` prefixes, AWS `AKIA` prefixes, Slack `xoxb` prefixes, JWT-shaped strings, or the dashed `BEGIN …` banners).
  Gitleaks matches the shape, not the value, so a placeholder that merely *looks* like a
  credential fails the pipeline. Use inert placeholders such as `fixture-token-placeholder`.
- **Suppressions must be justified.** Entries in `.gitleaksignore`, `.trivyignore`,
  `.semgrepignore`, or `.codeql-false-positives` need a fingerprint, a dated comment, and a
  reason. A suppression added to silence a real finding is a Critical.
- Validate and sanitise every external input; use parameterised queries; apply least
  privilege; keep secrets out of logs.
- Dependency manifest changes are reviewed for new transitive vulnerabilities. When a fix
  exists, bump the version rather than suppressing the finding.

## What not to flag

A review that raises noise gets ignored. Do not report these:

- The C++/CLI managed-native mixture — it is the project's chosen model.
- The absence of `make`, lint, or CI tooling.
- Anything the guide does not require and this file does not list, unless it is a genuine correctness or security defect — say so plainly and label it a Suggestion.

## Review output format

```
## Code review: <branch or PR>

### Critical (must fix before merge)
- `path/to/file.ext:LINE` — <what is wrong> — violates <rule> (<guide page or repo file>)

### Warning (should fix)
- `path/to/file.ext:LINE` — <what is wrong> — violates <rule>

### Suggestion (optional)
- `path/to/file.ext:LINE` — <improvement>

### Change-control checklist
- [ ] Changelog entry present for every behavioural change
- [ ] `README.md` updated if usage, setup, or architecture changed
- [ ] `.github/copilot-instructions.md` and `CLAUDE.md` updated if the workflow, commands, or structure changed
- [ ] Commit messages follow `type(SCOPE): message` in simple past tense
- [ ] Breaking changes flagged in the commit footer, the changelog, and the PR description

### Verdict: APPROVE / REQUEST CHANGES
<one paragraph: the blocking findings, or why the change is ready>
```

## Severity

| Severity       | Use for                                                                                                                            |
|----------------|------------------------------------------------------------------------------------------------------------------------------------|
| **Critical**   | Broken dependency direction, a leaked secret, an injection or authentication flaw, a missing changelog entry, a banned mock library, a load-bearing invariant broken, a test deleted rather than fixed. |
| **Warning**    | Naming that departs from the guide, a missing test for a new branch of logic, an unexplained magic value, a stale README or instructions file, a `switch` that should be a map. |
| **Suggestion** | Readability, consistency with neighbouring modules, and performance ideas that no rule mandates.                                     |

Rank findings most severe first, and state plainly when nothing blocks the merge — an empty
Critical section is a valid, useful review.
