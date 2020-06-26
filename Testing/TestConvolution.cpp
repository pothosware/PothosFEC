// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TestUtility.hpp"

#include <Pothos/Framework.hpp>
#include <Pothos/Proxy.hpp>
#include <Pothos/Testing.hpp>

#include <Poco/Format.h>
#include <Poco/String.h>

#include <iostream>
#include <string>
#include <vector>

//
// Utility functions
//

static std::string convertStandardName(const std::string& standardName)
{
    static const std::vector<std::string> charsToReplace{" ","-","."};
    auto convertedStandardName = Poco::toLower(standardName);
    for(const auto& charToReplace: charsToReplace)
    {
        Poco::replaceInPlace(convertedStandardName, charToReplace, std::string("_"));
    }

    return convertedStandardName;
}

static void testCodersAndGetBER(
    const Pothos::Proxy& encoder,
    const Pothos::Proxy& decoder,
    double* berOut)
{
    const auto length = encoder.call<size_t>("length");
    const auto numInputs = length * 32;

    //
    // Encode random inputs and introduce some noise.
    //

    auto randomInput = FECTests::getRandomInput(numInputs, true /*asBits*/);

    auto feederSource = Pothos::BlockRegistry::make("/blocks/feeder_source", "uint8");
    feederSource.call("feedBuffer", randomInput);

    auto collectorSink = Pothos::BlockRegistry::make("/blocks/collector_sink", "uint8");

    {
        Pothos::Topology topology;

        topology.connect(feederSource, 0, encoder, 0);
        topology.connect(encoder, 0, collectorSink, 0);

        topology.commit();
        POTHOS_TEST_TRUE(topology.waitInactive(0.05));
    }

    auto encodedValues = collectorSink.call<Pothos::BufferChunk>("getBuffer");

    int numBitsChanged = 0;
    auto noisyEncodedValues = FECTests::addNoiseAndGetError(
        encodedValues,
        FECTests::defaultSNR,
        FECTests::defaultAmp,
        &numBitsChanged);

    //
    // Decode the noisy encoded values.
    //

    feederSource.call("feedBuffer", noisyEncodedValues);
    collectorSink.call("clear");

    {
        Pothos::Topology topology;

        topology.connect(feederSource, 0, decoder, 0);
        topology.connect(decoder, 0, collectorSink, 0);

        topology.commit();
        POTHOS_TEST_TRUE(topology.waitInactive(0.05));
    }

    //
    // See how close our decoded values are to the initial random inputs.
    //

    auto decodedValues = collectorSink.call<Pothos::BufferChunk>("getBuffer");
    POTHOS_TEST_EQUAL(randomInput.length, decodedValues.length);

    auto decodedFeederSource = Pothos::BlockRegistry::make("/blocks/feeder_source", "uint8");
    decodedFeederSource.call("feedBuffer", decodedValues);
    feederSource.call("feedBuffer", randomInput);

    auto ber = Pothos::BlockRegistry::make("/fec/bit_error_rate", false /*packed*/);

    {
        Pothos::Topology topology;

        topology.connect(feederSource, 0, ber, 0);
        topology.connect(decodedFeederSource, 0, ber, 1);

        topology.commit();
        POTHOS_TEST_TRUE(topology.waitInactive(0.05));
    }

    *berOut = ber.call("ber");
}

//
// Test standard values
//

