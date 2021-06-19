// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AFF3CTDynamic/EntryPoints.hpp"
#include "CoderBase.hpp"
#include "TypeTraits.hpp"
#include "Utility.hpp"

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>

#include <Poco/File.h>

#include <cassert>
#include <string>
#include <vector>

//
// Helper class
//

template <typename B, typename Q>
struct LDPCHelper
{
    using EncoderParamType = aff3ct::factory::Encoder_LDPC::parameters;
    using DecoderParamType = aff3ct::factory::Decoder_LDPC::parameters;
    using PuncturerParamType = aff3ct::factory::Puncturer_LDPC::parameters;

    static void initCoderParams(
        std::unique_ptr<aff3ct::factory::Encoder::parameters>& rEncoderParamsUPtr,
        std::unique_ptr<aff3ct::factory::Decoder::parameters>& rDecoderParamsUPtr)
    {
        rEncoderParamsUPtr.reset(new EncoderParamType);
        rDecoderParamsUPtr.reset(new DecoderParamType);
    }

    static void resetCodec(
        const std::unique_ptr<aff3ct::factory::Encoder::parameters>& encoderParamsUPtr,
        const std::unique_ptr<aff3ct::factory::Decoder::parameters>& decoderParamsUPtr,
        PuncturerParamType* puncturerParamsPtr,
        std::unique_ptr<aff3ct::module::Codec<B,Q>>& rCodecUPtr)
    {
        assert(encoderParamsUPtr);
        assert(decoderParamsUPtr);
        assert(puncturerParamsPtr);

        rCodecUPtr = AFF3CTDynamic::makeLDPCCodec<B,Q>(
                         *safeDynamicCast<EncoderParamType>(encoderParamsUPtr),
                         *safeDynamicCast<DecoderParamType>(decoderParamsUPtr),
                         puncturerParamsPtr);
        assert(rCodecUPtr);
    }
};

//
// Encoder
//

template <typename B, typename Q, bool dvbs2>
class LDPCEncoder: public AFF3CTEncoder<B,Q>
{
public:
    using Class = LDPCEncoder<B,Q,dvbs2>;
    using EncoderParamType = typename LDPCHelper<B,Q>::EncoderParamType;
    using DecoderParamType = typename LDPCHelper<B,Q>::DecoderParamType;
    using PuncturerParamType = typename LDPCHelper<B,Q>::PuncturerParamType;

    LDPCEncoder(): AFF3CTEncoder<B,Q>()
    {
        LDPCHelper<B,Q>::initCoderParams(this->_encoderParamsUPtr, this->_decoderParamsUPtr);

        this->_encoderParamsUPtr->type = dvbs2 ? "LDPC_DVBS2" : "LDPC";
        this->_decoderParamsUPtr->type = dvbs2 ? "LDPC_DVBS2" : "LDPC";

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, puncturePattern));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setPuncturePattern));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, GMatrixPath));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setGMatrixPath));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, HMatrixPath));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setHMatrixPath));
    }

    virtual ~LDPCEncoder() = default;

    void setN(size_t N) override
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        this->_encoderParamsUPtr->N_cw = this->_decoderParamsUPtr->N_cw = N;
        this->_puncturerParams.N = this->_puncturerParams.N_cw = N;
        this->_resetCodec();
    }

    void setK(size_t K) override
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        this->_encoderParamsUPtr->K = this->_decoderParamsUPtr->K = K;
        this->_puncturerParams.K = K;
        this->_resetCodec();
    }

    std::vector<bool> puncturePattern() const
    {
        return this->_puncturerParams.pattern;
    }

    void setPuncturePattern(const std::vector<bool>& puncturePattern)
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        this->_puncturerParams.pattern = puncturePattern;
        this->_resetCodec();
    }

    std::string GMatrixPath() const
    {
        assert(this->_encoderParamsUPtr);

        return safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->G_path;
    }

    void setGMatrixPath(const std::string& path)
    {
        if(!Poco::File(path).exists())
        {
            throw Pothos::FileNotFoundException(path);
        }

        assert(this->_encoderParamsUPtr);
        this->_throwIfBlockIsActive();

        safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->G_path = path;
        this->_resetCodec();
    }

    std::string HMatrixPath() const
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        assert(safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->H_path ==
               safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->H_path);

        return safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->H_path;
    }

    void setHMatrixPath(const std::string& path)
    {
        if(!Poco::File(path).exists())
        {
            throw Pothos::FileNotFoundException(path);
        }

        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->H_path = path;
        safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->H_path = path;
        this->_resetCodec();
    }

