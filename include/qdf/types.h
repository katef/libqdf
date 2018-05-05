/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_TYPES_H
#define LIBQDF_TYPES_H

struct qdf_filter;

/*
 * ISO PDF 2.0 7.3.1 "PDF includes eight basic types of objects:
 * Boolean values, Integer and Real numbers, Strings, Names,
 * Arrays, Dictionaries, Streams, and the null object."
 */

enum qdf_type {
	QDF_TYPE_BOOL,
	QDF_TYPE_INT,
	QDF_TYPE_SIZE, /* size_t */
	QDF_TYPE_REAL,
	QDF_TYPE_STRING,
	QDF_TYPE_BIN,
	QDF_TYPE_NAME,
	QDF_TYPE_ARRAY,
	QDF_TYPE_DICT,
	QDF_TYPE_STREAM,
	QDF_TYPE_NULL
};

/* ISO PDF 2.0 7.3.3 "The range and precision of numbers may be limited by the
 * internal representations used in the computer on which the PDF processor
 * is running; ..."
 * ISO PDF 2.0 Annex C.1 "Integer values (such as object numbers) can often be
 * expressed within 32 bits" */
typedef int_fast32_t qdf_int;
#define QDF_PRId     PRIdFAST32

/* ISO PDF 2.0 Annex C.1 "Modern computers often represent ... real numbers
 * using ... IEEE 754 single or double precision." */
typedef double       qdf_real;
#define QDF_PRIr     "f"

struct qdf_data {
	const void *p;
	size_t n;
};

struct qdf_dict {
	size_t n;
	struct qdf_entry *e;
};

struct qdf_array {
	size_t n;
	struct qdf_object *o;
};

struct qdf_filter_array {
	size_t n;
	struct qdf_filter *a;
};

struct qdf_stream {
	struct qdf_data data;
	struct qdf_filter_array filters;
};

struct qdf_object {
	enum qdf_type type;
	union {
		bool v;
		qdf_int i;
		size_t z;
		qdf_real n;
		const char *s;
		struct qdf_data data;
		const char *name;
		struct qdf_array a;
		struct qdf_dict d;
		struct qdf_stream st;
	} u;
};

struct qdf_entry {
	const char *name;
	struct qdf_object o;
};

#endif

