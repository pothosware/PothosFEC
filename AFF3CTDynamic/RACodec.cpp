#include <sstream>
#include <string>

#include "Tools/Exception/exception.hpp"
#include "Factory/Tools/Interleaver/Interleaver_core.hpp"
#include "Factory/Module/Encoder/Encoder.hpp"
#include "Factory/Module/Puncturer/Puncturer.hpp"

#include "RACodec.hpp"
#include "RADecoderFactory.hpp"

using namespace aff3ct;
using namespace aff3ct::module;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B, typename Q>
RACodec<B,Q>
::RACodec(const factory::Encoder_RA ::parameters &enc_params,
          const factory::Decoder_RA ::parameters &dec_params,
          const factory::Interleaver::parameters &itl_params)
: Codec     <B,Q>(enc_params.K, enc_params.N_cw, enc_params.N_cw, enc_params.tail_length, enc_params.n_frames),
  Codec_SIHO<B,Q>(enc_params.K, enc_params.N_cw, enc_params.N_cw, enc_params.tail_length, enc_params.n_frames)
{
	const std::string name = "RACodec";
	this->set_name(name);

	// ----------------------------------------------------------------------------------------------------- exceptions
	if (enc_params.K != dec_params.K)
	{
		std::stringstream message;
		message << "'enc_params.K' has to be equal to 'dec_params.K' ('enc_params.K' = " << enc_params.K
		        << ", 'dec_params.K' = " << dec_params.K << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	if (enc_params.N_cw != dec_params.N_cw)
	{
		std::stringstream message;
		message << "'enc_params.N_cw' has to be equal to 'dec_params.N_cw' ('enc_params.N_cw' = " << enc_params.N_cw
		        << ", 'dec_params.N_cw' = " << dec_params.N_cw << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	if (enc_params.n_frames != dec_params.n_frames)
	{
		std::stringstream message;
		message << "'enc_params.n_frames' has to be equal to 'dec_params.n_frames' ('enc_params.n_frames' = "
		        << enc_params.n_frames << ", 'dec_params.n_frames' = " << dec_params.n_frames << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	// ---------------------------------------------------------------------------------------------------- allocations
	factory::Puncturer::parameters pct_params;
	pct_params.type     = "NO";
	pct_params.K        = enc_params.K;
	pct_params.N        = enc_params.N_cw;
	pct_params.N_cw     = enc_params.N_cw;
	pct_params.n_frames = enc_params.n_frames;

	this->set_puncturer  (factory::Puncturer::build<B,Q>(pct_params));
	this->set_interleaver(factory::Interleaver_core::build<>(*itl_params.core));

	try
	{
		this->set_encoder(factory::Encoder_RA::build<B>(enc_params, this->get_interleaver_bit()));
	}
	catch (tools::cannot_allocate const&)
	{
		this->set_encoder(factory::Encoder::build<B>(enc_params));
	}

	this->set_decoder_siho(RADecoderFactory::build<B,Q>(dec_params, this->get_interleaver_llr(), this->get_encoder()));
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RACodec<B_8,Q_8>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RACodec<B_16,Q_16>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RACodec<B_32,Q_32>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RACodec<B_64,Q_64>;
// ==================================================================================== explicit template instantiation
