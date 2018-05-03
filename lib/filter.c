/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <qdf/version.h>
#include <qdf/types.h>
#include <qdf/params.h>
#include <qdf/filter.h>

#include "params.h"
#include "filter.h"

const char *
qdf_filter_name(enum qdf_filter_type type)
{
	switch (type) {
	case QDF_FILTER_ASCII_HEX: return "ASCIIHexDecode";
	case QDF_FILTER_ASCII_85:  return "ASCII85Decode";
	case QDF_FILTER_LZW:       return "LZWDecode";
	case QDF_FILTER_FLATE:     return "FlateDecode";
	case QDF_FILTER_RLE:       return "RunLengthDecode";
	case QDF_FILTER_FAX:       return "CCITTFaxDecode";
	case QDF_FILTER_JBIG2:     return "JBIG2Decode";
	case QDF_FILTER_DCT:       return "DCTDecode";
	case QDF_FILTER_JPX:       return "JPXDecode";

	/* Not implemented: case QDF_FILTER_CRYPT: */

	default:
		return "?";
	}
}

struct qdf_object
qdf_filter_to_object(const struct qdf_filter *f, struct qdf_entry e[])
{
	assert(f != NULL);
	assert(e != NULL);

	/*
	 * ISO PDF 2.0 7.3.8.2 t5 /DecodeParms "... unless all the filter’s
	 * parameters have their default values, in which case the DecodeParms
	 * entry may be omitted."
	 *
	 * As far as I can see, individual parameters may *not* be elided
	 * if their values are equal to a default value. The wording about
	 * only talks about eliding the entire dict when all its parameters
	 * are defaults. So if one parameter has a default value and the
	 * others do not, all parameters must be output.
	 */

	switch (f->type) {
	case QDF_FILTER_ASCII_HEX:
	case QDF_FILTER_ASCII_85:
	case QDF_FILTER_RLE:
	case QDF_FILTER_JPX:
		goto none;

	case QDF_FILTER_LZW:
	case QDF_FILTER_FLATE: if (parameq_lzw_flate(&f->u.lzw_flate, &qdf_param_lzw_flate_default)) goto omit; break;
	case QDF_FILTER_FAX:   if (parameq_fax      (&f->u.fax,       &qdf_param_fax_default      )) goto omit; break;
	case QDF_FILTER_JBIG2: if (parameq_jbig2    (&f->u.jbig2,     &qdf_param_jbig2_default    )) goto omit; break;
	case QDF_FILTER_DCT:   if (parameq_dct      (&f->u.dct,       &qdf_param_dct_default      )) goto omit; break;

	/* Not implemented: case QDF_FILTER_CRYPT: */

	default:
		abort();
	}

	switch (f->type) {
	case QDF_FILTER_LZW:
	case QDF_FILTER_FLATE: return (struct qdf_object) { QDF_TYPE_DICT, .u.d = param_dict_lzw_flate(&f->u.lzw_flate, e) };
	case QDF_FILTER_FAX:   return (struct qdf_object) { QDF_TYPE_DICT, .u.d = param_dict_fax      (&f->u.fax,       e) };
	case QDF_FILTER_JBIG2: return (struct qdf_object) { QDF_TYPE_DICT, .u.d = param_dict_jbig2    (&f->u.jbig2,     e) };
	case QDF_FILTER_DCT:   return (struct qdf_object) { QDF_TYPE_DICT, .u.d = param_dict_dct      (&f->u.dct,       e) };

	/* Not implemented: case QDF_FILTER_CRYPT: */

	default:
		abort();
	}

none:
omit:

	return (struct qdf_object) { .type = QDF_TYPE_NULL };
}

