/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void
qdf_vprint_comment(FILE *f, const char *fmt, va_list ap)
{
	char buf[256];
	bool overflow;
	size_t r, n;

	assert(f != NULL);
	assert(fmt != NULL);

	overflow = false;

	r = vsnprintf(buf, sizeof buf, fmt, ap);

	if (r > sizeof buf) {
		overflow = true;
	}

	n = strcspn(buf, "\r\n");
	if (buf[n] != '\0') {
		buf[n] = '\0';
		overflow = true;
	}

	fprintf(f, "%% %s%s\n", buf,
		overflow ? " ..." : "");
}

void
qdf_print_comment(FILE *f, const char *fmt, ...)
{
	va_list ap;

	assert(f != NULL);
	assert(fmt != NULL);

	va_start(ap, fmt);
	qdf_vprint_comment(f, fmt, ap);
	va_end(ap);
}

void
qdf_print_bool(FILE *f, bool v)
{
	assert(f != NULL);

	fprintf(f, "%s", v ? "true" : "false");
}

