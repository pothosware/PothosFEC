// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BCHCodec.hpp"
#include "CRCFactory.hpp"
#include "LDPCCodec.hpp"
#include "PolarCodec.hpp"
#include "RACodec.hpp"

#include <Tools/types.h>

namespace AFF3CTDynamic { namespace POTHOS_SIMD_NAMESPACE {

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<B,Q>> makeBCHCodec(
    const aff3ct::factory::Encoder_BCH::parameters& encParams,
    const aff3ct::factory::Decoder_BCH::parameters& decParams)
{
    return std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<B,Q>>(new BCHCodec<B,Q>(encParams, decParams));
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<B,Q>> makeLDPCCodec(
    const aff3ct::factory::Encoder_LDPC::parameters& encParams,
    const aff3ct::factory::Decoder_LDPC::parameters& decParams,
    aff3ct::factory::Puncturer_LDPC::parameters* pPctParams)
{
    return std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<B,Q>>(new LDPCCodec<B,Q>(encParams, decParams, pPctParams));
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_polar<B,Q>> makePolarCodec(
    const aff3ct::factory::Frozenbits_generator::parameters &frozenBitsParams,
    const aff3ct::factory::Encoder_polar       ::parameters &encoderParams,
    const aff3ct::factory::Decoder_polar       ::parameters &decoderParams,
    const aff3ct::factory::CRC                 ::parameters *pCRCParams,
    const aff3ct::factory::Puncturer_polar     ::parameters *pPuncturerParams)
{
    std::unique_ptr<aff3ct::module::CRC<B>> crcUPtr(pCRCParams ? CRCFactory::build<B>(*pCRCParams) : nullptr);

    return std::unique_ptr<aff3ct::module::Codec_polar<B,Q>>(new PolarCodec<B,Q>(
        frozenBitsParams,
        encoderParams,
        decoderParams,
        pPuncturerParams));
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO<B,Q>> makeRACodec(
    const aff3ct::factory::Encoder_RA ::parameters &encParams,
    const aff3ct::factory::Decoder_RA ::parameters &decParams,
    const aff3ct::factory::Interleaver::parameters &itlParams)
{
    return std::unique_ptr<aff3ct::module::Codec_SIHO<B,Q>>(new RACodec<B,Q>(encParams, decParams, itlParams));
}

#define SPECIALIZE_TMPLS(T1,T2) \
    template \
    std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<T1,T2>> makeBCHCodec( \
        const aff3ct::factory::Encoder_BCH::parameters&, \
        const aff3ct::factory::Decoder_BCH::parameters&); \
    template \
    std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<T1,T2>> makeLDPCCodec( \
        const aff3ct::factory::Encoder_LDPC::parameters&, \
        const aff3ct::factory::Decoder_LDPC::parameters&, \
        aff3ct::factory::Puncturer_LDPC::parameters*); \
    template \
    std::unique_ptr<aff3ct::module::Codec_polar<T1,T2>> makePolarCodec( \
        const aff3ct::factory::Frozenbits_generator::parameters&, \
        const aff3ct::factory::Encoder_polar       ::parameters&, \
        const aff3ct::factory::Decoder_polar       ::parameters&, \
        const aff3ct::factory::CRC                 ::parameters*, \
        const aff3ct::factory::Puncturer_polar     ::parameters*); \
    template \
    std::unique_ptr<aff3ct::module::Codec_SIHO<T1,T2>> makeRACodec( \
        const aff3ct::factory::Encoder_RA::parameters&, \
        const aff3ct::factory::Decoder_RA::parameters&, \
        const aff3ct::factory::Interleaver::parameters&);

SPECIALIZE_TMPLS(B_8,Q_8)
SPECIALIZE_TMPLS(B_16,Q_16)
SPECIALIZE_TMPLS(B_32,Q_32)
SPECIALIZE_TMPLS(B_64,Q_64)

}}
