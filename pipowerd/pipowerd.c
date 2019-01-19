#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <wiringPi.h>

#define DEFAULT_PIN_SHUTDOWN 17
#define DEFAULT_PIN_BOOT 4

#define OPT_PIN_BOOT 'b'
#define OPT_PIN_SHUTDOWN 's'
#define OPT_DRYRUN 'n'
#define OPT_VERBOSE 'v'

#define OPTSTRING "b:s:nv"

const struct option longopts[] = {
    {"pin-shutdown", required_argument, 0, OPT_PIN_SHUTDOWN},
    {"pin-boot", required_argument, 0, OPT_PIN_BOOT},
    {"dry-run", no_argument, 0, OPT_DRYRUN},
    {"verbose", no_argument, 0, OPT_VERBOSE},
};

struct config {
    int pin_shutdown,
        pin_boot,
        verbose;
    bool    dry_run;
} config;

void init_config() {
    config.pin_shutdown = DEFAULT_PIN_SHUTDOWN;
    config.pin_boot = DEFAULT_PIN_BOOT;
    config.verbose = 0;
    config.dry_run = false;
}

void handle_shutdown() {
	printf("SHUTDOWN!\n");
}

int main(int argc, char *argv[]) {
    int option_index = 0;
    int ch;

    init_config();
    wiringPiSetupGpio();

    while (EOF != (ch = getopt_long(argc, argv, OPTSTRING, longopts, &option_index))) {
        switch (ch) {
            case OPT_PIN_SHUTDOWN:
                config.pin_shutdown = atoi(optarg);
                break;

            case OPT_PIN_BOOT:
                config.pin_boot = atoi(optarg);
                break;

            case OPT_DRYRUN:
                config.dry_run = true;
                break;

            case OPT_VERBOSE:
                config.verbose++;
                break;
        }
    }

    printf("pins: shutdown=%d boot=%d\n", config.pin_shutdown, config.pin_boot);
    pinMode(config.pin_shutdown, INPUT);
    pullUpDnControl(config.pin_shutdown, PUD_DOWN);
    pinMode(config.pin_boot, OUTPUT);

    digitalWrite(config.pin_boot, 0);
    wiringPiISR (config.pin_shutdown, INT_EDGE_RISING, handle_shutdown);

    while(1) {
	delay(1000);
    }

    return 0;
}
