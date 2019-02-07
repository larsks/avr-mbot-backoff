# Unit tests

To run the unit tests:

    make

For more verbose output:

    make VERBOSE=1

To run a specific unit test:

    make test_motor.ok

## Theory of operation

The `run_tests.py` script loads compiled binaries into a simulator controlled by [pysimavr][]. Tests indicate completion by setting bit 7 of `GPIOR0`.  Tests indicate success by setting bit 1 of `GPIOR0`.

[pysimavr]: https://github.com/ponty/pysimavr

## Test limitations

Support for timers and interrupts (in some configurations) appears to be lacking in both [simavr][] and [simulavr][], making it hard to write tests for modules like `distance.c`, which rely heavily on periodic timer interrupts to function.

[simavr]: https://github.com/buserror/simavr
[simulavr]: https://www.nongnu.org/simulavr/
