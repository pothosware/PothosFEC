// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include <Pothos/Framework.hpp>
#include <Pothos/Proxy.hpp>
#include <Pothos/Testing.hpp>

#include <turbofec/turbo.h>

#include <Poco/RandomStream.h>

#include <iostream>
#include <string>
#include <vector>

static Pothos::BufferChunk getRandomInput(size_t numElems)
{
    Pothos::BufferChunk bufferChunk("uint8", numElems);

    Poco::RandomBuf randomBuf;
    randomBuf.readFromDevice(
        bufferChunk,
        numElems);

    return bufferChunk;
}

// Note: Pothos::Object::operator== checks that the objects' data is the same,
// not just the value.
static void expectLabelsEqual(const Pothos::Label& label0, const Pothos::Label& label1)
{
    POTHOS_TEST_EQUAL(label0.id, label1.id);

    POTHOS_TEST_EQUAL(bool(label0.data), bool(label1.data));
    if(label0.data) POTHOS_TEST_EQUAL(0, label0.data.compareTo(label1.data));

    POTHOS_TEST_EQUAL(label0.index, label1.index);
    POTHOS_TEST_EQUAL(label0.width, label1.width);
}

POTHOS_TEST_BLOCK("/fec/tests", test_lte_encoder_output_length)
{
    constexpr size_t numElems = TURBO_MAX_K;
    constexpr size_t numJunkElems = 512;
    const std::string blockStartID = "START";

    auto feederSource = Pothos::BlockRegistry::make("/blocks/feeder_source", "uint8");
    auto lteEncoder = Pothos::BlockRegistry::make("/fec/lte_turbo_encoder", 013, 015);
    std::vector<Pothos::Proxy> collectorSinks;
    for(size_t port = 0; port < 3; ++port)
    {
        collectorSinks.emplace_back(Pothos::BlockRegistry::make("/blocks/collector_sink", "uint8"));
    }

    auto randomInputPlusJunk = getRandomInput(numJunkElems);
    randomInputPlusJunk.append(getRandomInput(numElems));
    randomInputPlusJunk.append(getRandomInput(numJunkElems));

    feederSource.call("feedBuffer", randomInputPlusJunk);
    feederSource.call("feedLabel", Pothos::Label(blockStartID, numElems, numJunkElems));

    lteEncoder.call("setBlockStartID", blockStartID);

    {
        Pothos::Topology topology;

        topology.connect(feederSource, 0, lteEncoder, 0);
        topology.connect(lteEncoder, 0, collectorSinks[0], 0);
        topology.connect(lteEncoder, 1, collectorSinks[1], 0);
        topology.connect(lteEncoder, 2, collectorSinks[2], 0);

        topology.commit();
        POTHOS_TEST_TRUE(topology.waitInactive(0.05));
    }

    // This will make sure that only the intended data is encoded.
    constexpr size_t numOutputElems = numElems * 3 + 4 * 3;
    POTHOS_TEST_EQUAL(numOutputElems, collectorSinks[0].call("getBuffer").get<size_t>("length"));
    POTHOS_TEST_EQUAL(numOutputElems, collectorSinks[1].call("getBuffer").get<size_t>("length"));
    POTHOS_TEST_EQUAL(numOutputElems, collectorSinks[2].call("getBuffer").get<size_t>("length"));

    const auto expectedLabel = Pothos::Label(blockStartID, numOutputElems, 0);
    const auto actualLabels = collectorSinks[0].call<std::vector<Pothos::Label>>("getLabels");
    expectLabelsEqual(expectedLabel, actualLabels[0]);
}
