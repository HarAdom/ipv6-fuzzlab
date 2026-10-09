# Threat model

## Assets

The main assets considered by the project are:

- parser memory safety;
- CPU and memory consumption of the parser;
- correctness of packet classification;
- integrity of experimental results;
- reproducibility of test cases.

## Untrusted input

The parser assumes that every byte supplied to it is untrusted.

Input can come from:

- PCAP captures;
- files;
- fuzzers;
- generated test fixtures;
- application buffers.

## Security properties

A malformed input should not cause:

- an out-of-bounds read;
- an out-of-bounds write;
- integer overflow leading to an invalid allocation or access;
- infinite parser loops;
- uncontrolled recursion;
- undefined behavior.

## Out of scope

The parser is not a network firewall and does not attempt to determine whether an IPv6 packet is malicious based solely on its contents.

A security classification should be based on observable behavior, protocol requirements and documented evidence.
