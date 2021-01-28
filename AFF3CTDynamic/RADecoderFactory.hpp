#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "Tools/Arguments/Argument_tools.hpp"
#include "Tools/auto_cloned_unique_ptr.hpp"
#include "Module/Encoder/Encoder.hpp"
#include "Module/Decoder/Decoder_SIHO.hpp"
#include "Factory/Module/Interleaver/Interleaver.hpp"

#include "Factory/Module/Decoder/Decoder.hpp"
#include "Factory/Module/Decoder/RA/Decoder_RA.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
struct RADecoderFactory : public aff3ct::factory::Decoder
{
    using parameters = aff3ct::factory::Decoder_RA::parameters;

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO<B,Q>* build(const parameters &params, const aff3ct::module::Interleaver<Q> &itl,
	                                                const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);
};
}
}
