/**
 * \file pipowerd.c
 *
 * Monitor GPIO for the SHUTDOWN signal.
 */
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <linux/gpio.h>

#ifndef DEFAULT_GPIO_DEV
#define DEFAULT_GPIO_DEV "/dev/gpiochip0"
#endif

#ifndef DEFAULT_PIN
#define DEFAULT_PIN 17
#endif

#ifndef DEFAULT_SHUTDOWN_COMMAND
#define DEFAULT_SHUTDOWN_COMMAND "/bin/systemctl poweroff"
#endif

#define OPT_GPIO_DEV 'd'
#define OPT_PIN 'p'
#define OPT_SHUTDOWN_COMMAND 'c'
#define OPT_VERBOSE 'v'
#define OPT_IGNORE_INITIAL_STATE 'i'
#define OPT_HELP 'h'

#define OPTSTRING "d:p:c:vih"

const struct option longopts[] = {
    {"gpio-device", required_argument, 0, OPT_GPIO_DEV},
    {"gpio-pin", required_argument, 0, OPT_PIN},
    {"shutdown-command", required_argument, 0, OPT_SHUTDOWN_COMMAND},
    {"ignore-initial-state", required_argument, 0, OPT_IGNORE_INITIAL_STATE},
    {"verbose", no_argument, 0, OPT_VERBOSE},
    {"help", no_argument, 0, OPT_HELP},
};

struct config {
    char *device;

    int pin,
        verbose;

    bool ignore_initial_state;

    char *shutdown_command;
} config;

bool flag_shutdown = false;

void init_config() {
    config.device = DEFAULT_GPIO_DEV;
    config.pin = DEFAULT_PIN;
    config.verbose = 0;
    config.shutdown_command = DEFAULT_SHUTDOWN_COMMAND;
}

void handle_shutdown() {
	flag_shutdown = true;
}

bool check_shutdown() {
}

void usage(FILE *out) {
    fprintf(out, "pipower: usage: pipower [-d <device>] [-p <pin>] "
                 "[-c <shutdown_command> ] [-vi]\n");
}

void delay(long ms) {
    struct timespec ts;

    ts.tv_sec = (ms/1000);
    ts.tv_nsec = (ms-((ms/1000)*1000)) * 1000000;
    nanosleep(&ts, NULL);
}

void monitor_shutdown_pin() {
    struct gpioevent_request req;
    struct gpiohandle_data data;
    int fd;
    int ret;

    fd = open(config.device, 0);
    if (fd == -1) {
        ret = -errno;
        fprintf(stderr, "pipower: failed to open %s: %s\n",
                config.device, strerror(errno));
        exit(ret);
    }

    req.lineoffset = config.pin;
    req.handleflags = 0;
    req.eventflags = GPIOEVENT_REQUEST_RISING_EDGE;
    strcpy(req.consumer_label, "pipower-shutdown");

    ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &req);
    if (ret == -1) {
        ret = -errno;
        fprintf(stderr, "pipower: failed to GET_LINEEVENT: %s\n",
                strerror(errno));
        exit(ret);
    }

    ret = ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
    if (ret == -1) {
        ret = -errno;
        fprintf(stderr, "pipower: failed to GET_LINE_VALUES: %s\n",
                strerror(errno));
        exit(ret);
    }

    if (data.values[0]) {
        if (config.ignore_initial_state) {
            if (config.verbose > 0)
                printf("pipower: ignoring active shutdown request\n");

        } else {
            printf("pipower: shutdown request is already active\n");
            return;
        }
    }

    while (1) {
        struct gpioevent_data event;

        ret = read(req.fd, &event, sizeof(event));
        if (ret == -1) {
            if (errno == -EAGAIN) {
                continue;
            } else {
                ret = -errno;
                fprintf(stderr, "pipower: failed to read event: %s\n",
                        strerror(errno));
                exit(ret);
            }
        }

        if (ret != sizeof(event)) {
                ret = -EIO;
                fprintf(stderr, "pipower: failed to read event: %s\n",
                        strerror(errno));
                exit(ret);
        }

        break;
    }
}

int main(int argc, char *argv[]) {
    int option_index = 0;
    int ch;
    int shutdown_state;

    init_config();

    while (EOF != (ch = getopt_long(argc, argv, OPTSTRING, longopts, &option_index))) {
        switch (ch) {
            case OPT_GPIO_DEV:
                config.device = strdup(optarg);
                break;

            case OPT_PIN:
                config.pin = atoi(optarg);
                if (config.pin == 0) {
                    printf("pipower: invalid shutdown pin specification: %s\n", optarg);
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
	    printf("pipower: starting, device=%s pin=%d\n",
                config.device, config.pin);

    monitor_shutdown_pin();

    if (config.verbose > 0)
	    printf("pipower: received shutdown signal\n");
        if (config.verbose > 1)
            printf("pipower: running shutdown command: %s\n",
                    config.shutdown_command);

    system(config.shutdown_command);
    return 0;
}
