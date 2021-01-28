#include <algorithm>
#include <utility>

#include "Tools/Exception/exception.hpp"
#include "Tools/Documentation/documentation.h"
#include "Module/Decoder/Polar/SC/Decoder_polar_SC_naive.hpp"
#include "Module/Decoder/Polar/SC/Decoder_polar_SC_naive_sys.hpp"
#include "Module/Decoder/Polar/SC/Decoder_polar_SC_fast_sys.hpp"
#include "Module/Decoder/Polar/SCAN/Decoder_polar_SCAN_naive.hpp"
#include "Module/Decoder/Polar/SCAN/Decoder_polar_SCAN_naive_sys.hpp"
#include "Module/Decoder/Polar/SCF/Decoder_polar_SCF_naive.hpp"
#include "Module/Decoder/Polar/SCF/Decoder_polar_SCF_naive_sys.hpp"
#include "Module/Decoder/Polar/SCL/Decoder_polar_SCL_naive.hpp"
#include "Module/Decoder/Polar/SCL/Decoder_polar_SCL_naive_sys.hpp"
#include "Module/Decoder/Polar/SCL/Decoder_polar_SCL_fast_sys.hpp"
#include "Module/Decoder/Polar/SCL/Decoder_polar_SCL_MEM_fast_sys.hpp"
#include "Module/Decoder/Polar/SCL/CRC/Decoder_polar_SCL_naive_CA.hpp"
#include "Module/Decoder/Polar/SCL/CRC/Decoder_polar_SCL_naive_CA_sys.hpp"
#include "Module/Decoder/Polar/SCL/CRC/Decoder_polar_SCL_fast_CA_sys.hpp"
#include "Module/Decoder/Polar/SCL/CRC/Decoder_polar_SCL_MEM_fast_CA_sys.hpp"
#include "Module/Decoder/Polar/ASCL/Decoder_polar_ASCL_fast_CA_sys.hpp"
#include "Module/Decoder/Polar/ASCL/Decoder_polar_ASCL_MEM_fast_CA_sys.hpp"
//#define API_POLAR_DYNAMIC 1
#include "Tools/Code/Polar/API/API_polar_dynamic_seq.hpp"
#include "Tools/Code/Polar/API/API_polar_dynamic_intra.hpp"
#ifdef API_POLAR_DYNAMIC
#include "Tools/Code/Polar/API/API_polar_dynamic_inter.hpp"
#include "Tools/Code/Polar/API/API_polar_dynamic_inter_8bit_bitpacking.hpp"
#else
#include "Tools/Code/Polar/API/API_polar_static_seq.hpp"
#include "Tools/Code/Polar/API/API_polar_static_inter.hpp"
#include "Tools/Code/Polar/API/API_polar_static_inter_8bit_bitpacking.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_8bit.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_16bit.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_32bit.hpp"
#endif
#include "Tools/Code/Polar/Nodes_parser.h"

