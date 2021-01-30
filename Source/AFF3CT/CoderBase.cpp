// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CoderBase.hpp"

#include <Pothos/Framework.hpp>

//
// Base
//

template <typename B, typename Q>
AFF3CTCoderBase<B,Q>::AFF3CTCoderBase():
    _codecUPtr(nullptr)
{
}

template <typename B, typename Q>
AFF3CTCoderBase<B,Q>::~AFF3CTCoderBase()
{
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_throwIfBlockActive() const
{
    if(this->isActive())
    {
        throw Pothos::RuntimeException("Cannot set parameter while block is active");
    }
}

template class AFF3CTCoderBase<B_8,Q_8>;
template class AFF3CTCoderBase<B_16,Q_16>;
template class AFF3CTCoderBase<B_32,Q_32>;
template class AFF3CTCoderBase<B_64,Q_64>;

//
// Encoder
//

template <typename B, typename Q>
AFF3CTEncoder<B,Q>::AFF3CTEncoder():
    AFF3CTCoderBase<B,Q>(),
    _encoderPtr(nullptr)
{
    static const Pothos::DType BDType(typeid(B));

    this->setupInput(0, BDType);
    this->setupOutput(0, BDType);
}

template <typename B, typename Q>
AFF3CTEncoder<B,Q>::~AFF3CTEncoder()
{
}

template <typename B, typename Q>
void AFF3CTEncoder<B,Q>::work()
{
    if(!_encoderPtr) throw Pothos::AssertionViolationException("Null _encoderPtr");
}

template class AFF3CTEncoder<B_8,Q_8>;
template class AFF3CTEncoder<B_16,Q_16>;
template class AFF3CTEncoder<B_32,Q_32>;
template class AFF3CTEncoder<B_64,Q_64>;

//
// Decoder SISO
//

template <typename B, typename Q>
AFF3CTDecoderSISO<B,Q>::AFF3CTDecoderSISO():
    AFF3CTCoderBase<B,Q>(),
    _decoderSISOSPtr(nullptr)
{
    static const Pothos::DType BDType(typeid(B));
    static const Pothos::DType QDType(typeid(Q));

    this->setupInput(0, QDType);
    this->setupOutput(0, BDType);
}

template <typename B, typename Q>
AFF3CTDecoderSISO<B,Q>::~AFF3CTDecoderSISO()
{
}

template <typename B, typename Q>
void AFF3CTDecoderSISO<B,Q>::work()
{
    if(!_decoderSISOSPtr) throw Pothos::AssertionViolationException("Null _decoderSISOSPtr");
}

template class AFF3CTDecoderSISO<B_8,Q_8>;
template class AFF3CTDecoderSISO<B_16,Q_16>;
template class AFF3CTDecoderSISO<B_32,Q_32>;
template class AFF3CTDecoderSISO<B_64,Q_64>;

//
// Decoder SIHO
//

template <typename B, typename Q>
AFF3CTDecoderSIHO<B,Q>::AFF3CTDecoderSIHO():
    AFF3CTCoderBase<B,Q>(),
    _decoderSIHOSPtr(nullptr)
{
    static const Pothos::DType BDType(typeid(B));

    this->setupInput(0, BDType);
    this->setupOutput(0, BDType);
}

template <typename B, typename Q>
AFF3CTDecoderSIHO<B,Q>::~AFF3CTDecoderSIHO()
{
}

template <typename B, typename Q>
void AFF3CTDecoderSIHO<B,Q>::work()
{
    if(!_decoderSIHOSPtr) throw Pothos::AssertionViolationException("Null _decoderSIHOSPtr");
}

template class AFF3CTDecoderSIHO<B_8,Q_8>;
template class AFF3CTDecoderSIHO<B_16,Q_16>;
template class AFF3CTDecoderSIHO<B_32,Q_32>;
template class AFF3CTDecoderSIHO<B_64,Q_64>;

//
// Decoder HIHO
//

template <typename B, typename Q>
AFF3CTDecoderHIHO<B,Q>::AFF3CTDecoderHIHO():
    AFF3CTCoderBase<B,Q>(),
    _decoderHIHOSPtr(nullptr)
{
}

template <typename B, typename Q>
AFF3CTDecoderHIHO<B,Q>::~AFF3CTDecoderHIHO()
{
}

template <typename B, typename Q>
void AFF3CTDecoderHIHO<B,Q>::work()
{
    if(!_decoderHIHOSPtr) throw Pothos::AssertionViolationException("Null _decoderHIHOSPtr");
}

template class AFF3CTDecoderHIHO<B_8,Q_8>;
template class AFF3CTDecoderHIHO<B_16,Q_16>;
template class AFF3CTDecoderHIHO<B_32,Q_32>;
template class AFF3CTDecoderHIHO<B_64,Q_64>;
