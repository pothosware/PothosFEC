// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CoderTests.hpp"

#include <Pothos/Framework.hpp>
#include <Pothos/Proxy.hpp>
#include <Pothos/Testing.hpp>

#include <turbofec/turbo.h>

POTHOS_TEST_BLOCK("/fec/tests", test_lte_encoder_output_length)
{
    constexpr size_t numInputElems = TURBO_MAX_K;
    constexpr size_t numOutputElems = numInputElems * 3 + 4 * 3;
    constexpr unsigned rgen = 013;
    constexpr unsigned gen = 015;

    auto lteEncoder = Pothos::BlockRegistry::make("/fec/lte_turbo_encoder", rgen, gen);
    FECTests::testCoderOutputLength(
        lteEncoder,
        numInputElems,
        numOutputElems);
}

POTHOS_TEST_BLOCK("/fec/tests", test_lte_decoder_output_length)
{
    constexpr size_t numOutputElems = TURBO_MAX_K;
    constexpr size_t numInputElems = numOutputElems * 3 + 4 * 3;
    constexpr size_t numIterations = 4;

    auto lteDecoder = Pothos::BlockRegistry::make("/fec/lte_turbo_decoder", numIterations, true);
    FECTests::testCoderOutputLength(
        lteDecoder,
        numInputElems,
        numOutputElems);
}

POTHOS_TEST_BLOCK("/fec/tests", test_lte_coder_symmetry)
{
    constexpr size_t numElems = TURBO_MAX_K;
    constexpr unsigned rgen = 013;
    constexpr unsigned gen = 015;
    constexpr size_t numIterations = 4;

    auto lteEncoder = Pothos::BlockRegistry::make("/fec/lte_turbo_encoder", rgen, gen);
    auto lteDecoder = Pothos::BlockRegistry::make("/fec/lte_turbo_decoder", numIterations, true);
    FECTests::testCoderSymmetry(
        lteEncoder,
        lteDecoder,
        numElems);
}
