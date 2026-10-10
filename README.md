# IPv6-FuzzLab

[![DOI](https://zenodo.org/badge/1411625656.svg)](https://doi.org/10.5281/zenodo.23260499)

**IPv6-FuzzLab** is a modern, defensive-oriented IPv6 protocol robustness and security research toolkit inspired by the historical `ipv6fuck v1.0alfa` source code from around 2002.

The original program is preserved as historical context. This project does **not** reproduce an unrestricted packet-flooding/DoS mode. Instead, it focuses on:

- IPv6 packet parsing and validation
- Extension Header inspection
- ICMPv6 structure validation
- Fragmentation analysis
- malformed-input handling
- deterministic local fuzz cases
- PCAP-oriented research workflows
- regression tests
- AddressSanitizer / UndefinedBehaviorSanitizer support
- documenting how IPv6 security considerations evolved from early implementations to modern stacks

> **Research and laboratory use only.**
>
> Run packet-processing experiments only against systems, interfaces, captures and virtual networks that you own or are explicitly authorized to test.

---

## 1. Project background

`ipv6fuck.c` is a small C program identified in its source as:

```text
ipv6fuck v1.0alfa - (c)2002 schizoid
(deriving from icmp6fuck v3)
```

The source describes itself as a study program and contains compilation instructions for Linux, BSD and Solaris. It manually constructs IPv4/IPv6-related headers and uses raw sockets. It also contains options controlling payload size, timing, process multiplication and duration.

The historical source is valuable today for a different reason: it is a compact snapshot of how low-level IPv6 packet experimentation was approached in the early 2000s.

The modern project therefore asks a broader research question:

> **How should a packet-generation experiment from the early IPv6 era be redesigned as a safe, testable and maintainable IPv6 robustness laboratory for modern systems?**

The answer implemented here is deliberately defensive: parsing, validation, controlled malformed inputs, reproducibility and observability take priority over traffic volume.

---

## 2. Goals

### Primary goals

1. Preserve the historical context of the original program.
2. Build a clean IPv6 packet parser in portable C.
3. Validate IPv6 base headers.
4. Walk and inspect selected IPv6 Extension Headers.
5. Detect structurally suspicious or invalid packet layouts.
6. Parse ICMPv6 headers without transmitting traffic.
7. Provide deterministic malformed test cases.
8. Make the project suitable for sanitizers and fuzzing.
9. Provide a foundation for future PCAP analysis.
10. Produce research material that can later be turned into a technical paper/PDF.

### Non-goals

This repository intentionally does not provide:

- unrestricted IPv6 flooding;
- fork-based traffic multiplication;
- an Internet-targeting DoS mode;
- source-address spoofing infrastructure;
- a replacement for a professional network scanner;
- instructions for attacking third-party systems.

The historical source may contain such mechanisms, but the modern implementation intentionally does not carry them forward.

---

# 3. Repository structure

```text
ipv6-fuzzlab/
├── CMakeLists.txt
├── LICENSE
├── README.md
├── SECURITY.md
├── CONTRIBUTING.md
├── CHANGELOG.md
├── .gitignore
├── include/
│   └── ipv6_fuzzlab/
│       ├── ipv6.h
│       └── parser.h
├── src/
│   ├── main.c
│   └── parser.c
├── tests/
│   ├── test_parser.c
│   └── fixtures/
│       └── README.md
├── docs/
│   ├── HISTORY.md
│   ├── IPV6_SECURITY.md
│   ├── LAB.md
│   └── RESEARCH.md
├── examples/
│   └── README.md
└── legacy/
    └── README.md
```

---

# 4. Build

## Requirements

A C11-capable compiler and CMake 3.16 or newer are recommended.

Linux/macOS:

```bash
cmake -S . -B build
cmake --build build
```

Run:

```bash
./build/ipv6-fuzzlab --help
```

Run the built-in self-test:

```bash
./build/ipv6-fuzzlab --self-test
```

---

# 5. Command line interface

The first implementation intentionally keeps the CLI small.

```text
ipv6-fuzzlab [options]

Options:
  --help
      Show help.

  --version
      Show version.

  --self-test
      Execute parser regression tests.

  --hex HEXSTRING
      Parse one IPv6 packet represented as hexadecimal bytes.

  --file FILE
      Parse an IPv6 packet from a binary file.

  --fixture NAME
      Run one deterministic built-in test fixture.

Examples:

  ipv6-fuzzlab --self-test

  ipv6-fuzzlab --hex \
    6000000000083a4020010db800000000000000000000000120010db8000000000000000000000002

  ipv6-fuzzlab --fixture basic

  ipv6-fuzzlab --fixture malformed-length
```

The parser operates on supplied bytes and does not create a network flood.


---

# 6. What the parser checks

The IPv6 base header is 40 bytes.

The parser checks, among other things:

- minimum packet size;
- IPv6 version;
- Payload Length consistency;
- Next Header;
- Hop Limit;
- source address;
- destination address.

For extension headers, the project provides a framework for inspecting:

- Hop-by-Hop Options;
- Routing Header;
- Fragment Header;
- Destination Options;
- AH/ESP classification;
- upper-layer protocols.

The parser is deliberately strict about buffer boundaries. A malformed packet must produce a diagnostic result rather than an out-of-bounds memory access.

---

# 7. Why this matters

Old packet-processing programs frequently assumed that fields contained sensible values. Modern defensive tooling should make the opposite assumption:

> **Every byte received from a network capture is untrusted input.**

A parser should therefore be able to answer questions such as:

```text
Is the packet long enough?

Does Payload Length agree with the supplied buffer?

Does an Extension Header fit completely inside the packet?

Does a Fragment Header have a structurally valid layout?

Does the Next Header chain terminate correctly?

Can the packet be rejected without reading outside the buffer?
```

This is particularly important for security research because parser bugs can themselves become security vulnerabilities.

---

# 8. Historical comparison

The original source uses raw sockets and constructs packet headers manually. It also includes options such as payload size, microsecond delay, process count, multiplication and execution time.

The modern design removes the unrestricted traffic-generation path.

| Historical concept | IPv6-FuzzLab |
|---|---|
| Raw packet construction | Safe byte-buffer parser |
| Process multiplication | Deterministic test execution |
| Traffic rate | Test-case count |
| Infinite sending | No network flood mode |
| Random packet fields | Reproducible fixtures |
| Manual memory assumptions | Explicit length checks |
| Platform-specific networking | Portable parser core |
| Informal testing | Regression tests |
| Limited diagnostics | Structured parser results |
| One-off experiment | Reusable research project |

The objective is not to pretend the original program never existed. The objective is to preserve its historical value while changing the engineering model.

---

# 9. Research methodology

A useful experiment should be reproducible.

Each test case should document:

1. Input bytes.
2. Expected parser result.
3. Expected reason for rejection/acceptance.
4. Software version.
5. Compiler.
6. Operating system.
7. Sanitizer configuration.
8. Test date.
9. Relevant RFC or technical reference.

For example:

```text
Case: malformed-length

Input:
    IPv6 header advertises an 80-byte payload
    but only 8 payload bytes are supplied.

Expected:
    REJECT

Reason:
    supplied buffer is shorter than the declared packet length.
```

This turns a packet experiment into an auditable research artifact.

---

# 10. Fuzzing philosophy

The project is designed so that the parser can eventually be connected to a coverage-guided fuzzer.

A useful fuzzing loop is:

```text
random / mutated bytes
        |
        v
   IPv6 parser
        |
   +----+----+
   |         |
 valid     invalid
   |         |
 diagnostics  rejection
        |
        v
 crash / sanitizer report
        |
        v
 regression fixture
```

The most important property is that malformed input must remain **data**, not become an uncontrolled network operation.

---

# 11. Sanitizers

For Clang or GCC, an instrumented build can be created with:

```bash
cmake -S . -B build-asan \
  -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer -g"

cmake --build build-asan

./build-asan/ipv6-fuzzlab --self-test
```

A future CI configuration can run the same test suite automatically.

---

# 12. Security model

IPv6-FuzzLab follows three principles.

### Principle 1 — Parse locally

Malformed data should normally be processed from memory, files or captures.

### Principle 2 — Fail closed

When a packet cannot be safely parsed, the parser should reject it rather than guessing.

### Principle 3 — Reproduce before transmitting

If a research finding eventually requires live-network validation, reproduce it first using a private lab topology.

A minimal lab can consist of:

```text
+------------------+        +------------------+
| Linux VM         |        | Linux VM         |
| IPv6-FuzzLab     | <----> | Test stack       |
| parser/fuzzer    |        | isolated network |
+------------------+        +------------------+
          \                       /
           \_____________________/
                 no Internet
```

---

# 13. Suggested research questions

The repository can support a future paper covering questions such as:

### Q1
How much has IPv6 packet validation changed since the early 2000s?

### Q2
Which classes of malformed IPv6 input are rejected at the parser boundary?

### Q3
How do different operating systems represent and reject unusual Extension Header chains?

### Q4
What security lessons can be learned from early IPv6 packet-generation tools?

### Q5
Does stricter parsing reduce the observable attack surface?

### Q6
How does fuzzing improve confidence compared with manually selected malformed packets?

---

# 14. Historical significance

The original `ipv6fuck.c` is small, but it illustrates several characteristics of early low-level networking research:

- direct manipulation of protocol headers;
- dependence on platform-specific structures;
- raw socket programming;
- hand-written checksums;
- limited input validation;
- process-based traffic multiplication;
- incomplete portability.

These characteristics make the source useful as a historical artifact.

The purpose of this project is not to judge the original code by modern standards, but to demonstrate how the same research topic can be approached with modern software-engineering practices.

---

# 15. Future roadmap

## v0.1

- [x] IPv6 base-header parser
- [x] IPv6 address formatting
- [x] Payload-length validation
- [x] basic CLI
- [x] deterministic fixtures
- [x] regression tests
- [x] CMake build

## v0.2

- [ ] complete Extension Header walker
- [ ] Fragment Header analysis
- [ ] ICMPv6 parser
- [ ] structured diagnostics
- [ ] JSON output

## v0.3

- [ ] PCAP reader
- [ ] packet statistics
- [ ] malformed-packet corpus
- [ ] reproducible fuzz seeds

## v0.4

- [ ] libFuzzer integration
- [ ] AFL++ integration
- [ ] sanitizer CI
- [ ] GitHub Actions

## v1.0

- [ ] stable parser API
- [ ] comprehensive IPv6 test corpus
- [ ] research report
- [ ] benchmark results
- [ ] cross-platform validation

---

# 16. Research ethics

IPv6-FuzzLab is intended for education, defensive research and authorized testing.

Do not use it to generate disruptive traffic against systems that you do not own or have explicit authorization to test.

For experiments involving real network stacks, use:

- isolated virtual machines;
- private IPv6 networks;
- network namespaces;
- containers;
- dedicated test equipment;
- explicit written authorization.

---

# 17. License

The modern code in this repository is released under the MIT License unless otherwise stated.

The historical source is retained separately for research/reference purposes. Its original copyright and attribution notices should not be removed or rewritten.

---

# 18. Citation

If this project is used in academic or technical work, cite the repository and identify the historical source separately.

Suggested citation:

```text
IPv6-FuzzLab: IPv6 Protocol Robustness and Security Research Toolkit.
Historical inspiration: ipv6fuck v1.0alfa, circa 2002.
```

---

# 19. Final note

The most important difference between this project and its historical inspiration is not the programming language or compiler.

It is the experimental model.

The historical approach was primarily concerned with constructing and transmitting unusual IPv6 traffic.

IPv6-FuzzLab instead treats the packet as a research specimen:

```text
capture
   ↓
parse
   ↓
validate
   ↓
classify
   ↓
reproduce
   ↓
fuzz
   ↓
measure
   ↓
document
```

That makes the project useful not only as a reconstruction of an old networking experiment, but as a platform for studying how robust modern IPv6 implementations are when confronted with malformed protocol input.

---

## Status

**Early research / development release**

The API and command-line interface may change before version 1.0.

---

# 20. Current parser limitations

The 0.1 parser is intentionally a research foundation rather than a complete IPv6 conformance checker.

In particular, the current release does not yet provide:

- complete ICMPv6 semantic validation;
- complete option parsing for every Extension Header;
- full Routing Header semantics;
- IPv6 reassembly;
- PCAP decoding;
- TCP/UDP checksum verification;
- JSON/SARIF output;
- full RFC conformance testing.

These are explicit roadmap items rather than undocumented gaps.

ESP is treated as an upper boundary because its protected contents do not expose a simple clear-text next-header chain to this parser. This is preferable to incorrectly guessing the structure of encrypted/authenticated payloads.

---

# 21. Standards-based research

The project should be evaluated against the IPv6 standards rather than against assumptions made by the historical program.

The central specification is RFC 8200, which defines the IPv6 base header and Extension Header architecture. Security and operational research should also consider RFC 4443, RFC 5722, RFC 7112 and RFC 9099.

See [`docs/RFC_REFERENCES.md`](docs/RFC_REFERENCES.md).

---

# 22. Fuzzing harness

A minimal libFuzzer-compatible harness is included in `fuzz/fuzz_parser.c`.

With Clang/libFuzzer available, a local build can be created along these lines:

```bash
clang -std=c11 -fsanitize=fuzzer,address,undefined \\
  -Iinclude \\
  fuzz/fuzz_parser.c src/parser.c \\
  -o ipv6-fuzzlab-fuzzer

./ipv6-fuzzlab-fuzzer corpus/
```

The harness only feeds bytes to the parser. It does not transmit packets on the network.

Every crash found by a sanitizer should become a minimal regression fixture before being considered resolved.

---

# 23. Recommended evidence format

For future experiments, store results in a machine-readable form containing at least:

```text
case_id
input_sha256
parser_status
extension_count
compiler
compiler_version
os
kernel
project_commit
sanitizer
result
reference
```

This makes it possible to compare results across releases without relying on screenshots or manually copied terminal output.

---

# 24. Research publication plan

A future PDF/research paper should not simply reproduce the README.

A stronger structure is:

1. Historical background.
2. Source-code analysis of the 2002 program.
3. Threat model.
4. Modern redesign.
5. Standards review.
6. Test corpus design.
7. Fuzzing methodology.
8. Experimental environment.
9. Results.
10. Reproducibility package.
11. Limitations.
12. Conclusions.

The Git repository should remain the primary source of code and raw experimental artifacts, while the PDF explains the methodology and interprets the results.
