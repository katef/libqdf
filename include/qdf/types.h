/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_TYPES_H
#define LIBQDF_TYPES_H

/*
 * ISO PDF 2.0 7.3.1 "PDF includes eight basic types of objects:
 * Boolean values, Integer and Real numbers, Strings, Names,
 * Arrays, Dictionaries, Streams, and the null object."
 */

enum qdf_type {
	QDF_TYPE_BOOL,
	QDF_TYPE_INT,
	QDF_TYPE_REAL,
	QDF_TYPE_STRING,
	QDF_TYPE_BIN,
	QDF_TYPE_NAME,
	QDF_TYPE_ARRAY,
	QDF_TYPE_DICT,
	QDF_TYPE_STREAM,
	QDF_TYPE_NULL
};

struct qdf_data {
	const void *p;
	size_t n;
};

struct qdf_object {
	enum qdf_type type;
	union {
		bool v;
		int32_t i;
		double n;
		const char *s;
		struct qdf_data data;
		const char *name;
		/* TODO: array, dict, stream */
	} u;
};

#endif

