// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CoderTests.hpp"

#include <Pothos/Framework.hpp>
#include <Pothos/Testing.hpp>

#include <iostream>

//
// BCH coders
//

template <typename T>
static void testBCHCoders()
{
    const auto dtype = Pothos::DType(typeid(T));

    std::cout << "Testing " << dtype.toString() << "..." << std::endl;

    auto encoderBlock = Pothos::BlockRegistry::make(
                            "/fec/bch_encoder",
                            dtype);

    auto decoderBlockSIHO = Pothos::BlockRegistry::make(
                                "/fec/bch_decoder",
                                dtype,
                                "SIHO");

    auto decoderBlockHIHO = Pothos::BlockRegistry::make(
                                "/fec/bch_decoder",
                                dtype,
                                "HIHO");
}

POTHOS_TEST_BLOCK("/fec/tests", test_bch_coders)
{
    testBCHCoders<std::int8_t>();
    testBCHCoders<std::int16_t>();
    testBCHCoders<std::int32_t>();
    testBCHCoders<std::int64_t>();
}

//
// LDPC coders
//

template <typename T>
static void testLDPCCoders()
{
    const auto dtype = Pothos::DType(typeid(T));

    std::cout << "Testing " << dtype.toString() << "..." << std::endl;

    auto encoderBlock = Pothos::BlockRegistry::make(
                            "/fec/ldpc_encoder",
                            dtype);

    auto decoderBlockSISO = Pothos::BlockRegistry::make(
                                "/fec/ldpc_decoder",
                                dtype,
                                "SISO");

    auto decoderBlockSIHO = Pothos::BlockRegistry::make(
                                "/fec/ldpc_decoder",
                                dtype,
                                "SIHO");
}

POTHOS_TEST_BLOCK("/fec/tests", test_ldpc_coders)
{
    testLDPCCoders<std::int8_t>();
    testLDPCCoders<std::int16_t>();
    testLDPCCoders<std::int32_t>();
    testLDPCCoders<std::int64_t>();
}

//
// LDPC (DVBS2) coders
//

template <typename T>
static void testLDPCDVBS2Coders()
{
    const auto dtype = Pothos::DType(typeid(T));

    std::cout << "Testing " << dtype.toString() << "..." << std::endl;

    auto encoderBlock = Pothos::BlockRegistry::make(
                            "/fec/ldpc_encoder_dvbs2",
                            dtype);

    auto decoderBlockSISO = Pothos::BlockRegistry::make(
                                "/fec/ldpc_decoder_dvbs2",
                                dtype,
                                "SISO");

    auto decoderBlockSIHO = Pothos::BlockRegistry::make(
                                "/fec/ldpc_decoder_dvbs2",
                                dtype,
                                "SIHO");
}

POTHOS_TEST_BLOCK("/fec/tests", test_ldpc_dvbs2_coders)
{
    testLDPCDVBS2Coders<std::int8_t>();
    testLDPCDVBS2Coders<std::int16_t>();
    testLDPCDVBS2Coders<std::int32_t>();
    testLDPCDVBS2Coders<std::int64_t>();
}

//
// RA coders
//

template <typename T>
static void testRACoders()
{
    const auto dtype = Pothos::DType(typeid(T));

    std::cout << "Testing " << dtype.toString() << "..." << std::endl;

    auto encoderBlock = Pothos::BlockRegistry::make(
                            "/fec/ra_encoder",
                            dtype);

    auto decoderBlock = Pothos::BlockRegistry::make(
                            "/fec/ra_decoder",
                            dtype);
}

POTHOS_TEST_BLOCK("/fec/tests", test_ra_coders)
{
    testRACoders<std::int8_t>();
    testRACoders<std::int16_t>();
    testRACoders<std::int32_t>();
    testRACoders<std::int64_t>();
}
