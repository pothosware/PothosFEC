#pragma once

#include <mipp.h>

#include "Module/Decoder/Decoder_SISO_SIHO.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
template <typename B = int, typename R = float>
class RepetitionDecoder : public aff3ct::module::Decoder_SISO_SIHO<B,R>
{
protected:
	const int rep_count; // number of repetitions

	const bool buffered_encoding;

	mipp::vector<R> sys;
	mipp::vector<R> par;
	mipp::vector<R> ext;

	RepetitionDecoder(const int& K, const int& N, const bool buffered_encoding = true, const int n_frames = 1);
	virtual ~RepetitionDecoder() = default;

	void _load       (const R *Y_N                            );
	void _decode_siho(const R *Y_N, B *V_K, const int frame_id);
};
}
}
