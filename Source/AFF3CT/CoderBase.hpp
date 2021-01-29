// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <Pothos/Framework.hpp>

#include <aff3ct.hpp>

#include <memory>

enum class CoderType
{
    ENCODER,
    DECODER_SISO,
    DECODER_SIHO,
    DECODER_HIHO
};

template <typename B, typename Q>
class AFF3CTCoderBase: public Pothos::Block
{
public:
    using Class = AFF3CTCoderBase<B,Q>;
    using CodecUPtr = std::unique_ptr<aff3ct::module::Codec<B,Q>>;

    AFF3CTCoderBase(CoderType coderType);
    virtual ~AFF3CTCoderBase();

    virtual void work();

protected:
    CoderType _coderType;
    CodecUPtr _codecUPtr;

    virtual void _resetCodec() = 0;

    std::shared_ptr<aff3ct::module::Encoder<B>> _encoderSPtr;
    std::shared_ptr<aff3ct::module::Decoder_SISO<B>> _encoderSISOSPtr;
    std::shared_ptr<aff3ct::module::Decoder_SIHO<B>> _encoderSIHOSPtr;
    std::shared_ptr<aff3ct::module::Decoder_HIHO<B>> _encoderHIHOSPtr;

    void _workEncoder();
    void _workDecoderSISO();
    void _workDecoderSIHO();
    void _workDecoderHIHO();
};
