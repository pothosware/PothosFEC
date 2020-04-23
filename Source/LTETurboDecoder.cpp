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
#include <cstring>
#include <memory>

using DecodeFcn = int(*)(struct tdecoder*, int, int, uint8_t*, const int8_t*, const int8_t*, const int8_t*);

using tDecoderUPtr = std::unique_ptr<tdecoder, decltype(&::free_tdec)>;
static inline tDecoderUPtr makeDecoderUPtr()
{
    return tDecoderUPtr((tdecoder*)calloc(1, sizeof(tdecoder*)), &free_tdec);
}

class LTETurboDecoder: public Pothos::Block
{
    public:
        static Pothos::Block* make(size_t numIterations, bool unpack)
        {
            return new LTETurboDecoder(numIterations, unpack);
        }

        LTETurboDecoder(size_t numIterations, bool unpack):
            Pothos::Block(),
            _numIterations(numIterations),
            _unpack(unpack),
            _decodeFcn(_unpack ? ::lte_turbo_decode_unpack : ::lte_turbo_decode),
            _tDecoderUPtr(makeDecoderUPtr())
        {
            // Despite the function taking in int8_t*, it's immmediately casted to uint8_t*
            // internally, so for consistency with the encoder, we'll take in uint8_t* buffers.
            this->setupInput(0, "uint8");
            this->setupInput(1, "uint8");
            this->setupInput(2, "uint8");

            this->setupOutput(0, "uint8");

            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboDecoder, numIterations));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboDecoder, setNumIterations));
            this->registerProbe("numIterations");
            this->registerSignal("numIterationsChanged");
        }

        size_t numIterations() const
        {
            return _numIterations;
        }

        void setNumIterations(unsigned numIterations)
        {
            _numIterations = numIterations;

            this->emitSignal("numIterationsChanged", _numIterations);
        }

        void work() override
        {
            const auto elems = this->workInfo().minElements;
            if(0 == elems)
            {
                return;
            }

            const auto& inputs = this->inputs();
            auto output = this->output(0);

            _decodeFcn(
                _tDecoderUPtr.get(),
                static_cast<int>(elems),
                static_cast<int>(_numIterations),
                output->buffer(),
                inputs[0]->buffer(),
                inputs[1]->buffer(),
                inputs[2]->buffer());

            for(auto* input: inputs) input->consume(elems);
            output->produce(elems);
        }

    private:
        size_t _numIterations;
        bool _unpack;
        DecodeFcn _decodeFcn;
        tDecoderUPtr _tDecoderUPtr;
};

static Pothos::BlockRegistry registerLTETurboDecoder(
    "/fec/lte_turbo_decoder",
    Pothos::Callable(&LTETurboDecoder::make));
