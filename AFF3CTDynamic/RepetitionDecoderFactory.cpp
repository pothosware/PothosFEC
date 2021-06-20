#include <utility>

#include "Tools/Exception/exception.hpp"
#include "Tools/Documentation/documentation.h"

#include "RepetitionDecoderFast.hpp"
#include "RepetitionDecoderFactory.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* RepetitionDecoderFactory
::build(const parameters &params, const std::unique_ptr<module::Encoder<B>>&)
{
    return new RepetitionDecoderFast<B,Q>(params.K, params.N_cw, params.buffered, params.n_frames);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFactory::build<B_8 ,Q_8 >(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFactory::parameters&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFactory::build<B_16,Q_16>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFactory::parameters&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFactory::build<B_32,Q_32>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFactory::parameters&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFactory::build<B_64,Q_64>(const AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RepetitionDecoderFactory::parameters&, const std::unique_ptr<module::Encoder<B_64>>&);
// ==================================================================================== explicit template instantiation
