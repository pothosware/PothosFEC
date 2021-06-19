// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AFF3CTDynamic/EntryPoints.hpp"
#include "CoderBase.hpp"
#include "TypeTraits.hpp"
#include "Utility.hpp"

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>

#include <cassert>
#include <functional>
#include <string>

using SetterCallback = std::function<void(void)>;

class InterleaverParamsWrapper
{
public:
    InterleaverParamsWrapper() = delete;

    InterleaverParamsWrapper(
        const SetterCallback& throwIfActiveCallback,
        const SetterCallback& resetCodecCallback
    ):
        _paramsUPtr(new aff3ct::factory::Interleaver::parameters),
        _throwIfActiveCallback(throwIfActiveCallback),
        _resetCodecCallback(resetCodecCallback)
    {}

    virtual ~InterleaverParamsWrapper() = default;

    inline void reset()
    {
        _paramsUPtr.reset(new aff3ct::factory::Interleaver::parameters);
    }

    inline const aff3ct::factory::Interleaver::parameters& params() const
    {
        return *_paramsUPtr;
    }

    inline int size() const
    {
        assert(_paramsUPtr);

        return _paramsUPtr->core->size;
    }

    void setSize(int size)
    {
        assert(_paramsUPtr);

        _throwIfActiveCallback();
        _paramsUPtr->core->size = size;
        _resetCodecCallback();
    }

    inline std::string type() const
    {
        assert(_paramsUPtr);

        return _paramsUPtr->core->type;
    }

    // TODO: validate input
    void setType(const std::string& type)
    {
        assert(_paramsUPtr);

        _throwIfActiveCallback();
        _paramsUPtr->core->type = type;
        _resetCodecCallback();
    }

    inline std::string path() const
    {
        assert(_paramsUPtr);

        return _paramsUPtr->core->path;
    }

    // TODO: validate input
    void setPath(const std::string& path)
    {
        assert(_paramsUPtr);

        _throwIfActiveCallback();
        _paramsUPtr->core->path = path;
        _resetCodecCallback();
    }

    inline std::string readOrder() const
    {
        assert(_paramsUPtr);

        return _paramsUPtr->core->read_order;
    }

    // TODO: validate input
    void setReadOrder(const std::string& readOrder)
    {
        assert(_paramsUPtr);

        _throwIfActiveCallback();
        _paramsUPtr->core->read_order = readOrder;
        _resetCodecCallback();
    }

    inline int numColumns() const
    {
        assert(_paramsUPtr);

        return _paramsUPtr->core->n_cols;
    }

    void setNumColumns(int numColumns)
    {
        assert(_paramsUPtr);

        _throwIfActiveCallback();
        _paramsUPtr->core->n_cols = numColumns;
        _resetCodecCallback();
    }

    inline int numFrames() const
    {
        assert(_paramsUPtr);

        return _paramsUPtr->core->n_frames;
    }

    void setNumFrames(int numFrames)
    {
        assert(_paramsUPtr);

        _throwIfActiveCallback();
        _paramsUPtr->core->n_frames = numFrames;
        _resetCodecCallback();
    }

    inline int seed() const
    {
        assert(_paramsUPtr);

        return _paramsUPtr->core->seed;
    }

    void setSeed(int seed)
    {
        assert(_paramsUPtr);

        _throwIfActiveCallback();
        _paramsUPtr->core->seed = seed;
        _resetCodecCallback();
    }

    inline bool uniform() const
    {
        assert(_paramsUPtr);

        return _paramsUPtr->core->uniform;
    }

    void setUniform(bool uniform)
    {
        assert(_paramsUPtr);

        _throwIfActiveCallback();
        _paramsUPtr->core->uniform = uniform;
        _resetCodecCallback();
    }

private:

