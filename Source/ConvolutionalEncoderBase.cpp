// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvCommon.hpp"
#include "ConvolutionalEncoderBase.hpp"
#include "Utility.hpp"

#include <Pothos/Exception.hpp>

ConvolutionalEncoderBase::ConvolutionalEncoderBase(lte_conv_code* pConvCode):
    Pothos::Block(),
    _pConvCode(pConvCode),
    _genArrLength(4)
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

int ConvolutionalEncoderBase::N() const
{
    return _pConvCode->n;
}

int ConvolutionalEncoderBase::K() const
{
    return _pConvCode->k;
}

int ConvolutionalEncoderBase::length() const
{
    return _pConvCode->len;
}

unsigned ConvolutionalEncoderBase::recursiveGeneratorPolynomial() const
{
    return _pConvCode->rgen;
}

std::vector<unsigned> ConvolutionalEncoderBase::generatorPolynomial() const
{
    return std::vector<unsigned>(
              _pConvCode->gen,
              (_pConvCode->gen + _genArrLength));
}

std::vector<int> ConvolutionalEncoderBase::puncture() const
{
    std::vector<int> ret;
    if(_pConvCode->punc)
    {
        // Since we manage the underlying buffer, we should be able to assume
        // that there's a -1 somewhere in there.
        size_t negOnePos = 0;
        while(_pConvCode->punc[negOnePos] != -1) ++negOnePos;

        // This removes the -1 terminator, since that's an implementation
        // detail.
        ret = std::vector<int>(_pConvCode->punc, _pConvCode->punc+negOnePos);
    }

    return ret;
}

std::string ConvolutionalEncoderBase::terminationType() const
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

void ConvolutionalEncoderBase::work()
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
