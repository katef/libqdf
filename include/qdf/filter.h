/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_FILTER_H
#define LIBQDF_FILTER_H

enum qdf_filter_type {
	QDF_FILTER_ASCII_HEX,
	QDF_FILTER_ASCII_85,
	QDF_FILTER_LZW,
	QDF_FILTER_FLATE,
	QDF_FILTER_RLE,
	QDF_FILTER_FAX,
	QDF_FILTER_JBIG2,
	QDF_FILTER_DCT,
	QDF_FILTER_JPX
	/* Not implemented: QDF_FILTER_CRYPT */
};

struct qdf_filter {
	enum qdf_filter_type type;
	union {
		struct qdf_param_lzw_flate lzw_flate;
		struct qdf_param_fax fax;
		struct qdf_param_jbig2 jbig2;
		struct qdf_param_dct dct;
	} u;
};

const char *
qdf_filter_name(enum qdf_filter_type type);

#endif

