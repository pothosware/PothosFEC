#pragma once

#include "Factory/Module/Encoder/RA/Encoder_RA.hpp"
#include "Factory/Module/Decoder/RA/Decoder_RA.hpp"
#include "Factory/Module/Interleaver/Interleaver.hpp"
#include "Module/Codec/Codec_SIHO.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
template <typename B = int, typename Q = float>
class RACodec : public aff3ct::module::Codec_SIHO<B,Q>
{
public:
	RACodec(const aff3ct::factory::Encoder_RA ::parameters &enc_params,
	        const aff3ct::factory::Decoder_RA ::parameters &dec_params,
	        const aff3ct::factory::Interleaver::parameters &itl_params);
	virtual ~RACodec() = default;
};
}
}
