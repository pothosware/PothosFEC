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
        {"WiMax FCH", get_wimax_conv_fch()},
        //{"", get_gmr1_conv_tch3_speech()},
        {"LTE PBCH", get_lte_conv_pbch()},
        //{"", get_conv_trunc()},
    };

    return ConvCodeMap;
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
