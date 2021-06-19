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
class RACoder: public BaseClass
{
public:
    using Class = RACoder<B,Q,BaseClass>;
    using EncoderParamType = aff3ct::factory::Encoder_RA::parameters;
    using DecoderParamType = aff3ct::factory::Decoder_RA::parameters;

    RACoder(): BaseClass()
    {
        _ctorCommon();
    }

    RACoder(AFF3CTDecoderType type): BaseClass(type)
    {
        _ctorCommon();
    }

    virtual ~RACoder() = default;

    int interleaverSize() const
    {
        assert(_interleaverParamsUPtr);

        return _interleaverParamsUPtr->core->size;
    }

    void setInterleaverSize(int size)
    {
        assert(_interleaverParamsUPtr);

        this->_throwIfBlockIsActive();
        _interleaverParamsUPtr->core->size = size;
        this->_resetCodec();
    }

    std::string interleaverType() const
    {
        assert(_interleaverParamsUPtr);

        return _interleaverParamsUPtr->core->type;
    }

    // TODO: validate input
    void setInterleaverType(const std::string& type)
    {
        assert(_interleaverParamsUPtr);

        this->_throwIfBlockIsActive();
        _interleaverParamsUPtr->core->type = type;
        this->_resetCodec();
    }

    std::string interleaverPath() const
    {
        assert(_interleaverParamsUPtr);

        return _interleaverParamsUPtr->core->path;
    }

    // TODO: validate input
    void setInterleaverPath(const std::string& path)
    {
        assert(_interleaverParamsUPtr);

        this->_throwIfBlockIsActive();
        _interleaverParamsUPtr->core->path = path;
        this->_resetCodec();
    }

    std::string interleaverReadOrder() const
    {
        assert(_interleaverParamsUPtr);

        return _interleaverParamsUPtr->core->read_order;
    }

    // TODO: validate input
    void setInterleaverReadOrder(const std::string& readOrder)
    {
        assert(_interleaverParamsUPtr);

        this->_throwIfBlockIsActive();
        _interleaverParamsUPtr->core->read_order = readOrder;
        this->_resetCodec();
    }

    int interleaverNumColumns() const
    {
        assert(_interleaverParamsUPtr);

        return _interleaverParamsUPtr->core->n_cols;
    }

    void setInterleaverNumColumns(int numColumns)
    {
        assert(_interleaverParamsUPtr);

        this->_throwIfBlockIsActive();
        _interleaverParamsUPtr->core->n_cols = numColumns;
        this->_resetCodec();
    }

    int interleaverNumFrames() const
    {
        assert(_interleaverParamsUPtr);

        return _interleaverParamsUPtr->core->n_frames;
    }

    void setInterleaverNumFrames(int numFrames)
    {
        assert(_interleaverParamsUPtr);

        this->_throwIfBlockIsActive();
        _interleaverParamsUPtr->core->n_frames = numFrames;
        this->_resetCodec();
    }

    int interleaverSeed() const
    {
        assert(_interleaverParamsUPtr);

        return _interleaverParamsUPtr->core->seed;
    }

    void setInterleaverSeed(int seed)
    {
        assert(_interleaverParamsUPtr);

        this->_throwIfBlockIsActive();
        _interleaverParamsUPtr->core->seed = seed;
        this->_resetCodec();
    }

    bool interleaverUniform() const
    {
        assert(_interleaverParamsUPtr);

        return _interleaverParamsUPtr->core->uniform;
    }

    void setInterleaverUniform(bool uniform)
    {
        assert(_interleaverParamsUPtr);

        this->_throwIfBlockIsActive();
        _interleaverParamsUPtr->core->uniform = uniform;
        this->_resetCodec();
    }

protected:
    std::unique_ptr<aff3ct::factory::Interleaver::parameters> _interleaverParamsUPtr;

    void _resetCodec() override
    {
        assert(!this->isActive());
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        assert(this->_interleaverParamsUPtr);

        this->_codecUPtr = AFF3CTDynamic::makeRACodec<B,Q>(
                               *safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr),
                               *safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr),
                               *this->_interleaverParamsUPtr);
        assert(this->_codecUPtr);
    }

private:
    void _ctorCommon()
    {
        this->_encoderParamsUPtr.reset(new EncoderParamType);
        this->_decoderParamsUPtr.reset(new DecoderParamType);
        this->_interleaverParamsUPtr.reset(new aff3ct::factory::Interleaver::parameters);

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, interleaverSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setInterleaverSize));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, interleaverType));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setInterleaverType));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, interleaverPath));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setInterleaverPath));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, interleaverReadOrder));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setInterleaverReadOrder));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, interleaverNumColumns));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setInterleaverNumColumns));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, interleaverNumFrames));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setInterleaverNumFrames));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, interleaverSeed));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setInterleaverSeed));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, interleaverUniform));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setInterleaverUniform));
    }
};

//
// Encoder
//

template <typename B, typename Q>
class RAEncoder: public RACoder<B,Q,AFF3CTEncoder<B,Q>>
{
public:
    using BaseClass = RACoder<B,Q,AFF3CTEncoder<B,Q>>;

    RAEncoder(): BaseClass(){}
    virtual ~RAEncoder() = default;;

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
class RADecoder: public RACoder<B,Q,AFF3CTDecoder<B,Q>>
{
public:
    using Class = RADecoder<B,Q>;
    using BaseClass = RACoder<B,Q,AFF3CTDecoder<B,Q>>;
    using DecoderParamType = typename BaseClass::DecoderParamType;

    RADecoder(): BaseClass(AFF3CTDecoderType::SIHO)
    {
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, numIterations));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setNumIterations));
    }

    virtual ~RADecoder() = default;

    size_t numIterations() const
    {
        assert(this->_decoderParamsUPtr);

        return static_cast<size_t>(safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->n_ite);
    }

    void setNumIterations(size_t numIterations)
    {
        assert(this->_decoderParamsUPtr);
        this->_throwIfBlockIsActive();

        if(0 == numIterations)
        {
            throw Pothos::RangeException("numIterations must be > 0");
        }

        safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->n_ite = static_cast<int>(numIterations);
        this->_resetCodec();
    }

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
