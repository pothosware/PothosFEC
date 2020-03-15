// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvCommon.hpp"
#include "ConvolutionBase.hpp"
#include "Utility.hpp"

#include <Pothos/Callable.hpp>
#include <Pothos/Exception.hpp>

#include <vector>

class StandardConvolution: public ConvolutionBase
{
public:
    static Pothos::Block* make(const std::string& standard, bool isEncoder)
    {
        const auto& convCodeMap = getConvCodeMap();
        auto mapIter = convCodeMap.find(standard);
        if(convCodeMap.end() != mapIter)
        {
            return new StandardConvolution(
                           standard,
                           const_cast<lte_conv_code*>(mapIter->second),
                           isEncoder);
        }

        throw Pothos::AssertionViolationException(
                  "StandardConvolution::make",
                  "Invalid standard");
    }

    StandardConvolution(const std::string& standard, lte_conv_code* pConvCode, bool isEncoder):
        ConvolutionBase(pConvCode, isEncoder),
        _standard(standard)
    {
        const auto& genArrLengthsMap = getGenArrLengthsMap();
        auto genArrLengthsIter = genArrLengthsMap.find(_standard);
        if(genArrLengthsMap.end() != genArrLengthsIter)
        {
            _genArrLength = genArrLengthsIter->second;
        }
        else
        {
            throw Pothos::AssertionViolationException(
                      "StandardConvolution::StandardConvolution",
                      "Could not find GenArrLengthsMap entry for "+_standard);
        }

        this->registerCall(this, POTHOS_FCN_TUPLE(StandardConvolution, overlay));
        this->registerCall(this, POTHOS_FCN_TUPLE(StandardConvolution, standard));
    }

    std::string overlay() const
    {
        return convCodeMapOverlay();
    }

    std::string standard() const
    {
        return _standard;
    }

private:
    std::string _standard;
};

#define REGISTRY(pathName, key) \
    Pothos::BlockRegistry( \
        "/fec/" pathName "_conv_encoder", \
        Pothos::Callable(&StandardConvolution::make) \
            .bind(key, 0) \
            .bind(true, 1)), \
    Pothos::BlockRegistry( \
        "/fec/" pathName "_conv_decoder", \
        Pothos::Callable(&StandardConvolution::make) \
            .bind(key, 0) \
            .bind(false, 1))

static const std::vector<Pothos::BlockRegistry> standardConvCoders =
{
    REGISTRY("gsm_xcch",        "GSM XCCH"),
    REGISTRY("gprs_cs2",        "GPRS CS2"),
    REGISTRY("gprs_cs3",        "GPRS CS3"),
    REGISTRY("gsm_rach",        "GSM RACH"),
    REGISTRY("gsm_sch",         "GSM SCH"),
    REGISTRY("gsm_tch_fr",      "GSM TCH-FR"),
    REGISTRY("gsm_tch_hr",      "GSM TCH-HR"),
    REGISTRY("gsm_tch_afs12-2", "GSM TCH-AFS12.2"),
    REGISTRY("gsm_tch_afs10-2", "GSM TCH-AFS10.2"),
    REGISTRY("gsm_tch_afs7-95", "GSM TCH-AFS7.95"),
    REGISTRY("gsm_tch_afs7-4",  "GSM TCH-AFS7.4"),
    REGISTRY("gsm_tch_afs6-7",  "GSM TCH-AFS6.7"),
    REGISTRY("gsm_tch_afs5-9",  "GSM TCH-AFS5.9"),
    REGISTRY("gsm_tch_ahs7-95", "GSM TCH-AHS7.95"),
    REGISTRY("gsm_tch_ahs7-4",  "GSM TCH-AHS7.4"),
    REGISTRY("gsm_tch_ahs6-7",  "GSM TCH-AHS6.7"),
    REGISTRY("gsm_tch_ahs5-9",  "GSM TCH-AHS5.9"),
    REGISTRY("gsm_tch_ahs5-15", "GSM TCH-AHS5.15"),
    REGISTRY("gsm_tch_ahs4-75", "GSM TCH-AHS4.75"),
    REGISTRY("wimax_fch",       "Wimax FCH"),
    REGISTRY("lte_pbch",        "LTE PBCH"),
};
