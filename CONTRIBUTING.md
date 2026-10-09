# Contributing

Contributions are welcome, especially in the following areas:

- IPv6 parser correctness
- Extension Header support
- ICMPv6 parsing
- PCAP analysis
- fuzzing infrastructure
- regression fixtures
- documentation
- portability

Please keep changes reproducible and testable.

Avoid adding unrestricted network-flooding functionality. The project's purpose is protocol robustness and defensive research.

Before opening a pull request:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/ipv6-fuzzlab --self-test
```
