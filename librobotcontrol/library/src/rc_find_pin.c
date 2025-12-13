#include <stdio.h>
#include <string.h>

#include "rc/rc_find_pin.h"


/*
 * ==========================================================
 *  PLATFORM-SWITCH:
 *  - Auf ARM (BeagleBone) → echte gpiod-Implementierung
 *  - Beim Cross-Build → Stub, damit static build funktioniert
 * ==========================================================
 */
#ifdef __arm__

#include <gpiod.h>

int rc_find_pin(const char *line_name, int *chip_out, int *line_out)
{
    if (!line_name || !chip_out || !line_out) {
        fprintf(stderr, "rc_find_pin: invalid arguments\n");
        return -1;
    }

    /* Wir haben auf dem BeagleBone IMMER 4 GPIO-Chips: gpiochip0..3 */
    char chipname[32];
    struct gpiod_chip *chip = NULL;
    struct gpiod_line *line = NULL;
    const char *lname;
    int num_lines;

    for (int chip_id = 0; chip_id < 4; chip_id++) {

        snprintf(chipname, sizeof(chipname), "gpiochip%d", chip_id);
        chip = gpiod_chip_open(chipname);
        if (!chip) continue;

        num_lines = gpiod_chip_num_lines(chip);

        for (int l = 0; l < num_lines; l++) {
            line = gpiod_chip_get_line(chip, l);
            if (!line) continue;

            lname = gpiod_line_name(line);
            if (!lname) continue;

            if (strcmp(lname, line_name) == 0) {
                *chip_out = chip_id;
                *line_out = l;
                gpiod_chip_close(chip);
                return 0; // Erfolg
            }
        }

        gpiod_chip_close(chip);
    }

    fprintf(stderr, "rc_find_pin: line name '%s' not found\n", line_name);
    return -1;
}

#else
/* ==========================================================
 *  CROSS-BUILD STUB (Windows)
 *  static library kann NICHT gegen libgpiod linken
 * ========================================================== */

int rc_find_pin(const char *line_name, int *chip_out, int *line_out)
{
    fprintf(stderr,
        "rc_find_pin WARNING: Stub called for '%s' (host build, no gpiod)\n",
        line_name);

    /*
     * Rückgabe −1 = not found
     * Das ist OK, solange keine Tests auf dem Host ausgeführt werden.
     */
    return -1;
}

#endif
