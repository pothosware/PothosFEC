#pragma once

#include <string>
#include <map>

#include "Tools/Arguments/Argument_tools.hpp"
#include "Tools/Algo/Matrix/Sparse_matrix/Sparse_matrix.hpp"
#include "Tools/Code/LDPC/Standard/DVBS2/DVBS2_constants.hpp"
#include "Module/Encoder/LDPC/Encoder_LDPC.hpp"
#include "Factory/Module/Encoder/Encoder.hpp"
#include "Factory/Module/Encoder/LDPC/Encoder_LDPC.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
struct LDPCEncoderFactory : public aff3ct::factory::Encoder
{
    using parameters = aff3ct::factory::Encoder_LDPC::parameters;

	template <typename B = int>
	static aff3ct::module::Encoder_LDPC<B>* build(const parameters           &params,
	                                              const aff3ct::tools::Sparse_matrix &G,
	                                              const aff3ct::tools::Sparse_matrix &H);
	template <typename B = int>
	static aff3ct::module::Encoder_LDPC<B>* build(const parameters           &params,
	                                              const aff3ct::tools::Sparse_matrix &G,
	                                              const aff3ct::tools::Sparse_matrix &H,
	                                              const aff3ct::tools::dvbs2_values  &dvbs2);
};
}
}
