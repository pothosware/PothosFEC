// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TestUtility.hpp"

#include <Pothos/Testing.hpp>

#include <Poco/RandomStream.h>

#include <math.h>
#include <stdlib.h>

namespace FECTests
{

/*
 * Copied directly from TurboFEC's unit tests.
 */

/*
 * Generate and add noise using Box-Muller method to transform two uniformly
 * distributed random values into normal distributed random values.
 */
static int add_noise(unsigned char *in, signed char *out,
	      int len, float snr, float amp)
{
	int i, _amp;
	float x1, x2, w, y1, y2, z1, z2, scale;

	if (fabsf(amp) > 127.0f)
		_amp = (int) 127;
	else
		_amp = (int) amp;

	scale = sqrt(powf((float) _amp, 2.0f) / powf(10.0f, snr / 10.0f));

	for (i = 0; i < len / 2; i++) {
		do {
			x1 = 2.0 * (float) rand() / (float) RAND_MAX - 1.0f;
			x2 = 2.0 * (float) rand() / (float) RAND_MAX - 1.0f;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0);

		w = sqrt((-2.0 * log(w)) / w);
		y1 = x1 * w;
		y2 = x2 * w;

		z1 = ((float) (-2 * in[2 * i + 0] + 1) * -amp) + y1 * scale;
		z2 = ((float) (-2 * in[2 * i + 1] + 1) * -amp) + y2 * scale;

		/* Saturate */
		if (z1 > 127.0f)
			z1 = 127.0f;
		else if (z1 < -127.0f)
			z1 = -127.0f;

		if (z2 > 127.0f)
			z2 = 127.0f;
		else if (z2 < -127.0f)
			z2 = -127.0f;

		out[2 * i + 0] = (signed char) z1;
		out[2 * i + 1] = (signed char) z2;
	}

	return 0;
}

/* Generate soft bits with ~2.3% crossover probability */
static int uint8_to_err(uint8_t *dst, uint8_t *src, int n)
{
	int i, err = 0;

	for (i = 0; i < n; i++) {
		if ((!src[i] && dst[i]) || (src[i] && !dst[i]))
			err++;
	}

	return err;
}

/*
 * Test-facing functions
 */

Pothos::BufferChunk getRandomInput(size_t numElems, bool asBits)
{
    Pothos::BufferChunk bufferChunk("uint8", numElems);

    Poco::RandomBuf randomBuf;
    randomBuf.readFromDevice(
        bufferChunk,
        numElems);
    if(asBits)
    {
        for(size_t elem = 0; elem < numElems; ++elem)
        {
            bufferChunk.as<std::uint8_t*>()[elem] %= 2;
        }
    }

    return bufferChunk;
}

// Note: Pothos::Object::operator== checks that the objects' data is the same,
// not just the value.
void testLabelsEqual(const Pothos::Label& label0, const Pothos::Label& label1)
{
    POTHOS_TEST_EQUAL(label0.id, label1.id);
    POTHOS_TEST_TRUE(label0.data.type() == label1.data.type());

    if(label0.data.type() == typeid(bool))
    {
        POTHOS_TEST_EQUAL(bool(label0.data), bool(label1.data));
        if(label0.data) POTHOS_TEST_EQUAL(0, label0.data.compareTo(label1.data));
    }

    POTHOS_TEST_EQUAL(label0.index, label1.index);
    POTHOS_TEST_EQUAL(label0.width, label1.width);
}

Pothos::BufferChunk addNoiseAndGetError(
    const Pothos::BufferChunk& bufferChunk,
    float snr,
    float amp,
    int* errorOut)
{
    Pothos::BufferChunk noisyBufferChunk("uint8", bufferChunk.length);
    add_noise(bufferChunk, noisyBufferChunk, bufferChunk.length, snr, amp);
    *errorOut = uint8_to_err(noisyBufferChunk, bufferChunk, bufferChunk.length);

    return noisyBufferChunk;
}

}
