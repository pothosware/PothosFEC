// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <Pothos/Framework.hpp>

#include <aff3ct.hpp>

#include <memory>

enum class AFF3CTDecoderType
{
    SISO,
    SIHO,
    HIHO
};

template <typename B, typename Q>
using CodecUPtr = std::unique_ptr<aff3ct::module::Codec<B,Q>>;

template <typename B, typename Q>
class AFF3CTCoderBase: public Pothos::Block
{
public:
    using Class = AFF3CTCoderBase<B,Q>;

    AFF3CTCoderBase();
    virtual ~AFF3CTCoderBase();

    size_t N() const;
    virtual void setN(size_t N);

    size_t K() const;
    virtual void setK(size_t K);

    bool systematic() const;
    virtual void setSystematic(bool systematic);

protected:
    std::unique_ptr<aff3ct::factory::Encoder::parameters> _encoderParamsUPtr;
    std::unique_ptr<aff3ct::factory::Decoder::parameters> _decoderParamsUPtr;
    CodecUPtr<B,Q> _codecUPtr;

    void _throwIfBlockIsActive() const;

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

    std::shared_ptr<aff3ct::module::Decoder_SISO<B>> _decoderSISOSPtr;
    std::shared_ptr<aff3ct::module::Decoder_SIHO<B>> _decoderSIHOSPtr;
    std::shared_ptr<aff3ct::module::Decoder_HIHO<B>> _decoderHIHOSPtr;

    void _workSISO();
    void _workSIHO();
    void _workHIHO();
};