// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BCHCodec.hpp"

namespace AFF3CTDynamic { namespace POTHOS_SIMD_NAMESPACE {

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<B,Q>> makeBCHCodec(
    const aff3ct::factory::Encoder_BCH::parameters& encParams,
    const aff3ct::factory::Decoder_BCH::parameters& decParams)
{
    return new BCHCodec<B,Q>(encParams, decParams);
}

}}
