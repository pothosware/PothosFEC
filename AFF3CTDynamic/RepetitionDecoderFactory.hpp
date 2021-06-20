#pragma once

#include <string>
#include <memory>
#include <map>

#include "Tools/Arguments/Argument_tools.hpp"
#include "Module/Encoder/Encoder.hpp"
#include "Module/Decoder/Decoder_SIHO.hpp"

#include "Factory/Module/Decoder/Repetition/Decoder_repetition.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
struct RepetitionDecoderFactory : public aff3ct::factory::Decoder
{
    using parameters = aff3ct::factory::Decoder_repetition::parameters;

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO<B,Q>* build(const parameters &params, const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);
};
}
}
