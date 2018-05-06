/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_PRINT_H
#define LIBQDF_PRINT_H

void
qdf_print_comment(FILE *f, const char *s);

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

