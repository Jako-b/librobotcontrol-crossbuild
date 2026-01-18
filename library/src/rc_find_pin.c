#include <stdio.h>
#include <string.h>
#include "rc/rc_find_pin.h"
#include <gpiod.h>

int rc_find_pin(const char *line_name, int *chip_out, int *line_out)
{
    if (!line_name || !chip_out || !line_out) {
        fprintf(stderr, "rc_find_pin: invalid arguments\n");
        return -1;
    }

    char chipname[32];
    struct gpiod_chip *chip = NULL;
    struct gpiod_line *line = NULL;
    const char *lname;
    int num_lines;

    // Scan up to 4 gpio chips
    for (int chip_id = 0; chip_id < 4; chip_id++) {

    	chip = gpiod_chip_open_by_number(chip_id);
    	if (!chip) {
    	    continue;
    	}

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
                return 0; // Success
            }
        }

        gpiod_chip_close(chip);
    }

    fprintf(stderr, "rc_find_pin: line name '%s' not found\n", line_name);
    return -1;
}