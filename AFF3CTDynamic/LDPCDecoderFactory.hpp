#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "Tools/Arguments/Argument_tools.hpp"
#include "Tools/Algo/Matrix/Sparse_matrix/Sparse_matrix.hpp"
#include "Module/Encoder/Encoder.hpp"
#include "Module/Decoder/Decoder_SIHO.hpp"
#include "Module/Decoder/Decoder_SISO_SIHO.hpp"
#include "Factory/Module/Decoder/Decoder.hpp"
#include "Factory/Module/Decoder/LDPC/Decoder_LDPC.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
struct LDPCDecoderFactory : public aff3ct::factory::Decoder
{
    using parameters = aff3ct::factory::Decoder_LDPC::parameters;

    template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO<B,Q>* build(const parameters& params, const aff3ct::tools::Sparse_matrix &H,
        	                                        const std::vector<unsigned> &info_bits_pos,
	                                                const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SISO_SIHO<B,Q>* build_siso(const parameters& params,
                                                              const aff3ct::tools::Sparse_matrix &H,
                                                              const std::vector<unsigned> &info_bits_pos,
                                                              const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);
};
}
}
