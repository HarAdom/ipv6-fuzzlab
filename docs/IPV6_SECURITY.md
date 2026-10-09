# IPv6 security research notes

This document is a starting point for a future literature review.

## IPv6 is not simply IPv4 with longer addresses

IPv6 introduces a different header structure, extension headers and protocol interactions.

A robust implementation must consider:

- base header parsing;
- extension-header chains;
- fragmentation;
- ICMPv6;
- Neighbor Discovery;
- Path MTU Discovery;
- routing and destination options;
- resource consumption.

## Extension Header chains

A parser should never assume that an Extension Header is complete merely because its type is recognized.

Every length calculation must be checked against:

1. the supplied buffer;
2. the IPv6 Payload Length;
3. the remaining bytes.

## Fragmentation

Fragment handling deserves special attention because fragments can create state and reassembly work.

Research should distinguish:

- syntactically invalid fragments;
- overlapping fragments;
- duplicate fragments;
- incomplete reassembly;
- excessive fragment chains;
- resource exhaustion.

## ICMPv6

ICMPv6 is part of normal IPv6 operation. Security testing therefore needs to distinguish malformed or abusive ICMPv6 traffic from legitimate control-plane messages.

## Defensive measurement

Useful measurements include:

- packets rejected;
- packets accepted;
- parse time;
- memory consumption;
- CPU consumption;
- reassembly state;
- error categories.

The project should prefer reproducible local measurements over uncontrolled network traffic.
