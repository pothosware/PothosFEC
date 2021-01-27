#include <utility>

#include "Tools/Exception/exception.hpp"
#include "Tools/Documentation/documentation.h"
#include "Tools/Arguments/Splitter/Splitter.hpp"
#include "Tools/Display/rang_format/rang_format.h"
#include "Tools/Math/max.h"
#include "Tools/Code/LDPC/Matrix_handler/LDPC_matrix_handler.hpp"
#include "Module/Decoder/LDPC/BP/Flooding/Decoder_LDPC_BP_flooding.hpp"
#include "Module/Decoder/LDPC/BP/Horizontal_layered/Decoder_LDPC_BP_horizontal_layered.hpp"
#include "Module/Decoder/LDPC/BP/Vertical_layered/Decoder_LDPC_BP_vertical_layered.hpp"
#include "Tools/Code/LDPC/Update_rule/SPA/Update_rule_SPA.hpp"
#include "Tools/Code/LDPC/Update_rule/LSPA/Update_rule_LSPA.hpp"
#include "Tools/Code/LDPC/Update_rule/MS/Update_rule_MS.hpp"
#include "Tools/Code/LDPC/Update_rule/OMS/Update_rule_OMS.hpp"
#include "Tools/Code/LDPC/Update_rule/NMS/Update_rule_NMS.hpp"
#include "Tools/Code/LDPC/Update_rule/AMS/Update_rule_AMS.hpp"
#ifdef __cpp_aligned_new
#include "Module/Decoder/LDPC/BP/Horizontal_layered/Decoder_LDPC_BP_horizontal_layered_inter.hpp"
#include "Module/Decoder/LDPC/BP/Vertical_layered/Decoder_LDPC_BP_vertical_layered_inter.hpp"
#include "Module/Decoder/LDPC/BP/Flooding/Decoder_LDPC_BP_flooding_inter.hpp"
#include "Tools/Code/LDPC/Update_rule/SPA/Update_rule_SPA_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/LSPA/Update_rule_LSPA_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/MS/Update_rule_MS_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/OMS/Update_rule_OMS_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/NMS/Update_rule_NMS_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/AMS/Update_rule_AMS_simd.hpp"
#endif
#include "Module/Decoder/LDPC/BP/Flooding/Gallager/Decoder_LDPC_BP_flooding_Gallager_A.hpp"
#include "Module/Decoder/LDPC/BP/Flooding/Gallager/Decoder_LDPC_BP_flooding_Gallager_B.hpp"
#include "Module/Decoder/LDPC/BP/Flooding/Gallager/Decoder_LDPC_BP_flooding_Gallager_E.hpp"
#include "Module/Decoder/LDPC/BP/Flooding/SPA/Decoder_LDPC_BP_flooding_SPA.hpp"
#include "Module/Decoder/LDPC/BP/Peeling/Decoder_LDPC_BP_peeling.hpp"
#include "Module/Decoder/LDPC/BF/OMWBF/Decoder_LDPC_bit_flipping_OMWBF.hpp"
#include "Module/Decoder/LDPC/BF/PPBF/Decoder_LDPC_probabilistic_parallel_bit_flipping.hpp"

