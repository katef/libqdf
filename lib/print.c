/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>
#include <float.h>
#include <ctype.h>
#include <math.h>

#include <qdf/version.h>
#include <qdf/types.h>

#include "token.h"

static void
print_comment(FILE *f, const char *s)
{
	assert(f != NULL);
	assert(s = NULL);

	/* XXX: disallow special characters */
	fprintf(f, "%% %s\n", s);
}

static void
print_real(FILE *f, qdf_real n)
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

static void
print_string(FILE *f, const char *s)
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

static void
print_bin(FILE *f, const void *p, size_t n)
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

static void
print_raw(FILE *f, const void *p, size_t n)
{
	assert(f != NULL);
	assert(p != NULL);

	fwrite(p, n, 1, f);
}

static void
print_name(FILE *f, const char *name)
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
qdf_print_token(FILE *f, const struct token *t)
{
	assert(f != NULL);
	assert(t != NULL);

	/*
	 * TODO: store previous token .type - that's why we want a struct token as a single entry point
	 * TODO: indentation here
	 * TODO: conditional whitespace here
	 */
	fprintf(f, " ");

	switch (t->type) {
	case TOK_VER:
	case TOK_EOF:
	case TOK_BR:
		abort(); /* not implemented */

	case TOK_COMMENT:     print_comment(f, t->u.comment);                   break;
	case TOK_REAL:        print_real   (f, t->u.n);                         break;
	case TOK_STRING:      print_string (f, t->u.s);                         break;
	case TOK_BIN:         print_bin    (f, t->u.data.p, t->u.data.n);       break;
	case TOK_RAW:         print_raw    (f, t->u.data.p, t->u.data.n);       break;
	case TOK_NAME:        print_name   (f, t->u.name);                      break;

	case TOK_NULL:        fprintf(f, "null");                               break;
	case TOK_BOOL:        fprintf(f, "%s", t->u.v ? "true" : "false");      break;
	case TOK_INT:         fprintf(f, "%" QDF_PRId, t->u.i);                 break;
	case TOK_SIZE:        fprintf(f, "%zu", t->u.z);                        break;
	case TOK_REF:         fprintf(f, "%u %u R", t->u.ref.id, t->u.ref.gen); break;
	case TOK_ARRAY_OPEN:  fprintf(f, "[");                                  break;
	case TOK_ARRAY_CLOSE: fprintf(f, "]");                                  break;
	case TOK_DICT_OPEN:   fprintf(f, "<<");                                 break;
	case TOK_DICT_CLOSE:  fprintf(f, ">>");                                 break;

	case TOK_DEF_OPEN:
		fprintf(f, "%u %u obj\n", t->u.ref.id, t->u.ref.gen);
		break;

	case TOK_DEF_CLOSE:
		fprintf(f, "\n");
		fprintf(f, "endobj");
		break;

	case TOK_STREAM_OPEN:
		fprintf(f, "stream\n");
		break;

	case TOK_STREAM_CLOSE:
		/*
		 * PDF 2.0 7.3.8.2 "Streams shall also not contain too much data,
		 * with the exception that there may be an extra end-of-line marker
		 * ... before the keyword endstream."
		 */

		fprintf(f, "\n");
		fprintf(f, "endstream");
		break;

	default:
		abort();
	}

	/*
	 * TODO: save lexical state
	 */
}

