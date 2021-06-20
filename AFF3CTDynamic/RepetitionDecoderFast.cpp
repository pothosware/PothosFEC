#include <sstream>
#include <string>
#include <mipp.h>

#include "Tools/Exception/exception.hpp"

#include "RepetitionDecoderFast.hpp"

using namespace aff3ct;
using namespace aff3ct::module;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B, typename R>
RepetitionDecoderFast<B,R>
::RepetitionDecoderFast(const int& K, const int& N, const bool buffered_encoding, const int n_frames)
: Decoder(K, N, n_frames, 1),
  RepetitionDecoder<B,R>(K, N, buffered_encoding, n_frames)
{
	const std::string name = "RepetitionDecoderFast";
	this->set_name(name);

	if (this->K % mipp::nElReg<R>())
	{
		std::stringstream message;
		message << "'K' has to be a multiple of 'mipp::nElReg<R>()' ('K' = " << K
		        << ", 'mipp::nElReg<R>()' = " << mipp::nElReg<R>() << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}
}

template <typename B, typename R>
void RepetitionDecoderFast<B,R>
::_decode_siso(const R *sys, const R *par, R *ext, const int /*frame_id*/)
{
	for (auto i = 0; i < this->K; i += mipp::nElReg<R>())
	{
		auto r_ext = mipp::Reg<R>(&sys[i]);
		for (auto j = 0; j < this->rep_count; j++)
			r_ext += &par[j*this->K +i];
		r_ext.store(&ext[i]);
	}
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFast<B_8,Q_8>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFast<B_16,Q_16>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFast<B_32,Q_32>;
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFast<B_64,Q_64>;
#else
template class AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFast<B,Q>;
#endif
// ==================================================================================== explicit template instantiation
