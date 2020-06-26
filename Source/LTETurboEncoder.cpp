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
#include <string>

constexpr size_t calcOutputSize(size_t inputSize)
{
    return (inputSize * 3) + (4 * 3);
}

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
            _gen(gen),
            _blockStartID()
        {
            this->setupInput(0, "uint8");

            this->setupOutput(0, "uint8");
            this->setupOutput(1, "uint8");
            this->setupOutput(2, "uint8");

            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, rgen));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, setRGen));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, gen));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, setGen));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, blockStartID));
            this->registerCall(this, POTHOS_FCN_TUPLE(LTETurboEncoder, setBlockStartID));

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
                        for(auto* output: this->outputs()) output->postLabel(label);
                    }
                }
            }
            else Pothos::Block::propagateLabels(input);
        }

        void work() override
        {
            const auto inputSize = this->input(0)->elements();
            if(inputSize < TURBO_MIN_K)
            {
                // We don't have enough data to encode yet.
                return;
            }

            if(_blockStartID.empty()) _work(inputSize);
            else                      _blockIDWork(inputSize);
        }

    private:
        unsigned _rgen;
        unsigned _gen;

        std::string _blockStartID;

        // Common code when we've determined our input size.
        void _work(size_t inputSize)
        {
            auto input = this->input(0);
            const auto& outputs = this->outputs();

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

            const bool mustPostBuffer = (calcOutputSize(inputSize) > this->workInfo().minOutElements);

            std::vector<Pothos::BufferChunk> outputBuffers;
            for(size_t port = 0; port < 3; ++port)
            {
                if(mustPostBuffer) outputBuffers.emplace_back(Pothos::BufferChunk("uint8", calcOutputSize(inputSize)));
                else               outputBuffers.emplace_back(outputs[0]->buffer());
            }

            int outSizeOrErr = ::lte_turbo_encode(
                                   &turboCode,
                                   input->buffer(),
                                   outputBuffers[0],
                                   outputBuffers[1],
                                   outputBuffers[2]);
            throwOnErrCode(outSizeOrErr);
            if(static_cast<size_t>(outSizeOrErr) != calcOutputSize(inputSize))
            {
                throw Pothos::AssertionViolationException("Output did not match expected length");
            }

            input->consume(inputSize);
            for(size_t port = 0; port < 3; ++port)
            {
                if(mustPostBuffer) outputs[port]->postBuffer(std::move(outputBuffers[port]));
                else               outputs[port]->produce(outSizeOrErr);
            }

            // Output a start block ID so an decoder can operate on the same data.
            if(!_blockStartID.empty()) outputs[0]->postLabel(_blockStartID, static_cast<size_t>(outSizeOrErr), 0);
        }

        void _blockIDWork(size_t maxInputSize)
        {
            size_t inputSize = maxInputSize;

            auto input = this->input(0);

            bool blockFound = false;

            for(const auto& label: input->labels())
            {
                // Skip if we haven't received enough data for this label.
                if(label.index > maxInputSize) continue;

                // Skip if this isn't a block start label.
                if(label.id != _blockStartID) continue;

                // If we have a length, use it.
                if(label.data.canConvert(typeid(size_t)))
                {
                    inputSize = label.data.convert<size_t>();
                    if(inputSize > TURBO_MAX_K)
                    {
                        throw Pothos::InvalidArgumentException("Max block size: " + std::to_string(TURBO_MAX_K));
                    }
                }

                // Skip all data before the block starts.
                if(0 != label.index)
                {
                    input->consume(label.index);
                    input->setReserve(inputSize);
                    return;
                }

                // If our block starts at the beginning of our buffer, wait until we have
                // enough data to encode.
                if(maxInputSize < inputSize)
                {
                    input->setReserve(inputSize);
                    return;
                }

                blockFound = true;
                break;
            }

            if(blockFound) this->_work(inputSize);
            else           input->consume(maxInputSize);
        }
};

/*
 * |PothosDoc LTE Turbo Encoder
 *
 * |category /FEC/Encoders
 * |keywords coder
 * |factory /fec/lte_turbo_encoder(rgen,gen)
 * |setter setRGen(rgen)
 * |setter setGen(gen)
 * |setter setBlockStartID(blockStartID)
 *
 * |param rgen[RGen] Recursive generator polynomial
 * |widget SpinBox(minimum=0,base=8)
 * |default 0
 * |preview enable
 *
 * |param gen[Gen] Generator polynomial
 * |widget SpinBox(minimum=0,base=8)
 * |default 0
 * |preview enable
 *
 * |param blockStartID[Block Start ID]
 * The label used by the block to determine the beginning of the block to encode.
 * This label will be placed at the start of the corresponding decoded block on
 * output port 0. If the given string is empty, the block will encode the entire
 * input buffer at once.
 * |widget LineEdit()
 * |default "START"
 * |preview disable
 */
static Pothos::BlockRegistry registerLTETurboEncoder(
    "/fec/lte_turbo_encoder",
    Pothos::Callable(&LTETurboEncoder::make));
