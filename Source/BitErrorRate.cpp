// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>
#include <Pothos/Plugin.hpp>

#include <Poco/Mutex.h>

class BitErrorRate: public Pothos::Block
{
    public:
        static Pothos::Block* make(bool packed)
        {
            return new BitErrorRate(packed);
        }

        BitErrorRate(bool packed):
            Pothos::Block(),
            _packed(packed),
            _bitsChecked(0),
            _mismatchedBits(0)
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
            Poco::FastMutex::ScopedLock lock(_fieldMutex);

            return (double(_mismatchedBits) / double(_bitsChecked));
        }

        void work() override
        {
            if(0 == this->workInfo().minInElements) return;

            Poco::FastMutex::ScopedLock lock(_fieldMutex);

            if(_packed) _packedWork();
            else        _unpackedWork();
        }

    private:
        bool _packed;

        size_t _bitsChecked;
        size_t _mismatchedBits;

        mutable Poco::FastMutex _fieldMutex;

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

/*
 * |PothosDoc Bit Error Rate
 *
 * Takes in two input streams and records the bit error rate (BER), which
 * is calculated by:
 *
 * <p><b>BER = # bits different / # bits recorded</b></p>
 *
 * |category /FEC/Analysis
 * |keywords ber
 * |factory /fec/bit_error_rate(packed)
 *
 * |param packed[Packed?]
 * When set to true, each individual bit of the inputs is compared. When set to false, checks for
 * each byte being zero or non-zero.
 * |widget ToggleSwitch(on="True", off="False")
 * |default false
 */
static Pothos::BlockRegistry registerBER(
    "/fec/bit_error_rate",
    Pothos::Callable(&BitErrorRate::make));
