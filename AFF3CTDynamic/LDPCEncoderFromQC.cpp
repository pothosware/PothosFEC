#include <string>
#include <cstdint>
#include <sstream>
#include <algorithm>

#include "Tools/Exception/exception.hpp"
#include "Tools/Code/LDPC/Matrix_handler/LDPC_matrix_handler.hpp"

#include "LDPCEncoderFromQC.hpp"

using namespace aff3ct;
using namespace aff3ct::module;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B>
LDPCEncoderFromQC<B>
::LDPCEncoderFromQC(const int K, const int N, const tools::Sparse_matrix &_H, const int n_frames)
: Encoder_LDPC<B>(K, N, n_frames),
  invH2(aff3ct::tools::LDPC_matrix_handler::LU_decomposition(_H))
{
	const std::string name = "LDPCEncoderFromQC";
	this->set_name(name);

	this->H = _H;

	this->check_H_dimensions();
}

template <typename B>
void LDPCEncoderFromQC<B>
::_encode(const B *U_K, B *X_N, const int)
{
	int M = this->N - this->K;

	//Systematic part
	std::copy_n(U_K, this->K, X_N);

	//Calculate parity part
	mipp::vector<int8_t> parity(M, 0);

	for (auto i = 0; i < M; i++)
		for (auto& l : this->H.get_rows_from_col(i))
			if (l < (unsigned)this->K)
				parity[i] ^= U_K[l];

	auto* X_N_ptr = X_N + this->K;
	for (auto i = 0; i < M; i++)
	{
		X_N_ptr[i] = 0;
		for (auto j = 0; j < M; j++)
			X_N_ptr[i] ^= parity[j] & invH2[i][j];
	}
}

template <typename B>
void LDPCEncoderFromQC<B>
::_check_H_dimensions()
{
	Encoder_LDPC<B>::_check_H_dimensions();

	if ((this->N-this->K) != (int)this->H.get_n_cols())
	{
		std::stringstream message;
		message << "The built H matrix has a dimension '(N-K)' different than the given one ('(N-K)' = " << (this->N-this->K)
		        << ", 'H.get_n_cols()' = " << this->H.get_n_cols() << ").";
		throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
	}
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFromQC<B_8>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFromQC<B_16>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFromQC<B_32>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCEncoderFromQC<B_64>;
// ==================================================================================== explicit template instantiation
