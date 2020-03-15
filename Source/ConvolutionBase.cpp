// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvolutionBase.hpp"
#include "Utility.hpp"

#include <Pothos/Exception.hpp>

ConvolutionBase::ConvolutionBase(lte_conv_code* pConvCode, bool isEncoder):
    Pothos::Block(),
    _pConvCode(pConvCode),
    _genArrLength(4),
    _isEncoder(isEncoder)
{
    this->setupInput(0, (_isEncoder ? "uint8" : "int8"));
    this->setupOutput(0, "uint8");

    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, N));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, K));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, length));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, recursiveGeneratorPolynomial));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, generatorPolynomial));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, puncture));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, terminationType));

    this->registerProbe("N");
    this->registerProbe("K");
    this->registerProbe("length");
    this->registerProbe("recursiveGeneratorPolynomial");
    this->registerProbe("generatorPolynomial");
    this->registerProbe("puncture");
    this->registerProbe("terminationType");
}

ConvolutionBase::~ConvolutionBase() {}

int ConvolutionBase::N() const
{
    return _pConvCode->n;
}

int ConvolutionBase::K() const
{
    return _pConvCode->k;
}

int ConvolutionBase::length() const
{
    return _pConvCode->len;
}

unsigned ConvolutionBase::recursiveGeneratorPolynomial() const
{
    return _pConvCode->rgen;
}

std::vector<unsigned> ConvolutionBase::generatorPolynomial() const
{
    return std::vector<unsigned>(
              _pConvCode->gen,
              (_pConvCode->gen + _genArrLength));
}

std::vector<int> ConvolutionBase::puncture() const
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

std::string ConvolutionBase::terminationType() const
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

void ConvolutionBase::work()
{
    if(_isEncoder) this->encoderWork();
    else           this->decoderWork();
}

void ConvolutionBase::encoderWork()
{
    const auto elems = this->workInfo().minElements;
    if(elems < static_cast<size_t>(_pConvCode->len))
    {
        return;
    }

    auto input = this->input(0);
    auto output = this->output(0);

    int encodeRet = ::lte_conv_encode(
                         _pConvCode,
                         input->buffer(),
                         output->buffer());
    throwOnErrCode(encodeRet);

    input->consume(_pConvCode->len);
    output->produce(encodeRet);
}

void ConvolutionBase::decoderWork()
{
    const auto elems = this->workInfo().minElements;
    if(elems < static_cast<size_t>(_pConvCode->len))
    {
        return;
    }

    auto input = this->input(0);
    auto output = this->output(0);

    int decodeRet = ::lte_conv_decode(
                         _pConvCode,
                         input->buffer(),
                         output->buffer());
    throwOnErrCode(decodeRet);

    input->consume(_pConvCode->len);
    output->produce(decodeRet);
}
