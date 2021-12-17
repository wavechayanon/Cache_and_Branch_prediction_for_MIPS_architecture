# Cache and Branch prediction for MIPS architecture

This repository is a part of the IST computer architecture subject at VISTEC to perform the cache and branch prediction in the pipelining ISAs.

## Makefile
To compile the simulator:
```
$make
```
To compile the simulator and verify it against the reference simulator using one or more test
inputs:
```
$ make verify INPUT=inputs/inst/addiu.x
$ make verify INPUT=inputs/inst/*.x
$ make verify
```
