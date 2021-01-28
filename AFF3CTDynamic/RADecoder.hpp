#pragma once

#include <vector>
#include <mipp.h>

#include "Module/Interleaver/Interleaver.hpp"
#include "Module/Decoder/Decoder_SIHO.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
template <typename B = int, typename R = float>
class RADecoder : public aff3ct::module::Decoder_SIHO<B,R>
{
protected:
	const int rep_count; // number of repetitions
	int max_iter;        // max number of iterations

	std::vector<R> Fw, Bw;
	std::vector<R> Tu, Td, Wu, Wd, U;
	std::vector<mipp::vector<R>> Xd, Xu;

	const aff3ct::module::Interleaver<R>& interleaver;

public:
	RADecoder(const int& K, const int& N, const aff3ct::module::Interleaver<R>& interleaver, int max_iter, const int n_frames = 1);
	virtual ~RADecoder() = default;

protected:
	void _decode_siho(const R *Y_N, B *V_K, const int frame_id);

private:
	R check_node(R a, R b);
	int sign(R x);
};
}
}
