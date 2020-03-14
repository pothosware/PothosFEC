// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvolutionalEncoderBase.hpp"
#include "Utility.hpp"

#include <Pothos/Callable.hpp>
#include <Pothos/Exception.hpp>

#include <algorithm>
#include <cstring>

// TODO: all fields
// TODO: handle struct storing both gen and rgen
// TODO: register calls, changed signals

class GenericConvolutionalEncoder: public ConvolutionalEncoderBase
{
public:
    GenericConvolutionalEncoder():
        ConvolutionalEncoderBase(nullptr)
    {
        _pConvCode = &_convCode;

        this->setN(2);
        this->setK(5);
        this->setLength(224);
        this->setPuncture({});
        this->setTerminationType("Flush");
    }

    void setN(int n)
    {
        if((n < 2) || (n > 4))
        {
            throw Pothos::InvalidArgumentException("N must be in range [2,4]");
        }

        _pConvCode->n = n;
    }

    void setK(int k)
    {
        if((k != 5) && (k != 7))
        {
            throw Pothos::InvalidArgumentException("K must be 5 or 7");
        }

        _pConvCode->k = k;
    }

    void setLength(int length)
    {
        if(length < 1)
        {
            throw Pothos::InvalidArgumentException("Length must be positive");
        }

        _pConvCode->len = length;
    }

    void setPuncture(const std::vector<int>& puncture)
    {
        // All puncture values must be positive. TurboFEC expects puncture
        // arrays to be terminated with -1, but we handle that ourselves.
        auto negativePuncIter = std::find_if(
                                    puncture.begin(),
                                    puncture.end(),
                                    [](int p)
                                    {
                                        return (p < 0);
                                    });
        if(puncture.end() != negativePuncIter)
        {
            throw Pothos::InvalidArgumentException("All puncture values must be >= 0.");
        }

        _puncture = puncture;

        if(puncture.empty())
        {
            _pConvCode->punc = nullptr;
        }
        else
        {
            // TurboFEC expects puncture arrays to be null-terminated.
            _puncture.emplace_back(-1);
            _pConvCode->punc = _puncture.data();
        }
    }

    void setTerminationType(const std::string& terminationType)
    {
        if("Flush" == terminationType)
        {
            _pConvCode->term = ::CONV_TERM_FLUSH;
        }
        else if("Tail-biting" == terminationType)
        {
            _pConvCode->term = ::CONV_TERM_TAIL_BITING;
        }
    }

private:
    lte_conv_code _convCode; // For the base class to point at

    std::vector<int> _puncture;
};
