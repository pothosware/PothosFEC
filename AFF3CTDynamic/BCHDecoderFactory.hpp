#pragma once

#include <string>
#include <memory>
#include <map>

#include "Tools/Arguments/Argument_tools.hpp"
#include "Tools/Code/BCH/BCH_polynomial_generator.hpp"
#include "Module/Encoder/Encoder.hpp"
#include "Module/Decoder/Decoder_SIHO.hpp"
#include "Module/Decoder/Decoder_SIHO_HIHO.hpp"
#include "Factory/Module/Decoder/Decoder.hpp"

namespace POTHOS_SIMD_NAMESPACE
{
extern const std::string BCHDecoderFactory_name;
extern const std::string BCHDecoderFactory_prefix;
struct BCHDecoderFactory : public aff3ct::factory::Decoder
{
	class parameters : public aff3ct::factory::Decoder::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional parameters
		int t = 5; // correction power of the BCH

		// deduced parameters
		int m = 0; // Gallois field order

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = BCHDecoderFactory_prefix);
		virtual ~parameters() = default;
		BCHDecoderFactory::parameters* clone() const;

		// parameters construction
		void get_description(aff3ct::tools::Argument_map_info &args) const;
		void store          (const aff3ct::tools::Argument_map_value &vals);
		void get_headers    (std::map<std::string,aff3ct::factory::header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		aff3ct::module::Decoder_SIHO<B,Q>* build(const aff3ct::tools::BCH_polynomial_generator<B> &GF,
		                                         const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr) const;

		template <typename B = int, typename Q = float>
		aff3ct::module::Decoder_SIHO_HIHO<B,Q>* build_hiho(const aff3ct::tools::BCH_polynomial_generator<B> &GF,
		                                                   const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr) const;

	};

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO<B,Q>* build(const parameters &params, const aff3ct::tools::BCH_polynomial_generator<B> &GF,
	                                                const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);

	template <typename B = int, typename Q = float>
	static aff3ct::module::Decoder_SIHO_HIHO<B,Q>* build_hiho(const parameters &params, const aff3ct::tools::BCH_polynomial_generator<B> &GF,
	                                                          const std::unique_ptr<aff3ct::module::Encoder<B>>& encoder = nullptr);
};
}
