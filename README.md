# Planetarium Lighting System

This README file is in dire need of updates!
This project enhances the planetarium's 16-bit DMX controllers. It exposes a JSON API over UDP that allows the client to seamlessly control 16 bit DMX ( as two 8-bit channels), and to initiate fade curves.

## Hardware
This will work on an Arduino compatible board: Recommend a Mega 2560.
- With Wiznet 5100 or 5500 ethernet.
- a DMX shield.
Note: an 328 based (Uno/Ethernet) will run out of memory > 5 nodes to control.
You must set the json doc size to ~350 or less.

For 20 nodes (what we needed), a 2560 based Mega board was required for it's 8K of RAM.

## Installation

1. Clone this repository.
1. Run `pio lib install` to get the necessary dependencies.
   - If you prefer to manually install dependencies, install them in `.pio/libdeps/uno/` so they remain distinct from custom classes.

## Contribution workflow

The process of contributing to this repository is as follows:

1. If a new class is being created, add it to `lib/` and add an appropriate test file to `test/`.
1. Make incremental changes to the class in `lib/` that is relevant to the change. Use the class's test file to debug problems.
1. Once the class is working as intended, implement it is `main.cpp`.

### Rationale behind this workflow

Keeping all imperative code in classes is good for the following reasons:

- It allows the code to exist in small, testable units.
- It allows the units being tested to be identical to those implemented in the cod base, meaning development is quicker and less error-prone.
- It keeps the code base organized and easy to maintain.

## API
UDP packet is used to send control data to board.
Maximum safe UDP packate capacity is 508, so Arduino json doc size is set to recommended 512.
This limits the number of nodes you can control in a single packet transmission!
For our 20 node project, therefore, we define THREE kinds of packet transmission:
1) Normal RGBW node control (nodes 1,2,3,4,6,7,8,9,11,12,13,14,16,17,18,19)
2) AUX node control (nodes 5,10,15,20)
3) Device control (node 0) Not Installed

```js
[
  {
    node: 1,
    val: 65534,
    dur: 65534,
    ramp: 12, [depreciated]
    loop: 2, [depreciated]
  },
  //...
];
```

### node (uint8_t)

A node is a coarse-fine channel pair. The planetarium has 20 nodes.

### val (uint16_t)

This is the 16-bit value you want the node's brightness to be set to.

### dur (uint16_t)

This is the duration over which you want the fade to occur. The default duration is 0.

### ramp (uint8_t)
[depreciated]
This is the curve that describes the fade. The available options are:

0. `NONE`
1. `LINEAR` (default)
1. `QUADRATIC_IN`
1. `QUADRATIC_OUT`
1. `QUADRATIC_INOUT`
1. `CUBIC_IN`
1. `CUBIC_OUT`
1. `CUBIC_INOUT`
1. `QUARTIC_IN`
1. `QUARTIC_OUT`
1. `QUARTIC_INOUT`
1. `QUINTIC_IN`
1. `QUINTIC_OUT`
1. `QUINTIC_INOUT`
1. `SINUSOIDAL_IN`
1. `SINUSOIDAL_OUT`
1. `SINUSOIDAL_INOUT`
1. `EXPONENTIAL_IN`
1. `EXPONENTIAL_OUT`
1. `EXPONENTIAL_INOUT`
1. `CIRCULAR_IN`
1. `CIRCULAR_OUT`
1. `CIRCULAR_INOUT`
1. `ELASTIC_IN`
1. `ELASTIC_OUT`
1. `ELASTIC_INOUT`
1. `BACK_IN`
1. `BACK_OUT`
1. `BACK_INOUT`
1. `BOUNCE_IN`
1. `BOUNCE_OUT`
1. `BOUNCE_INOUT`

### loop (uint8_t)
[depreciated]
This is the loop that describes the how/if the curve should repeat. The default is `ONCEFORWARD`. The options are:

0. `ONCEFORWARD` (default)
1. `LOOPFORWARD`
1. `FORTHANDBACK`
1. `ONCEBACKWARD`
1. `LOOPBACKWARD`
1. `BACKANDFORTH`
