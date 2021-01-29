// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AFF3CTDynamic/EntryPoints.hpp"
#include "CoderBase.hpp"
#include "Utility.hpp"

#include <Pothos/Framework.hpp>

//
// Base class
//

template <typename B, typename Q>
class AFF3CTBCHCoderBase: public AFF3CTCoderBase<B,Q>
{
public:
    AFF3CTBCHCoderBase(CoderType coderType): AFF3CTCoderBase<B,Q>(coderType)
    {
        // TODO: set default parameter fields
    }

    virtual ~AFF3CTBCHCoderBase() = default;

protected:
    aff3ct::factory::Encoder_BCH::parameters _encParams;
    aff3ct::factory::Decoder_BCH::parameters _decParams;

    void _resetCodec() override
    {
        this->_codecUPtr = AFF3CTDynamic::makeBCHCodec<B,Q>(_encParams, _decParams);
    }
};

//
// Encoder
//

template <typename B, typename Q>
class AFF3CTBCHEncoder: public AFF3CTBCHCoderBase<B,Q>
{
public:
    AFF3CTBCHEncoder(): AFF3CTBCHCoderBase<B,Q>(CoderType::ENCODER)
    {}

    virtual ~AFF3CTBCHEncoder() = default;

private:
    void _resetCodec() override
    {
        AFF3CTBCHCoderBase<B,Q>::_resetCodec();

        this->_encoderSPtr = this->_codecUPtr->get_encoder();
    }
};

//
// Decoder SIHO
//

template <typename B, typename Q>
class AFF3CTBCHDecoderSIHO: public AFF3CTBCHCoderBase<B,Q>
{
public:
    AFF3CTBCHDecoderSIHO(): AFF3CTBCHCoderBase<B,Q>(CoderType::DECODER_SIHO)
    {}

    virtual ~AFF3CTBCHDecoderSIHO() = default;

private:
};

//
// Decoder HIHO
//

template <typename B, typename Q>
class AFF3CTBCHDecoderHIHO: public AFF3CTBCHCoderBase<B,Q>
{
public:
    AFF3CTBCHDecoderHIHO(): AFF3CTBCHCoderBase<B,Q>(CoderType::DECODER_HIHO)
    {}

    virtual ~AFF3CTBCHDecoderHIHO() = default;

private:
    aff3ct::module::Decoder_HIHO<Q>* getDecoderHIHO() const override;
};

//
// Registration
//

//
// Template specializations
//

template class AFF3CTBCHCoderBase<B_8,Q_8>;
template class AFF3CTBCHCoderBase<B_16,Q_16>;
template class AFF3CTBCHCoderBase<B_32,Q_32>;
template class AFF3CTBCHCoderBase<B_64,Q_64>;

/*
template <typename B, typename Q>
AFF3CTCoderBase<B,Q>::AFF3CTCoderBase(CoderType coderType):
    _coderType(coderType),
    _codecUPtr(nullptr)
{
    static const Pothos::DType dtype(typeid(B));

    this->setupInput(0, dtype);
    this->setupOutput(0, dtype);
}

template <typename B, typename Q>
AFF3CTCoderBase<B,Q>::~AFF3CTCoderBase()
{
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::work()
{
    const auto elems = this->workInfo().minElements;
    if(0 == elems) return;

    switch(_coderType)
    {
        case CoderType::ENCODER:
            this->_workEncoder();
            break;

        case CoderType::DECODER_SISO:
            this->_workDecoderSISO();
            break;

        case CoderType::DECODER_SIHO:
            this->_workDecoderSIHO();
            break;

        case CoderType::DECODER_HIHO:
            this->_workDecoderHIHO();
            break;

        default:
            throw Pothos::AssertionViolationException("Invalid internal coder type enum: "+std::to_string(int(_coderType)));
    }
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_workEncoder()
{
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_workDecoderSISO()
{
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_workDecoderSIHO()
{
}

template <typename B, typename Q>
void AFF3CTCoderBase<B,Q>::_workDecoderHIHO()
{
}

*/
