#pragma once

#include "RepetitionDecoder.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
template <typename B = int, typename R = float>
class RepetitionDecoderFast : public RepetitionDecoder<B,R>
{
public:
	RepetitionDecoderFast(const int& K, const int& N, const bool buffered_encoding = true, const int n_frames = 1);
	virtual ~RepetitionDecoderFast() = default;

protected:
	void _decode_siso(const R *sys, const R *par, R *ext, const int frame_id);
};
}
}
