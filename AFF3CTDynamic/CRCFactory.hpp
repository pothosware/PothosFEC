#pragma once

#include "Tools/Arguments/Argument_tools.hpp"
#include "Module/CRC/CRC.hpp"
#include "Factory/Factory.hpp"
#include "Factory/Module/CRC/CRC.hpp"

#include <string>

namespace AFF3CTDynamic
{
namespace POTHOS_SIMD_NAMESPACE
{
struct CRCFactory : public aff3ct::factory::Factory
{
    using parameters = aff3ct::factory::CRC::parameters;

	template <typename B = int>
	static aff3ct::module::CRC<B>* build(const parameters &params);
};
}
}
