/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>

#include <qdf/version.h>
#include <qdf/types.h>
#include <qdf/print.h>
#include <qdf/params.h>
#include <qdf/filter.h>

#include "filter.h"
#include "token.h"

/* for C99 compound literals */
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

void
qdf_vprintf_comment(FILE *f, const char *fmt, va_list ap)
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
		abort();
		buf[n] = '\0';
		overflow = true;
	}

	if (overflow) {
		abort();
	}

	qdf_print_token(f, & (struct token) { TOK_COMMENT, .u.comment = buf } );
}

void
qdf_printf_comment(FILE *f, const char *fmt, ...)
{
	va_list ap;

	assert(f != NULL);
	assert(fmt != NULL);

	va_start(ap, fmt);
	qdf_vprintf_comment(f, fmt, ap);
	va_end(ap);
}

void
qdf_print_object(FILE *f, const struct qdf_object *o)
{
	assert(f != NULL);
	assert(o != NULL);

	switch (o->type) {
	case QDF_TYPE_NULL:   qdf_print_token(f, & (struct token) { TOK_NULL                        }); return;
	case QDF_TYPE_BOOL:   qdf_print_token(f, & (struct token) { TOK_BOOL,   .u.v    = o->u.v    }); return;
	case QDF_TYPE_INT:    qdf_print_token(f, & (struct token) { TOK_INT,    .u.i    = o->u.i    }); return;
	case QDF_TYPE_SIZE:   qdf_print_token(f, & (struct token) { TOK_SIZE,   .u.z    = o->u.z    }); return;
	case QDF_TYPE_REAL:   qdf_print_token(f, & (struct token) { TOK_REAL,   .u.n    = o->u.n    }); return;
	case QDF_TYPE_STRING: qdf_print_token(f, & (struct token) { TOK_STRING, .u.s    = o->u.s    }); return;
	case QDF_TYPE_BIN:    qdf_print_token(f, & (struct token) { TOK_BIN,    .u.data = o->u.data }); return;
	case QDF_TYPE_NAME:   qdf_print_token(f, & (struct token) { TOK_NAME,   .u.name = o->u.name }); return;

	case QDF_TYPE_ARRAY:  qdf_print_array (f, &o->u.a);  return;
	case QDF_TYPE_DICT:   qdf_print_dict  (f, &o->u.d);  return;
	case QDF_TYPE_STREAM: qdf_print_stream(f, &o->u.st); return;

	default:
		assert(!"unreached");
		return;
	}
}

void
qdf_print_def(FILE *f, unsigned id, const struct qdf_object *o)
{
	const unsigned gen = 0;

	assert(f != NULL);
	assert(o != NULL);

	qdf_print_token(f, & (struct token) { TOK_DEF_OPEN, .u.ref = { id, gen } });
	qdf_print_object(f, o);
	qdf_print_token(f, & (struct token) { TOK_DEF_CLOSE });
}

void
qdf_print_array(FILE *f, const struct qdf_array *a)
{
	size_t i;

	assert(f != NULL);
	assert(a != NULL);

	qdf_print_token(f, & (struct token) { TOK_ARRAY_OPEN });

	for (i = 0; i < a->n; i++) {
		qdf_print_object(f, &a->o[i]);
	}

	qdf_print_token(f, & (struct token) { TOK_ARRAY_CLOSE });
}

void
qdf_print_dict(FILE *f, const struct qdf_dict *d)
{
	size_t i, j;

	assert(f != NULL);
	assert(d != NULL);

	qdf_print_token(f, & (struct token) { TOK_DICT_OPEN });

	/* ISO PDF 2.0 7.3.7 "A dictonary whose value is null ... shall be
	 * treated the same as if the entry does not exist." */

	for (i = 0, j = 0; i < d->n; i++) {
		if (d->e[i].o.type == QDF_TYPE_NULL) {
			continue;
		}

		qdf_print_token(f, & (struct token) { TOK_NAME, .u.name = d->e[i].name });
		qdf_print_object(f, &d->e[i].o);

		j++;
	}

	qdf_print_token(f, & (struct token) { TOK_DICT_CLOSE });
}

static struct qdf_object
devolve(const struct qdf_array *a)
{
	assert(a != NULL);

	switch (a->n) {
	case 0:  return (struct qdf_object) { .type = QDF_TYPE_NULL };
	case 1:  return a->o[0];
	default: return (struct qdf_object) { .type = QDF_TYPE_ARRAY, .u.a = *a };
	}
}

static bool
at_least_one_is_non_null(const struct qdf_object *a, size_t n)
{
	size_t i;

	assert(a != NULL);

	for (i = 0; i < n; i++) {
		if (a[i].type != QDF_TYPE_NULL) {
			return true;
		}
	}

	return false;
}

