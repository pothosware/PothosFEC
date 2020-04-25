#ifndef _NOISE_H_
#define _NOISE_H_

#include <Pothos/Framework.hpp>

static constexpr float defaultSNR = 8.0f;
static constexpr float defaultAmp = 32.0f;

/*
 * Generate and add noise using Box-Muller method to transform two uniformly
 * distributed random values into normal distributed random values.
 */
Pothos::BufferChunk addNoiseAndGetError(
    const Pothos::BufferChunk& bufferChunk,
    float snr,
    float amp,
    int* errorOut);

#endif /* _NOISE_H_ */
