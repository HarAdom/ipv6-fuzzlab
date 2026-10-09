# Research plan

## Working title

**From IPv6-Fuzzing Experiments to Modern Protocol Robustness Testing: A Historical and Technical Analysis**

## Abstract outline

This research investigates the evolution from early-2000s IPv6 packet-generation experiments to modern protocol robustness testing.

The work uses a historical C program as a primary artifact and develops a modern defensive parser and test framework inspired by it.

The research compares:

- implementation assumptions;
- packet construction;
- validation;
- memory safety;
- extension-header processing;
- fragmentation;
- reproducibility;
- fuzzing methodology.

## Proposed chapters

### 1. Introduction

Motivation and research questions.

### 2. Historical context

IPv6 adoption and early implementation maturity.

### 3. Analysis of the historical source

Architecture, APIs, portability and assumptions.

### 4. Modern IPv6 parsing

Base header and extension-header processing.

### 5. Security considerations

Malformed input, fragmentation and resource consumption.

### 6. Methodology

Deterministic fixtures, fuzzing and controlled experiments.

### 7. Results

Measurements collected across test systems.

### 8. Discussion

What changed, what remained difficult and why parser robustness matters.

### 9. Conclusion

Lessons for modern protocol implementations.

## Important methodological rule

Do not claim that an old implementation was vulnerable merely because a malformed packet can be constructed.

A vulnerability claim should identify:

- affected implementation;
- affected version;
- reproducible input;
- observable impact;
- conditions;
- remediation/status.

The project itself should separate historical observations, standards-based facts and experimental results.
