#pragma once

#include <string>
#include <memory>
#include <map>

#include "Tools/Arguments/Argument_tools.hpp"
#include "Tools/Code/BCH/BCH_polynomial_generator.hpp"
#include "Module/Encoder/Encoder.hpp"
#include "Module/Decoder/Decoder_SIHO.hpp"
#include "Module/Decoder/Decoder_SIHO_HIHO.hpp"
#include "Factory/Module/Decoder/Decoder.hpp"
#include "Factory/Module/Decoder/BCH/Decoder_BCH.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
struct BCHDecoderFactory : public aff3ct::factory::Decoder
{
    using parameters = aff3ct::factory::Decoder_BCH::parameters;

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO<B,Q>* build(const parameters &params, const aff3ct::tools::BCH_polynomial_generator<B> &GF,
	                                                const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO_HIHO<B,Q>* build_hiho(const parameters &params, const aff3ct::tools::BCH_polynomial_generator<B> &GF,
	                                                          const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);
};
}
}
