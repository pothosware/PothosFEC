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
        _puncturerParamsUPtr(new aff3ct::factory::Puncturer_polar::parameters),
        _throwIfActiveCallback(throwIfActiveCallback),
        _resetCodecCallback(resetCodecCallback)
    {}

    virtual ~PolarParamsWrapper() = default;

    inline void reset()
    {
        _crcParamsUPtr.reset(new aff3ct::factory::CRC::parameters);
        _puncturerParamsUPtr.reset(new aff3ct::factory::Puncturer_polar::parameters);
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

    inline const aff3ct::factory::Puncturer_polar::parameters& puncturerParams() const
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

        _throwIfActiveCallback();
        _crcParamsUPtr->K = _frozenBitsGeneratorParamsUPtr->K = _puncturerParamsUPtr->K = static_cast<int>(K);
        _resetCodecCallback();
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

        _throwIfActiveCallback();
        _crcParamsUPtr->n_frames = _puncturerParamsUPtr->n_frames = static_cast<int>(numFrames);
        _resetCodecCallback();
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

        _throwIfActiveCallback();
        _frozenBitsGeneratorParamsUPtr->N_cw = _puncturerParamsUPtr->N_cw = static_cast<int>(NCW);
        _resetCodecCallback();
    }

    inline size_t crcSize() const
    {
        assert(_crcParamsUPtr);

        return static_cast<size_t>(_crcParamsUPtr->size);
    }

    inline void setCRCSize(size_t size)
    {
        assert(_crcParamsUPtr);

        _throwIfActiveCallback();
        _crcParamsUPtr->size = static_cast<int>(size);
        _resetCodecCallback();
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

        _throwIfActiveCallback();
        _crcParamsUPtr->type = crcType;
        _resetCodecCallback();
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

        _throwIfActiveCallback();
        _crcParamsUPtr->implem = crcImplementation;
        _resetCodecCallback();
    }

    inline size_t puncturerN() const
    {
        assert(_puncturerParamsUPtr);

        return static_cast<size_t>(_puncturerParamsUPtr->N);
    }

    inline void setPuncturerN(size_t N)
    {
        assert(_puncturerParamsUPtr);

        _throwIfActiveCallback();
        _puncturerParamsUPtr->N = static_cast<int>(N);
        _resetCodecCallback();
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

        _throwIfActiveCallback();
        _puncturerParamsUPtr->type = puncturerType;
        _resetCodecCallback();
    }

private:

    std::unique_ptr<aff3ct::factory::CRC::parameters> _crcParamsUPtr;
    std::unique_ptr<aff3ct::factory::Frozenbits_generator::parameters> _frozenBitsGeneratorParamsUPtr;
    std::unique_ptr<aff3ct::factory::Puncturer_polar::parameters> _puncturerParamsUPtr;
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
        std::unique_ptr<aff3ct::module::Codec<B,Q>>& rCodecUPtr)
    {
        assert(encoderParamsUPtr);
        assert(decoderParamsUPtr);

        std::unique_ptr<aff3ct::module::CRC<B>> crcUPtr;

        const auto *pPuncturerParams = usePuncturer ? &polarParamsWrapper.puncturerParams() : nullptr;
        const auto *pCRCParams = useCRC ? &polarParamsWrapper.crcParams() : nullptr;
        rCodecUPtr = AFF3CTDynamic::makePolarCodec<B,Q>(
                         polarParamsWrapper.frozenBitsGeneratorParams(),
                         *safeDynamicCast<EncoderParamType>(encoderParamsUPtr),
                         *safeDynamicCast<DecoderParamType>(decoderParamsUPtr),
                         pCRCParams,
                         pPuncturerParams,
                         crcUPtr);
        assert(rCodecUPtr);
    }
};

#define REGISTER_POLAR_CALL(fcn) \
    this->registerCall(&this->_polarParamsWrapper, #fcn, &PolarParamsWrapper::fcn)

#define REGISTER_POLAR_CALLS() \
    REGISTER_POLAR_CALL(numFrames); \
    REGISTER_POLAR_CALL(setNumFrames); \
    REGISTER_POLAR_CALL(NCW); \
    REGISTER_POLAR_CALL(setNCW); \
    REGISTER_POLAR_CALL(crcSize); \
    REGISTER_POLAR_CALL(setCRCSize); \
    REGISTER_POLAR_CALL(crcType); \
    REGISTER_POLAR_CALL(setCRCType); \
    REGISTER_POLAR_CALL(crcImplementation); \
    REGISTER_POLAR_CALL(setCRCImplementation); \
    REGISTER_POLAR_CALL(puncturerN); \
    REGISTER_POLAR_CALL(setPuncturerN); \
    REGISTER_POLAR_CALL(puncturerType); \
    REGISTER_POLAR_CALL(setPuncturerType); \

//
// Encoder
//

template <typename B, typename Q>
class PolarEncoder: public AFF3CTEncoder<B,Q>
{
public:
    using Class = PolarEncoder<B,Q>;

    PolarEncoder():
        AFF3CTEncoder<B,Q>(),
        _polarParamsWrapper(
            [this](){this->_throwIfBlockIsActive();},
            [this](){this->_resetCodec();}),
        _usePuncturer(false),
        _useCRC(false)
    {
        PolarHelper<B,Q>::initCoderParams(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_polarParamsWrapper);

        REGISTER_POLAR_CALLS();

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, usePuncturer));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setUsePuncturer));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, useCRC));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setUseCRC));
    }

    virtual ~PolarEncoder() = default;

    void setK(size_t K) override
    {
        this->_throwIfBlockIsActive();

        AFF3CTCoderBase<B,Q>::setK(K);
        _polarParamsWrapper.setK(K);

        this->_resetCodec();
    }

    bool usePuncturer() const
    {
        return _usePuncturer;
    }

    void setUsePuncturer(bool usePuncturer)
    {
        this->_throwIfBlockIsActive();
        _usePuncturer = usePuncturer;
        this->_resetCodec();
    }

    bool useCRC() const
    {
        return _useCRC;
    }

    void setUseCRC(bool useCRC)
    {
        this->_throwIfBlockIsActive();
        _useCRC = useCRC;
        this->_resetCodec();
    }

