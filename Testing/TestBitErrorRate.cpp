// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TestUtility.hpp"

#include <Pothos/Framework.hpp>
#include <Pothos/Testing.hpp>

#include <turbofec/turbo.h>

using namespace FECTests;

constexpr size_t numElems = TURBO_MAX_K;

POTHOS_TEST_BLOCK("/fec/tests", test_bit_error_rate_no_noise)
{
    auto feederSource = Pothos::BlockRegistry::make("/blocks/feeder_source", "uint8");
    feederSource.call("feedBuffer", getRandomInput(numElems, true /*asBits*/));

    auto bitErrorRate = Pothos::BlockRegistry::make("/fec/bit_error_rate", false /*packed*/);

    {
        Pothos::Topology topology;

        topology.connect(feederSource, 0, bitErrorRate, 0);
        topology.connect(feederSource, 0, bitErrorRate, 1);

        topology.commit();
        POTHOS_TEST_TRUE(topology.waitInactive(0.05));
    }

    // Since the inputs are the same, there should be no error.
    POTHOS_TEST_EQUAL(0.0, bitErrorRate.call<double>("ber"));
}

POTHOS_TEST_BLOCK("/fec/tests", test_bit_error_rate_with_noise)
{
    constexpr size_t numElems = TURBO_MAX_K;

    int numBitsChanged = 0;
    auto randomInput = getRandomInput(numElems, true /*asBits*/);
    auto noisyInput = addNoiseAndGetError(
                          randomInput,
                          defaultSNR,
                          defaultAmp,
                          &numBitsChanged);
    const double expectedBER = (double(numBitsChanged) / double(randomInput.length));

    auto feederSource0 = Pothos::BlockRegistry::make("/blocks/feeder_source", "uint8");
    feederSource0.call("feedBuffer", randomInput);

    auto feederSource1 = Pothos::BlockRegistry::make("/blocks/feeder_source", "uint8");
    feederSource1.call("feedBuffer", noisyInput);

    auto bitErrorRate = Pothos::BlockRegistry::make("/fec/bit_error_rate", false /*packed*/);

    {
        Pothos::Topology topology;

        topology.connect(feederSource0, 0, bitErrorRate, 0);
        topology.connect(feederSource1, 0, bitErrorRate, 1);

        topology.commit();
        POTHOS_TEST_TRUE(topology.waitInactive(0.05));
    }

    POTHOS_TEST_CLOSE(expectedBER, bitErrorRate.call<double>("ber"), 1e-6);
}
