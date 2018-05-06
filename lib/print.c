/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>
#include <float.h>
#include <ctype.h>
#include <math.h>

#include <qdf/types.h>
#include <qdf/print.h>

void
qdf_print_space(FILE *f)
{
	assert(f != NULL);

	fprintf(f, " ");
}

void
qdf_print_comment(FILE *f, const char *s)
{
	assert(f != NULL);
	assert(s = NULL);

	/* XXX: disallow special characters */
	fprintf(f, "%% %s\n", s);
}

void
qdf_print_null(FILE *f)
{
	assert(f != NULL);

	fprintf(f, "null");
}

void
qdf_print_bool(FILE *f, bool v)
{
	assert(f != NULL);

	fprintf(f, "%s", v ? "true" : "false");
}

void
qdf_print_int(FILE *f, qdf_int n)
{
	assert(f != NULL);

	fprintf(f, "%" QDF_PRId, n);
}

void
qdf_print_size(FILE *f, size_t n)
{
	assert(f != NULL);

	fprintf(f, "%zu", n);
}

void
qdf_print_real(FILE *f, qdf_real n)
{
	const int precision = 8;
	double i, b; /* not qdf_real; for modf() and friends */

	assert(f != NULL);
	assert(!isnan(n));
	assert(!isinf(n));

	if (isnan(n) || isinf(n)) {
		fprintf(f, "0");
		return;
	}

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

	fprintf(f, "%.*" QDF_PRIr, precision, n);
}

static bool
balanced(const char *s)
{
	const char *p;
	int depth;

	depth = 0;

	for (p = s; *p != '\0'; p++) {
		switch (*p) {
		case '(': depth++; break;
		case ')': depth--; break;

		default:
			;
		}

		if (depth == 0) {
			return true;
		}
	}

	return false;
}

void
qdf_print_string(FILE *f, const char *s)
{
	const int limit = 70;
	const char *p;
	int depth;

	assert(f != NULL);
	assert(s != NULL);

	fprintf(f, "(");

	depth = 0;

	for (p = s; *p != '\0'; p++) {
		/* ISO PDF 2.0 "A PDF writer may split a literal string
		 * across multiple lines." */
		if ((p - s + 1) % limit == 0) {
			fprintf(f, "\\\n");
		}

		/* ISO PDF 2.0 7.3.4.2 "Three octal digits shall be used,
		 * with leading zeroes as needed, if the next character of
		 * the string is also a digit." */
		if (!isprint((unsigned char) *p)) {
			fprintf(f, "\\%0*o",
				isdigit((unsigned char) *(p + 1)) ? 3 : 0,
				(unsigned char) *p);
			continue;
		}

		switch (*p) {
		case '\\': fprintf(f, "\\\\"); continue;
		case '\n': fprintf(f, "\\n");  continue;
		case '\r': fprintf(f, "\\r");  continue;
		case '\t': fprintf(f, "\\t");  continue;
		case '\b': fprintf(f, "\\b");  continue;
		case '\f': fprintf(f, "\\f");  continue;

		/* ISO PDF 2.0 7.3.4.2 "Balanced pairs of patentheses ... require
		 * no special treatment." */

		case '(':
			if (depth >= 0 && balanced(p)) {
				depth++;
				fprintf(f, "(");
				continue;
			}

			fprintf(f, "\\(");
			continue;

		case ')':
			if (depth > 0) {
				depth--;
				fprintf(f, ")");
				continue;
			}

			fprintf(f, "\\)");
			continue;

		default:
			fprintf(f, "%c", *p);
			continue;
		}
	}

	fprintf(f, ")");
}

void
qdf_print_bin(FILE *f, const void *p, size_t n)
{
	size_t i;

	assert(f != NULL);
	assert(p != NULL);

	fprintf(f, "<");

	for (i = 0; i < n; i++) {
		fprintf(f, "%02x", ((unsigned char *) p)[i]);
	}

	fprintf(f, ">");
}

void
qdf_print_raw(FILE *f, const void *p, size_t n)
{
	assert(f != NULL);
	assert(p != NULL);

	fwrite(p, n, 1, f);
}

void
qdf_print_name(FILE *f, const char *name)
{
	const char *p;

	assert(f != NULL);
	assert(name != NULL);

	/* ISO PDF 2.0 7.3.5 "Begnning with PDF 1.2 a name object is ..."
	 * and 1.2 is the minimum version libqdf supports for this reason. */

	fprintf(f, "/");

	for (p = name; *p != '\0'; p++) {
		if (*p == '#') {
			fprintf(f, "#%02x", (unsigned char) *p);
			continue;
		}

		if (!isprint((unsigned char) *p)) {
			fprintf(f, "#%02x", (unsigned char) *p);
			continue;
		}

		if (isspace((unsigned char) *p)) {
			fprintf(f, "#%02x", (unsigned char) *p);
			continue;
		}

		if (((unsigned char) *p) < 0x21 || ((unsigned char) *p) > 0x7e) {
			fprintf(f, "#%02x", (unsigned char) *p);
			continue;
		}

		fprintf(f, "%c", *p);
	}
}

void
qdf_print_def_open(FILE *f, unsigned id, unsigned gen)
{
	assert(f != NULL);

	fprintf(f, "%u %u obj\n", id, gen);
}

void
qdf_print_def_close(FILE *f)
{
	assert(f != NULL);

	fprintf(f, "\n");
	fprintf(f, "endobj");
}

void
qdf_print_ref(FILE *f, unsigned id)
{
	const unsigned gen = 0;

	assert(f != NULL);

	fprintf(f, "%u %u R", id, gen);
}

void
qdf_print_array_open(FILE *f)
{
	assert(f != NULL);

	fprintf(f, "[");
}

void
qdf_print_array_close(FILE *f)
{
	assert(f != NULL);

	fprintf(f, "]");
}

void
qdf_print_dict_open(FILE *f)
{
	assert(f != NULL);

	fprintf(f, "<<");
}

void
qdf_print_dict_close(FILE *f)
{
	assert(f != NULL);

	fprintf(f, ">>");
}

void
qdf_print_stream_open(FILE *f)
{
	assert(f != NULL);

	fprintf(f, "stream\n");
}

void
qdf_print_stream_close(FILE *f)
{
	assert(f != NULL);

	/*
	 * PDF 2.0 7.3.8.2 "Streams shall also not contain too much data,
	 * with the exception that there may be an extra end-of-line marker
	 * ... before the keyword endstream."
	 */

	fprintf(f, "\n");
	fprintf(f, "endstream");
}

