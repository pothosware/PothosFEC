// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvCommon.hpp"
#include "ConvolutionalEncoderBase.hpp"
#include "Utility.hpp"

#include <Pothos/Callable.hpp>
#include <Pothos/Exception.hpp>

#include <vector>

class StandardConvolutionalEncoder: public ConvolutionalEncoderBase
{
public:
    static Pothos::Block* make(const std::string& standard)
    {
        const auto& convCodeMap = getConvCodeMap();
        auto mapIter = convCodeMap.find(standard);
        if(convCodeMap.end() != mapIter)
        {
            return new StandardConvolutionalEncoder(const_cast<lte_conv_code*>(mapIter->second));
        }

        throw Pothos::AssertionViolationException("Invalid standard");
    }

    StandardConvolutionalEncoder(lte_conv_code* pConvCode):
        ConvolutionalEncoderBase(pConvCode)
    {
        this->registerCall(this, POTHOS_FCN_TUPLE(StandardConvolutionalEncoder, overlay));
    }

    std::string overlay() const
    {
        return convCodeMapOverlay();
    }
};

#define REGISTRY(pathName, key) \
    Pothos::BlockRegistry( \
        "/fec/" pathName "_conv_encoder", \
        Pothos::Callable(&StandardConvolutionalEncoder::make) \
            .bind(key, 0))

static const std::vector<Pothos::BlockRegistry> standardConvEncoders =
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
    REGISTRY("wimax_fch",       "Wimax FCH"),
    REGISTRY("lte_pbch",        "LTE PBCH"),
};
