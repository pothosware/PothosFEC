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
#include <memory>
#include <string>

using DecodeFcn = int(*)(struct tdecoder*, int, int, uint8_t*, const int8_t*, const int8_t*, const int8_t*);

using tDecoderUPtr = std::unique_ptr<tdecoder, decltype(&::free_tdec)>;
static inline tDecoderUPtr makeDecoderUPtr()
{
    return tDecoderUPtr(::alloc_tdec(), &free_tdec);
}

constexpr size_t calcOutputSize(size_t inputSize)
{
    return (inputSize / 3) - 4;
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
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboDecoder, blockStartID));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboDecoder, setBlockStartID));

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

        std::string blockStartID() const
        {
            return _blockStartID;
        }

        void setBlockStartID(const std::string& blockStartID)
        {
            _blockStartID = blockStartID;
        }

        void propagateLabels(const Pothos::InputPort* input) override
        {
            if(!_blockStartID.empty())
            {
                // Don't propagate input label.
                for(const auto& label: input->labels())
                {
                    if(label.id != _blockStartID)
                    {
                        this->output(0)->postLabel(label);
                    }
                }
            }
            else Pothos::Block::propagateLabels(input);
        }

        void work() override
        {
            const auto elems = this->workInfo().minInElements;
            if((0 == elems) || (calcOutputSize(elems) < TURBO_MIN_K))
            {
                return;
            }

            if(_blockStartID.empty()) _work(elems);
            else                      _blockIDWork(elems);
        }

    private:
        size_t _numIterations;
        bool _unpack;
        DecodeFcn _decodeFcn;
        tDecoderUPtr _tDecoderUPtr;

        std::string _blockStartID;

        // Common code when we've determined our input size.
        void _work(size_t inputSize)
        {
            const auto& inputs = this->inputs();
            auto output = this->output(0);

            const auto outputSize = calcOutputSize(inputSize);

            _decodeFcn(
                _tDecoderUPtr.get(),
                static_cast<int>(outputSize),
                static_cast<int>(_numIterations),
                output->buffer(),
                inputs[0]->buffer(),
                inputs[1]->buffer(),
                inputs[2]->buffer());

            for(auto* input: inputs) input->consume(inputSize);
            output->produce(_unpack ? outputSize : (outputSize / 8));

            // Output a start block ID so an decoder can operate on the same data.
            if(!_blockStartID.empty()) output->postLabel(_blockStartID, outputSize, 0);
        }

        void _blockIDWork(size_t maxInputSize)
        {
            size_t inputSize = maxInputSize;

            const auto& inputs = this->inputs();

            bool blockFound = false;

            // We take in three inputs, but input 0 is expected to have
            // the block ID label.
            for(const auto& label: inputs[0]->labels())
            {
                // Skip if we haven't received enough data for this label.
                if(label.index > maxInputSize) continue;

                // Skip if this isn't a block start label.
                if(label.id != _blockStartID) continue;

                // If we have a length, use it.
                if(label.data.canConvert(typeid(size_t)))
                {
                    inputSize = label.data.convert<size_t>();
                    if(calcOutputSize(inputSize) > TURBO_MAX_K)
                    {
                        throw Pothos::InvalidArgumentException("Input length corresponds to an invalid block size. Max block size: " + std::to_string(TURBO_MAX_K));
                    }
                }

                // Skip all data before the block starts.
                if(0 != label.index)
                {
                    for(auto* input: inputs)
                    {
                        input->consume(label.index);
                        input->setReserve(inputSize);
                    }
                    return;
                }

                // If our block starts at the beginning of our buffer, wait until we have
                // enough data to encode.
                if(maxInputSize < inputSize)
                {
                    for(auto* input: inputs) input->setReserve(inputSize);
                    return;
                }

                blockFound = true;
                break;
            }

            if(blockFound) this->_work(inputSize);
            else           for(auto* input: inputs) input->consume(maxInputSize);
        }
};

/*
 * |PothosDoc LTE Turbo Decoder
 *
 * |category /FEC/LTE
 * |keywords coder
 * |factory /fec/lte_turbo_decoder(numIterations,unpack)
 * |setter setNumIterations(numIterations)
 *
 * |param numIterations[Num Iterations]
 * |widget SpinBox(minimum=1)
 * |default 4
 * |preview enable
 *
 * |param unpack[Unpack?]
 * |widget ToggleSwitch(on="True",off="False")
 * |default true
 * |preview enable
 *
 * |param blockStartID[Block Start ID]
 * The label used by the block to determine the beginning of the block to decode.
 * This label will be placed at the start of the corresponding encoded block.
 * If the given string is empty, the block will decode the entire
 * input buffer at once.
 * |widget LineEdit()
 * |default "START"
 * |preview disable
 */
static Pothos::BlockRegistry registerLTETurboDecoder(
    "/fec/lte_turbo_decoder",
    Pothos::Callable(&LTETurboDecoder::make));
