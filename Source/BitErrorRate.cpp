// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>
#include <Pothos/Plugin.hpp>

class BitErrorRate: public Pothos::Block
{
    public:
        static Pothos::Block* make(bool packed)
        {
            return new BitErrorRate(packed);
        }

        BitErrorRate(bool packed):
            Pothos::Block(),
            _packed(packed)
        {
            this->setupInput(0, "uint8");
            this->setupInput(1, "uint8");

            this->registerCall(this, POTHOS_FCN_TUPLE(BitErrorRate, reset));
            this->registerCall(this, POTHOS_FCN_TUPLE(BitErrorRate, ber));

            this->registerProbe("ber");
        }

        void reset()
        {
            _bitsChecked = 0;
            _mismatchedBits = 0;
        }

        double ber() const
        {
            return (double(_mismatchedBits) / double(_bitsChecked));
        }

        void work() override
        {
            if(0 == this->workInfo().minInElements) return;

            if(_packed) _packedWork();
            else        _unpackedWork();
        }

    private:
        bool _packed;

        size_t _bitsChecked;
        size_t _mismatchedBits;

        void _packedWork()
        {
            const auto elems = this->workInfo().minInElements;

            auto input0 = this->input(0);
            auto input1 = this->input(1);

            const auto* inBuff0 = input0->buffer().as<const std::uint8_t*>();
            const auto* inBuff1 = input1->buffer().as<const std::uint8_t*>();

            for(size_t elem = 0; elem < elems; ++elem)
            {
                for(std::uint8_t bit = 0; bit < 8; ++bit)
                {
                    if((inBuff0[elem] & (1 << bit)) != (inBuff1[elem] & (1 << bit)))
                    {
                        ++_mismatchedBits;
                    }
                    ++_bitsChecked;
                }
            }

            input0->consume(elems);
            input1->consume(elems);
        }

        void _unpackedWork()
        {
            const auto elems = this->workInfo().minInElements;

            auto input0 = this->input(0);
            auto input1 = this->input(1);

            const auto* inBuff0 = input0->buffer().as<const std::uint8_t*>();
            const auto* inBuff1 = input1->buffer().as<const std::uint8_t*>();

            for(size_t elem = 0; elem < elems; ++elem)
            {
                if((0 == inBuff0[elem]) != (0 == inBuff1[elem]))
                {
                    ++_mismatchedBits;
                }
                ++_bitsChecked;
            }

            input0->consume(elems);
            input1->consume(elems);
        }
};

static Pothos::BlockRegistry registerBER(
    "/fec/bit_error_rate",
    Pothos::Callable(&BitErrorRate::make));
