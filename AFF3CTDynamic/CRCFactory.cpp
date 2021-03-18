#include <utility>
#include <sstream>

#include "Tools/Exception/exception.hpp"
#include "Tools/Documentation/documentation.h"
#include "Tools/types.h"
#include "Module/CRC/NO/CRC_NO.hpp"
#include "Module/CRC/Polynomial/CRC_polynomial.hpp"
#include "Module/CRC/Polynomial/CRC_polynomial_fast.hpp"

#include "CRCFactory.hpp"
#include "CRCPolynomialInter.hpp"

using namespace aff3ct;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B>
module::CRC<B>* CRCFactory
::build(const parameters &params)
{
	if (params.type != "NO" && !params.type.empty())
	{
		const auto poly = params.type;

		if (params.implem == "STD"  ) return new module::CRC_polynomial     <B>(params.K, poly, params.size, params.n_frames);
		if (params.implem == "FAST" ) return new module::CRC_polynomial_fast<B>(params.K, poly, params.size, params.n_frames);
		if (params.implem == "INTER") return new CRCPolynomialInter         <B>(params.K, poly, params.size, params.n_frames);
	}
	else                              return new module::CRC_NO             <B>(params.K,                    params.n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::CRC<B_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::CRCFactory::build<B_8 >(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::CRCFactory::parameters&);
template aff3ct::module::CRC<B_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::CRCFactory::build<B_16>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::CRCFactory::parameters&);
template aff3ct::module::CRC<B_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::CRCFactory::build<B_32>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::CRCFactory::parameters&);
template aff3ct::module::CRC<B_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::CRCFactory::build<B_64>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::CRCFactory::parameters&);
// ==================================================================================== explicit template instantiation
