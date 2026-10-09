# Historical background

## The original program

The supplied historical source identifies itself as:

`ipv6fuck v1.0alfa - (c)2002 schizoid`

and says that it derives from `icmp6fuck v3`.

The source describes the program as a study tool and contains compilation examples for Linux, BSD and Solaris.

Its architecture includes:

- raw sockets;
- manually constructed IPv4 and IPv6 headers;
- ICMPv6/TCP/UDP branches;
- custom checksum routines;
- randomized fields;
- `fork()`-based process creation;
- timing and multiplication parameters.

## Why preserve it?

Small historical programs can be useful primary-source material. They reveal:

- assumptions made by programmers of the period;
- the APIs available at the time;
- portability limitations;
- how protocol experimentation was performed;
- which parts of packet construction were considered important.

The modern project keeps that historical context while deliberately avoiding reproduction of unrestricted packet flooding.

## Source preservation

The original source should be stored separately as historical material, with its original attribution and notices intact.
