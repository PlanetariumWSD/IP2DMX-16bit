#include <KeywordParser.h>
#include <Ramp.h>

enum ramp_mode parseRampMode(const char* command) {
  if (strcmp(command, "NONE") == 0) return NONE;
  if (strcmp(command, "LINEAR") == 0) return LINEAR;
  if (strcmp(command, "QUADRATIC_IN") == 0) return QUADRATIC_IN;
  if (strcmp(command, "QUADRATIC_OUT") == 0) return QUADRATIC_OUT;
  if (strcmp(command, "QUADRATIC_INOUT") == 0) return QUADRATIC_INOUT;
  if (strcmp(command, "CUBIC_IN") == 0) return CUBIC_IN;
  if (strcmp(command, "CUBIC_OUT") == 0) return CUBIC_OUT;
  if (strcmp(command, "CUBIC_INOUT") == 0) return CUBIC_INOUT;
  if (strcmp(command, "QUARTIC_IN") == 0) return QUARTIC_IN;
  if (strcmp(command, "QUARTIC_OUT") == 0) return QUARTIC_OUT;
  if (strcmp(command, "QUARTIC_INOUT") == 0) return QUARTIC_INOUT;
  if (strcmp(command, "QUINTIC_IN") == 0) return QUINTIC_IN;
  if (strcmp(command, "QUINTIC_OUT") == 0) return QUINTIC_OUT;
  if (strcmp(command, "QUINTIC_INOUT") == 0) return QUINTIC_INOUT;
  if (strcmp(command, "SINUSOIDAL_IN") == 0) return SINUSOIDAL_IN;
  if (strcmp(command, "SINUSOIDAL_OUT") == 0) return SINUSOIDAL_OUT;
  if (strcmp(command, "SINUSOIDAL_INOUT") == 0) return SINUSOIDAL_INOUT;
  if (strcmp(command, "EXPONENTIAL_IN") == 0) return EXPONENTIAL_IN;
  if (strcmp(command, "EXPONENTIAL_OUT") == 0) return EXPONENTIAL_OUT;
  if (strcmp(command, "EXPONENTIAL_INOUT") == 0) return EXPONENTIAL_INOUT;
  if (strcmp(command, "CIRCULAR_IN") == 0) return CIRCULAR_IN;
  if (strcmp(command, "CIRCULAR_OUT") == 0) return CIRCULAR_OUT;
  if (strcmp(command, "CIRCULAR_INOUT") == 0) return CIRCULAR_INOUT;
  if (strcmp(command, "ELASTIC_IN") == 0) return ELASTIC_IN;
  if (strcmp(command, "ELASTIC_OUT") == 0) return ELASTIC_OUT;
  if (strcmp(command, "ELASTIC_INOUT") == 0) return ELASTIC_INOUT;
  if (strcmp(command, "BACK_IN") == 0) return BACK_IN;
  if (strcmp(command, "BACK_OUT") == 0) return BACK_OUT;
  if (strcmp(command, "BACK_INOUT") == 0) return BACK_INOUT;
  if (strcmp(command, "BOUNCE_IN") == 0) return BOUNCE_IN;
  if (strcmp(command, "BOUNCE_OUT") == 0) return BOUNCE_OUT;
  if (strcmp(command, "BOUNCE_INOUT") == 0) return BOUNCE_INOUT;

  return NONE;
};

enum loop_mode parseLoopMode(const char* command) {
  if (strcmp(command, "ONCEFORWARD") == 0) return ONCEFORWARD;
  if (strcmp(command, "LOOPFORWARD") == 0) return LOOPFORWARD;
  if (strcmp(command, "FORTHANDBACK") == 0) return FORTHANDBACK;
  if (strcmp(command, "ONCEBACKWARD") == 0) return ONCEBACKWARD;
  if (strcmp(command, "LOOPBACKWARD") == 0) return LOOPBACKWARD;
  if (strcmp(command, "BACKANDFORTH") == 0) return BACKANDFORTH;

  return ONCEFORWARD;
};