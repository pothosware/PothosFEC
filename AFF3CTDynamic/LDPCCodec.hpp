#pragma once

#include <memory>
#include <vector>

#include "Factory/Module/Encoder/LDPC/Encoder_LDPC.hpp"
#include "Factory/Module/Puncturer/LDPC/Puncturer_LDPC.hpp"
#include "Factory/Module/Decoder/LDPC/Decoder_LDPC.hpp"
#include "Tools/Algo/Matrix/Sparse_matrix/Sparse_matrix.hpp"
#include "Tools/Code/LDPC/Standard/DVBS2/DVBS2_constants.hpp"
#include "Tools/Code/LDPC/Matrix_handler/LDPC_matrix_handler.hpp"
#include "Module/Codec/Codec_SISO_SIHO.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
template <typename B = int, typename Q = float>
class LDPCCodec : public aff3ct::module::Codec_SISO_SIHO<B,Q>
{
protected:
    aff3ct::tools::Sparse_matrix H;
    aff3ct::tools::Sparse_matrix G;
    aff3ct::tools::LDPC_matrix_handler::Positions_vector info_bits_pos;
	std::vector<bool> pctPattern;
	std::unique_ptr<aff3ct::tools::dvbs2_values> dvbs2;

public:
	LDPCCodec(const aff3ct::factory::Encoder_LDPC::parameters   &enc_params,
	          const aff3ct::factory::Decoder_LDPC::parameters   &dec_params,
	                aff3ct::factory::Puncturer_LDPC::parameters *pct_params);
	virtual ~LDPCCodec() = default;

protected:
	void _extract_sys_par(const Q *Y_N, Q *sys, Q *par, const int frame_id);
	void _extract_sys_llr(const Q *Y_N, Q *Y_K,         const int frame_id);
	void _extract_sys_bit(const Q *Y_N, B *V_K,         const int frame_id);
};
}
}
