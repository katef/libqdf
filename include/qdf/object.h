/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_OBJECT_H
#define LIBQDF_OBJECT_H

void
qdf_vprintf_comment(FILE *f, const char *fmt, va_list ap);

void
qdf_printf_comment(FILE *f, const char *fmt, ...);

void
qdf_print_object(FILE *f, const struct qdf_object *o);

void
qdf_print_def(FILE *f, unsigned id, const struct qdf_object *o);

void
qdf_print_array(FILE *f, const struct qdf_array *a);

void
qdf_print_dict(FILE *f, const struct qdf_dict *d);

bool
qdf_print_stream(FILE *f, const struct qdf_stream *st);

#endif

