// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <aff3ct.hpp>

namespace AFF3CTDynamic {

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO_HIHO<B,Q>> makeBCHCodec(
    const aff3ct::factory::Encoder_BCH::parameters& encParams,
    const aff3ct::factory::Decoder_BCH::parameters& decParams);

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SISO_SIHO<B,Q>> makeLDPCCodec(
    const aff3ct::factory::Encoder_LDPC::parameters& encParams,
    const aff3ct::factory::Decoder_LDPC::parameters& decParams,
    aff3ct::factory::Puncturer_LDPC::parameters* pPctParams);

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_polar<B,Q>> makePolarCodec(
    const aff3ct::factory::Frozenbits_generator::parameters &frozenBitsParams,
    const aff3ct::factory::Encoder_polar       ::parameters &encoderParams,
    const aff3ct::factory::Decoder_polar       ::parameters &decoderParams,
    const aff3ct::factory::CRC                 ::parameters *pCRCParams,
    const aff3ct::factory::Puncturer_polar     ::parameters *pPuncturerParams,
    std::unique_ptr<aff3ct::module::CRC<B>>& rCRCUPtr);

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_SIHO<B,Q>> makeRACodec(
    const aff3ct::factory::Encoder_RA ::parameters &encParams,
    const aff3ct::factory::Decoder_RA ::parameters &decParams,
    const aff3ct::factory::Interleaver::parameters &itlParams);

}