protected:

    PuncturerParamType _puncturerParams;

    void _resetCodec() override
    {
        assert(!this->isActive());

        this->_encoderPtr = nullptr;
        LDPCHelper<B,Q>::resetCodec(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            &_puncturerParams,
            this->_codecUPtr);

        this->_encoderPtr = this->_codecUPtr->get_encoder().get();
        assert(this->_encoderPtr);
    }
};

//
// Decoder
//

template <typename B, typename Q, bool dvbs2, bool siho>
class LDPCDecoder: public AFF3CTDecoder<B,Q>
{
public:
    using Class = LDPCDecoder<B,Q,dvbs2,siho>;
    using EncoderParamType = typename LDPCHelper<B,Q>::EncoderParamType;
    using DecoderParamType = typename LDPCHelper<B,Q>::DecoderParamType;
    using PuncturerParamType = typename LDPCHelper<B,Q>::PuncturerParamType;

    LDPCDecoder():
        AFF3CTDecoder<B,Q>(siho ? AFF3CTDecoderType::SIHO : AFF3CTDecoderType::SISO)
    {
        LDPCHelper<B,Q>::initCoderParams(this->_encoderParamsUPtr, this->_decoderParamsUPtr);

        this->_encoderParamsUPtr->type = dvbs2 ? "LDPC_DVBS2" : "LDPC";
        this->_decoderParamsUPtr->type = dvbs2 ? "LDPC_DVBS2" : "LDPC";

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, puncturePattern));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setPuncturePattern));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, GMatrixPath));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setGMatrixPath));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, HMatrixPath));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setHMatrixPath));
    }

    virtual ~LDPCDecoder() = default;

    void setN(size_t N) override
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        this->_encoderParamsUPtr->N_cw = this->_decoderParamsUPtr->N_cw = N;
        this->_puncturerParams.N = this->_puncturerParams.N_cw = N;
        this->_resetCodec();
    }

    void setK(size_t K) override
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        this->_encoderParamsUPtr->K = this->_decoderParamsUPtr->K = K;
        this->_puncturerParams.K = K;
        this->_resetCodec();
    }

    std::vector<bool> puncturePattern() const
    {
        return this->_puncturerParams.pattern;
    }

    void setPuncturePattern(const std::vector<bool>& puncturePattern)
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        this->_puncturerParams.pattern = puncturePattern;
        this->_resetCodec();
    }

    std::string GMatrixPath() const
    {
        assert(this->_encoderParamsUPtr);

        return safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->G_path;
    }

    void setGMatrixPath(const std::string& path)
    {
        if(!Poco::File(path).exists())
        {
            throw Pothos::FileNotFoundException(path);
        }

        assert(this->_encoderParamsUPtr);
        this->_throwIfBlockIsActive();

        safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->G_path = path;
        this->_resetCodec();
    }

    std::string HMatrixPath() const
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        assert(safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->H_path ==
               safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->H_path);

        return safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->H_path;
    }

    void setHMatrixPath(const std::string& path)
    {
        if(!Poco::File(path).exists())
        {
            throw Pothos::FileNotFoundException(path);
        }

        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr)->H_path = path;
        safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->H_path = path;
        this->_resetCodec();
    }

