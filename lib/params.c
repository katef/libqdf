/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <qdf/version.h>
#include <qdf/types.h>
#include <qdf/params.h>

const struct qdf_param_lzw_flate qdf_param_lzw_flate_default = {
	.predictor          = 1,
	.colors             = 1,
	.bits_per_component = 8,
	.columns            = 1,
	.early_change       = 1
};

const struct qdf_param_fax qdf_param_fax_default = {
	.k                         = 0,
	.end_of_line               = false,
	.encoded_byte_align        = false,
	.columns                   = 1728,
	.rows                      = 0,
	.end_of_block              = true,
	.black_is_one              = false,
	.damaged_rows_before_error = 0
};

const struct qdf_param_jbig2 qdf_param_jbig2_default = {
	.dummy = 0 /* TODO */
};

const struct qdf_param_dct qdf_param_dct_default = {
	.color_transform = 0 /* XXX: not actually specified */
};

