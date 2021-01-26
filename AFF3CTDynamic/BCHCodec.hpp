#pragma once

#include "Tools/Code/BCH/BCH_polynomial_generator.hpp"
#include "Factory/Module/Encoder/BCH/Encoder_BCH.hpp"
#include "Module/Codec/Codec_SIHO_HIHO.hpp"

#include "BCHDecoderFactory.hpp"

namespace POTHOS_SIMD_NAMESPACE
{
template <typename B = int, typename Q = float>
class BCHCodec : public aff3ct::module::Codec_SIHO_HIHO<B,Q>
{
protected:
	const aff3ct::tools::BCH_polynomial_generator<B> GF_poly;

public:
	BCHCodec(const aff3ct::factory::Encoder_BCH::parameters &enc_params,
             const BCHDecoderFactory::parameters &dec_params);
	virtual ~BCHCodec() = default;
};
}
