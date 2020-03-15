// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvCommon.hpp"

extern "C"
{
#include "ConvCodes.h"
}

#include <json.hpp>

#include <string>
#include <unordered_map>

const std::unordered_map<std::string, const lte_conv_code*>& getConvCodeMap()
{
    static const std::unordered_map<std::string, const lte_conv_code*> ConvCodeMap =
    {
        {"GSM XCCH", get_gsm_conv_xcch()},
        {"GPRS CS2", get_gsm_conv_cs2()},
        {"GPRS CS3", get_gsm_conv_cs3()},
        {"GSM RACH", get_gsm_conv_rach()},
        {"GSM SCH", get_gsm_conv_sch()},
        {"GSM TCH-FR", get_gsm_conv_tch_fr()},
        {"GSM TCH-HR", get_gsm_conv_tch_hr()},
        {"GSM TCH-AFS12.2", get_gsm_conv_tch_afs_12_2()},
        {"GSM TCH-AFS10.2", get_gsm_conv_tch_afs_10_2()},
        {"GSM TCH-AFS7.95", get_gsm_conv_tch_afs_7_95()},
        {"GSM TCH-AFS7.4", get_gsm_conv_tch_afs_7_4()},
        {"GSM TCH-AFS6.7", get_gsm_conv_tch_afs_6_7()},
        {"GSM TCH-AFS5.9", get_gsm_conv_tch_afs_5_9()},
        //{"", get_gsm_conv_tch_afs_5_15()},
        //{"", get_gsm_conv_tch_afs_4_75()},
        {"GSM TCH-AHS7.95", get_gsm_conv_tch_ahs_7_95()},
        {"GSM TCH-AHS7.4", get_gsm_conv_tch_ahs_7_4()},
        {"GSM TCH-AHS6.7", get_gsm_conv_tch_ahs_6_7()},
        {"GSM TCH-AHS5.9", get_gsm_conv_tch_ahs_5_9()},
        {"GSM TCH-AHS5.15", get_gsm_conv_tch_ahs_5_15()},
        {"GSM TCH-AHS4.75", get_gsm_conv_tch_ahs_4_75()},
        {"WiMax FCH", get_wimax_conv_fch()},
        //{"", get_gmr1_conv_tch3_speech()},
        {"LTE PBCH", get_lte_conv_pbch()},
        //{"", get_conv_trunc()},
    };

    return ConvCodeMap;
}

// The generator polynomial is stored in a static array of size 4, but the
// values may not be of size 4. The simplest workaround for the case where
// existing values exist is just to store the array lengths.
const std::unordered_map<std::string, size_t>& getGenArrLengthsMap()
{
    static const std::unordered_map<std::string, size_t> GenArrLengthsMap =
    {
        {"GSM XCCH", 2},
        {"GPRS CS2", 2},
        {"GPRS CS3", 2},
        {"GSM RACH", 2},
        {"GSM SCH", 2},
        {"GSM TCH-FR", 2},
        {"GSM TCH-HR", 3},
        {"GSM TCH-AFS12.2", 2},
        {"GSM TCH-AFS10.2", 3},
        {"GSM TCH-AFS7.95", 3},
        {"GSM TCH-AFS7.4", 3},
        {"GSM TCH-AFS6.7", 4},
        {"GSM TCH-AFS5.9", 4},
        //{"", 0},
        //{"", 0},
        {"GSM TCH-AHS7.95", 2},
        {"GSM TCH-AHS7.4", 2},
        {"GSM TCH-AHS6.7", 2},
        {"GSM TCH-AHS5.9", 2},
        {"GSM TCH-AHS5.15", 3},
        {"GSM TCH-AHS4.75", 3},
        {"WiMax FCH", 2},
        //{"", 0},
        {"LTE PBCH", 3},
        //{"", 0},
    };

    return GenArrLengthsMap;
}

std::string convCodeMapOverlay()
{
    nlohmann::json json;
    auto& params = json["params"];

    nlohmann::json standardParam;
    standardParam["key"] = "standard";
    standardParam["widgetType"] = "ComboBox";
    standardParam["widgetKwargs"]["editable"] = false;

    auto& standardParamOpts = standardParam["options"];
    for(const auto& mapPair: getConvCodeMap())
    {
        const auto& standardName = mapPair.first;

        nlohmann::json option;
        option["name"] = standardName;
        option["value"] = "\""+standardName+"\"";
        standardParamOpts.push_back(option);
    }

    params.push_back(standardParam);
    return json.dump();
}
