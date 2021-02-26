// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CoderBase.hpp"

#include <Pothos/Framework.hpp>

//
// Base
//

template <typename B, typename Q>
AFF3CTCoderBase<B,Q>::AFF3CTCoderBase():
    _encoderParamsUPtr(nullptr),
    _decoderParamsUPtr(nullptr),
    _codecUPtr(nullptr)
{
    this->registerCall(this, POTHOS_FCN_TUPLE(Class, N));
    this->registerCall(this, POTHOS_FCN_TUPLE(Class, setN));
    this->registerCall(this, POTHOS_FCN_TUPLE(Class, K));
    this->registerCall(this, POTHOS_FCN_TUPLE(Class, setK));
    this->registerCall(this, POTHOS_FCN_TUPLE(Class, systematic));
    this->registerCall(this, POTHOS_FCN_TUPLE(Class, setSystematic));
}

template <typename B, typename Q>
AFF3CTCoderBase<B,Q>::~AFF3CTCoderBase()
{
}

template <typename B, typename Q>
size_t AFF3CTCoderBase<B,Q>::N() const
{
    assert(_encoderParamsUPtr);
    assert(_decoderParamsUPtr);
    assert(_encoderParamsUPtr->N_cw == _decoderParamsUPtr->N_cw);

    return size_t(_encoderParamsUPtr->N_cw);
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::setN(size_t N)
{
    assert(_encoderParamsUPtr);
    assert(_decoderParamsUPtr);
    this->_throwIfBlockIsActive();

    _encoderParamsUPtr->N_cw = _decoderParamsUPtr->N_cw = int(N);
    this->_resetCodec();
}

template <typename B, typename Q>
size_t AFF3CTCoderBase<B,Q>::K() const
{
    assert(_encoderParamsUPtr);
    assert(_decoderParamsUPtr);
    assert(_encoderParamsUPtr->K == _decoderParamsUPtr->K);

    return size_t(_encoderParamsUPtr->K);
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::setK(size_t K)
{
    assert(_encoderParamsUPtr);
    assert(_decoderParamsUPtr);
    this->_throwIfBlockIsActive();

    _encoderParamsUPtr->K = _decoderParamsUPtr->K = int(K);
    this->_resetCodec();
}

template <typename B, typename Q>
bool AFF3CTCoderBase<B,Q>::systematic() const
{
    assert(_encoderParamsUPtr);
    assert(_decoderParamsUPtr);
    assert(_encoderParamsUPtr->systematic == _decoderParamsUPtr->systematic);

    return _encoderParamsUPtr->systematic;
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::setSystematic(bool systematic)
{
    assert(_encoderParamsUPtr);
    assert(_decoderParamsUPtr);
    this->_throwIfBlockIsActive();

    _encoderParamsUPtr->systematic = _decoderParamsUPtr->systematic = systematic;
    this->_resetCodec();
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_throwIfBlockIsActive() const
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
// Decoder
//

template <typename B, typename Q>
AFF3CTDecoder<B,Q>::AFF3CTDecoder(AFF3CTDecoderType decoderType):
    AFF3CTCoderBase<B,Q>(),
    _decoderType(decoderType),
    _decoderSISOSPtr(nullptr),
    _decoderSIHOSPtr(nullptr),
    _decoderHIHOSPtr(nullptr)
{
    static const Pothos::DType BDType(typeid(B));
    static const Pothos::DType QDType(typeid(Q));

    switch(decoderType)
    {
        case AFF3CTDecoderType::SISO:
            this->setupInput(0, QDType);
            this->setupOutput(0, QDType);
            break;

        case AFF3CTDecoderType::SIHO:
            this->setupInput(0, QDType);
            this->setupOutput(0, BDType);
            break;

        case AFF3CTDecoderType::HIHO:
            this->setupInput(0, BDType);
            this->setupOutput(0, BDType);
            break;

        default:
            throw Pothos::AssertionViolationException("Invalid decoder type enum "+std::to_string(int(_decoderType)));
    }
}

template <typename B, typename Q>
AFF3CTDecoder<B,Q>::~AFF3CTDecoder()
{
}

template <typename B, typename Q>
void AFF3CTDecoder<B,Q>::work()
{
    switch(_decoderType)
    {
        case AFF3CTDecoderType::SISO:
            this->_workSISO();
            break;

        case AFF3CTDecoderType::SIHO:
            this->_workSIHO();
            break;

        case AFF3CTDecoderType::HIHO:
            this->_workHIHO();
            break;

        default:
            throw Pothos::AssertionViolationException("Invalid decoder type enum "+std::to_string(int(_decoderType)));
    }
}

template <typename B, typename Q>
void AFF3CTDecoder<B,Q>::_workSISO()
{
}

template <typename B, typename Q>
void AFF3CTDecoder<B,Q>::_workSIHO()
{
}

template <typename B, typename Q>
void AFF3CTDecoder<B,Q>::_workHIHO()
{
}

template class AFF3CTDecoder<B_8,Q_8>;
template class AFF3CTDecoder<B_16,Q_16>;
template class AFF3CTDecoder<B_32,Q_32>;
template class AFF3CTDecoder<B_64,Q_64>;
