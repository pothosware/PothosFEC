#pragma once

#include "Module/Codec/Polar/Codec_polar.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{

// Note: unlike the other classes in this library, this is a subclass of its
// AFF3CT equivalent because the AFF3CT equivalent has functions we'll likely
// need. The abstraction function will return a Codec_polar pointer instead of
// a Codec_SISO_SIHO pointer.
template <typename B = int, typename Q = float>
class PolarCodec: public aff3ct::module::Codec_polar<B,Q>
{
public:
	PolarCodec(const aff3ct::factory::Frozenbits_generator::parameters &fb_par,
	           const aff3ct::factory::Encoder_polar       ::parameters &enc_par,
	           const aff3ct::factory::Decoder_polar       ::parameters &dec_par,
	           const aff3ct::factory::Puncturer_polar     ::parameters *pct_par = nullptr,
	           aff3ct::module::CRC<B>* crc = nullptr);
	virtual ~PolarCodec() = default;
};

}
}
