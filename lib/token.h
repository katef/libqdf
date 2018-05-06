/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_TOKEN_INTERNAL_H
#define LIBQDF_TOKEN_INTERNAL_H

enum token_type {
	TOK_VER,
	TOK_EOF,
	TOK_BR,
	TOK_COMMENT,

	TOK_NULL,
	TOK_BOOL,
	TOK_INT,
	TOK_SIZE,
	TOK_REAL,
	TOK_STRING,
	TOK_BIN,
	TOK_RAW, /* stream data */
	TOK_NAME,
	TOK_REF,

	TOK_DEF_OPEN,    TOK_DEF_CLOSE,
	TOK_ARRAY_OPEN,  TOK_ARRAY_CLOSE,
	TOK_DICT_OPEN,   TOK_DICT_CLOSE,
	TOK_STREAM_OPEN, TOK_STREAM_CLOSE
};

struct qdf_ref {
	unsigned id;
	unsigned gen;
};

struct token {
	enum token_type type;
	union {
		enum qdf_version ver;
		const char *comment;
		struct qdf_ref ref;
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

void
qdf_print_token(FILE *f, const struct token *t);

#endif

