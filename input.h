/**
 * \file input.h
 */
#ifndef _input_h
#define _input_h

#include <stdint.h>
#include "bool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Input Input;

extern Input *input_new(int pin, bool pullup);
extern void input_delete(Input *);
extern void input_update(Input *);
extern bool input_went_high(Input *);
extern bool input_went_low(Input *);
extern bool input_is_high(Input *);
extern bool input_is_low(Input *);

#ifdef __cplusplus
}
#endif

#endif