protected:

    PuncturerParamType _puncturerParams;

    void _resetCodec() override
    {
        assert(!this->isActive());

        this->_decoderSISOSPtr.reset();
        this->_decoderSIHOSPtr.reset();
        LDPCHelper<B,Q>::resetCodec(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            &_puncturerParams,
            this->_codecUPtr);

        if(siho) this->_decoderSIHOSPtr = this->_codecAsCodecSIHO()->get_decoder_siho();
        else     this->_decoderSISOSPtr = this->_codecAsCodecSISO()->get_decoder_siso();
    }
};

//
// Registration
//

static Pothos::Block* makeLDPCEncoderBlock(const Pothos::DType& dtype)
{
#define IfTypeThenEncoder(T) \
    if(doesDTypeMatch<T>(dtype)) return new LDPCEncoder<B, AFF3CTTypeTraits<B>::Q, false>();

    IfTypeThenEncoder(B_8)
    IfTypeThenEncoder(B_16)
    IfTypeThenEncoder(B_32)
    IfTypeThenEncoder(B_64)

    throw Pothos::InvalidArgumentException("LDPCEncoder: invalid type "+dtype.toString());
}

static Pothos::Block* makeLDPCEncoderDVBS2Block(const Pothos::DType& dtype)
{
#define IfTypeThenEncoderDVBS2(T) \
    if(doesDTypeMatch<T>(dtype)) return new LDPCEncoder<B, AFF3CTTypeTraits<B>::Q, true>();

    IfTypeThenEncoderDVBS2(B_8)
    IfTypeThenEncoderDVBS2(B_16)
    IfTypeThenEncoderDVBS2(B_32)
    IfTypeThenEncoderDVBS2(B_64)

    throw Pothos::InvalidArgumentException("LDPCEncoderDVBS2: invalid type "+dtype.toString());
}

static Pothos::Block* makeLDPCDecoderBlock(const Pothos::DType& dtype, const std::string& mode)
{
#define IfTypeThenDecoder(T) \
    if(doesDTypeMatch<T>(dtype) && (mode == "SISO")) return new LDPCDecoder<B, AFF3CTTypeTraits<B>::Q, false, false>(); \
    if(doesDTypeMatch<T>(dtype) && (mode == "SIHO")) return new LDPCDecoder<B, AFF3CTTypeTraits<B>::Q, false, true>();

    IfTypeThenDecoder(B_8)
    IfTypeThenDecoder(B_16)
    IfTypeThenDecoder(B_32)
    IfTypeThenDecoder(B_64)

    throw Pothos::InvalidArgumentException("LDPCDecoder: invalid type "+dtype.toString() + ", mode "+mode);
}

static Pothos::Block* makeLDPCDecoderDVBS2Block(const Pothos::DType& dtype, const std::string& mode)
{
#define IfTypeThenDecoderDVBS2(T) \
    if(doesDTypeMatch<T>(dtype) && (mode == "SISO")) return new LDPCDecoder<B, AFF3CTTypeTraits<B>::Q, true, false>(); \
    if(doesDTypeMatch<T>(dtype) && (mode == "SIHO")) return new LDPCDecoder<B, AFF3CTTypeTraits<B>::Q, true, true>();

    IfTypeThenDecoderDVBS2(B_8)
    IfTypeThenDecoderDVBS2(B_16)
    IfTypeThenDecoderDVBS2(B_32)
    IfTypeThenDecoderDVBS2(B_64)

    throw Pothos::InvalidArgumentException("LDPCDecoderDVBS2: invalid type "+dtype.toString() + ", mode "+mode);
}

static Pothos::BlockRegistry registerLDPCEncoder(
    "/fec/ldpc_encoder",
    &makeLDPCEncoderBlock);

static Pothos::BlockRegistry registerLDPCEncoderDVBS2(
    "/fec/ldpc_encoder_dvbs2",
    &makeLDPCEncoderDVBS2Block);

static Pothos::BlockRegistry registerLDPCDecoder(
    "/fec/ldpc_decoder",
    &makeLDPCDecoderBlock);

static Pothos::BlockRegistry registerLDPCDecoderDVBS2(
    "/fec/ldpc_decoder_dvbs2",
    &makeLDPCDecoderDVBS2Block);
