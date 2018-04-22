/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_PARAMS_H
#define LIBQDF_PARAMS_H

/* ISO PDF 2.0 7.4.4.3 t8 "Optional parameters for LZWDecode
 * and FlateDecode filters" */
struct qdf_param_lzw_flate {
	qdf_int predictor;
	qdf_int colors;
	qdf_int bits_per_component;
	qdf_int columns;
	qdf_int early_change;
};
extern const struct qdf_param_lzw_flate qdf_param_lzw_flate_default;

/* ISO PDF 2.0 7.4.6 t11 "Optional parameters for the CCITTFaxDecode filter" */
struct qdf_param_fax {
	qdf_int k;
	bool end_of_line;
	bool encoded_byte_align;
	qdf_int columns;
	qdf_int rows;
	bool end_of_block;
	bool black_is_one;
	qdf_int damaged_rows_before_error;
};
extern const struct qdf_param_fax qdf_param_fax_default;

/* ISO PDF 2.0 7.4.7 t12 "Optional parameters for the JBIG2Decode filter" */
struct qdf_param_jbig2 {
/* TODO
	struct qdf_stream stream;
*/
	int dummy;
};
extern const struct qdf_param_jbig2 qdf_param_jbig2_default;

/* ISO PDF 2.0 7.4.8 t13 "Optional parameters for the DCTDecode filter" */
struct qdf_param_dct {
	qdf_int color_transform;
};
extern const struct qdf_param_dct qdf_param_dct_default;

/* ISO PDF 2.0 7.4.9 "JPXDecode filter" no optional parameters */

/* ISO PDF 2.0 7.4.10 t14 "Optional parameters for Crypt filters"
 * Not implemented */

#endif

