// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "Utility.hpp"

#include <Pothos/Framework.hpp>

#include <aff3ct.hpp>

#include <cassert>
#include <functional>
#include <memory>

enum class AFF3CTDecoderType
{
    SISO,
    SIHO,
    HIHO
};

using DecodeFcn = std::function<void(const void*, void*)>;

template <typename B, typename Q>
using CodecUPtr = std::unique_ptr<aff3ct::module::Codec<B,Q>>;

template <typename B, typename Q>
class AFF3CTCoderBase: public Pothos::Block
{
public:
    using Class = AFF3CTCoderBase<B,Q>;

    AFF3CTCoderBase();
    virtual ~AFF3CTCoderBase();

    void activate() override;

    size_t N() const;
    virtual void setN(size_t N);

    size_t K() const;
    virtual void setK(size_t K);

    bool systematic() const;
    virtual void setSystematic(bool systematic);

    std::string blockStartID() const;
    void setBlockStartID(const std::string& blockStartID);

    void propagateLabels(const Pothos::InputPort* input) override;

    virtual void work() = 0;

protected:
    std::unique_ptr<aff3ct::factory::Encoder::parameters> _encoderParamsUPtr;
    std::unique_ptr<aff3ct::factory::Decoder::parameters> _decoderParamsUPtr;
    CodecUPtr<B,Q> _codecUPtr;

    std::string _blockStartID;

    void _throwIfBlockIsActive() const;

    virtual void _setPortReserves() = 0;
    virtual void _resetCodec() = 0;
};

template <typename B, typename Q>
class AFF3CTEncoder: public AFF3CTCoderBase<B,Q>
{
public:
    using Class = AFF3CTEncoder<B,Q>;

    AFF3CTEncoder();
    virtual ~AFF3CTEncoder();

    void work() override;

protected:
    // Stored as unique_ptr
    aff3ct::module::Encoder<B>* _encoderPtr;

    virtual void _work();
    virtual void _blockIDWork();
    void _setPortReserves() override;
};

template <typename B, typename Q>
class AFF3CTDecoder: public AFF3CTCoderBase<B,Q>
{
public:
    using Class = AFF3CTDecoder<B,Q>;

    AFF3CTDecoder(AFF3CTDecoderType decoderType);
    virtual ~AFF3CTDecoder();

    void work() override;

protected:
    AFF3CTDecoderType _decoderType;
    DecodeFcn _decodeFcn;

    std::shared_ptr<aff3ct::module::Decoder_SISO<Q>> _decoderSISOSPtr;
    std::shared_ptr<aff3ct::module::Decoder_SIHO<B,Q>> _decoderSIHOSPtr;
    std::shared_ptr<aff3ct::module::Decoder_HIHO<B>> _decoderHIHOSPtr;

    inline aff3ct::module::Codec_SISO<B,Q>* _codecAsCodecSISO()
    {
        assert(this->_codecUPtr);
        return safeDynamicCast<aff3ct::module::Codec_SISO<B,Q>>(this->_codecUPtr);
    }

    inline const aff3ct::module::Codec_SISO<B,Q>* _codecAsCodecSISO() const
    {
        return const_cast<Class*>(this)->_codecAsCodecSISO();
    }

    inline aff3ct::module::Codec_SIHO<B,Q>* _codecAsCodecSIHO()
    {
        assert(this->_codecUPtr);
        return safeDynamicCast<aff3ct::module::Codec_SIHO<B,Q>>(this->_codecUPtr);
    }

    inline const aff3ct::module::Codec_SIHO<B,Q>* _codecAsCodecSIHO() const
    {
        return const_cast<Class*>(this)->_codecAsCodecSIHO();
    }

    inline aff3ct::module::Codec_HIHO<B,Q>* _codecAsCodecHIHO()
    {
        assert(this->_codecUPtr);
        return safeDynamicCast<aff3ct::module::Codec_HIHO<B,Q>>(this->_codecUPtr);
    }

    inline const aff3ct::module::Codec_HIHO<B,Q>* _codecAsCodecHIHO() const
    {
        return const_cast<Class*>(this)->_codecAsCodecHIHO();
    }

    void _setPortReserves() override;
};
