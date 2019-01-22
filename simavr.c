#include <simavr/avr/avr_mcu_section.h>
#include "pins.h"

extern uint8_t state;

const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
    { AVR_MCU_VCD_SYMBOL("PIN_POWER"),    .mask = (1<<PIN_POWER),    .what = (void*)&PINB,  },
    { AVR_MCU_VCD_SYMBOL("PIN_USB"),      .mask = (1<<PIN_USB),      .what = (void*)&PINB,  },
    { AVR_MCU_VCD_SYMBOL("PIN_EN"),       .mask = (1<<PIN_EN),       .what = (void*)&PORTB, },
    { AVR_MCU_VCD_SYMBOL("PIN_SHUTDOWN"), .mask = (1<<PIN_SHUTDOWN), .what = (void*)&PORTB, },
    { AVR_MCU_VCD_SYMBOL("PIN_BOOT"),     .mask = (1<<PIN_BOOT),     .what = (void*)&PINB,  },
    { AVR_MCU_VCD_SYMBOL("STATE"),                                   .what = (void*)&state, },
};

