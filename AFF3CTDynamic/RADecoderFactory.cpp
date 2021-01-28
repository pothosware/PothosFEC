#include <utility>

#include "RADecoder.hpp"
#include "RADecoderFactory.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* RADecoderFactory
::build(const parameters &params, const module::Interleaver<Q> &itl, const std::unique_ptr<module::Encoder<B>>&)
{
    if (params.type == "RA")
    {
        if (params.implem == "STD" ) return new RADecoder<B,Q>(params.K, params.N_cw, itl, params.n_ite, params.n_frames);
    }

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RADecoderFactory::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RADecoderFactory::build<B_16,Q_16>(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RADecoderFactory::build<B_32,Q_32>(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::RADecoderFactory::build<B_64,Q_64>(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
// ==================================================================================== explicit template instantiation
