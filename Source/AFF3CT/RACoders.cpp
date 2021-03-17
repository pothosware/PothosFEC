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
// Helper class
//

template <typename B, typename Q>
struct RAHelper
{
    using EncoderParamType = aff3ct::factory::Encoder_RA::parameters;
    using DecoderParamType = aff3ct::factory::Decoder_RA::parameters;

    static void initCoderParams(
        std::unique_ptr<aff3ct::factory::Encoder::parameters>& rEncoderParamsUPtr,
        std::unique_ptr<aff3ct::factory::Decoder::parameters>& rDecoderParamsUPtr,
        std::unique_ptr<aff3ct::factory::Interleaver::parameters>& rInterleaverParamsUPtr)
    {
        rEncoderParamsUPtr.reset(new EncoderParamType);
        rDecoderParamsUPtr.reset(new DecoderParamType);
        rInterleaverParamsUPtr.reset(new aff3ct::factory::Interleaver::parameters);
    }

    static void resetCodec(
        const std::unique_ptr<aff3ct::factory::Encoder::parameters>& encoderParamsUPtr,
        const std::unique_ptr<aff3ct::factory::Decoder::parameters>& decoderParamsUPtr,
        const std::unique_ptr<aff3ct::factory::Interleaver::parameters>& interleaverParamsUPtr,
        std::unique_ptr<aff3ct::module::Codec<B,Q>>& rCodecUPtr)
    {
        assert(encoderParamsUPtr);
        assert(decoderParamsUPtr);
        assert(interleaverParamsUPtr);

        rCodecUPtr = AFF3CTDynamic::makeRACodec<B,Q>(
                         *safeDynamicCast<EncoderParamType>(encoderParamsUPtr),
                         *safeDynamicCast<DecoderParamType>(decoderParamsUPtr),
                         *interleaverParamsUPtr);
        assert(rCodecUPtr);
    }
};

//
// Encoder
//

template <typename B, typename Q>
class RAEncoder: public AFF3CTEncoder<B,Q>
{
public:
    RAEncoder(): AFF3CTEncoder<B,Q>()
    {
        RAHelper<B,Q>::initCoderParams(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_interleaverParamsUPtr);
    }

    virtual ~RAEncoder() = default;

protected:
    std::unique_ptr<aff3ct::factory::Interleaver::parameters> _interleaverParamsUPtr;

    void _resetCodec() override
    {
        assert(!this->isActive());

        this->_encoderPtr = nullptr;
        RAHelper<B,Q>::resetCodec(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_interleaverParamsUPtr,
            this->_codecUPtr);

        this->_encoderPtr = this->_codecUPtr->get_encoder().get();
        assert(this->_encoderPtr);
    }
};

//
// Decoder
//


template <typename B, typename Q>
class RADecoder: public AFF3CTDecoder<B,Q>
{
public:
    using Class = RADecoder<B,Q>;
    using DecoderParamType = typename RAHelper<B,Q>::DecoderParamType;

    RADecoder(): AFF3CTDecoder<B,Q>(AFF3CTDecoderType::SIHO)
    {
        RAHelper<B,Q>::initCoderParams(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_interleaverParamsUPtr);
    }

    virtual ~RADecoder() = default;

protected:
    std::unique_ptr<aff3ct::factory::Interleaver::parameters> _interleaverParamsUPtr;

    void _resetCodec() override
    {
        assert(!this->isActive());

        this->_decoderSIHOSPtr.reset();
        RAHelper<B,Q>::resetCodec(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_interleaverParamsUPtr,
            this->_codecUPtr);

        this->_decoderSIHOSPtr = this->_codecAsCodecSIHO()->get_decoder_siho();
    }
};

//
// Registration
//

static Pothos::Block* makeRAEncoderBlock(const Pothos::DType& dtype)
{
#define IfTypeThenEncoder(T) \
    if(doesDTypeMatch<T>(dtype)) return new RAEncoder<B, AFF3CTTypeTraits<B>::Q>();

    IfTypeThenEncoder(B_8)
    IfTypeThenEncoder(B_16)
    IfTypeThenEncoder(B_32)
    IfTypeThenEncoder(B_64)

    throw Pothos::InvalidArgumentException("RAEncoder: invalid type "+dtype.toString());
}

static Pothos::Block* makeRADecoderBlock(const Pothos::DType& dtype)
{
#define IfTypeThenDecoder(T) \
    if(doesDTypeMatch<T>(dtype)) return new RADecoder<B, AFF3CTTypeTraits<B>::Q>(); \

    IfTypeThenDecoder(B_8)
    IfTypeThenDecoder(B_16)
    IfTypeThenDecoder(B_32)
    IfTypeThenDecoder(B_64)

    throw Pothos::InvalidArgumentException("RADecoder: invalid type "+dtype.toString());
}

static Pothos::BlockRegistry registerRAEncoder(
    "/fec/ra_encoder",
    &makeRAEncoderBlock);

static Pothos::BlockRegistry registerRADecoder(
    "/fec/ra_decoder",
    &makeRADecoderBlock);
