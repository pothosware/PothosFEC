// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EntryPoints.hpp"

namespace AFF3CTDynamic
{

#ifdef AFF3CT_SIMD_SUPPORTED

#include "AFF3CTDynamic_SIMD.hpp"

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
    aff3ct::factory::Puncturer_LDPC::parameters* pctParams)
{
    static const auto simdMaker = AFF3CTDynamic::makeLDPCCodecDispatch<B,Q>();

    return simdMaker(encParams, decParams, pctParams);
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
    aff3ct::factory::Puncturer_LDPC::parameters* pctParams)
{
    return std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<B,Q>>(new aff3ct::module::Codec_LDPC<B,Q>(encParams, decParams, pctParams));
}

#endif

#define SPECIALIZE_TMPLS(T1,T2) \
    template \
    std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<T1,T2>> makeBCHCodec( \
        const aff3ct::factory::Encoder_BCH::parameters&, \
        const aff3ct::factory::Decoder_BCH::parameters&); \
    template <typename B, typename Q> \
    std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<T1,T2>> makeLDPCCodec( \
        const aff3ct::factory::Encoder_LDPC::parameters&, \
        const aff3ct::factory::Decoder_LDPC::parameters&, \
        aff3ct::factory::Puncturer_LDPC::parameters*);

SPECIALIZE_TMPLS(B_16,Q_16)
SPECIALIZE_TMPLS(B_32,Q_32)
SPECIALIZE_TMPLS(B_64,Q_64)

}