static void
qdf_print_stream_filters(FILE *f,
	size_t length, const struct qdf_filter_array *a,
	const char *filter_name, const char *decodeparams_name)
{
	struct qdf_entry e[3];
	size_t i;
	size_t k;

	assert(f != NULL);
	assert(a != NULL);
	assert(filter_name != NULL);
	assert(decodeparams_name != NULL);

	k = 0;

	e[k].name   = "Length";
	e[k].o.type = QDF_TYPE_SIZE;
	e[k].o.u.z  = length;

	k++;

	/*
	 * Single-item arrays are handled by devolve().
	 * These elements are optional, so we take advantage of an element
	 * being null to omit an empty array when printing the dict.
	 */

	/*
	 * ISO PDF 2.0 7.3.8.2 t5 /Filters "The name, or an array of zero,
	 * one or several names, of filter(s) ... Multiple filters shall
	 * be specified in the order in which they are to be applied.
	 */

	struct qdf_object filters[a->n];

	for (i = 0; i < a->n; i++) {
		filters[i].type   = QDF_TYPE_NAME;
		filters[i].u.name = qdf_filter_name(a->a[i].type);
	}

	e[k].name = filter_name;
	e[k].o    = devolve(& (struct qdf_array) { a->n, filters });

	k++;

	struct qdf_object decodeparams[a->n];

	/*
	 * Storage for the elements within the /DecodeParms dict.
	 * XXX: This is worst case; could count exactly
	 */
	struct qdf_entry l[a->n * QDF_PARAMS_MAX];
	struct qdf_entry *p;

	p = l;

	/*
	 * ISO PDF 2.0 7.3.8.2 t5 /DecodeParms "If ... any of the filters
	 * has parameters set to nondefault values, DecodeParms shall be
	 * an array ... in the same order as the Filter array:
	 * either the parameter dictionary for that filter, or the null
	 * object if that filter has no parameters
	 * (or if all of its parameters have their default values)."
	 */

	for (i = 0; i < a->n; i++) {
		decodeparams[i] = qdf_filter_to_object(&a->a[i], p);
		if (decodeparams[i].type == QDF_TYPE_DICT) {
			p += decodeparams[i].u.d.n;
		}
	}

	/*
	 * ISO PDF 2.0 7.3.8.2 t5 /DecodeParms "If there is only
	 * one filter and that filter has parameters, DecodeParms
	 * shall be set to the filter's parameter dictionary ..."
	 *
	 * ISO PDF 2.0 7.3.8.2 t5 /DecodeParms "If none of the filters
	 * have parameters, or if all their parameters have default values,
	 * the DecodeParms entry may be omitted."
	 *
	 * Default values are handled by qdf_filter_to_object().
	 */

	if (at_least_one_is_non_null(decodeparams, a->n)) {
		e[k].name = decodeparams_name;
		e[k].o    = devolve(& (struct qdf_array) { a->n, decodeparams });

		k++;
	}

	/*
	 * TODO: (optional)
	 * ISO PDF 2.0 7.3.8.2 t5 /DL "A non-negative integer representing
	 * the number of bytes in the decoded (defiltered) stream.
	 * This value is only a hint; ..."
	 */

	/*
	 * TODO: merge in a stream's own extra dict entries - I think each
	 * stream can provide its own. Then check e[] for unique names
	 */

	qdf_print_dict(f, & (struct qdf_dict) { k, e });
}

static bool
qdf_print_filter_encoded(FILE *f,
	const void *p, size_t n,
	const struct qdf_filter_array *a)
{
	size_t i;

	assert(f != NULL);
	assert(p != NULL);
	assert(a != NULL);

	for (i = 0; i < a->n; i++) {
		const void *out;
		size_t outsz;
		bool r;

		r = qdf_filter_encode(&a->a[i], p, n, &out, &outsz);

		if (i > 0) {
			free((void *) p);
		}

		if (!r) {
			return false;
		}

		p = out;
		n = outsz;
	}

	qdf_print_token(f, & (struct token) { TOK_RAW, .u.data = { p, n } });

	if (i > 0) {
		free((void *) p);
	}

	return true;
}

bool
qdf_print_stream(FILE *f, const struct qdf_stream *st)
{
	assert(f != NULL);
	assert(st != NULL);

	/* XXX: /Length is incorrect here; it should be the *encoded* length */
	qdf_print_stream_filters(f,
		st->data.n, &st->filters,
		"Filter", "DecodeParams");

	qdf_print_token(f, & (struct token) { TOK_STREAM_OPEN });

	if (!qdf_print_filter_encoded(f, st->data.p, st->data.n, &st->filters)) {
		return false;
	}

	qdf_print_token(f, & (struct token) { TOK_STREAM_CLOSE });

	return true;
}

