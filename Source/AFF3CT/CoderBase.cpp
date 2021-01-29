// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CoderBase.hpp"

#include <Pothos/Framework.hpp>

template <typename B, typename Q>
AFF3CTCoderBase<B,Q>::AFF3CTCoderBase(CoderType coderType):
    _coderType(coderType),
    _codecUPtr(nullptr)
{
    static const Pothos::DType dtype(typeid(B));

    this->setupInput(0, dtype);
    this->setupOutput(0, dtype);
}

template <typename B, typename Q>
AFF3CTCoderBase<B,Q>::~AFF3CTCoderBase()
{
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::work()
{
    const auto elems = this->workInfo().minElements;
    if(0 == elems) return;

    switch(_coderType)
    {
        case CoderType::ENCODER:
            this->_workEncoder();
            break;

        case CoderType::DECODER_SISO:
            this->_workDecoderSISO();
            break;

        case CoderType::DECODER_SIHO:
            this->_workDecoderSIHO();
            break;

        case CoderType::DECODER_HIHO:
            this->_workDecoderHIHO();
            break;

        default:
            throw Pothos::AssertionViolationException("Invalid internal coder type enum: "+std::to_string(int(_coderType)));
    }
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_workEncoder()
{
    if(!_encoderSPtr) throw Pothos::AssertionViolationException("Null encoder shared_ptr");
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_workDecoderSISO()
{
    if(!_encoderSPtr) throw Pothos::AssertionViolationException("Null decoder SISO shared_ptr");
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_workDecoderSIHO()
{
    if(!_encoderSPtr) throw Pothos::AssertionViolationException("Null decoder SIHO shared_ptr");
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_workDecoderHIHO()
{
    if(!_encoderSPtr) throw Pothos::AssertionViolationException("Null decoder HIHO shared_ptr");
}

template class AFF3CTCoderBase<B_8,Q_8>;
template class AFF3CTCoderBase<B_16,Q_16>;
template class AFF3CTCoderBase<B_32,Q_32>;
template class AFF3CTCoderBase<B_64,Q_64>;
