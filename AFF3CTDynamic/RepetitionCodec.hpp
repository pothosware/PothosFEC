#pragma once

#include "Factory/Module/Encoder/Repetition/Encoder_repetition.hpp"
#include "Factory/Module/Decoder/Repetition/Decoder_repetition.hpp"
#include "Module/Codec/Codec_SIHO.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
template <typename B = int, typename Q = float>
class RepetitionCodec : public aff3ct::module::Codec_SIHO<B,Q>
{
public:
	RepetitionCodec(const aff3ct::factory::Encoder_repetition::parameters &enc_params,
	                const aff3ct::factory::Decoder_repetition::parameters &dec_params);
	virtual ~RepetitionCodec() = default;
};
}
}
