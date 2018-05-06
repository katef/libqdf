/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_PRINT_H
#define LIBQDF_PRINT_H

void
qdf_print_space(FILE *f);

void
qdf_print_comment(FILE *f, const char *s);

void
qdf_print_null(FILE *f);

void
qdf_print_bool(FILE *f, bool v);

void
qdf_print_int(FILE *f, qdf_int n);

void
qdf_print_size(FILE *f, size_t n);

void
qdf_print_real(FILE *f, qdf_real n);

void
qdf_print_string(FILE *f, const char *s);

void
qdf_print_bin(FILE *f, const void *p, size_t n);

void
qdf_print_raw(FILE *f, const void *p, size_t n);

void
qdf_print_name(FILE *f, const char *name);

void
qdf_print_def_open(FILE *f, unsigned id, unsigned gen);

void
qdf_print_def_close(FILE *f);

void
qdf_print_ref(FILE *f, unsigned id);

void
qdf_print_array_open(FILE *f);

void
qdf_print_array_close(FILE *f);

void
qdf_print_dict_open(FILE *f);

void
qdf_print_dict_close(FILE *f);

void
qdf_print_stream_open(FILE *f);

void
qdf_print_stream_close(FILE *f);

#endif

