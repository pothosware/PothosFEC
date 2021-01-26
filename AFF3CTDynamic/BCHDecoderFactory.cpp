#include <utility>
#include <sstream>
#include <cmath>

#include "Tools/Exception/exception.hpp"
#include "Tools/Documentation/documentation.h"

#include "BCHDecoder.hpp"
#include "BCHDecoderFactory.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* BCHDecoderFactory
::build(const parameters &params, const aff3ct::tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder)
{
    return build_hiho<B,Q>(params, GF, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO_HIHO<B,Q>* BCHDecoderFactory
::build_hiho(const parameters &params, const aff3ct::tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>&)
{
    return new BCHDecoder<B,Q>(params.K, params.N_cw, GF, params.n_frames);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build<B_8 ,Q_8 >(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build<B_16,Q_16>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build<B_32,Q_32>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build<B_64,Q_64>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
// ==================================================================================== explicit template instantiation


// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Decoder_SIHO_HIHO<B_8 ,Q_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build_hiho<B_8 ,Q_8 >(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_16,Q_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build_hiho<B_16,Q_16>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_32,Q_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build_hiho<B_32,Q_32>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_64,Q_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build_hiho<B_64,Q_64>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
// ==================================================================================== explicit template instantiation
