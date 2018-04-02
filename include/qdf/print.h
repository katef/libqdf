/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_PRINT_H
#define LIBQDF_PRINT_H

void
qdf_vprint_comment(FILE *f, const char *fmt, va_list ap);

void
qdf_print_comment(FILE *f, const char *fmt, ...);

void
qdf_print_null(FILE *f);

void
qdf_print_bool(FILE *f, bool v);

void
qdf_print_int(FILE *f, int32_t n);

void
qdf_print_real(FILE *f, double n);

void
qdf_print_string(FILE *f, const char *s);

void
qdf_print_bin(FILE *f, const void *p, size_t n);

void
qdf_print_name(FILE *f, const char *name);

void
qdf_print_array(FILE *f, const struct qdf_array *a);

void
qdf_print_object(FILE *f, const struct qdf_object *o);

#endif