#include "PolarDecoderFactory.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B, typename Q, class API_polar>
module::Decoder_SIHO<B,Q>* PolarDecoderFactory
::_build(const parameters &params, const std::vector<bool> &frozen_bits,
         module::CRC<B> *crc, const std::unique_ptr<module::Encoder<B>>&)
{
	if (!params.systematic) // non-systematic encoding
	{
		if (params.implem == "NAIVE")
		{
			if (crc == nullptr || crc->get_size() == 0)
			{
				if (params.type == "SC"  ) return new module::Decoder_polar_SC_naive        <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>,tools::h_LLR<B,Q>>(params.K, params.N_cw,              frozen_bits,       params.n_frames);
				if (params.type == "SCAN") return new module::Decoder_polar_SCAN_naive      <B,Q,tools::f_LLR<Q>,tools::v_LLR<  Q>,tools::h_LLR<B,Q>>(params.K, params.N_cw, params.n_ite, frozen_bits,       params.n_frames);
				if (params.type == "SCL" ) return new module::Decoder_polar_SCL_naive       <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>                  >(params.K, params.N_cw, params.L,     frozen_bits,       params.n_frames);
			}
			else
			{
				if (params.type == "SCF" ) return new module::Decoder_polar_SCF_naive       <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>,tools::h_LLR<B,Q>>(params.K, params.N_cw, frozen_bits, *crc, params.flips, params.n_frames);
				if (params.type == "SCL" ) return new module::Decoder_polar_SCL_naive_CA    <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>                  >(params.K, params.N_cw, params.L,     frozen_bits, *crc, params.n_frames);
			}
		}
	}
	else // systematic encoding
	{
		if (params.implem == "NAIVE")
		{
			if (crc == nullptr || crc->get_size() == 0)
			{
				if (params.type == "SC"  ) return new module::Decoder_polar_SC_naive_sys    <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>,tools::h_LLR<B,Q>>(params.K, params.N_cw,              frozen_bits,       params.n_frames);
				if (params.type == "SCAN") return new module::Decoder_polar_SCAN_naive_sys  <B,Q,tools::f_LLR<Q>,tools::v_LLR<  Q>,tools::h_LLR<B,Q>>(params.K, params.N_cw, params.n_ite, frozen_bits,       params.n_frames);
				if (params.type == "SCL" ) return new module::Decoder_polar_SCL_naive_sys   <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>                  >(params.K, params.N_cw, params.L,     frozen_bits,       params.n_frames);
			}
			else
			{
				if (params.type == "SCF" ) return new module::Decoder_polar_SCF_naive_sys   <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>,tools::h_LLR<B,Q>>(params.K, params.N_cw, frozen_bits, *crc, params.flips, params.n_frames);
				if (params.type == "SCL" ) return new module::Decoder_polar_SCL_naive_CA_sys<B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>                  >(params.K, params.N_cw, params.L,     frozen_bits, *crc, params.n_frames);
			}
		}
		else if (params.implem == "FAST")
		{
			if (crc == nullptr || crc->get_size() == 0)
			{
				int idx_r0, idx_r1;
				auto polar_patterns = tools::Nodes_parser<>::parse_uptr(params.polar_nodes, idx_r0, idx_r1);
				if (params.type == "SC"  ) return new module::Decoder_polar_SC_fast_sys<B, Q, API_polar>(params.K, params.N_cw, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, params.n_frames);
			}
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q, class API_polar>
module::Decoder_SIHO<B,Q>* PolarDecoderFactory
::_build_scl_fast(const parameters &params, const std::vector<bool> &frozen_bits,
                  module::CRC<B> *crc, const std::unique_ptr<module::Encoder<B>>&)
{
	int idx_r0, idx_r1;
	auto polar_patterns = tools::Nodes_parser<>::parse_uptr(params.polar_nodes, idx_r0, idx_r1);

	if (params.implem == "FAST" && params.systematic)
	{
		if (crc != nullptr && crc->get_size() > 0)
		{
			if (params.type == "ASCL"    ) return new module::Decoder_polar_ASCL_fast_CA_sys    <B, Q, API_polar>(params.K, params.N_cw, params.L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, *crc, params.full_adaptive, params.n_frames);
			if (params.type == "ASCL_MEM") return new module::Decoder_polar_ASCL_MEM_fast_CA_sys<B, Q, API_polar>(params.K, params.N_cw, params.L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, *crc, params.full_adaptive, params.n_frames);
			if (params.type == "SCL"     ) return new module::Decoder_polar_SCL_fast_CA_sys     <B, Q, API_polar>(params.K, params.N_cw, params.L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, *crc,                      params.n_frames);
			if (params.type == "SCL_MEM" ) return new module::Decoder_polar_SCL_MEM_fast_CA_sys <B, Q, API_polar>(params.K, params.N_cw, params.L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, *crc,                      params.n_frames);
		}
		else
		{
			if (params.type == "SCL"     ) return new module::Decoder_polar_SCL_fast_sys        <B, Q, API_polar>(params.K, params.N_cw, params.L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1,                            params.n_frames);
			if (params.type == "SCL_MEM" ) return new module::Decoder_polar_SCL_MEM_fast_sys    <B, Q, API_polar>(params.K, params.N_cw, params.L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1,                            params.n_frames);
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* PolarDecoderFactory
::build_siso(const parameters& params, const std::vector<bool> &frozen_bits,
             const std::unique_ptr<module::Encoder<B>>&)
{
	if (params.type == "SCAN" && params.systematic)
	{
		if (params.implem == "NAIVE") return new module::Decoder_polar_SCAN_naive_sys<B, Q, tools::f_LLR<Q>, tools::v_LLR<Q>, tools::h_LLR<B,Q>>(params.K, params.N_cw, params.n_ite, frozen_bits, params.n_frames);
	}
	else if (params.type == "SCAN" && !params.systematic)
	{
		if (params.implem == "NAIVE") return new module::Decoder_polar_SCAN_naive    <B, Q, tools::f_LLR<Q>, tools::v_LLR<Q>, tools::h_LLR<B,Q>>(params.K, params.N_cw, params.n_ite, frozen_bits, params.n_frames);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* PolarDecoderFactory
::build(const parameters& params, const std::vector<bool> &frozen_bits, module::CRC<B> *crc,
        const std::unique_ptr<module::Encoder<B>>& encoder)
{
    if (params.type.find("SCL") != std::string::npos && params.implem == "FAST")
    {
        if (params.simd_strategy == "INTRA")
        {
            if (typeid(B) == typeid(signed char))
            {
                return _build_scl_fast<B,Q,tools::API_polar_dynamic_intra<B,Q>>(params, frozen_bits, crc, encoder);
            }
            else if (typeid(B) == typeid(short))
            {
                return _build_scl_fast<B,Q,tools::API_polar_dynamic_intra<B,Q>>(params, frozen_bits, crc, encoder);
            }
            else if (typeid(B) == typeid(int))
            {
                return _build_scl_fast<B,Q,tools::API_polar_dynamic_intra<B,Q>>(params, frozen_bits, crc, encoder);
            }
        }
        else if (params.simd_strategy.empty())
        {
            return _build_scl_fast<B,Q,tools::API_polar_dynamic_seq<B,Q>>(params, frozen_bits, crc, encoder);
        }
    }

    if (params.simd_strategy == "INTER" && params.type == "SC" && params.implem == "FAST")
    {
        if (typeid(B) == typeid(signed char))
        {
#ifdef AFF3CT_POLAR_BIT_PACKING
#ifdef API_POLAR_DYNAMIC
            using API_polar = tools::API_polar_dynamic_inter_8bit_bitpacking<B,Q>;
#else
            using API_polar = tools::API_polar_static_inter_8bit_bitpacking<B,Q>;
#endif
#else
#ifdef API_POLAR_DYNAMIC
            using API_polar = tools::API_polar_dynamic_inter<B,Q>;
#else
            using API_polar = tools::API_polar_static_inter<B,Q>;
#endif
#endif
            return _build<B,Q,API_polar>(params, frozen_bits, crc, encoder);
        }
        else
        {
#ifdef API_POLAR_DYNAMIC
            using API_polar = tools::API_polar_dynamic_inter<B,Q>;
#else
            using API_polar = tools::API_polar_static_inter<B,Q>;
#endif
            return _build<B,Q,API_polar>(params, frozen_bits, crc, encoder);
        }
    }
    else if (params.simd_strategy == "INTRA" && params.implem == "FAST")
    {
        if (typeid(B) == typeid(signed char))
        {
#ifdef API_POLAR_DYNAMIC
            using API_polar = tools::API_polar_dynamic_intra<B,Q>;
#else
            using API_polar = tools::API_polar_static_intra_8bit<B,Q>;
#endif
            return _build<B,Q,API_polar>(params, frozen_bits, crc, encoder);
        }
        else if (typeid(B) == typeid(short))
        {
#ifdef API_POLAR_DYNAMIC
            using API_polar = tools::API_polar_dynamic_intra<B,Q>;
#else
            using API_polar = tools::API_polar_static_intra_16bit<B,Q>;
#endif
            return _build<B,Q,API_polar>(params, frozen_bits, crc, encoder);
        }
        else if (typeid(B) == typeid(int))
        {
#ifdef API_POLAR_DYNAMIC
            using API_polar = tools::API_polar_dynamic_intra<B,Q>;
#else
            using API_polar = tools::API_polar_static_intra_32bit<B,Q>;
#endif
            return _build<B,Q,API_polar>(params, frozen_bits, crc, encoder);
        }
    }
    else if (params.simd_strategy.empty())
    {
#ifdef API_POLAR_DYNAMIC
        using API_polar = tools::API_polar_dynamic_seq<B,Q>;
#else
        using API_polar = tools::API_polar_static_seq<B,Q>;
#endif
        return _build<B,Q,API_polar>(params, frozen_bits, crc, encoder);
    }

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarDecoderFactory::build_siso<B_8 ,Q_8 >(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarDecoderFactory::build_siso<B_16,Q_16>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarDecoderFactory::build_siso<B_32,Q_32>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarDecoderFactory::build_siso<B_64,Q_64>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_64>>&);

template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarDecoderFactory::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B_8 >*, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarDecoderFactory::build<B_16,Q_16>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B_16>*, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarDecoderFactory::build<B_32,Q_32>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B_32>*, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::PolarDecoderFactory::build<B_64,Q_64>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B_64>*, const std::unique_ptr<module::Encoder<B_64>>&);
// ==================================================================================== explicit template instantiation
