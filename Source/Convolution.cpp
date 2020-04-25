// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvolutionBase.hpp"
#include "Utility.hpp"

extern "C"
{
    #include "ConvCodes.h"
};

#include <json.hpp>

#include <Pothos/Callable.hpp>
#include <Pothos/Exception.hpp>
#include <Pothos/Plugin.hpp>

#include <Poco/Format.h>
#include <Poco/String.h>

#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

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
using ConvCodeMapPair = std::unordered_map<std::string, const lte_conv_code*>::value_type;

// The generator polynomial is stored in a static array of size 4, but the
// values may not be of size 4. The simplest workaround for the case where
// existing values exist is just to store the array lengths.
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

class Convolution: public ConvolutionBase
{
public:
    static Pothos::Block* make(const std::string& standard, bool isEncoder)
    {
        auto mapIter = ConvCodeMap.find(standard);
        if(ConvCodeMap.end() != mapIter)
        {
            return new Convolution(
                           standard,
                           const_cast<lte_conv_code*>(mapIter->second),
                           isEncoder);
        }

        throw Pothos::InvalidArgumentException("Invalid standard: "+standard);
    }

    Convolution(const std::string& standard, lte_conv_code* pConvCode, bool isEncoder):
        ConvolutionBase(pConvCode, isEncoder),
        _standard(standard)
    {
        auto genArrLengthsIter = GenArrLengthsMap.find(_standard);
        if(GenArrLengthsMap.end() != genArrLengthsIter)
        {
            _genArrLength = genArrLengthsIter->second;
        }
        else
        {
            throw Pothos::AssertionViolationException(
                      "Convolution::Convolution",
                      "Could not find GenArrLengthsMap entry for "+_standard);
        }

        this->registerCall(this, POTHOS_FCN_TUPLE(Convolution, overlay));
        this->registerCall(this, POTHOS_FCN_TUPLE(Convolution, standard));
    }

    std::string overlay() const
    {
        nlohmann::json json;
        auto& params = json["params"];

        nlohmann::json standardParam;
        standardParam["key"] = "standard";
        standardParam["widgetType"] = "ComboBox";
        standardParam["widgetKwargs"]["editable"] = false;

        auto& standardParamOpts = standardParam["options"];
        for(const auto& mapPair: ConvCodeMap)
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

    std::string standard() const
    {
        return _standard;
    }

private:
    std::string _standard;
};

//
// Registrations
//

static std::string convertStandardName(const std::string& standardName)
{
    static const std::vector<std::string> charsToReplace{" ","-","."};
    auto convertedStandardName = Poco::toLower(standardName);
    for(const auto& charToReplace: charsToReplace)
    {
        Poco::replaceInPlace(convertedStandardName, charToReplace, std::string("_"));
    }

    return convertedStandardName;
}

static std::vector<Pothos::BlockRegistry> _getConvolutionBlockRegistries()
{
    auto convCodeMapPairToBlockRegistry = [&](const ConvCodeMapPair& mapPair, bool isEncoder) -> Pothos::BlockRegistry
    {
        const auto& standardName = mapPair.first;
        const auto convertedStandardName = convertStandardName(standardName);

        return Pothos::BlockRegistry(
                   ("/fec/"+convertedStandardName+(isEncoder ? "_encoder" : "_decoder")),
                   Pothos::Callable(&Convolution::make)
                       .bind(standardName, 0)
                       .bind(isEncoder, 1));
    };

    auto convCodeMapPairToEncoderBlockRegistry = std::bind(convCodeMapPairToBlockRegistry, std::placeholders::_1, true);
    auto convCodeMapPairToDecoderBlockRegistry = std::bind(convCodeMapPairToBlockRegistry, std::placeholders::_1, false);

    std::vector<Pothos::BlockRegistry> blockRegistries;
    blockRegistries.reserve(ConvCodeMap.size()*2);

    std::transform(
        ConvCodeMap.begin(),
        ConvCodeMap.end(),
        std::back_inserter(blockRegistries),
        convCodeMapPairToEncoderBlockRegistry);
    std::transform(
        ConvCodeMap.begin(),
        ConvCodeMap.end(),
        std::back_inserter(blockRegistries),
        convCodeMapPairToDecoderBlockRegistry);

    return blockRegistries;
}

static const auto convolutionBlockRegistries = _getConvolutionBlockRegistries();
