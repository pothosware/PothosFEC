#pragma once

#include "Tools/Algo/Matrix/Sparse_matrix/Sparse_matrix.hpp"
#include "Tools/Code/LDPC/Matrix_handler/LDPC_matrix_handler.hpp"
#include "Module/Encoder/LDPC/Encoder_LDPC.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{

template <typename B = int>
class LDPCEncoderFromQC : public aff3ct::module::Encoder_LDPC<B>
{
protected:
    aff3ct::tools::LDPC_matrix_handler::LDPC_matrix invH2;

public:
	LDPCEncoderFromQC(const int K, const int N, const aff3ct::tools::Sparse_matrix &H, const int n_frames = 1);
	virtual ~LDPCEncoderFromQC() = default;

protected:
	void _encode(const B *U_K, B *X_N, const int frame_id);
	void _check_H_dimensions();
};

}
}
