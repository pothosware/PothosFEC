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

template <typename B, typename Q>
class BCHCoder
{
public:
    BCHCoder()
    {
        // TODO: override default parameters
    }

    virtual ~BCHCoder() = default;

protected:
    aff3ct::factory::Encoder_BCH::parameters _encoderParams;
    aff3ct::factory::Decoder_BCH::parameters _decoderParams;

    void _resetCodec(CodecUPtr<B,Q>& rCodecUPtr)
    {
        rCodecUPtr = AFF3CTDynamic::makeBCHCodec<B,Q>(_encoderParams, _decoderParams);
    }
};

//
// Encoder
//

template <typename B, typename Q>
class BCHEncoder: public AFF3CTEncoder<B,Q>, private BCHCoder<B,Q>
{
public:
    BCHEncoder(): AFF3CTEncoder<B,Q>(), BCHCoder<B,Q>()
    {
    }

    ~BCHEncoder() = default;

private:
    void _resetCodec() override
    {
        BCHCoder<B,Q>::_resetCodec(this->_codecUPtr);
        assert(this->_codecUPtr);

        this->_encoderPtr = this->_codecUPtr->get_encoder().get();
        assert(this->_encoderPtr);
    }
};

//
// Decoder SIHO
//

template <typename B, typename Q>
class BCHDecoderSIHO: public AFF3CTDecoderSIHO<B,Q>, private BCHCoder<B,Q>
{
public:
    BCHDecoderSIHO(): AFF3CTDecoderSIHO<B,Q>(), BCHCoder<B,Q>()
    {
        // TODO: setter for optional field
    }

    ~BCHDecoderSIHO() = default;

private:
    void _resetCodec() override
    {
        BCHCoder<B,Q>::_resetCodec(this->_codecUPtr);
        assert(this->_codecUPtr);

        this->_decoderSIHOSPtr = safeDynamicCast<
                                     aff3ct::module::Codec<B,Q>,
                                     aff3ct::module::Codec_SIHO<B,Q>
                                 >(this->_codecUPtr.get())->get_decoder_siho();
    }
};

//
// Decoder HIHO
//

template <typename B, typename Q>
class BCHDecoderHIHO: public AFF3CTDecoderHIHO<B,Q>, private BCHCoder<B,Q>
{
public:
    BCHDecoderHIHO(): AFF3CTDecoderHIHO<B,Q>(), BCHCoder<B,Q>()
    {
        // TODO: setter for optional field
    }

    ~BCHDecoderHIHO() = default;

private:
    void _resetCodec() override
    {
        BCHCoder<B,Q>::_resetCodec(this->_codecUPtr);
        assert(this->_codecUPtr);

        this->_decoderHIHOSPtr = safeDynamicCast<
                                     aff3ct::module::Codec<B,Q>,
                                     aff3ct::module::Codec_HIHO<B,Q>
                                 >(this->_codecUPtr.get())->get_decoder_hiho();
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
    if(doesDTypeMatch<T>(dtype) && (mode == "SIHO")) return new BCHDecoderSIHO<B, AFF3CTTypeTraits<B>::Q>(); \
    if(doesDTypeMatch<T>(dtype) && (mode == "HIHO")) return new BCHDecoderHIHO<B, AFF3CTTypeTraits<B>::Q>(); \

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
