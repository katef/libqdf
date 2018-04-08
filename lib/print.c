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
#include <ctype.h>
#include <math.h>

#include <qdf/types.h>
#include <qdf/print.h>

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
qdf_print_int(FILE *f, int32_t n)
{
	assert(f != NULL);

	/* ISO PDF 2.0 Annex C.1 "Integer values ... can often be expressed
	 * within 32 bits." */

	fprintf(f, "%" PRId32, n);
}

void
qdf_print_size(FILE *f, size_t n)
{
	assert(f != NULL);

	fprintf(f, "%zu", n);
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
qdf_print_object(FILE *f, const struct qdf_object *o)
{
	assert(f != NULL);
	assert(o != NULL);

	switch (o->type) {
	case QDF_TYPE_NULL:   qdf_print_null  (f);            return;
	case QDF_TYPE_BOOL:   qdf_print_bool  (f, o->u.v);    return;
	case QDF_TYPE_INT:    qdf_print_int   (f, o->u.i);    return;
	case QDF_TYPE_SIZE:   qdf_print_size  (f, o->u.z);    return;
	case QDF_TYPE_REAL:   qdf_print_real  (f, o->u.n);    return;
	case QDF_TYPE_STRING: qdf_print_string(f, o->u.s);    return;
	case QDF_TYPE_NAME:   qdf_print_name  (f, o->u.name); return;
	case QDF_TYPE_ARRAY:  qdf_print_array (f, &o->u.a);   return;

	case QDF_TYPE_BIN:
		qdf_print_bin(f, o->u.data.p, o->u.data.n);
		return;

	case QDF_TYPE_DICT:
		qdf_print_dict(f, &o->u.d);
		return;

	case QDF_TYPE_STREAM:
		assert(!"unimplemented");
		return;

	default:
		assert(!"unreached");
		return;
	}
}

void
qdf_print_array(FILE *f, const struct qdf_array *a)
{
	size_t i;

	assert(f != NULL);
	assert(a != NULL);

	fprintf(f, "[");

	for (i = 0; i < a->n; i++) {
		qdf_print_object(f, &a->o[i]);

		if (i + 1 < a->n) {
			fprintf(f, " ");
		}
	}

	fprintf(f, "]");
}

void
qdf_print_dict(FILE *f, const struct qdf_dict *d)
{
	size_t i, j, n;

	assert(f != NULL);
	assert(d != NULL);

	fprintf(f, "<<");

	/* ISO PDF 2.0 7.3.7 "A dictonary whose value is null ... shall be
	 * treated the same as if the entry does not exist." */

	n = 0;

	for (i = 0; i < d->n; i++) {
		if (d->e[i].o.type == QDF_TYPE_NULL) {
			continue;
		}

		n++;
	}

	for (i = 0, j = 0; i < d->n; i++) {
		if (d->e[i].o.type == QDF_TYPE_NULL) {
			continue;
		}

		qdf_print_name(f, d->e[i].name);
		fprintf(f, " ");
		qdf_print_object(f, &d->e[i].o);

		if (j + 1 < n) {
			fprintf(f, " ");
		}

		j++;
	}

	fprintf(f, ">>");
}

void
qdf_print_def(FILE *f, unsigned id, const struct qdf_object *o)
{
	const unsigned gen = 0;

	assert(f != NULL);
	assert(o != NULL);

	fprintf(f, "%u %u obj\n", id, gen);
	qdf_print_object(f, o);
	fprintf(f, "\nendobj");
}

void
qdf_print_ref(FILE *f, unsigned id)
{
	const unsigned gen = 0;

	assert(f != NULL);

	fprintf(f, "%u %u R", id, gen);
}