    std::unique_ptr<aff3ct::factory::Interleaver::parameters> _paramsUPtr;
    SetterCallback _throwIfActiveCallback;
    SetterCallback _resetCodecCallback;
};

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
        InterleaverParamsWrapper& rInterleaverParamsWrapper)
    {
        rEncoderParamsUPtr.reset(new EncoderParamType);
        rDecoderParamsUPtr.reset(new DecoderParamType);
        rInterleaverParamsWrapper.reset();
    }

    static void resetCodec(
        const std::unique_ptr<aff3ct::factory::Encoder::parameters>& encoderParamsUPtr,
        const std::unique_ptr<aff3ct::factory::Decoder::parameters>& decoderParamsUPtr,
        const InterleaverParamsWrapper& interleaverParamsWrapper,
        std::unique_ptr<aff3ct::module::Codec<B,Q>>& rCodecUPtr)
    {
        assert(encoderParamsUPtr);
        assert(decoderParamsUPtr);

        rCodecUPtr = AFF3CTDynamic::makeRACodec<B,Q>(
                         *safeDynamicCast<EncoderParamType>(encoderParamsUPtr),
                         *safeDynamicCast<DecoderParamType>(decoderParamsUPtr),
                         interleaverParamsWrapper.params());
        assert(rCodecUPtr);
    }
};

#define REGISTER_INTERLEAVER_CALL(name, fcn) \
    this->registerCall(&this->_interleaverParamsWrapper, name, &InterleaverParamsWrapper::fcn)

#define REGISTER_INTERLEAVER_CALLS() \
    REGISTER_INTERLEAVER_CALL("interleaverSize", size); \
    REGISTER_INTERLEAVER_CALL("setInterleaverSize", setSize); \
    REGISTER_INTERLEAVER_CALL("interleaverType", type); \
    REGISTER_INTERLEAVER_CALL("setInterleaverType", setType); \
    REGISTER_INTERLEAVER_CALL("interleaverPath", path); \
    REGISTER_INTERLEAVER_CALL("setInterleaverPath", setPath); \
    REGISTER_INTERLEAVER_CALL("interleaverReadOrder", readOrder); \
    REGISTER_INTERLEAVER_CALL("setInterleaverReadOrder", setReadOrder); \
    REGISTER_INTERLEAVER_CALL("interleaverNumColumns", numColumns); \
    REGISTER_INTERLEAVER_CALL("setInterleaverNumColumns", setNumColumns); \
    REGISTER_INTERLEAVER_CALL("interleaverNumFrames", numFrames); \
    REGISTER_INTERLEAVER_CALL("setInterleaverNumFrames", setNumFrames); \
    REGISTER_INTERLEAVER_CALL("interleaverSeed", seed); \
    REGISTER_INTERLEAVER_CALL("setInterleaverSeed", setSeed); \
    REGISTER_INTERLEAVER_CALL("interleaverUniform", uniform); \
    REGISTER_INTERLEAVER_CALL("setInterleaverUniform", setUniform);

//
// Encoder
//

template <typename B, typename Q>
class RAEncoder: public AFF3CTEncoder<B,Q>
{
public:
    using Class = RAEncoder<B,Q>;

    RAEncoder():
        AFF3CTEncoder<B,Q>(),
        _interleaverParamsWrapper(
            [this](){this->_throwIfBlockIsActive();},
            [this](){this->_resetCodec();})
    {
        RAHelper<B,Q>::initCoderParams(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_interleaverParamsWrapper);

        REGISTER_INTERLEAVER_CALLS();
    }

    virtual ~RAEncoder() = default;

protected:
    InterleaverParamsWrapper _interleaverParamsWrapper;

    void _resetCodec() override
    {
        assert(this->isActive());

        this->_encoderPtr = nullptr;
        RAHelper<B,Q>::resetCodec(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_interleaverParamsWrapper,
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

    RADecoder():
        AFF3CTDecoder<B,Q>(AFF3CTDecoderType::SIHO),
        _interleaverParamsWrapper(
            [this](){this->_throwIfBlockIsActive();},
            [this](){this->_resetCodec();})
    {
        RAHelper<B,Q>::initCoderParams(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_interleaverParamsWrapper);

        REGISTER_INTERLEAVER_CALLS();

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

protected:
    InterleaverParamsWrapper _interleaverParamsWrapper;

    void _resetCodec() override
    {
        assert(!this->isActive());

        this->_decoderSIHOSPtr.reset();
        RAHelper<B,Q>::resetCodec(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_interleaverParamsWrapper,
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
