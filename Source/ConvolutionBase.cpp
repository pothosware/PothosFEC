// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvolutionBase.hpp"
#include "Utility.hpp"

#include <Poco/Format.h>
#include <Poco/NumberFormatter.h>

#include <Pothos/Exception.hpp>

#include <iostream>

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
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, rgen));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, gen));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, puncture));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, terminationType));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, blockStartID));
    this->registerCall(this, POTHOS_FCN_TUPLE(ConvolutionBase, setBlockStartID));

    this->registerProbe("N");
    this->registerProbe("K");
    this->registerProbe("length");
    this->registerProbe("rgen");
    this->registerProbe("gen");
    this->registerProbe("puncture");
    this->registerProbe("terminationType");
}

ConvolutionBase::~ConvolutionBase() {}

void ConvolutionBase::activate()
{
    this->_updatePortReserves();
}

int ConvolutionBase::N() const
{
    Poco::FastMutex::ScopedLock lock(_convCodeMutex);

    return _pConvCode->n;
}

int ConvolutionBase::K() const
{
    Poco::FastMutex::ScopedLock lock(_convCodeMutex);

    return _pConvCode->k;
}

int ConvolutionBase::length() const
{
    Poco::FastMutex::ScopedLock lock(_convCodeMutex);

    return _pConvCode->len;
}

unsigned ConvolutionBase::rgen() const
{
    Poco::FastMutex::ScopedLock lock(_convCodeMutex);

    return _pConvCode->rgen;
}

std::vector<unsigned> ConvolutionBase::gen() const
{
    Poco::FastMutex::ScopedLock lock(_convCodeMutex);

    return this->_gen();
}

std::vector<int> ConvolutionBase::puncture() const
{
    Poco::FastMutex::ScopedLock lock(_convCodeMutex);

    return this->_punctureFunc();
}

std::string ConvolutionBase::terminationType() const
{
    Poco::FastMutex::ScopedLock lock(_convCodeMutex);

    return this->_terminationType();
}

std::string ConvolutionBase::blockStartID() const
{
    return _blockStartID;
}

void ConvolutionBase::setBlockStartID(const std::string& blockStartID)
{
    _blockStartID = blockStartID;
}

void ConvolutionBase::work()
{
    Poco::FastMutex::ScopedLock lock(_convCodeMutex);

    if(_isEncoder)
    {
        if(_blockStartID.empty()) this->_encoderWork();
        else                      this->_encoderBlockIDWork();
    }
    else
    {
        if(_blockStartID.empty()) this->_decoderWork();
        else                      this->_decoderBlockIDWork();
    }
}

std::vector<unsigned> ConvolutionBase::_gen() const
{
    return std::vector<unsigned>(
              _pConvCode->gen,
              (_pConvCode->gen + _genArrLength));
}

std::vector<int> ConvolutionBase::_punctureFunc() const
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

std::string ConvolutionBase::_terminationType() const
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

// The logic to determine the expected output size is somewhat
// convoluted (no pun intended), so we'll run the underlying
// function on dummy data and note the output length. This isn't
// ideal, but the function *should* be fast enough so this isn't
// noticeable, since it only happens on construction and when
// a field is changed.
void ConvolutionBase::_updatePortReserves()
{
    // Don't instantiate our test vectors each time.
    if(_expectedEncodeCalcInputVec.empty())
    {
        _expectedEncodeCalcInputVec.resize(_pConvCode->len);
    }

    // This should be more than enough.
    _expectedEncodeCalcOutputVec.resize(_pConvCode->len * 500);

    int encodeRet = ::lte_conv_encode(
                        _pConvCode,
                        _expectedEncodeCalcInputVec.data(),
                        _expectedEncodeCalcOutputVec.data());
    throwOnErrCode(encodeRet);

    _expectedEncodeSize = encodeRet;

    this->input(0)->setReserve(static_cast<size_t>(this->_isEncoder ? _pConvCode->len : _expectedEncodeSize));
    this->output(0)->setReserve(static_cast<size_t>(this->_isEncoder ? _expectedEncodeSize :_pConvCode->len));
}

void ConvolutionBase::_encoderWork()
{
    auto input = this->input(0);
    auto output = this->output(0);

    if((input->elements() < static_cast<size_t>(_pConvCode->len)) ||
       (output->elements() < static_cast<size_t>(_expectedEncodeSize)))
    {
        return;
    }

    int encodeRet = ::lte_conv_encode(
                         _pConvCode,
                         input->buffer(),
                         output->buffer());
    throwOnErrCode(encodeRet);

    if(encodeRet != _expectedEncodeSize)
    {
        throw Pothos::AssertionViolationException(
                  "lte_conv_encode returned an unexpected output length",
                  Poco::format(
                      "Expected %s, got %s",
                      Poco::NumberFormatter::format(encodeRet),
                      Poco::NumberFormatter::format(_expectedEncodeSize)));
    }

    input->consume(_pConvCode->len);
    output->produce(_expectedEncodeSize);
}

void ConvolutionBase::_encoderBlockIDWork()
{
    size_t inputSize = _pConvCode->len;

    auto input = this->input(0);

    bool blockFound = false;

    for(const auto& label: input->labels())
    {
        // Skip if we haven't received enough data for this label.
        if(label.index > inputSize) continue;

        // Skip if this isn't a block start label.
        if(label.id != _blockStartID) continue;

        // Skip all data before the block starts.
        if(0 != label.index)
        {
            input->consume(label.index);
            input->setReserve(inputSize);
            return;
        }

        // If our block starts at the beginning of our buffer, wait until we have
        // enough data to encode.
        if(input->elements() < inputSize)
        {
            input->setReserve(inputSize);
            return;
        }

        blockFound = true;
        break;
    }

    if(blockFound) this->_encoderWork();
}

void ConvolutionBase::_decoderWork()
{
    auto input = this->input(0);
    auto output = this->output(0);

    if((input->elements() < static_cast<size_t>(_expectedEncodeSize)) ||
       (output->elements() < static_cast<size_t>(_pConvCode->len)))
    {
        return;
    }

    int decodeRet = ::lte_conv_decode(
                         _pConvCode,
                         input->buffer(),
                         output->buffer());
    throwOnErrCode(decodeRet);

    input->consume(_expectedEncodeSize);
    output->produce(_pConvCode->len);
}

void ConvolutionBase::_decoderBlockIDWork()
{
    size_t inputSize = _expectedEncodeSize;

    const auto& inputs = this->inputs();

    bool blockFound = false;

    // We take in three inputs, but input 0 is expected to have
    // the block ID label.
    for(const auto& label: inputs[0]->labels())
    {
        // Skip if we haven't received enough data for this label.
        if(label.index > inputSize) continue;

        // Skip if this isn't a block start label.
        if(label.id != _blockStartID) continue;

        // Skip all data before the block starts.
        if(0 != label.index)
        {
            for(auto* input: inputs)
            {
                input->consume(label.index);
                input->setReserve(inputSize);
            }
            return;
        }

        // If our block starts at the beginning of our buffer, wait until we have
        // enough data to encode.
        if(this->workInfo().minInElements < inputSize)
        {
            for(auto* input: inputs) input->setReserve(inputSize);
            return;
        }

        blockFound = true;
        break;
    }

    if(blockFound) this->_decoderWork();
}
