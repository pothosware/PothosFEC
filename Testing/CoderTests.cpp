// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CoderTests.hpp"
#include "TestUtility.hpp"

#include <Pothos/Framework.hpp>
#include <Pothos/Proxy.hpp>
#include <Pothos/Testing.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace FECTests
{
    void testCoderOutputLength(
        const Pothos::Proxy& coder,
        size_t numInputElems,
        size_t expectedNumOutputElems)
    {
        const auto blockStartID = "START";
        const auto numJunkElems = (numInputElems / 5);

        const auto numInputs = coder.call("inputs").call<size_t>("size");
        const auto numOutputs = coder.call("outputs").call<size_t>("size");

        std::vector<Pothos::Proxy> feederSources(numInputs);
        std::vector<Pothos::Proxy> collectorSinks(numOutputs);

        for(size_t i = 0; i < numInputs; ++i)
        {
            feederSources[i] = Pothos::BlockRegistry::make("/blocks/feeder_source", "uint8");

            feederSources[i].call("feedBuffer", getRandomInput(numInputElems+(2*numJunkElems), true));
            feederSources[i].call("feedLabel", Pothos::Label(blockStartID, numInputElems, numJunkElems));
        }
        for(size_t i = 0; i < numOutputs; ++i)
        {
            collectorSinks[i] = Pothos::BlockRegistry::make("/blocks/collector_sink", "uint8");
        }

        coder.call("setBlockStartID", blockStartID);

        {
            Pothos::Topology topology;

            for(size_t i = 0; i < numInputs; ++i)  topology.connect(feederSources[i], 0, coder, i);
            for(size_t i = 0; i < numOutputs; ++i) topology.connect(coder, i, collectorSinks[i], 0);

            topology.commit();
            POTHOS_TEST_TRUE(topology.waitInactive(0.1));
        }

        for(size_t i = 0; i < numOutputs; ++i)
        {
            std::cout << " * Checking output " << i << "..." << std::endl;

            auto output = collectorSinks[i].call<Pothos::BufferChunk>("getBuffer");
            POTHOS_TEST_EQUAL(expectedNumOutputElems, output.length);
        }
    }

    void testCoderSymmetry(
        const Pothos::Proxy& encoder,
        const Pothos::Proxy& decoder,
        size_t numElems)
    {
        const auto blockStartID = "START";

        encoder.call("setBlockStartID", blockStartID);
        decoder.call("setBlockStartID", blockStartID);

        // Make sure coder ports are symmetrical.
        POTHOS_TEST_EQUAL(
            encoder.call("inputs").call<size_t>("size"),
            decoder.call("outputs").call<size_t>("size"));
        POTHOS_TEST_EQUAL(
            decoder.call("inputs").call<size_t>("size"),
            encoder.call("outputs").call<size_t>("size"));

        const auto numEncoderInputs = encoder.call("inputs").call<size_t>("size");
        const auto numEncoderOutputs = encoder.call("outputs").call<size_t>("size");

        std::vector<Pothos::BufferChunk> inputs(numEncoderInputs);
        std::vector<Pothos::Proxy> feederSources(numEncoderInputs);
        std::vector<Pothos::Proxy> collectorSinks(numEncoderInputs);

        for(size_t i = 0; i < numEncoderInputs; ++i)
        {
            inputs[i] = getRandomInput(numElems, true);

            feederSources[i] = Pothos::BlockRegistry::make("/blocks/feeder_source", "uint8");
            feederSources[i].call("feedBuffer", inputs[i]);
            feederSources[i].call("feedLabel", Pothos::Label(blockStartID, numElems, 0));

            collectorSinks[i] = Pothos::BlockRegistry::make("/blocks/collector_sink", "uint8");
        }

        {
            Pothos::Topology topology;

            for(size_t i = 0; i < numEncoderInputs; ++i)
            {
                topology.connect(feederSources[i], 0, encoder, i);
                topology.connect(decoder, i, collectorSinks[i], 0);
            }
            for(size_t i = 0; i < numEncoderOutputs; ++i)
            {
                topology.connect(encoder, i, decoder, i);
            }

            topology.commit();
            POTHOS_TEST_TRUE(topology.waitInactive(0.05));
        }

        for(size_t i = 0; i < numEncoderInputs; ++i)
        {
            std::cout << " * Checking decoder output " << i << "..." << std::endl;

            const auto outputBuffer = collectorSinks[i].call<Pothos::BufferChunk>("getBuffer");
            POTHOS_TEST_EQUAL(numElems, outputBuffer.elements());

            // With no noise added between encoding and decoding, the decoded output should be identical
            // to the input.
            POTHOS_TEST_EQUALA(
                inputs[i].as<const std::uint8_t*>(),
                outputBuffer.as<const std::uint8_t*>(),
                inputs[i].elements());
        }
    }
}
