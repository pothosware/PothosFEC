#include <utility>

#include "Tools/Exception/exception.hpp"
#include "Tools/Documentation/documentation.h"
#include "Tools/Display/rang_format/rang_format.h"
#include "Tools/Code/LDPC/Matrix_handler/LDPC_matrix_handler.hpp"
#include "Module/Encoder/LDPC/Encoder_LDPC.hpp"
#include "Module/Encoder/LDPC/From_H/Encoder_LDPC_from_H.hpp"
#include "Module/Encoder/LDPC/From_IRA/Encoder_LDPC_from_IRA.hpp"
#include "Module/Encoder/LDPC/DVBS2/Encoder_LDPC_DVBS2.hpp"

#include "LDPCEncoderFactory.hpp"
#include "LDPCEncoderFromQC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B>
module::Encoder_LDPC<B>* LDPCEncoderFactory
::build(const parameters           &params,
        const tools::Sparse_matrix &G,
        const tools::Sparse_matrix &H)
{
	if (params.type == "LDPC"    ) return new module::Encoder_LDPC         <B>(params.K, params.N_cw, G, params.n_frames);
	if (params.type == "LDPC_H"  ) return new module::Encoder_LDPC_from_H  <B>(params.K, params.N_cw, H, params.G_method, params.G_save_path, true, params.n_frames);
	if (params.type == "LDPC_QC" ) return new LDPCEncoderFromQC            <B>(params.K, params.N_cw, H, params.n_frames);
	if (params.type == "LDPC_IRA") return new module::Encoder_LDPC_from_IRA<B>(params.K, params.N_cw, H, params.n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_LDPC<B>* LDPCEncoderFactory
::build(const parameters           &params,
        const tools::Sparse_matrix &G,
        const tools::Sparse_matrix &H,
        const tools::dvbs2_values& dvbs2)
{
	if (params.type == "LDPC_DVBS2") return new module::Encoder_LDPC_DVBS2<B>(dvbs2, params.n_frames);

    return build<B>(params, G, H);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Encoder_LDPC<B_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFactory::build<B_8 >(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);
template aff3ct::module::Encoder_LDPC<B_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFactory::build<B_16>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);
template aff3ct::module::Encoder_LDPC<B_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFactory::build<B_32>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);
template aff3ct::module::Encoder_LDPC<B_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFactory::build<B_64>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);

template aff3ct::module::Encoder_LDPC<B_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFactory::build<B_8 >(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
template aff3ct::module::Encoder_LDPC<B_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFactory::build<B_16>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
template aff3ct::module::Encoder_LDPC<B_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFactory::build<B_32>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
template aff3ct::module::Encoder_LDPC<B_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFactory::build<B_64>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
// ==================================================================================== explicit template instantiation
