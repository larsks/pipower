#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <wiringPi.h>

#define DEFAULT_PIN_SHUTDOWN 17
#define DEFAULT_PIN_BOOT 4
#define DEFAULT_SHUTDOWN_COMMAND "/bin/systemctl poweroff"

#define OPT_PIN_BOOT 'b'
#define OPT_PIN_SHUTDOWN 's'
#define OPT_SHUTDOWN_COMMAND 'c'
#define OPT_VERBOSE 'v'
#define OPT_IGNORE_INITIAL_STATE 'i'
#define OPT_HELP 'h'

#define OPTSTRING "b:s:c:vih"

const struct option longopts[] = {
    {"pin-shutdown", required_argument, 0, OPT_PIN_SHUTDOWN},
    {"pin-boot", required_argument, 0, OPT_PIN_BOOT},
    {"shutdown-command", required_argument, 0, OPT_SHUTDOWN_COMMAND},
    {"ignore-initial-state", required_argument, 0, OPT_IGNORE_INITIAL_STATE},
    {"verbose", no_argument, 0, OPT_VERBOSE},
    {"help", no_argument, 0, OPT_HELP},
};

struct config {
    int pin_shutdown,
        pin_boot,
        verbose;

    bool ignore_initial_state;

    char *shutdown_command;
} config;

bool flag_shutdown = false;

void init_config() {
    config.pin_shutdown = DEFAULT_PIN_SHUTDOWN;
    config.pin_boot = DEFAULT_PIN_BOOT;
    config.verbose = 0;
    config.shutdown_command = DEFAULT_SHUTDOWN_COMMAND;
}

void handle_shutdown() {
	flag_shutdown = true;
}

void setup_pins() {
    pinMode(config.pin_shutdown, INPUT);
    pullUpDnControl(config.pin_shutdown, PUD_DOWN);
    pinMode(config.pin_boot, OUTPUT);

    wiringPiISR (config.pin_shutdown, INT_EDGE_RISING, handle_shutdown);
}

void set_boot() {
    if (config.verbose > 0)
	    printf("pipower: asserting BOOT signal\n");
    digitalWrite(config.pin_boot, 0);
}

void reset_boot() {
    if (config.verbose > 0)
	    printf("pipower: de-asserting BOOT signal\n");
    digitalWrite(config.pin_boot, 1);
}

bool check_shutdown() {
        int val;
        val = digitalRead(config.pin_shutdown);
        if (config.verbose > 1)
            printf("pipower: initial shutdown state = %d\n", val);
        return val;
}

void usage(FILE *out) {
    fprintf(out, "pipower: usage: pipower [-b <pin_boot>] [-s <pin_shutdown>] "
                 "[-c <shutdown_command> ] [-vi]\n");
}

int main(int argc, char *argv[]) {
    int option_index = 0;
    int ch;
    int shutdown_state;

    init_config();
    wiringPiSetupGpio();

    while (EOF != (ch = getopt_long(argc, argv, OPTSTRING, longopts, &option_index))) {
        switch (ch) {
            case OPT_PIN_SHUTDOWN:
                config.pin_shutdown = atoi(optarg);
                if (config.pin_shutdown == 0) {
                    printf("pipower: invalid shutdown pin specification: %s\n", optarg);
                    exit(1);
                }
                break;

            case OPT_PIN_BOOT:
                config.pin_boot = atoi(optarg);
                if (config.pin_boot == 0) {
                    printf("pipower: invalid boot pin specification: %s\n", optarg);
                    exit(1);
                }
                break;

            case OPT_SHUTDOWN_COMMAND:
                config.shutdown_command = strdup(optarg);
                break;

            case OPT_IGNORE_INITIAL_STATE:
                config.ignore_initial_state = true;
                break;

            case OPT_VERBOSE:
                config.verbose++;
                break;

            case OPT_HELP:
                usage(stdout);
                exit(0);
                break;

            case '?':
                usage(stderr);
                exit(2);
                break;
        }
    }

    if (config.verbose > 0)
	    printf("pipower: starting, shutdown=%d boot=%d\n", config.pin_shutdown, config.pin_boot);

    setup_pins();

    if (check_shutdown()) {
        printf("pipower: shutdown flag is set at boot%s\n",
                config.ignore_initial_state?" (ignoring)":"");
        if (!config.ignore_initial_state)
            goto shutdown;
    }

    set_boot();

    while(!flag_shutdown) {
        delay(10);
    }

    reset_boot();

shutdown:
    if (config.verbose > 0)
	    printf("pipower: received shutdown signal\n");

    if (config.verbose > 1)
        printf("pipower: running shutdown command: %s\n", config.shutdown_command);

    system(config.shutdown_command);
    return 0;
}