#include "LDPCDecoderFactory.hpp"
#include "LDPCDecoderBPHorizontalLayeredONMSInter.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;
using namespace aff3ct::module;
using namespace AFF3CTDynamic::POTHOS_SIMD_NAMESPACE;

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* LDPCDecoderFactory
::build_siso(const parameters& params, const tools::Sparse_matrix &H, const std::vector<unsigned> &info_bits_pos,
             const std::unique_ptr<module::Encoder<B>>&)
{
	if (params.type == "BP_FLOODING" && params.simd_strategy.empty())
	{
		const auto max_CN_degree = (unsigned int)H.get_cols_max_degree();

		if (params.implem == "MS"  )  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_MS  <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_MS  <Q                           >(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "OMS" )  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_OMS <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_OMS <Q                           >((Q)params.offset  ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "NMS" )  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_NMS <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS <Q                           >(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "SPA" )  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_SPA <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_SPA <Q                           >(max_CN_degree    ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "LSPA")  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_LSPA<Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_LSPA<Q                           >(max_CN_degree    ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "AMS" )
		{
			if (params.min == "MIN" ) return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_AMS <Q,tools::min             <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min             <Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINL") return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINS") return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_AMS <Q,tools::min_star        <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star        <Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
	}
	else if (params.type == "BP_HORIZONTAL_LAYERED" && params.simd_strategy.empty())
	{
		const auto max_CN_degree = (unsigned int)H.get_cols_max_degree();

		if (params.implem == "MS"  )  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_MS  <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_MS  <Q                           >(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "OMS" )  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_OMS <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_OMS <Q                           >((Q)params.offset  ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "NMS" )  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_NMS <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS <Q                           >(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "SPA" )  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_SPA <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_SPA <Q                           >(max_CN_degree    ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "LSPA")  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_LSPA<Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_LSPA<Q                           >(max_CN_degree    ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "AMS" )
		{
			if (params.min == "MIN" ) return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_AMS <Q,tools::min             <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min             <Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINL") return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINS") return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_AMS <Q,tools::min_star        <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star        <Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
	}
	else if (params.type == "BP_VERTICAL_LAYERED" && params.simd_strategy.empty())
	{
		const auto max_CN_degree = (unsigned int)H.get_cols_max_degree();

		if (params.implem == "MS"  )  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_MS  <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_MS  <Q                           >(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "OMS" )  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_OMS <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_OMS <Q                           >((Q)params.offset  ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "NMS" )  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_NMS <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS <Q                           >(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "SPA" )  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_SPA <Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_SPA <Q                           >(max_CN_degree    ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "LSPA")  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_LSPA<Q                           >>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_LSPA<Q                           >(max_CN_degree    ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "AMS" )
		{
			if (params.min == "MIN" ) return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_AMS <Q,tools::min             <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min             <Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINL") return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINS") return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_AMS <Q,tools::min_star        <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star        <Q>>(                 ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
	}
	else if (params.type == "BIT_FLIPPING")
	{
		     if (params.implem == "WBF" ) return new module::Decoder_LDPC_bit_flipping_OMWBF<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, (Q)0.               , params.enable_syndrome, params.syndrome_depth, params.n_frames);
		     if (params.implem == "MWBF") return new module::Decoder_LDPC_bit_flipping_OMWBF<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, (Q)params.mwbf_factor, params.enable_syndrome, params.syndrome_depth, params.n_frames);
	}
#ifdef __cpp_aligned_new
	else if (params.type == "BP_HORIZONTAL_LAYERED" && params.simd_strategy == "INTER")
	{
		const auto max_CN_degree = (unsigned int)H.get_cols_max_degree();

		if (params.implem == "SPA" ) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_SPA_simd <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_SPA_simd <Q>(max_CN_degree), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "LSPA") return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_LSPA_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_LSPA_simd<Q>(max_CN_degree), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "MS"  ) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_MS_simd  <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_MS_simd  <Q>(             ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "OMS" ) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_OMS_simd <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_OMS_simd <Q>(params.offset ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "NMS" )
		{
			if (typeid(Q) == typeid(int16_t) || typeid(Q) == typeid(int8_t))
			{
				if (params.norm_factor == 0.125f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,1>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,1>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.250f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,2>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,2>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.375f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,3>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,3>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.500f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,4>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,4>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.625f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,5>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,5>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.750f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,6>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,6>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.875f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,7>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,7>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 1.000f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,8>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,8>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);

				return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			}
			else
				return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
		else if (params.implem == "AMS" )
		{
			if (params.min == "MIN" ) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINL") return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINS") return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
	}
#endif
#ifdef __cpp_aligned_new
	else if (params.type == "BP_HORIZONTAL_LAYERED_LEGACY" && params.simd_strategy == "INTER")
#else
	else if (params.type == "BP_HORIZONTAL_LAYERED" && params.simd_strategy == "INTER")
#endif
	{
		if (params.implem == "MS" ) return new LDPCDecoderBPHorizontalLayeredONMSInter<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, 1.f              , (Q)0           , params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "NMS") return new LDPCDecoderBPHorizontalLayeredONMSInter<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, params.norm_factor, (Q)0           , params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "OMS") return new LDPCDecoderBPHorizontalLayeredONMSInter<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, 1.f              , (Q)params.offset, params.enable_syndrome, params.syndrome_depth, params.n_frames);
	}
