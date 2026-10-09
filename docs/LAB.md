# Laboratory guide

## Recommended topology

Use two virtual machines on an isolated network.

```text
VM A                           VM B
IPv6-FuzzLab                   Test IPv6 stack
     |                              |
     +---------- isolated ----------+
```

Do not bridge the test network to the public Internet unless the experiment explicitly requires it and authorization is established.

## Test procedure

1. Build IPv6-FuzzLab.
2. Run `--self-test`.
3. Test deterministic fixtures.
4. Capture logs.
5. Add one malformed case at a time.
6. Record operating system and kernel versions.
7. Repeat under ASan/UBSan where possible.
8. Convert confirmed findings into regression tests.

## Reproducibility record

Record:

```text
Date:
Host OS:
Kernel:
Compiler:
Compiler version:
CMake version:
IPv6-FuzzLab commit:
Test fixture:
Result:
Relevant standard/reference:
```

## Recommended research progression

Start with offline parser testing.

Then move to PCAP analysis.

Only after that should a controlled live-stack experiment be considered.
