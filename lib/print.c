/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>
#include <float.h>
#include <math.h>

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

void
qdf_print_int(FILE *f, int32_t n)
{
	assert(f != NULL);

	/* ISO PDF 2.0 Annex C.1 "Integer values ... can often be expressed
	 * within 32 bits." */

	fprintf(f, "%" PRId32, n);
}

void
qdf_print_real(FILE *f, double n)
{
	double i, b;
	const int precision = 8;

	assert(f != NULL);
	assert(!isnan(n));
	assert(!isinf(n));

	if (isnan(n) || isinf(n)) {
		fprintf(f, "0");
		return;
	}

	/* ISO PDF 2.0 Annex C.1 "Modern computers often represent and process
	 * real numbers using ... IEEE 754 single or double precision." */

	/* ISO PDF 2.0 7.33 "A PDF writer shall not use the PostScript language
	 * syntax for numbers with non-decimal radicies (such as 16#FFFE) or in
	 * exponential format (such as 6.02E23)." */

	b = modf(n, &i);

	/* ISO PDF 2.0 7.3.3 "Wherever a real number is expected, an integer
	 * may be used instead."
	 *
	 * Note this may be out of range for qdf_print_int()'s integer type.
	 */
	if (b == 0.0) {
		fprintf(f, "%.0f", i);
		return;
	}

	if (i == 0.0) {
		char buf[2 + 149 + 1];
		char *p;
		size_t r;

		r = snprintf(buf, sizeof buf, "%.*f", precision, fabs(b));
		assert(r < sizeof buf);
		assert(buf[0] == '0');
		assert(buf[1] == '.');

		p = buf + strlen(buf) - 1;
		while (p >= buf && *p == '0') {
			*p = '\0';
			p--;
		}

		fprintf(f, "%s.%s",
			signbit(b) ? "-" : "",
			buf + 2);
		return;
	}

	fprintf(f, "%.*f", precision, n);
}

