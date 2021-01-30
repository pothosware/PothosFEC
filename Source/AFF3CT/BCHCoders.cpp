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
        _decoderParams.implem = "FAST";
    }

    virtual ~BCHCoder() = default;

    size_t N() const
    {
        assert(_encoderParams.N_cw == _decoderParams.N_cw);

        return static_cast<size_t>(_encoderParams.N_cw);
    }

    virtual void setN(size_t N)
    {
        _encoderParams.N_cw = static_cast<int>(N);
        _decoderParams.N_cw = static_cast<int>(N);
    }

    size_t K() const
    {
        assert(_encoderParams.K == _decoderParams.K);

        return static_cast<size_t>(_encoderParams.K);
    }

    virtual void setK(size_t K)
    {
        if(K <= 3) throw Pothos::RangeException("K must be > 3");

        _encoderParams.K = static_cast<int>(K);
        _decoderParams.K = static_cast<int>(K);
    }

    size_t correctionPower() const
    {
        return _decoderParams.t;
    }

    virtual void setCorrectionPower(size_t t)
    {
        _decoderParams.t = static_cast<int>(t);
    }

    std::string decoderImplementation() const
    {
        return _decoderParams.implem;
    }

    virtual void setDecoderImplementation(const std::string& impl)
    {
        static const std::vector<std::string> Impls
        {
            "STD",
            "FAST",
            "GENIUS"
        };
        throwIfValueNotInVector(Impls, impl);

        _decoderParams.implem = impl;
    }

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
class BCHEncoder: public AFF3CTEncoder<B,Q>, public BCHCoder<B,Q>
{
public:
    using Class = BCHEncoder<B,Q>;

    BCHEncoder(): AFF3CTEncoder<B,Q>(), BCHCoder<B,Q>()
    {
        // TODO: override default parameters

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, N));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setN));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, K));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setK));
    }

    ~BCHEncoder() = default;

    void setN(size_t N) override
    {
        this->_throwIfBlockActive();

        BCHCoder<B,Q>::setN(N);

        this->_resetCodec();
    }

    void setK(size_t K) override
    {
        this->_throwIfBlockActive();

        BCHCoder<B,Q>::setK(K);

        this->_resetCodec();
    }

private:
    void _resetCodec() override
    {
        this->_encoderPtr = nullptr;

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
class BCHDecoderSIHO: public AFF3CTDecoderSIHO<B,Q>, public BCHCoder<B,Q>
{
public:
    using Class = BCHDecoderSIHO<B,Q>;

    BCHDecoderSIHO(): AFF3CTDecoderSIHO<B,Q>(), BCHCoder<B,Q>()
    {
        // TODO: setter for optional field
        // TODO: override default parameters

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, N));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setN));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, K));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setK));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, correctionPower));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setCorrectionPower));
    }

    ~BCHDecoderSIHO() = default;

private:
    void _resetCodec() override
    {
        this->_decoderSIHOSPtr.reset();

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
class BCHDecoderHIHO: public AFF3CTDecoderHIHO<B,Q>, public BCHCoder<B,Q>
{
public:
    using Class = BCHDecoderHIHO<B,Q>;

    BCHDecoderHIHO(): AFF3CTDecoderHIHO<B,Q>(), BCHCoder<B,Q>()
    {
        // TODO: setter for optional field
        // TODO: override default parameters

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, N));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setN));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, K));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setK));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, correctionPower));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setCorrectionPower));
    }

    ~BCHDecoderHIHO() = default;

private:
    void _resetCodec() override
    {
        this->_decoderHIHOSPtr.reset();

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
    if(doesDTypeMatch<T>(dtype) && (mode == "HIHO")) return new BCHDecoderHIHO<B, AFF3CTTypeTraits<B>::Q>();

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
