/*
 * Copyright 2018 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef LIBQDF_PARAMS_INTERNAL_H
#define LIBQDF_PARAMS_INTERNAL_H

bool parameq_lzw_flate(const struct qdf_param_lzw_flate *a, const struct qdf_param_lzw_flate *b);
bool parameq_fax(const struct qdf_param_fax             *a, const struct qdf_param_fax       *b);
bool parameq_jbig2(const struct qdf_param_jbig2         *a, const struct qdf_param_jbig2     *b);
bool parameq_dct(const struct qdf_param_dct             *a, const struct qdf_param_dct       *b);

struct qdf_dict param_dict_lzw_flate(const struct qdf_param_lzw_flate *p, struct qdf_entry e[]);
struct qdf_dict param_dict_fax      (const struct qdf_param_fax       *p, struct qdf_entry e[]);
struct qdf_dict param_dict_jbig2    (const struct qdf_param_jbig2     *p, struct qdf_entry e[]);
struct qdf_dict param_dict_dct      (const struct qdf_param_dct       *p, struct qdf_entry e[]);

#endif

