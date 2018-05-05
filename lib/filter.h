/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_FILTER_INTERNAL_H
#define LIBQDF_FILTER_INTERNAL_H

struct qdf_object
qdf_filter_to_object(const struct qdf_filter *f, struct qdf_entry e[]);

bool
qdf_filter_encode(const struct qdf_filter *f,
	const void *in, size_t insz,
	const void **out, size_t *outsz);

bool
qdf_filter_decode(const struct qdf_filter *f,
	const void *in, size_t insz,
	const void **out, size_t *outsz);

#endif