struct StandardValueTestParams
{
    std::string standard;
    int expectedN;
    int expectedK;
    int expectedLength;
    std::vector<unsigned> expectedGen;
    int expectedRGen;
    std::vector<int> expectedPuncture;
    std::string expectedTerminationType;
};
static const std::vector<StandardValueTestParams> allTestParams =
{
    {
        "GSM XCCH",
        2,
        5,
        224,
        {023, 033},
        0,
        {},
        "Flush"
    },
    {
        "GPRS CS2",
        2,
        5,
        290,
        {023, 033},
        0,
        {},
        "Flush"
    },
    {
        "GPRS CS3",
        2,
        5,
        334,
        {023, 033},
        0,
        {},
        "Flush"
    },
    {
        "GSM RACH",
        2,
        5,
        14,
        {023, 033},
        0,
        {},
        "Flush"
    },
    {
        "GSM SCH",
        2,
        5,
        35,
        {023, 033},
        0,
        {},
        "Flush"
    },
    {
        "GSM TCH-FR",
        2,
        5,
        185,
        {023, 033},
        0,
        {},
        "Flush"
    },
    {
        "GSM TCH-HR",
        3,
        7,
        98,
        {0133, 0145, 0175},
        0,
        {
              1,   4,   7,  10,  13,  16,  19,  22,  25,  28,  31,  34,
             37,  40,  43,  46,  49,  52,  55,  58,  61,  64,  67,  70,
             73,  76,  79,  82,  85,  88,  91,  94,  97, 100, 103, 106,
            109, 112, 115, 118, 121, 124, 127, 130, 133, 136, 139, 142,
            145, 148, 151, 154, 157, 160, 163, 166, 169, 172, 175, 178,
            181, 184, 187, 190, 193, 196, 199, 202, 205, 208, 211, 214,
            217, 220, 223, 226, 229, 232, 235, 238, 241, 244, 247, 250,
            253, 256, 259, 262, 265, 268, 271, 274, 277, 280, 283, 295,
            298, 301, 304, 307, 310, 313
        },
        "Flush"
    },
    {
        "GSM TCH-AFS12.2",
        2,
        5,
        250,
        {020, 033},
        023,
        {
            321, 325, 329, 333, 337, 341, 345, 349, 353, 357, 361, 363,
            365, 369, 373, 377, 379, 381, 385, 389, 393, 395, 397, 401,
            405, 409, 411, 413, 417, 421, 425, 427, 429, 433, 437, 441,
            443, 445, 449, 453, 457, 459, 461, 465, 469, 473, 475, 477,
            481, 485, 489, 491, 493, 495, 497, 499, 501, 503, 505, 507,
        },
        "Flush"
    },
    {
        "GSM TCH-AFS10.2",
        3,
        5,
        210,
        {033, 025, 020},
        037,
        {
              1,   4,   7,  10,  16,  19,  22,  28,  31,  34,  40,  43,
             46,  52,  55,  58,  64,  67,  70,  76,  79,  82,  88,  91,
             94, 100, 103, 106, 112, 115, 118, 124, 127, 130, 136, 139,
            142, 148, 151, 154, 160, 163, 166, 172, 175, 178, 184, 187,
            190, 196, 199, 202, 208, 211, 214, 220, 223, 226, 232, 235,
            238, 244, 247, 250, 256, 259, 262, 268, 271, 274, 280, 283,
            286, 292, 295, 298, 304, 307, 310, 316, 319, 322, 325, 328,
            331, 334, 337, 340, 343, 346, 349, 352, 355, 358, 361, 364,
            367, 370, 373, 376, 379, 382, 385, 388, 391, 394, 397, 400,
            403, 406, 409, 412, 415, 418, 421, 424, 427, 430, 433, 436,
            439, 442, 445, 448, 451, 454, 457, 460, 463, 466, 469, 472,
            475, 478, 481, 484, 487, 490, 493, 496, 499, 502, 505, 508,
            511, 514, 517, 520, 523, 526, 529, 532, 535, 538, 541, 544,
            547, 550, 553, 556, 559, 562, 565, 568, 571, 574, 577, 580,
            583, 586, 589, 592, 595, 598, 601, 604, 607, 609, 610, 613,
            616, 619, 621, 622, 625, 627, 628, 631, 633, 634, 636, 637,
            639, 640
        },
        "Flush"
    },
};

static void testBlockGetters(
    const Pothos::Proxy& block,
    const StandardValueTestParams& testParams)
{
    POTHOS_TEST_EQUAL(
        testParams.standard,
        block.call<std::string>("standard"));
    POTHOS_TEST_EQUAL(
        testParams.expectedN,
        block.call<int>("N"));
    POTHOS_TEST_EQUAL(
        testParams.expectedK,
        block.call<int>("K"));
    POTHOS_TEST_EQUAL(
        testParams.expectedLength,
        block.call<int>("length"));
    POTHOS_TEST_EQUALV(
        testParams.expectedGen,
        block.call<std::vector<int>>("gen"));
    POTHOS_TEST_EQUAL(
        testParams.expectedRGen,
        block.call<int>("rgen"));
    POTHOS_TEST_EQUALV(
        testParams.expectedPuncture,
        block.call<std::vector<int>>("puncture"));
    POTHOS_TEST_EQUAL(
        testParams.expectedTerminationType,
        block.call<std::string>("terminationType"));
}

POTHOS_TEST_BLOCK("/fec/tests", test_standard_conv_encoder_fields)
{
    for(const auto& testParams: allTestParams)
    {
        std::cout << " * Testing " << testParams.standard << "..." << std::endl;
        const auto blockRegistryPath = Poco::format("/fec/%s_encoder", convertStandardName(testParams.standard));

        auto block = Pothos::BlockRegistry::make(blockRegistryPath);
        testBlockGetters(block, testParams);
    }
}

POTHOS_TEST_BLOCK("/fec/tests", test_standard_conv_decoder_fields)
{
    for(const auto& testParams: allTestParams)
    {
        std::cout << " * Testing " << testParams.standard << "..." << std::endl;
        const auto blockRegistryPath = Poco::format("/fec/%s_decoder", convertStandardName(testParams.standard));

        auto block = Pothos::BlockRegistry::make(blockRegistryPath);
        testBlockGetters(block, testParams);
    }
}

