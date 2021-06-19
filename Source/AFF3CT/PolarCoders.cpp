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
class PolarCoder: public BaseClass
{
public:
    using Class = PolarCoder<B,Q,BaseClass>;
    using EncoderParamType = aff3ct::factory::Encoder_polar::parameters;
    using DecoderParamType = aff3ct::factory::Decoder_polar::parameters;

    PolarCoder(): BaseClass()
    {
        _ctorCommon();
    }

    PolarCoder(AFF3CTDecoderType type): BaseClass(type)
    {
        _ctorCommon();
    }

    virtual ~PolarCoder() = default;

    void setK(size_t K) override
    {
        this->_throwIfBlockIsActive();

        AFF3CTCoderBase<B,Q>::setK(K);
        _crcParamsUPtr->K = _frozenBitsGeneratorParamsUPtr->K = _puncturerParamsUPtr->K = static_cast<int>(K);

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

    size_t numFrames() const
    {
        assert(_crcParamsUPtr);
        assert(_puncturerParamsUPtr);
        assert(_crcParamsUPtr->n_frames == _puncturerParamsUPtr->n_frames);

        return static_cast<int>(_crcParamsUPtr->n_frames);
    }

    void setNumFrames(size_t numFrames)
    {
        assert(_crcParamsUPtr);
        assert(_puncturerParamsUPtr);

        this->_throwIfBlockIsActive();
        _crcParamsUPtr->n_frames = _puncturerParamsUPtr->n_frames = static_cast<int>(numFrames);
        this->_resetCodec();
    }

    size_t NCW() const
    {
        assert(_frozenBitsGeneratorParamsUPtr);
        assert(_puncturerParamsUPtr);

        return static_cast<size_t>(_puncturerParamsUPtr->N_cw);
    }

    void setNCW(size_t NCW)
    {
        assert(_frozenBitsGeneratorParamsUPtr);
        assert(_puncturerParamsUPtr);

        this->_throwIfBlockIsActive();
        _frozenBitsGeneratorParamsUPtr->N_cw = _puncturerParamsUPtr->N_cw = static_cast<int>(NCW);
        this->_resetCodec();
    }

    size_t crcSize() const
    {
        assert(_crcParamsUPtr);

        return static_cast<size_t>(_crcParamsUPtr->size);
    }

    void setCRCSize(size_t size)
    {
        assert(_crcParamsUPtr);

        this->_throwIfBlockIsActive();
        _crcParamsUPtr->size = static_cast<int>(size);
        this->_resetCodec();
    }

    std::string crcType() const
    {
        assert(_crcParamsUPtr);

        return _crcParamsUPtr->type;
    }

    // TODO: input validation
    void setCRCType(const std::string& crcType)
    {
        assert(_crcParamsUPtr);

        this->_throwIfBlockIsActive();
        _crcParamsUPtr->type = crcType;
        this->_resetCodec();
    }

    std::string crcImplementation() const
    {
        assert(_crcParamsUPtr);

        return _crcParamsUPtr->implem;
    }

    // TODO: input validation
    void setCRCImplementation(const std::string& crcImplementation)
    {
        assert(_crcParamsUPtr);

        this->_throwIfBlockIsActive();
        _crcParamsUPtr->implem = crcImplementation;
        this->_resetCodec();
    }

    size_t puncturerN() const
    {
        assert(_puncturerParamsUPtr);

        return static_cast<size_t>(_puncturerParamsUPtr->N);
    }

    void setPuncturerN(size_t N)
    {
        assert(_puncturerParamsUPtr);

        this->_throwIfBlockIsActive();
        _puncturerParamsUPtr->N = static_cast<int>(N);
        this->_resetCodec();
    }

    std::string puncturerType() const
    {
        assert(_puncturerParamsUPtr);

        return _puncturerParamsUPtr->type;
    }

    // TODO: input validation
    void setPuncturerType(const std::string& puncturerType)
    {
        assert(_puncturerParamsUPtr);

        this->_throwIfBlockIsActive();
        _puncturerParamsUPtr->type = puncturerType;
        this->_resetCodec();
    }

protected:
    bool _usePuncturer;
    bool _useCRC;
    std::unique_ptr<aff3ct::factory::CRC::parameters> _crcParamsUPtr;
    std::unique_ptr<aff3ct::factory::Frozenbits_generator::parameters> _frozenBitsGeneratorParamsUPtr;
    std::unique_ptr<aff3ct::factory::Puncturer_polar::parameters> _puncturerParamsUPtr;
    std::unique_ptr<aff3ct::module::CRC<B>> _crcUPtr;

    void _resetCodec() override
    {
        assert(this->_encoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        assert(this->_decoderParamsUPtr);
        assert(this->_crcParamsUPtr);
        assert(this->_frozenBitsParamsUPtr);
        assert(this->_puncturerParamsUPtr);

        const auto *pPuncturerParams = this->_usePuncturer ? this->_puncturerParamsUPtr.get() : nullptr;
        const auto *pCRCParams = this->_useCRC ? this->_crcParamsUPtr.get() : nullptr;
        this->_codecUPtr = AFF3CTDynamic::makePolarCodec<B,Q>(
                               *this->_frozenBitsGeneratorParamsUPtr,
                               *safeDynamicCast<EncoderParamType>(this->_encoderParamsUPtr),
                               *safeDynamicCast<DecoderParamType>(this->_decoderParamsUPtr),
                               pCRCParams,
                               pPuncturerParams,
                               this->_crcUPtr);
        assert(this->_codecUPtr);
    }

private:
    void _ctorCommon()
    {
        _usePuncturer = true;
        _useCRC = true;

        this->_encoderParamsUPtr.reset(new EncoderParamType);
        this->_decoderParamsUPtr.reset(new DecoderParamType);
        this->_crcParamsUPtr.reset(new aff3ct::factory::CRC::parameters);
        this->_frozenBitsGeneratorParamsUPtr.reset(new aff3ct::factory::Frozenbits_generator::parameters);
        this->_puncturerParamsUPtr.reset(new aff3ct::factory::Puncturer_polar::parameters);

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, usePuncturer));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setUsePuncturer));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, useCRC));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setUseCRC));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, numFrames));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setNumFrames));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, NCW));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setNCW));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, crcSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setCRCSize));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, crcType));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setCRCType));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, crcImplementation));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setCRCImplementation));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, puncturerN));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setPuncturerN));

        this->registerCall(this, POTHOS_FCN_TUPLE(Class, puncturerType));
        this->registerCall(this, POTHOS_FCN_TUPLE(Class, setPuncturerType));
    }
};

//
// Encoder
//

template <typename B, typename Q>
class PolarEncoder: public PolarCoder<B,Q,AFF3CTEncoder<B,Q>>
{
public:
    using BaseClass = PolarCoder<B,Q,AFF3CTEncoder<B,Q>>;

    PolarEncoder(): BaseClass(){}
    virtual ~PolarEncoder() = default;

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
class PolarDecoder: public PolarCoder<B,Q,AFF3CTDecoder<B,Q>>
{
public:
    using BaseClass = PolarCoder<B,Q,AFF3CTDecoder<B,Q>>;

    PolarDecoder(): BaseClass(siho ? AFF3CTDecoderType::SIHO : AFF3CTDecoderType::SISO){}
    virtual ~PolarDecoder() = default;

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
            this->_decoderSISOSPtr = this->_codecAsCodecSISO()->get_decoder_siso();
            assert(this->_decoderSISOSPtr);
        }
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
