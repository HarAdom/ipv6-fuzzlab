# Examples

## Basic parser test

```bash
./build/ipv6-fuzzlab --fixture basic
```

## Malformed length

```bash
./build/ipv6-fuzzlab --fixture malformed-length
```

This should be rejected because the IPv6 Payload Length is larger than the supplied buffer.

## Truncated extension header

```bash
./build/ipv6-fuzzlab --fixture truncated-extension
```

This should be rejected because the advertised extension header does not fit in the supplied packet.

## Hex input

A minimal IPv6 packet can also be supplied as hexadecimal bytes:

```bash
./build/ipv6-fuzzlab --hex HEXSTRING
```

No network transmission occurs.
