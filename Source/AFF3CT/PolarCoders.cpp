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

class PolarParamsWrapper
{
public:
    PolarParamsWrapper() = delete;

    PolarParamsWrapper(
        const SetterCallback& throwIfActiveCallback,
        const SetterCallback& resetCodecCallback
    ):
        _crcParamsUPtr(new aff3ct::factory::CRC::parameters),
        _frozenBitsGeneratorParamsUPtr(new aff3ct::factory::Frozenbits_generator::parameters),
        _puncturerParamsUPtr(new aff3ct::factory::Puncturer::parameters),
        _throwIfActiveCallback(throwIfActiveCallback),
        _resetCodecCallback(resetCodecCallback)
    {}

    virtual ~PolarParamsWrapper() = default;

    inline void reset()
    {
        _crcParamsUPtr.reset(new aff3ct::factory::CRC::parameters);
        _puncturerParamsUPtr.reset(new aff3ct::factory::Puncturer::parameters);
    }

    inline const aff3ct::factory::CRC::parameters& crcParams() const
    {
        assert(_crcParamsUPtr);

        return *_crcParamsUPtr;
    }

    inline const aff3ct::factory::Frozenbits_generator::parameters& frozenBitsGeneratorParams() const
    {
        assert(_frozenBitsGeneratorParamsUPtr);

        return *_frozenBitsGeneratorParamsUPtr;
    }

    inline const aff3ct::factory::Puncturer::parameters& puncturerParams() const
    {
        assert(_crcParamsUPtr);

        return *_puncturerParamsUPtr;
    }

    inline size_t K() const
    {
        assert(_crcParamsUPtr);
        assert(_frozenBitsGeneratorParamsUPtr);
        assert(_puncturerParamsUPtr);
        assert(_crcParamsUPtr->K == _frozenBitsGeneratorParamsUPtr->K);
        assert(_crcParamsUPtr->K == _puncturerParamsUPtr->K);

        return static_cast<int>(_crcParamsUPtr->K);
    }

    inline void setK(size_t K)
    {
        assert(_crcParamsUPtr);
        assert(_frozenBitsGeneratorParamsUPtr);
        assert(_puncturerParamsUPtr);

        _crcParamsUPtr->K = _frozenBitsGeneratorParamsUPtr->K = _puncturerParamsUPtr->K = static_cast<int>(K);
    }

    inline size_t numFrames() const
    {
        assert(_crcParamsUPtr);
        assert(_puncturerParamsUPtr);
        assert(_crcParamsUPtr->n_frames == _puncturerParamsUPtr->n_frames);

        return static_cast<int>(_crcParamsUPtr->n_frames);
    }

    inline void setNumFrames(size_t numFrames)
    {
        assert(_crcParamsUPtr);
        assert(_puncturerParamsUPtr);

        _crcParamsUPtr->n_frames = _puncturerParamsUPtr->n_frames = static_cast<int>(numFrames);
    }

    inline size_t NCW() const
    {
        assert(_frozenBitsGeneratorParamsUPtr);
        assert(_puncturerParamsUPtr);

        return static_cast<size_t>(_puncturerParamsUPtr->N_cw);
    }

    inline void setNCW(size_t NCW)
    {
        assert(_frozenBitsGeneratorParamsUPtr);
        assert(_puncturerParamsUPtr);

        _frozenBitsGeneratorParamsUPtr->N_cw = _puncturerParamsUPtr->N_cw = static_cast<int>(NCW);
    }

    inline size_t crcSize() const
    {
        assert(_crcParamsUPtr);

        return static_cast<size_t>(_crcParamsUPtr->size);
    }

    inline void setCRCSize(size_t size)
    {
        assert(_crcParamsUPtr);

        _crcParamsUPtr->size = static_cast<int>(size);
    }

    inline std::string crcType() const
    {
        assert(_crcParamsUPtr);

        return _crcParamsUPtr->type;
    }

    // TODO: input validation
    inline void setCRCType(const std::string& crcType)
    {
        assert(_crcParamsUPtr);

        _crcParamsUPtr->type = crcType;
    }

    inline std::string crcImplementation() const
    {
        assert(_crcParamsUPtr);

        return _crcParamsUPtr->implem;
    }

    // TODO: input validation
    inline void setCRCImplementation(const std::string& crcImplementation)
    {
        assert(_crcParamsUPtr);

        _crcParamsUPtr->implem = crcImplementation;
    }

    inline size_t puncturerN() const
    {
        assert(_puncturerParamsUPtr);

        return static_cast<size_t>(_puncturerParamsUPtr->N);
    }

    inline void setPuncturerN(size_t N)
    {
        assert(_puncturerParamsUPtr);

        _puncturerParamsUPtr->N = static_cast<int>(N);
    }

    inline std::string puncturerType() const
    {
        assert(_puncturerParamsUPtr);

        return _puncturerParamsUPtr->type;
    }

    // TODO: input validation
    inline void setPuncturerType(const std::string& puncturerType)
    {
        assert(_puncturerParamsUPtr);

        _puncturerParamsUPtr->type = puncturerType;
    }

private:

    std::unique_ptr<aff3ct::factory::CRC::parameters> _crcParamsUPtr;
    std::unique_ptr<aff3ct::factory::Frozenbits_generator::parameters> _frozenBitsGeneratorParamsUPtr;
    std::unique_ptr<aff3ct::factory::Puncturer::parameters> _puncturerParamsUPtr;
    SetterCallback _throwIfActiveCallback;
    SetterCallback _resetCodecCallback;
};

//
// Helper class
//

template <typename B, typename Q>
struct PolarHelper
{
    using EncoderParamType = aff3ct::factory::Encoder_polar::parameters;
    using DecoderParamType = aff3ct::factory::Decoder_polar::parameters;

    static void initCoderParams(
        std::unique_ptr<aff3ct::factory::Encoder::parameters>& rEncoderParamsUPtr,
        std::unique_ptr<aff3ct::factory::Decoder::parameters>& rDecoderParamsUPtr,
        PolarParamsWrapper& rPolarParamsWrapper)
    {
        rEncoderParamsUPtr.reset(new EncoderParamType);
        rDecoderParamsUPtr.reset(new DecoderParamType);
        rPolarParamsWrapper.reset();
    }

    static void resetCodec(
        const std::unique_ptr<aff3ct::factory::Encoder::parameters>& encoderParamsUPtr,
        const std::unique_ptr<aff3ct::factory::Decoder::parameters>& decoderParamsUPtr,
        const PolarParamsWrapper& polarParamsWrapper,
        bool usePuncturer,
        bool useCRC,
        std::unique_ptr<aff3ct::module::CRC<B>>& rCRCUPtr,
        std::unique_ptr<aff3ct::module::Codec<B,Q>>& rCodecUPtr)
    {
        assert(encoderParamsUPtr);
        assert(decoderParamsUPtr);

        /*
        rCodecUPtr = AFF3CTDynamic::makePolarCodec<B,Q>(
                         *safeDynamicCast<EncoderParamType>(encoderParamsUPtr),
                         *safeDynamicCast<DecoderParamType>(decoderParamsUPtr),
                         polarParamsWrapper.params());
        assert(rCodecUPtr);
        */
    }
};

/*

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
    REGISTER_INTERLEAVER_CALL("setInterleaverUniform", setUniform); \

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
        assert(!this->isActive());

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

        if(0 == numIterations)
        {
            throw Pothos::RangeException("numIterations must be > 0");
        }

        safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr)->n_ite = static_cast<int>(numIterations);
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

*/
