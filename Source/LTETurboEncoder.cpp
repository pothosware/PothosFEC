// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Utility.hpp"

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>
#include <Pothos/Plugin.hpp>

extern "C"
{
#include <turbofec/turbo.h>
}

#include <algorithm>

class LTETurboEncoder: public Pothos::Block
{
    public:
        static Pothos::Block* make(unsigned rgen, unsigned gen)
        {
            return new LTETurboEncoder(rgen, gen);
        }

        LTETurboEncoder(unsigned rgen, unsigned gen):
            Pothos::Block(),
            _rgen(rgen),
            _gen(gen)
        {
            this->setupInput(0, "uint8");

            this->setupOutput(0, "uint8");
            this->setupOutput(1, "uint8");
            this->setupOutput(2, "uint8");

            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, rgen));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, setRGen));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, gen));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, setGen));

            this->registerProbe("rgen");
            this->registerProbe("gen");

            this->registerSignal("rgenChanged");
            this->registerSignal("genChanged");
        }

        unsigned rgen() const
        {
            return _rgen;
        }

        void setRGen(unsigned rgen)
        {
            _rgen = rgen;

            this->emitSignal("rgenChanged", _rgen);
        }

        unsigned gen() const
        {
            return _gen;
        }

        void setGen(unsigned gen)
        {
            _gen = gen;

            this->emitSignal("genChanged", _gen);
        }

        void work() override
        {
            if(0 == this->workInfo().minElements)
            {
                return;
            }

            auto in = this->input(0);
            const auto& out = this->outputs();

            // We know the output is (len * 3 + 4 * 3).
            const std::vector<size_t> maxSizes{
                                          this->workInfo().minInElements,
                                          (this->workInfo().minOutElements / 3) - 4,
                                          TURBO_MAX_K};
            const auto inputSize = *std::min_element(maxSizes.begin(), maxSizes.end());
            if(inputSize < TURBO_MIN_K)
            {
                // We don't have enough data to encode yet.
                return;
            }

            // Per TurboFEC's documentation, only a rate of 2 and
            // constraint length of 4 are supported.
            struct lte_turbo_code turboCode =
            {
                2, // n
                4, // k
                static_cast<int>(inputSize),
                _rgen,
                _gen
            };

            int outSizeOrErr = ::lte_turbo_encode(
                                   &turboCode,
                                   in->buffer(),
                                   out[0]->buffer(),
                                   out[1]->buffer(),
                                   out[2]->buffer());
            throwOnErrCode(outSizeOrErr);

            in->consume(inputSize);
            for(auto* output: out) output->produce(outSizeOrErr);
        }

    private:
        unsigned _rgen;
        unsigned _gen;
};

static Pothos::BlockRegistry registerLTETurboEncoder(
    "/fec/lte_turbo_encoder",
    Pothos::Callable(&LTETurboEncoder::make));
