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
#include <type_traits>

//
// Base class
//

template <typename B, typename Q, typename BaseClass>
class BCHCoder: public BaseClass
{
public:
    using Class = BCHCoder<B, Q, BaseClass>;
    using EncoderParamType = aff3ct::factory::Encoder_BCH::parameters;
    using DecoderParamType = aff3ct::factory::Decoder_BCH::parameters;

    BCHCoder(): BaseClass()
    {
        _ctorCommon();
    }

    BCHCoder(AFF3CTDecoderType type): BaseClass(type)
    {
        _ctorCommon();
    }

    virtual ~BCHCoder() = default;

protected:
    void _resetCodec() override
    {
        assert(!this->isActive());
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);

        this->_codecUPtr = AFF3CTDynamic::makeBCHCodec<B,Q>(
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
class BCHEncoder: public BCHCoder<B,Q,AFF3CTEncoder<B,Q>>
{
public:
    using BaseClass = BCHCoder<B,Q,AFF3CTEncoder<B,Q>>;

    BCHEncoder(): BaseClass(){}
    virtual ~BCHEncoder() = default;

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

template <typename B, typename Q, bool siho>
class BCHDecoder: public BCHCoder<B,Q,AFF3CTDecoder<B,Q>>
{
public:
    using Class = BCHDecoder<B,Q,siho>;
    using BaseClass = BCHCoder<B,Q,AFF3CTDecoder<B,Q>>;
    using DecoderParamType = typename BaseClass::DecoderParamType;

    BCHDecoder(): BaseClass(siho ? AFF3CTDecoderType::SIHO : AFF3CTDecoderType::HIHO)
    {
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, correctionPower));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setCorrectionPower));
    }

    virtual ~BCHDecoder() = default;

    size_t correctionPower() const
    {
        assert(this->_decoderParamsUPtr);

        return size_t(safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->t);
    }

    void setCorrectionPower(size_t correctionPower)
    {
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->t = int(correctionPower);
        this->_resetCodec();
    }

private:
    void _resetCodec() override
    {
        BaseClass::_resetCodec();

        if(siho)
        {
            this->_decoderSIHOSPtr = this->_codecAsCodecSIHO()->get_decoder_siho();
            assert(this->_decoderSIHOSPtr);
        }
        else
        {
            this->_decoderHIHOSPtr = this->_codecAsCodecHIHO()->get_decoder_hiho();
            assert(this->_decoderHIHOSPtr);
        }
    }
};

//
// Registration
//

static Pothos::Block* makeBCHEncoderBlock(const Pothos::DType& dtype)
{
#define IfTypeThenEncoder(T) \
    if(doesDTypeMatch<T>(dtype)) return new BCHEncoder<B, AFF3CTTypeTraits<B>::Q>();

    IfTypeThenEncoder(B_8)
    IfTypeThenEncoder(B_16)
    IfTypeThenEncoder(B_32)
    IfTypeThenEncoder(B_64)

    throw Pothos::InvalidArgumentException("BCHEncoder: invalid type "+dtype.toString());
}

static Pothos::Block* makeBCHDecoderBlock(const Pothos::DType& dtype, const std::string& mode)
{
#define IfTypeThenDecoder(T) \
    if(doesDTypeMatch<T>(dtype) && (mode == "SIHO")) return new BCHDecoder<B, AFF3CTTypeTraits<B>::Q, true>(); \
    if(doesDTypeMatch<T>(dtype) && (mode == "HIHO")) return new BCHDecoder<B, AFF3CTTypeTraits<B>::Q, false>();

    IfTypeThenDecoder(B_8)
    IfTypeThenDecoder(B_16)
    IfTypeThenDecoder(B_32)
    IfTypeThenDecoder(B_64)

    throw Pothos::InvalidArgumentException("BCHDecoder: invalid type "+dtype.toString() + ", mode "+mode);
}

static Pothos::BlockRegistry registerBCHEncoder(
    "/fec/bch_encoder",
    &makeBCHEncoderBlock);

static Pothos::BlockRegistry registerBCHDecoder(
    "/fec/bch_decoder",
    &makeBCHDecoderBlock);