#ifdef __cpp_aligned_new
	else if (params.type == "BP_FLOODING" && params.simd_strategy == "INTER")
	{
		const auto max_CN_degree = (unsigned int)H.get_cols_max_degree();

		if (params.implem == "SPA" ) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_SPA_simd <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_SPA_simd <Q>(max_CN_degree), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "LSPA") return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_LSPA_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_LSPA_simd<Q>(max_CN_degree), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "MS"  ) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_MS_simd  <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_MS_simd  <Q>(             ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "OMS" ) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_OMS_simd <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_OMS_simd <Q>(params.offset ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "NMS" )
		{
			if (typeid(Q) == typeid(int16_t) || typeid(Q) == typeid(int8_t))
			{
				if (params.norm_factor == 0.125f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,1>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,1>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.250f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,2>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,2>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.375f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,3>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,3>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.500f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,4>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,4>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.625f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,5>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,5>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.750f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,6>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,6>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.875f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,7>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,7>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 1.000f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,8>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,8>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);

				return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			}
			else
				return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
		else if (params.implem == "AMS" )
		{
			if (params.min == "MIN" ) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINL") return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINS") return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
	}
#endif
#ifdef __cpp_aligned_new
	else if (params.type == "BP_VERTICAL_LAYERED" && params.simd_strategy == "INTER")
	{
		const auto max_CN_degree = (unsigned int)H.get_cols_max_degree();

		if (params.implem == "SPA" ) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_SPA_simd <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_SPA_simd <Q>(max_CN_degree), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "LSPA") return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_LSPA_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_LSPA_simd<Q>(max_CN_degree), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "MS"  ) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_MS_simd  <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_MS_simd  <Q>(             ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "OMS" ) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_OMS_simd <Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_OMS_simd <Q>(params.offset ), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		if (params.implem == "NMS" )
		{
			if (typeid(Q) == typeid(int16_t) || typeid(Q) == typeid(int8_t))
			{
				if (params.norm_factor == 0.125f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,1>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,1>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.250f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,2>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,2>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.375f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,3>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,3>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.500f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,4>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,4>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.625f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,5>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,5>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.750f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,6>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,6>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 0.875f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,7>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,7>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
				if (params.norm_factor == 1.000f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,8>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,8>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);

				return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			}
			else
				return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(params.norm_factor), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
		else if (params.implem == "AMS" )
		{
			if (params.min == "MIN" ) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINL") return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
			if (params.min == "MINS") return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>(), params.enable_syndrome, params.syndrome_depth, params.n_frames);
		}
	}
#endif
	else if (params.type == "BP_FLOODING" && params.simd_strategy == "INTRA")
	{
		if (params.implem == "SPA" ) return new module::Decoder_LDPC_BP_flooding_SPA<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, params.enable_syndrome, params.syndrome_depth, params.n_frames);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* LDPCDecoderFactory
::build(const parameters& params, const tools::Sparse_matrix &H, const std::vector<unsigned> &info_bits_pos,
        const std::unique_ptr<module::Encoder<B>>&)
{
    if ((params.type == "BP" || params.type == "BP_FLOODING") && params.simd_strategy.empty())
    {
        if (params.implem == "GALA") return new module::Decoder_LDPC_BP_flooding_GALA<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, params.enable_syndrome, params.syndrome_depth, params.n_frames);
        if (params.implem == "GALB") return new module::Decoder_LDPC_BP_flooding_GALB<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, params.enable_syndrome, params.syndrome_depth, params.n_frames);
        if (params.implem == "GALE") return new module::Decoder_LDPC_BP_flooding_GALE<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, params.enable_syndrome, params.syndrome_depth, params.n_frames);
    }
    else if (params.type == "BP_PEELING")
    {
        if (params.implem == "STD") return new module::Decoder_LDPC_BP_peeling<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, params.enable_syndrome, params.syndrome_depth, params.n_frames);
    }
    else if (params.type == "BIT_FLIPPING")
    {
        if (params.implem == "PPBF") return new module::Decoder_LDPC_PPBF<B,Q>(params.K, params.N_cw, params.n_ite, H, info_bits_pos, params.ppbf_proba,  params.enable_syndrome, params.syndrome_depth, params.seed, params.n_frames);
    }
    return build_siso<B,Q>(params, H, info_bits_pos);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCDecoderFactory::build_siso<B_8 ,Q_8 >(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCDecoderFactory::build_siso<B_16,Q_16>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCDecoderFactory::build_siso<B_32,Q_32>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCDecoderFactory::build_siso<B_64,Q_64>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_64>>&);

#include "Tools/types.h"
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCDecoderFactory::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCDecoderFactory::build<B_16,Q_16>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCDecoderFactory::build<B_32,Q_32>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* AFF3CTDynamic::POTHOS_SIMD_NAMESPACE::LDPCDecoderFactory::build<B_64,Q_64>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_64>>&);
// ==================================================================================== explicit template instantiation
