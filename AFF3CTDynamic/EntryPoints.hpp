// Copyright (c) 2020 Nicholas Corgan
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
    aff3ct::factory::Puncturer_LDPC::parameters* pctParams);

template <typename B, typename Q>
std::unique_ptr<aff3ct::module::Codec_polar<B,Q>> makePolarCodec(
    const aff3ct::factory::Frozenbits_generator::parameters &frozenBitsParams,
    const aff3ct::factory::Encoder_polar       ::parameters &encoderParams,
    const aff3ct::factory::Decoder_polar       ::parameters &decoderParams,
    const aff3ct::factory::Puncturer_polar     ::parameters *puncturerParams = nullptr,
    aff3ct::module::CRC<B>* crc = nullptr);

}
