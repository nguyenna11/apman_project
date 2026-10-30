
# Working environment

Native on Linux/MacOS. MacOS/Windows users may/must install Docker.

Make sure you have like 15GB for the Docker image. Refer to the Docker documentation
for how to manage your containers and images.

Suppose your code is in `apman` directory and this is your current working directory.

Make sure that you have VSCode installed, and then:

```bash
apman$ code .
```

You should wait for VSCode to propose to `ReOpen in DevContainers` and accept.

The very first setup of VSCode is automatic but it takes several dozens minutes:
- Installing extensions
- Building the Debian 13 Docker image

Eventually, the project ends up being opened in a standard Debian 13 and you can
open a terminal in VSCode to work with it.

The remaining example commands in this file assume such a terminal with a Debian-like
distro (MacOS terminal should work reasonably too).

# Code bootstrap

Most of the tools you'll need have their config files tailored for the most seamless
experience. 

## Clean-up

For reasons that will become obvious, it will most often be a good idea to restart
the build process afresh:

```bash
apman$ make deep-clean
```

## Testing and debugging

Clean, compile and run all tests:
```bash
apman$ make deep-clean && make check
```

Clean, compile and run one particular test:
```bash
apman$ make deep-clean && make test/1-map@check
```

Regarding maps specifically, the first line of `include/ds/map.h` includes a switch
to turn advanced debugging on/off by un/commenting it.

Clean, compile and debug one particular test in the terminal:
```bash
apman$ make deep-clean && make test/1-map@debug
```

The graphical debugger of VSCode may also be used for better integration:
- Select the C file of the test so its pane gets the focus
- Start the Debugger


# Benchmarking

## Compile executables to be benchmarked

The `codegen=benchmark` option will remove ASan but keep debugging info. If you omit it,
your timings will not reflect the truth.

```bash
apman$ make deep-clean && make codegen=benchmark
```

The resulting executables are located in the `build` directory.

The following benchmarks assume the executables are available there.

## Listing available benchmarks

```bash
apman$ find experiment -name "benchmark"
```

## Benchmarking the final maps

Only upon completing the experiments and analyzing their results, you'll be able
to benchmark your best implementations of all kinds of general-purpose maps in
various situations:

```bash
apman$ ./maplab --experiment maps --preset medium|large --workload random
```

```bash
apman$ ./maplab --experiment maps --preset medium|large --workload ascend
```


# Experiments

Use these prepared experiments to guide your choice of options for instantiating
the final maps. Feel free to modify/augment them! They're not bullet-proof.

The goal of course is to isolate as precisely as possible one particular difference
in behavior so its cost may be assessed and hypotheses scientifically tested.

## Command-line experiment syntax

The suffixes of the executables to be benchmarked are supposed to be located in `experiment/foo/benchmark`:

```bash
apman$ ./maplab --experiment foo --preset tiny|small|medium|large --workload ascend|descend|random
```

See `./maplab --help` for more.

## Experiment #1: Assessing influence of map backend data structure

Complexity issues made real.

```bash
apman$ ./maplab --experiment maps/structure/tiny --preset tiny --workload random
```

```bash
apman$ ./maplab --experiment maps/structure/small --preset small --workload random
```

## Experiment #2: Assessing influence of addressing

Is there any hashtable design that is an all-round best performer?

```bash
apman$ ./maplab --experiment ht/addressing --preset medium --workload random
```

## Experiment #3: Assessing behavior under high load

Memory vs. speed tradeof: if we allow more dense tables to save memory, this should
likely be repaid elsewhere. There's no free lunch, but some are cheaper.

ds_ht_max_lf has been set to the following values:
- chained:    15    /* aka Max mean list length   */
- linear:      0.54 /* Not so much above 0.5!     */
- robin-hood:  0.9  /* Insane density!            */
- quadratic:   0.9  /* Insane density!            */

```bash
apman$ ./maplab --experiment ht/high-max-lf --preset small --workload random --verbose
```

## Experiment #4: Assessing influence of hash-related techniques

The cost of fool-proofing hashtables.

```bash
apman$ ./maplab --experiment ht/hashing --preset medium --workload random
```

```bash
apman$ ./maplab --experiment ht/hashing --preset medium --workload ascend
```

## Experiment #5: Assessing influence of home slot computation

An educated guess for the primes vs. powers-of-two and modulo vs. bitmask debates.

```bash
apman$ ./maplab --experiment ht/homeslot --preset medium --workload random
```

## Experiment #6: Assessing influence of tombstone policy

By default, we only accumulate and never reuse tombstones. How does that strike the
overall balance? Surprises to come!

```bash
apman$ $ ./maplab --experiment ht/tombstones --preset small --workload random 
```

## Experiment #7: Assessing influence of secure removals

How many damages are worth a nanosecond?

```bash
apman$ ./maplab --experiment ht/security --preset medium --workload random
```

