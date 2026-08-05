# Static and shared library packaging

This exercise packages a small merge-sort program into both a static library (`.a`) and shared library (`.so`) and links separate executables against each form. The Makefile demonstrates object compilation, position-independent code, archive creation with `ar`, shared-object linking, and runtime library lookup.

## Attribution boundary

The original `main.cpp` states that the merge-sort example source came from an external tutorial. The portfolio therefore presents the student's contribution as **build-system and library-packaging work**, not authorship of the sorting algorithm.

## Original files

The unchanged source, headers, Makefile, and report are in [`original/`](original/). Generated `.a`, `.so`, object, and executable files were excluded.
