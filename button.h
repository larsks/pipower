/**
 * \file button.h
 *
 * Simple debouncing routine from
 * <https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/>
 *
 */
#ifndef _button_h
#define _button_h

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Button Button;

extern Button *button_new(uint8_t pin);
extern void button_destryo(Button *);
extern void button_update(Button *);
extern bool button_is_pressed(Button *);
extern bool button_is_released(Button *);
extern bool button_is_up(Button *);
extern bool button_is_down(Button *);

#ifdef __cplusplus
}
#endif

#endif // _button_h
