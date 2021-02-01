# Planetarium Lighting System

This project enhances the planetarium's 16-bit DMX controllers. It exposes a JSON API over TCP that allows the client to seamlessly control both 8-bit channels, and to initiate fade curves.

## Installation

1. Clone this repository.
1. Run `pio lib install` to get the necessary dependencies.

## Debugging

Use the [PlatformIO Unified Debugger](https://www.youtube.com/watch?v=GtlsW3FDN3E) for debugging.

## API

```json
[
  {
    "node": 1,
    "val": 65000,
    "dur": 30000,
    "ramp": 12,
    "loop": 2
  },
  ...
]
```

### node (uint8_t)

A node is a coarse-fine channel pair. The planetarium has 20 nodes.

### val (uint16_t)

This is the 16-bit value you want this node to be set to.

### dur (uint32_t)

This is the duration over which you want the fade to occur. The default duration is 0.

### ramp (uint8_t)

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

This is the loop that describes the how/if the curve should repeat. The default is `ONCEFORWARD`. The options are:

0. `ONCEFORWARD` (default)
1. `LOOPFORWARD`
1. `FORTHANDBACK`
1. `ONCEBACKWARD`
1. `LOOPBACKWARD`
1. `BACKANDFORTH`