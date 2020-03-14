#ifndef CONVCODES_H
#define CONVCODES_H

// Adapted from: https://github.com/ttsou/turbofec/blob/master/tests/codes.h

#include <turbofec/conv.h>

const struct lte_conv_code* get_gsm_conv_xcch();
const struct lte_conv_code* get_gsm_conv_cs2();
const struct lte_conv_code* get_gsm_conv_cs3();
const struct lte_conv_code* get_gsm_conv_rach();
const struct lte_conv_code* get_gsm_conv_sch();
const struct lte_conv_code* get_gsm_conv_tch_fr();
const struct lte_conv_code* get_gsm_conv_tch_hr();
const struct lte_conv_code* get_gsm_conv_tch_afs_12_2();
const struct lte_conv_code* get_gsm_conv_tch_afs_10_2();
const struct lte_conv_code* get_gsm_conv_tch_afs_7_95();
const struct lte_conv_code* get_gsm_conv_tch_afs_7_4();
const struct lte_conv_code* get_gsm_conv_tch_afs_6_7();
const struct lte_conv_code* get_gsm_conv_tch_afs_5_9();
const struct lte_conv_code* get_gsm_conv_tch_afs_5_15();
const struct lte_conv_code* get_gsm_conv_tch_afs_4_75();
const struct lte_conv_code* get_gsm_conv_tch_ahs_7_95();
const struct lte_conv_code* get_gsm_conv_tch_ahs_7_4();
const struct lte_conv_code* get_gsm_conv_tch_ahs_6_7();
const struct lte_conv_code* get_gsm_conv_tch_ahs_5_9();
const struct lte_conv_code* get_gsm_conv_tch_ahs_5_15();
const struct lte_conv_code* get_gsm_conv_tch_ahs_4_75();
const struct lte_conv_code* get_wimax_conv_fch();
const struct lte_conv_code* get_gmr1_conv_tch3_speech();
const struct lte_conv_code* get_lte_conv_pbch();
const struct lte_conv_code* get_conv_trunc();

#endif /* CONVCODES_H */
