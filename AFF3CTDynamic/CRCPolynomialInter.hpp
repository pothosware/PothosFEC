#pragma once

#include <string>

#include "Module/CRC/CRC.hpp"
#include "Module/CRC/Polynomial/CRC_polynomial.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
template <typename B = int>
class CRCPolynomialInter : public aff3ct::module::CRC_polynomial<B>
{
public:
	CRCPolynomialInter(const int K, std::string poly_key, const int size, const int n_frames);
	virtual ~CRCPolynomialInter() = default;

	virtual bool check(const B *V_K, const int n_frames = -1, const int frame_id = -1);

    using aff3ct::module::CRC<B>::check;

protected:
	void _generate_INTER(const B *U_in,
	                           B *U_out,
	                     const int off_in,
	                     const int off_out,
	                     const int loop_size,
	                     const int n_frames);
};
}
}
