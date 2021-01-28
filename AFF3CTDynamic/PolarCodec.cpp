#include "PolarCodec.hpp"
#include "PolarDecoderFactory.hpp"

using namespace aff3ct;
using namespace aff3ct::module;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B, typename Q>
PolarCodec<B,Q>
::PolarCodec(const factory::Frozenbits_generator::parameters &fb_params,
             const factory::Encoder_polar       ::parameters &enc_params,
             const factory::Decoder_polar       ::parameters &dec_params,
             const factory::Puncturer_polar     ::parameters *pct_params,
             CRC<B>* crc)
: Codec          <B,Q>(enc_params.K, enc_params.N_cw, pct_params ? pct_params->N : enc_params.N_cw, enc_params.tail_length, enc_params.n_frames),
  Codec_polar<B,Q>(fb_params, enc_params, dec_params, pct_params, crc)
{
    // The only thing we need to change is overriding the decoder factory
    // with our SIMD-optimized version.
	try
	{
		this->set_decoder_siso_siho(PolarDecoderFactory::build_siso<B,Q>(dec_params, this->frozen_bits, this->get_encoder()));
	}
	catch (const std::exception&)
	{
		if (this->generated_decoder)
			this->set_decoder_siho(PolarDecoderFactory::build_gen<B,Q>(dec_params,                    crc, this->get_encoder()));
		else
			this->set_decoder_siho(PolarDecoderFactory::build    <B,Q>(dec_params, this->frozen_bits, crc, this->get_encoder()));
	}
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarCodec<B_8,Q_8>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarCodec<B_16,Q_16>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarCodec<B_32,Q_32>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarCodec<B_64,Q_64>;
// ==================================================================================== explicit template instantiation
