#include <Ramp.h>

#ifndef KeywordParser_h
#define KeywordParser_h

extern enum ramp_mode parseRampMode(const char* command);
extern enum loop_mode parseLoopMode(const char* command);

#endif