#include <utility>
#include <sstream>
#include <cmath>

#include "Tools/Exception/exception.hpp"
#include "Tools/Documentation/documentation.h"

#include "BCHDecoder.hpp"
#include "BCHDecoderFactory.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;
using namespace POTHOS_SIMD_NAMESPACE;

const std::string POTHOS_SIMD_NAMESPACE::BCHDecoderFactory_name   = "Decoder BCH";
const std::string POTHOS_SIMD_NAMESPACE::BCHDecoderFactory_prefix = "dec";

BCHDecoderFactory::parameters
::parameters(const std::string &prefix)
: Decoder::parameters(BCHDecoderFactory_name, prefix)
{
	this->type   = "ALGEBRAIC";
	this->implem = "STD";
}

BCHDecoderFactory::parameters* BCHDecoderFactory::parameters
::clone() const
{
	return new BCHDecoderFactory::parameters(*this);
}

void BCHDecoderFactory::parameters
::get_description(aff3ct::tools::Argument_map_info &args) const
{
	Decoder::parameters::get_description(args);

	auto p = this->get_prefix();
	const std::string class_name = "factory::BCHDecoderFactory::parameters::";

	aff3ct::tools::add_arg(args, p, class_name+"p+corr-pow,T",
		aff3ct::tools::Integer(aff3ct::tools::Positive(), aff3ct::tools::Non_zero()));

	args.add_link({p+"-corr-pow", "T"}, {p+"-info-bits", "K"});

	aff3ct::tools::add_options(args.at({p+"-type", "D"}), 0, "ALGEBRAIC");
	aff3ct::tools::add_options(args.at({p+"-implem"   }), 0, "GENIUS", "FAST");
}

void BCHDecoderFactory::parameters
::store(const aff3ct::tools::Argument_map_value &vals)
{
	Decoder::parameters::store(vals);

	auto p = this->get_prefix();

	this->m = (int)std::ceil(std::log2(this->N_cw));
	if (this->m == 0)
	{
		std::stringstream message;
		message << "The Galois Field order is null (because N_cw = " << this->N_cw << ").";
		throw aff3ct::tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
	}

	if (vals.exist({p+"-corr-pow", "T"}))
	{
		this->t = vals.to_int({p + "-corr-pow", "T"});
		if (K == 0)
		{
			this->K = this->N_cw - aff3ct::tools::BCH_polynomial_generator<>(this->N_cw, this->t).get_n_rdncy();
			this->R = (float) this->K / (float) this->N_cw;
		}
	}
	else
		this->t = (this->N_cw - this->K) / this->m;
}

void BCHDecoderFactory::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Decoder::parameters::get_headers(headers, full);

	if (this->type != "ML" && this->type != "CHASE")
	{
		auto p = this->get_prefix();

		headers[p].push_back(std::make_pair("Galois field order (m)", std::to_string(this->m)));
		headers[p].push_back(std::make_pair("Correction power (T)",   std::to_string(this->t)));
	}
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* BCHDecoderFactory::parameters
::build(const aff3ct::tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (aff3ct::tools::cannot_allocate const&)
	{
		return build_hiho<B,Q>(GF, encoder);
	}
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* BCHDecoderFactory
::build(const parameters &params, const aff3ct::tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(GF, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO_HIHO<B,Q>* BCHDecoderFactory::parameters
::build_hiho(const aff3ct::tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build_hiho<B,Q>(encoder);
	}
	catch (aff3ct::tools::cannot_allocate const&)
	{
        return new BCHDecoder<B,Q>(this->K, this->N_cw, GF, this->n_frames);
	}

	throw aff3ct::tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO_HIHO<B,Q>* BCHDecoderFactory
::build_hiho(const parameters &params, const aff3ct::tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build_hiho<B,Q>(GF, encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters::build<B_8 ,Q_8 >(const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters::build<B_16,Q_16>(const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters::build<B_32,Q_32>(const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters::build<B_64,Q_64>(const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build<B_8 ,Q_8 >(const POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build<B_16,Q_16>(const POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build<B_32,Q_32>(const POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build<B_64,Q_64>(const POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
// ==================================================================================== explicit template instantiation


// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Decoder_SIHO_HIHO<B_8 ,Q_8 >* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters::build_hiho<B_8 ,Q_8 >(const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_16,Q_16>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters::build_hiho<B_16,Q_16>(const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_32,Q_32>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters::build_hiho<B_32,Q_32>(const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_64,Q_64>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters::build_hiho<B_64,Q_64>(const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_8 ,Q_8 >* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build_hiho<B_8 ,Q_8 >(const POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_16,Q_16>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build_hiho<B_16,Q_16>(const POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_32,Q_32>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build_hiho<B_32,Q_32>(const POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_64,Q_64>* POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::build_hiho<B_64,Q_64>(const POTHOS_SIMD_NAMESPACE::BCHDecoderFactory::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
// ==================================================================================== explicit template instantiation
