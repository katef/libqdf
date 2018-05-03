/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <qdf/version.h>
#include <qdf/types.h>
#include <qdf/params.h>

#include "params.h"

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

bool
parameq_lzw_flate(
	const struct qdf_param_lzw_flate *a,
	const struct qdf_param_lzw_flate *b)
{
	assert(a != NULL);
	assert(b != NULL);

	if (a->predictor          != b->predictor         ) return false;
	if (a->colors             != b->colors            ) return false;
	if (a->bits_per_component != b->bits_per_component) return false;
	if (a->columns            != b->columns           ) return false;
	if (a->early_change       != b->early_change      ) return false;

	return true;
}

bool
parameq_fax(
	const struct qdf_param_fax *a,
	const struct qdf_param_fax *b)
{
	assert(a != NULL);
	assert(b != NULL);

	if (a->k                         != b->k                        ) return false;
	if (a->end_of_line               != b->end_of_line              ) return false;
	if (a->encoded_byte_align        != b->encoded_byte_align       ) return false;
	if (a->columns                   != b->columns                  ) return false;
	if (a->rows                      != b->rows                     ) return false;
	if (a->end_of_block              != b->end_of_block             ) return false;
	if (a->black_is_one              != b->black_is_one             ) return false;
	if (a->damaged_rows_before_error != b->damaged_rows_before_error) return false;

	return true;
}

bool
parameq_jbig2(
	const struct qdf_param_jbig2 *a,
	const struct qdf_param_jbig2 *b)
{
	assert(a != NULL);
	assert(b != NULL);

	/* TODO */

	return true;
}

bool
parameq_dct(
	const struct qdf_param_dct *a,
	const struct qdf_param_dct *b)
{
	assert(a != NULL);
	assert(b != NULL);

	if (a->color_transform != b->color_transform) return false;

	return true;
}

struct qdf_dict
param_dict_lzw_flate(const struct qdf_param_lzw_flate *p, struct qdf_entry e[])
{
	unsigned k;

	assert(p != NULL);
	assert(e != NULL);

	k = 0;

	e[k++] = (struct qdf_entry) { "Predictor",        (struct qdf_object) { QDF_TYPE_INT, .u.i = p->predictor          } };
	e[k++] = (struct qdf_entry) { "Colors",           (struct qdf_object) { QDF_TYPE_INT, .u.i = p->colors             } };
	e[k++] = (struct qdf_entry) { "BitsPerComponent", (struct qdf_object) { QDF_TYPE_INT, .u.i = p->bits_per_component } };
	e[k++] = (struct qdf_entry) { "Columns",          (struct qdf_object) { QDF_TYPE_INT, .u.i = p->columns            } };
	e[k++] = (struct qdf_entry) { "EarlyChange",      (struct qdf_object) { QDF_TYPE_INT, .u.i = p->early_change       } };

	return (struct qdf_dict) { k, e };
}

struct qdf_dict
param_dict_fax(const struct qdf_param_fax *p, struct qdf_entry e[])
{
	unsigned k;

	assert(p != NULL);
	assert(e != NULL);

	k = 0;

	e[k++] = (struct qdf_entry) { "K",                      (struct qdf_object) { QDF_TYPE_INT,  .u.i = p->k                         } };
	e[k++] = (struct qdf_entry) { "EndOfLine",              (struct qdf_object) { QDF_TYPE_BOOL, .u.i = p->end_of_line               } };
	e[k++] = (struct qdf_entry) { "EncodedByteAlign",       (struct qdf_object) { QDF_TYPE_BOOL, .u.i = p->encoded_byte_align        } };
	e[k++] = (struct qdf_entry) { "Columns",                (struct qdf_object) { QDF_TYPE_INT,  .u.i = p->columns                   } };
	e[k++] = (struct qdf_entry) { "Rows",                   (struct qdf_object) { QDF_TYPE_INT,  .u.i = p->rows                      } };
	e[k++] = (struct qdf_entry) { "EndOfBlock",             (struct qdf_object) { QDF_TYPE_BOOL, .u.i = p->end_of_block              } };
	e[k++] = (struct qdf_entry) { "BlackIsOne",             (struct qdf_object) { QDF_TYPE_BOOL, .u.i = p->black_is_one              } };
	e[k++] = (struct qdf_entry) { "DamagedRowsBeforeError", (struct qdf_object) { QDF_TYPE_INT,  .u.i = p->damaged_rows_before_error } };

	return (struct qdf_dict) { k, e };
}

struct qdf_dict
param_dict_jbig2(const struct qdf_param_jbig2 *p, struct qdf_entry e[])
{
	unsigned k;

	assert(p != NULL);
	assert(e != NULL);

	k = 0;

	/* TODO */

	return (struct qdf_dict) { k, e };
}

struct qdf_dict
param_dict_dct(const struct qdf_param_dct *p, struct qdf_entry e[])
{
	unsigned k;

	assert(p != NULL);
	assert(e != NULL);

	k = 0;

	e[k++] = (struct qdf_entry) { "ColorTransform", (struct qdf_object) { QDF_TYPE_INT, .u.i = p->color_transform } };

	return (struct qdf_dict) { k, e };
}

