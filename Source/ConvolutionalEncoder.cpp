// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Utility.hpp"

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>

#include <string>
#include <unordered_map>
#include <vector>

extern "C"
{
#include "ConvCodes.h"
#include <turbofec/conv.h>
}

static const std::unordered_map<std::string, const lte_conv_code*> ConvCodeMap =
{
    {"GSM XCCH", get_gsm_conv_xcch()},
    {"GPRS_CS2", get_gsm_conv_cs2()},
    {"GPRS_CS3", get_gsm_conv_cs3()},
    {"GSM_CONV_RACH", get_gsm_conv_rach()},
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

class ConvolutionalEncoder: public Pothos::Block
{
public:
    ConvolutionalEncoder(lte_conv_code* pConvCode):
        Pothos::Block(),
        _pConvCode(pConvCode)
    {
        if(!_pConvCode)
        {
            throw Pothos::AssertionViolationException("Factory passed in a null lte_conv_code");
        }

        this->setupInput(0, "uint8");
        this->setupOutput(0, "uint8");

        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoder, N));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoder, K));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoder, length));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoder, recursiveGeneratorPolynomial));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoder, generatorPolynomial));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoder, puncture));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoder, terminationType));

        this->registerProbe("N");
        this->registerProbe("K");
        this->registerProbe("length");
        this->registerProbe("recursiveGeneratorPolynomial");
        this->registerProbe("generatorPolynomial");
        this->registerProbe("puncture");
        this->registerProbe("terminationType");
    }

    int N() const
    {
        return _pConvCode->n;
    }

    int K() const
    {
        return _pConvCode->k;
    }

    int length() const
    {
        return _pConvCode->len;
    }

    unsigned recursiveGeneratorPolynomial() const
    {
        return _pConvCode->rgen;
    }

    std::vector<unsigned> generatorPolynomial() const
    {
        return std::vector<unsigned>(
                  _pConvCode->gen,
                  _pConvCode->gen+4);
    }

    std::vector<int> puncture() const
    {
        std::vector<int> ret;
        if(_pConvCode->punc)
        {
            // TODO: how best to find -1?
        }

        return ret;
    }

    std::string terminationType() const
    {
        std::string ret;

        switch(_pConvCode->term)
        {
            case ::CONV_TERM_FLUSH:
                ret = "Flush";
                break;

            case ::CONV_TERM_TAIL_BITING:
                ret = "Tail-biting";
                break;

            default:
                throw Pothos::AssertionViolationException("Invalid termination");
        }

        return ret;
    }

    void work() override
    {
        if(!_pConvCode)
        {
            throw Pothos::AssertionViolationException("ConvolutionalEncoder::work() called without setting _pConvCode");
        }

        const auto elems = this->workInfo().minElements;
        if(elems < static_cast<size_t>(_pConvCode->len))
        {
            return;
        }

        auto* input = this->input(0);
        auto* output = this->output(0);

        int encodeRet = ::lte_conv_encode(
                            _pConvCode,
                            input->buffer(),
                            output->buffer());
        throwOnErrCode(encodeRet);

        if(encodeRet > 0)
        {
            input->consume(_pConvCode->len);
            output->produce(encodeRet);
        }
    }

protected:
    lte_conv_code* _pConvCode;
};

//
// Factory/egistration
//

static Pothos::Block* makeConvolutionalEncoder(const std::string& name)
{
    auto mapIter = ConvCodeMap.find(name);
    if(ConvCodeMap.end() != mapIter)
    {
        // We know this won't be modified, but the class stores it non-const
        // to support a sublcass that does modify it.
        auto* pConvCode = const_cast<lte_conv_code*>(mapIter->second);

        return new ConvolutionalEncoder(pConvCode);
    }

    throw Pothos::InvalidArgumentException("Invalid code", name);
}

static Pothos::BlockRegistry registerConvolutionalEncoder(
    "/fec/convolutional_encoder",
    Pothos::Callable(&makeConvolutionalEncoder));
