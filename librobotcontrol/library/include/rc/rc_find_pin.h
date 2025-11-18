#pragma once

typedef struct {
	int chip;
	int line;
} rc_pin_t;

/**
 * Find GPIO line by name.
 *
 * Returns rc_pin_t with chip = -1, line = -1 if not found.
 */
rc_pin_t rc_find_pin(const char *line_name);