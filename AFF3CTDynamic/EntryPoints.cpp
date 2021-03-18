// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EntryPoints.hpp"

#ifdef AFF3CT_SIMD_SUPPORTED
#include "AFF3CTDynamic_SIMD.hpp"
#endif

namespace AFF3CTDynamic
{
#ifdef AFF3CT_SIMD_SUPPORTED

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<B,Q>> makeBCHCodec(
    const aff3ct::factory::Encoder_BCH::parameters& encParams,
    const aff3ct::factory::Decoder_BCH::parameters& decParams)
{
    static const auto simdMaker = AFF3CTDynamic::makeBCHCodecDispatch<B,Q>();

    return simdMaker(encParams, decParams);
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<B,Q>> makeLDPCCodec(
    const aff3ct::factory::Encoder_LDPC::parameters& encParams,
    const aff3ct::factory::Decoder_LDPC::parameters& decParams,
    aff3ct::factory::Puncturer_LDPC::parameters* pPctParams)
{
    static const auto simdMaker = AFF3CTDynamic::makeLDPCCodecDispatch<B,Q>();

    return simdMaker(encParams, decParams, pPctParams);
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_polar<B,Q>> makePolarCodec(
    const aff3ct::factory::Frozenbits_generator::parameters &frozenBitsParams,
    const aff3ct::factory::Encoder_polar       ::parameters &encoderParams,
    const aff3ct::factory::Decoder_polar       ::parameters &decoderParams,
    const aff3ct::factory::CRC                 ::parameters &crcParams,
    const aff3ct::factory::Puncturer_polar     ::parameters *pPuncturerParams,
    std::unique_ptr<aff3ct::module::CRC<B>>& rCRCUPtr)
{
    static const auto simdMaker = AFF3CTDynamic::makePolarCodecDispatch<B,Q>();

    return simdMaker(
        frozenBitsParams,
        encoderParams,
        decoderParams,
        crcParams,
        pPuncturerParams,
        rCRCUPtr);
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO<B,Q>> makeRACodec(
    const aff3ct::factory::Encoder_RA ::parameters &encParams,
    const aff3ct::factory::Decoder_RA ::parameters &decParams,
    const aff3ct::factory::Interleaver::parameters &itlParams)
{
    static const auto simdMaker = AFF3CTDynamic::makeRACodecDispatch<B,Q>();

    return simdMaker(encParams, decParams, itlParams);
}

#else

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<B,Q>> makeBCHCodec(
    const aff3ct::factory::Encoder_BCH::parameters& encParams,
    const aff3ct::factory::Decoder_BCH::parameters& decParams)
{
    return std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<B,Q>>(new aff3ct::module::Codec_BCH<B,Q>(encParams, decParams));
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<B,Q>> makeLDPCCodec(
    const aff3ct::factory::Encoder_LDPC::parameters& encParams,
    const aff3ct::factory::Decoder_LDPC::parameters& decParams,
    aff3ct::factory::Puncturer_LDPC::parameters* pPctParams)
{
    return std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<B,Q>>(new aff3ct::module::Codec_LDPC<B,Q>(encParams, decParams, pPctParams));
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_polar<B,Q>> makePolarCodec(
    const aff3ct::factory::Frozenbits_generator::parameters &frozenBitsParams,
    const aff3ct::factory::Encoder_polar       ::parameters &encoderParams,
    const aff3ct::factory::Decoder_polar       ::parameters &decoderParams,
    const aff3ct::factory::CRC                 ::parameters &crcParams,
    const aff3ct::factory::Puncturer_polar     ::parameters *pPuncturerParams,
    std::unique_ptr<aff3ct::module::CRC<B>>& rCRCUPtr)
{
    rCRCUPtr.reset(aff3ct::factory::CRC::build<B>(crcParams));

    return std::unique_ptr<aff3ct::module::Codec_polar<B,Q>>(new aff3ct::module::Codec_polar<B,Q>(
        frozenBitsParams,
        encoderParams,
        decoderParams,
        pPuncturerParams,
        rCRCUPtr.get()));
}

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO<B,Q>> makeRACodec(
    const aff3ct::factory::Encoder_RA ::parameters &encParams,
    const aff3ct::factory::Decoder_RA ::parameters &decParams,
    const aff3ct::factory::Interleaver::parameters &itlParams)
{
    return std::unique_ptr<aff3ct::module::Codec_RA<B,Q>>(new aff3ct::module::Codec_RA<B,Q>(encParams, decParams, itlParams));
}

#endif

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
        const aff3ct::factory::CRC                 ::parameters&, \
        const aff3ct::factory::Puncturer_polar     ::parameters*, \
        std::unique_ptr<aff3ct::module::CRC<T1>>&); \
    template \
    std::unique_ptr<aff3ct::module::Codec_SIHO<T1,T2>> makeRACodec( \
        const aff3ct::factory::Encoder_RA ::parameters &encParams, \
        const aff3ct::factory::Decoder_RA ::parameters &decParams, \
        const aff3ct::factory::Interleaver::parameters &itlParams);

SPECIALIZE_TMPLS(B_8,Q_8)
SPECIALIZE_TMPLS(B_16,Q_16)
SPECIALIZE_TMPLS(B_32,Q_32)
SPECIALIZE_TMPLS(B_64,Q_64)

}
