/**
 * \file input.h
 */
#ifndef _input_h
#define _input_h

#include <stdint.h>

class Input {
    public:
        Input(int pin, bool pullup);
        void update();
        bool went_high();
        bool went_low();
        bool is_high();
        bool is_low();

    private:
        int pin;
	bool state,
	     last_state;
};

#endif
