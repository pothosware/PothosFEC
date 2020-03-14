// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Utility.hpp"

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>

#include <string>
#include <vector>

extern "C"
{
#include <turbofec/conv.h>
}

class ConvolutionalEncoderBase: public Pothos::Block
{
public:
    ConvolutionalEncoderBase():
        Pothos::Block(),
        _pConvCode(nullptr)
    {
        this->setupInput(0, "uint8");
        this->setupOutput(0, "uint8");

        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoderBase, N));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoderBase, K));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoderBase, length));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoderBase, recursiveGeneratorPolynomial));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoderBase, generatorPolynomial));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoderBase, puncture));
        this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionalEncoderBase, terminationType));

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
            throw Pothos::AssertionViolationException("ConvolutionalEncoderBase::work() called without setting _pConvCode");
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
