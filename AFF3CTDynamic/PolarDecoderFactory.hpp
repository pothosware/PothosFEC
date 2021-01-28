#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "Tools/Arguments/Argument_tools.hpp"
#include "Module/CRC/CRC.hpp"
#include "Module/Encoder/Encoder.hpp"
#include "Module/Decoder/Decoder_SIHO.hpp"
#include "Module/Decoder/Decoder_SISO_SIHO.hpp"
#include "Factory/Module/Decoder/Decoder.hpp"

#include "Factory/Module/Decoder/Polar/Decoder_polar.hpp"

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
struct PolarDecoderFactory : public aff3ct::factory::Decoder
{
public:
    using parameters = aff3ct::factory::Decoder_polar::parameters;

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SISO_SIHO<B,Q>* build_siso(const parameters& params, const std::vector<bool> &frozen_bits,
        	                                                  const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO<B,Q>* build(const parameters& params, const std::vector<bool> &frozen_bits,
        	                                        aff3ct::module::CRC<B> *crc = nullptr, const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO<B,Q>* build_gen(const parameters &params, aff3ct::module::CRC<B> *crc = nullptr,
	                                                    const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);

    static const std::vector<bool>& get_frozen_bits(const std::string &implem);

private:
    template <typename B, typename Q, class API_polar>
    static aff3ct::module::Decoder_SIHO<B,Q>* _build(const parameters &params, const std::vector<bool> &frozen_bits,
                                                     aff3ct::module::CRC<B> *crc, const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder);

    template <typename B, typename Q, class API_polar>
    static aff3ct::module::Decoder_SIHO<B,Q>* _build_scl_fast(const parameters &params, const std::vector<bool> &frozen_bits,
                                                              aff3ct::module::CRC<B> *crc, const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder);

    template <typename B, typename Q, class API_polar>
    static aff3ct::module::Decoder_SIHO<B,Q>* _build_gen(const parameters &params, aff3ct::module::CRC<B> *crc,
                                                         const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder);
};
}
}
