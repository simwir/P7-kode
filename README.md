# P7-kode

[![Build Status](https://travis-ci.com/simwir/P7-kode.svg?token=oex4ZGEv93sgS2xELYpN&branch=master)](https://travis-ci.com/simwir/P7-kode)

A work-in-progress distributed fleet management system. 
Uses [Webots](https://cyberbotics.com/) as simulation back-end and UPPAAL Stratego for the routing part of fleet management.

## Dependencies
Depends on [jsoncpp](https://github.com/open-source-parsers/jsoncpp) for JSON writing and parsing, 
and [Webots](https://cyberbotics.com/) for simulation.
Only tested on MacOS/Linux.

## Build instructions
We use CMake >= 3.10. Sample usage:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
After this, the executables can be found in build/bin.