//
// Test symmetry between encoders and decoders
//

static const std::vector<std::string> StandardNames =
{
    "GSM XCCH",
    "GPRS CS2",
    "GPRS CS3",
    "GSM RACH",
    "GSM SCH",
    "GSM TCH-FR",
    "GSM TCH-HR",
    "GSM TCH-AFS12.2",
    "GSM TCH-AFS10.2",
    "GSM TCH-AFS7.95",
    "GSM TCH-AFS7.4",
    "GSM TCH-AFS6.7",
    "GSM TCH-AFS5.9",
    "GSM TCH-AHS7.95",
    "GSM TCH-AHS7.4",
    "GSM TCH-AHS6.7",
    "GSM TCH-AHS5.9",
    "GSM TCH-AHS5.15",
    "GSM TCH-AHS4.75",
    "WiMax FCH",
    "LTE PBCH"
};

static void testStandardCoderSymmetry(const std::string& standardName)
{
    std::cout << " * Testing " << standardName << "..." << std::endl;

    const auto encoderBlockPath = Poco::format("/fec/%s_encoder", convertStandardName(standardName));
    const auto decoderBlockPath = Poco::format("/fec/%s_decoder", convertStandardName(standardName));

    auto encoder = Pothos::BlockRegistry::make(encoderBlockPath);
    auto decoder = Pothos::BlockRegistry::make(decoderBlockPath);
    POTHOS_TEST_EQUAL(standardName, encoder.call<std::string>("standard"));
    POTHOS_TEST_EQUAL(standardName, decoder.call<std::string>("standard"));

    double ber = 0.0;
    testCodersAndGetBER(encoder, decoder, &ber);

    POTHOS_TEST_LT(ber, 1e-3);
}

POTHOS_TEST_BLOCK("/fec/tests", test_standard_conv_coder_symmetry)
{
    for(const auto& standardName: StandardNames)
    {
        testStandardCoderSymmetry(standardName);
    }
}

//
// Test that getters and setters match for generic coders.
//

static void testGenericConvCoderSetter(const Pothos::Proxy& convCoder)
{
    static const std::vector<int> validN = {2,3,4};
    static const std::vector<int> validK = {5,7};
    constexpr size_t testLength = 128;
    constexpr unsigned testRGen = 037;
    static const std::vector<unsigned> testGen = {0100, 0145, 0175, 020};
    static const std::vector<int> testPunc = {5, 10, 50, 20, 25};
    static const std::vector<std::string> validTermTypes = {"Tail-biting", "Flush"};

    for(int N: validN)
    {
        convCoder.call("setN", N);
        POTHOS_TEST_EQUAL(N, convCoder.call("N"));
    }
    for(int K: validK)
    {
        convCoder.call("setK", K);
        POTHOS_TEST_EQUAL(K, convCoder.call("K"));
    }

    convCoder.call("setLength", testLength);
    POTHOS_TEST_EQUAL(testLength, convCoder.call("length"));

    convCoder.call("setGen", testGen);
    POTHOS_TEST_EQUALV(testGen, convCoder.call<std::vector<unsigned>>("gen"));

    convCoder.call("setPuncture", testPunc);
    POTHOS_TEST_EQUALV(testPunc, convCoder.call<std::vector<int>>("puncture"));

    for(const auto& termType: validTermTypes)
    {
        convCoder.call("setTerminationType", termType);
        POTHOS_TEST_EQUAL(termType, convCoder.call<std::string>("terminationType"));
    }

    convCoder.call("setRGen", testRGen);
    POTHOS_TEST_EQUAL(testRGen, convCoder.call("rgen"));
}

POTHOS_TEST_BLOCK("/fec/tests", test_generic_conv_coder_setter)
{
    const std::vector<std::string> genericCoders =
    {
        "/fec/generic_conv_encoder",
        "/fec/generic_conv_decoder"
    };
    for(const auto& coder: genericCoders)
    {
        std::cout << " * Testing " << coder << std::endl;
        testGenericConvCoderSetter(Pothos::BlockRegistry::make(coder));
    }
}

//
// Test with different parameters
//

POTHOS_TEST_BLOCK("/fec/tests", test_generic_conv_default_params)
{
    auto encoder = Pothos::BlockRegistry::make("/fec/generic_conv_encoder");
    auto decoder = Pothos::BlockRegistry::make("/fec/generic_conv_decoder");
    double ber = 0.0;

    testCodersAndGetBER(encoder, decoder, &ber);
    std::cout << ber << std::endl;
}