protected:
    PolarParamsWrapper _polarParamsWrapper;
    bool _usePuncturer;
    bool _useCRC;

    void _resetCodec() override
    {
        assert(!this->isActive());

        this->_encoderPtr = nullptr;
        PolarHelper<B,Q>::resetCodec(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_polarParamsWrapper,
            _usePuncturer,
            _useCRC,
            this->_codecUPtr);

        this->_encoderPtr = this->_codecUPtr->get_encoder().get();
        assert(this->_encoderPtr);
    }
};

//
// Decoder
//

template <typename B, typename Q, bool siho>
class PolarDecoder: public AFF3CTDecoder<B,Q>
{
public:
    using Class = PolarDecoder<B,Q,siho>;
    using DecoderParamType = typename PolarHelper<B,Q>::DecoderParamType;

    PolarDecoder():
        AFF3CTDecoder<B,Q>(siho ? AFF3CTDecoderType::SIHO : AFF3CTDecoderType::SISO),
        _polarParamsWrapper(
            [this](){this->_throwIfBlockIsActive();},
            [this](){this->_resetCodec();}),
        _usePuncturer(false),
        _useCRC(false)
    {
        PolarHelper<B,Q>::initCoderParams(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_polarParamsWrapper);

        REGISTER_POLAR_CALLS();

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, usePuncturer));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setUsePuncturer));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, useCRC));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setUseCRC));
    }

    virtual ~PolarDecoder() = default;

    void setK(size_t K) override
    {
        this->_throwIfBlockIsActive();

        AFF3CTCoderBase<B,Q>::setK(K);
        _polarParamsWrapper.setK(K);

        this->_resetCodec();
    }

    bool usePuncturer() const
    {
        return _usePuncturer;
    }

    void setUsePuncturer(bool usePuncturer)
    {
        this->_throwIfBlockIsActive();
        _usePuncturer = usePuncturer;
        this->_resetCodec();
    }

    bool useCRC() const
    {
        return _useCRC;
    }

    void setUseCRC(bool useCRC)
    {
        this->_throwIfBlockIsActive();
        _useCRC = useCRC;
        this->_resetCodec();
    }

protected:
    PolarParamsWrapper _polarParamsWrapper;
    bool _usePuncturer;
    bool _useCRC;

    void _resetCodec() override
    {
        assert(!this->isActive());

        this->_decoderSISOSPtr.reset();
        this->_decoderSIHOSPtr.reset();
        PolarHelper<B,Q>::resetCodec(
            this->_encoderParamsUPtr,
            this->_decoderParamsUPtr,
            this->_polarParamsWrapper,
            _usePuncturer,
            _useCRC,
            this->_codecUPtr);

        if(siho) this->_decoderSIHOSPtr = this->_codecAsCodecSIHO()->get_decoder_siho();
        else     this->_decoderSISOSPtr = this->_codecAsCodecSISO()->get_decoder_siso();
    }
};

//
// Registration
//

static Pothos::Block* makePolarEncoderBlock(const Pothos::DType& dtype)
{
#define IfTypeThenEncoder(T) \
    if(doesDTypeMatch<T>(dtype)) return new PolarEncoder<B, AFF3CTTypeTraits<B>::Q>();

    IfTypeThenEncoder(B_8)
    IfTypeThenEncoder(B_16)
    IfTypeThenEncoder(B_32)
    IfTypeThenEncoder(B_64)

    throw Pothos::InvalidArgumentException("PolarEncoder: invalid type "+dtype.toString());
}

static Pothos::Block* makePolarDecoderBlock(const Pothos::DType& dtype, const std::string& mode)
{
#define IfTypeThenDecoder(T) \
    if(doesDTypeMatch<T>(dtype) && (mode == "SIHO")) return new PolarDecoder<B, AFF3CTTypeTraits<B>::Q, true>(); \
    if(doesDTypeMatch<T>(dtype) && (mode == "SISO")) return new PolarDecoder<B, AFF3CTTypeTraits<B>::Q, false>();

    IfTypeThenDecoder(B_8)
    IfTypeThenDecoder(B_16)
    IfTypeThenDecoder(B_32)
    IfTypeThenDecoder(B_64)

    throw Pothos::InvalidArgumentException("PolarDecoder: invalid type "+dtype.toString() + ", mode "+mode);
}

static Pothos::BlockRegistry registerPolarEncoder(
    "/fec/polar_encoder",
    &makePolarEncoderBlock);

static Pothos::BlockRegistry registerPolarDecoder(
    "/fec/polar_decoder",
    &makePolarDecoderBlock);
