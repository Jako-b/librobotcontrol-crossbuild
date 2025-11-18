#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#include <rc/rc_find_pin.h>

/**
 * Parse a number from "gpiochipX"
 */
static int parse_chip_number(const char *name)
{
    if (strncmp(name, "gpiochip", 8) != 0) return -1;
    return atoi(name + 8);
}

/**
 * Parse "line:  16:  \"MDIR2A\"" → return line=16 if name matches
 */
static int match_line(const char *line, const char *wanted)
{
    const char *p = strstr(line, ":");
    if (!p) return -1;
    p++; // skip first :

    int offset = atoi(p);

    const char *name_start = strstr(line, "\"");
    if (!name_start) return -1;
    name_start++;

    const char *name_end = strstr(name_start, "\"");
    if (!name_end) return -1;

    char name[128];
    int len = name_end - name_start;
    if (len <= 0 || len >= (int)sizeof(name)) return -1;

    strncpy(name, name_start, len);
    name[len] = '\0';

    if (strcmp(name, wanted) == 0)
        return offset;

    return -1;
}

rc_pin_t rc_find_pin(const char *line_name)
{
    rc_pin_t result = { -1, -1 };

    DIR *d = opendir("/sys/kernel/debug/gpio");
    if (!d) return result;

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {

        // look for gpiochipX files
        if (strncmp(ent->d_name, "gpiochip", 8) != 0)
            continue;

        int chip_num = parse_chip_number(ent->d_name);
        if (chip_num < 0)
            continue;

        char path[256];
        snprintf(path, sizeof(path),
                 "/sys/kernel/debug/gpio/%s", ent->d_name);

        FILE *f = fopen(path, "r");
        if (!f) continue;

        char line[512];
        while (fgets(line, sizeof(line), f)) {

            int offset = match_line(line, line_name);
            if (offset >= 0) {
                result.chip = chip_num;
                result.line = offset;
                fclose(f);
                closedir(d);
                return result;
            }
        }

        fclose(f);
    }

    closedir(d);
    return result;
}
