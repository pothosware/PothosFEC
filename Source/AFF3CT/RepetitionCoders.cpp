// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AFF3CTDynamic/EntryPoints.hpp"
#include "CoderBase.hpp"
#include "TypeTraits.hpp"
#include "Utility.hpp"

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>

#include <cassert>
#include <string>

//
// Base class
//

template <typename B, typename Q, typename BaseClass>
class RepetitionCoder: public BaseClass
{
public:
    using Class = RepetitionCoder<B,Q,BaseClass>;
    using EncoderParamType = aff3ct::factory::Encoder_repetition::parameters;
    using DecoderParamType = aff3ct::factory::Decoder_repetition::parameters;

    RepetitionCoder(): BaseClass()
    {
        _ctorCommon();
    }

    RepetitionCoder(AFF3CTDecoderType type): BaseClass(type)
    {
        _ctorCommon();
    }

protected:
    void _resetCodec() override
    {
        assert(!this->isActive());
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);

        this->_codecUPtr = AFF3CTDynamic::makeRepetitionCodec<B,Q>(
                               *safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr),
                               *safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr));
        assert(this->_codecUPtr);
    }

private:
    void _ctorCommon()
    {
        this->_encoderParamsUPtr.reset(new EncoderParamType);
        this->_decoderParamsUPtr.reset(new DecoderParamType);
    }
};

//
// Encoder
//

template <typename B, typename Q>
class RepetitionEncoder: public RepetitionCoder<B,Q,AFF3CTEncoder<B,Q>>
{
public:
    using BaseClass = RepetitionCoder<B,Q,AFF3CTEncoder<B,Q>>;

    RepetitionEncoder(): BaseClass(){}
    virtual ~RepetitionEncoder() = default;;

private:
    void _resetCodec() override
    {
        BaseClass::_resetCodec();

        this->_encoderPtr = this->_codecUPtr->get_encoder().get();
        assert(this->_encoderPtr);
    }
};

//
// Decoder
//

template <typename B, typename Q>
class RepetitionDecoder: public RepetitionCoder<B,Q,AFF3CTDecoder<B,Q>>
{
public:
    using Class = RepetitionDecoder<B,Q>;
    using BaseClass = RepetitionCoder<B,Q,AFF3CTDecoder<B,Q>>;
    using DecoderParamType = typename BaseClass::DecoderParamType;

    RepetitionDecoder(): BaseClass(AFF3CTDecoderType::SIHO){}
    virtual ~RepetitionDecoder() = default;

private:
    void _resetCodec() override
    {
        BaseClass::_resetCodec();

        this->_decoderSIHOSPtr = this->_codecAsCodecSIHO()->get_decoder_siho();
        assert(this->_decoderSIHOSPtr);
    }
};

//
// Registration
//

static Pothos::Block* makeRepetitionEncoderBlock(const Pothos::DType& dtype)
{
#define IfTypeThenEncoder(T) \
    if(doesDTypeMatch<T>(dtype)) return new RepetitionEncoder<B, AFF3CTTypeTraits<B>::Q>();

    IfTypeThenEncoder(B_8)
    IfTypeThenEncoder(B_16)
    IfTypeThenEncoder(B_32)
    IfTypeThenEncoder(B_64)

    throw Pothos::InvalidArgumentException("RepetitionEncoder: invalid type "+dtype.toString());
}

static Pothos::Block* makeRepetitionDecoderBlock(const Pothos::DType& dtype)
{
#define IfTypeThenDecoder(T) \
    if(doesDTypeMatch<T>(dtype)) return new RepetitionDecoder<B, AFF3CTTypeTraits<B>::Q>(); \

    IfTypeThenDecoder(B_8)
    IfTypeThenDecoder(B_16)
    IfTypeThenDecoder(B_32)
    IfTypeThenDecoder(B_64)

    throw Pothos::InvalidArgumentException("RepetitionDecoder: invalid type "+dtype.toString());
}

static Pothos::BlockRegistry registerRepetitionEncoder(
    "/fec/repetition_encoder",
    &makeRepetitionEncoderBlock);

static Pothos::BlockRegistry registerRepetitionDecoder(
    "/fec/repetition_decoder",
    &makeRepetitionDecoderBlock);
