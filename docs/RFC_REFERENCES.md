# Standards and references

The project should distinguish between three kinds of material:

1. **Normative standards** — what an implementation is expected to do.
2. **Operational/security guidance** — practical deployment considerations.
3. **Historical material** — what an old implementation actually did.

## Core references

- **RFC 8200 — Internet Protocol, Version 6 (IPv6) Specification**
  Defines the IPv6 base header and the initial Extension Header architecture.
- **RFC 4443 — ICMPv6**
  Defines ICMPv6 messages and processing.
- **RFC 5722 — Handling of Overlapping IPv6 Fragments**
  Addresses overlapping IPv6 fragments and their security implications.
- **RFC 7112 — Implications of Oversized IPv6 Header Chains**
  Addresses the interaction between fragmentation and long IPv6 header chains.
- **RFC 9099 — Operational Security Considerations for IPv6 Networks**
  Provides operational security guidance, including Extension Header and fragmentation considerations.

## Why these references matter to IPv6-FuzzLab

The parser should eventually map diagnostics to standards-based observations. For example:

```text
parser diagnostic
      |
      +--> structural rule
      |
      +--> relevant RFC section
      |
      +--> reproducible fixture
      |
      +--> test result
```

This is preferable to labelling a packet simply as "malicious". A malformed packet may be rejected for protocol correctness reasons without constituting a vulnerability.

## Current limitation

The project does not yet implement a complete RFC conformance engine. The references are therefore research guidance, not a claim that every rule in these documents is currently enforced by the parser.
