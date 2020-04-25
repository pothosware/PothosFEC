// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvolutionBase.hpp"
#include "Utility.hpp"

#include <Pothos/Callable.hpp>
#include <Pothos/Exception.hpp>

#include <algorithm>
#include <cstring>

class GenericConvolution: public ConvolutionBase
{
public:
    static Pothos::Block* make(bool isEncoder)
    {
        return new GenericConvolution(isEncoder);
    }

    GenericConvolution(bool isEncoder):
        ConvolutionBase(nullptr, isEncoder)
    {
        std::memset(&_convCode, 0, sizeof(_convCode));
        _pConvCode = &_convCode;

        this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolution, setN));
        this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolution, setK));
        this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolution, setLength));
        this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolution, setRGen));
        this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolution, setGen));
        this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolution, setPuncture));
        this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolution, setTerminationType));

        this->registerSignal("NChanged");
        this->registerSignal("KChanged");
        this->registerSignal("lengthChanged");
        this->registerSignal("rgenChanged");
        this->registerSignal("genChanged");
        this->registerSignal("punctureChanged");
        this->registerSignal("terminationTypeChanged");

        this->setN(2);
        this->setK(5);
        this->setLength(224);
        this->setRGen(0);
        this->setGen({});
        this->setPuncture({});
        this->setTerminationType("Flush");
    }

    ~GenericConvolution() {}

    void setN(int n)
    {
        if((n < 2) || (n > 4))
        {
            throw Pothos::InvalidArgumentException("N must be in range [2,4]");
        }

        _pConvCode->n = n;

        this->emitSignal("NChanged", n);
    }

    void setK(int k)
    {
        if((k != 5) && (k != 7))
        {
            throw Pothos::InvalidArgumentException("K must be 5 or 7");
        }

        _pConvCode->k = k;

        this->emitSignal("KChanged", k);
    }

    void setLength(int length)
    {
        if(length < 1)
        {
            throw Pothos::InvalidArgumentException("Length must be positive");
        }

        _pConvCode->len = length;

        this->emitSignal("lengthChanged", length);
    }

    void setRGen(int rgen)
    {
        if(rgen < 0)
        {
            throw Pothos::InvalidArgumentException("RGen must be >= 0");
        }

        _pConvCode->rgen = rgen;

        this->emitSignal("rgenChanged", rgen);
    }

    void setGen(const std::vector<unsigned>& gen)
    {
        if(gen.size() > 4)
        {
            throw Pothos::InvalidArgumentException("Gen must be of size 0-4");
        }

        // This is stored as a static array of size 4.
        std::memset(_pConvCode->gen, 0, sizeof(_pConvCode->gen));
        if(!gen.empty())
        {
            std::memcpy(
                _pConvCode->gen,
                gen.data(),
                (gen.size() * sizeof(unsigned)));
        }

        this->emitSignal("genChanged", gen);
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
            // TurboFEC expects puncture arrays to be terminated with -1.
            _puncture.emplace_back(-1);
            _pConvCode->punc = _puncture.data();
        }

        this->emitSignal("punctureChanged", puncture);
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
        else
        {
            throw Pothos::InvalidArgumentException("Invalid termination type: "+terminationType);
        }

        this->emitSignal("terminationTypeChanged", terminationType);
    }

private:
    lte_conv_code _convCode; // For base class to point to
    std::vector<int> _puncture; // For _convCode's pointer to point to
};

static Pothos::BlockRegistry registerGenericConvolutionEncoder(
    "/fec/generic_conv_encoder",
    Pothos::Callable(&GenericConvolution::make)
        .bind(true, 0));

static Pothos::BlockRegistry registerGenericConvolutionDecoder(
    "/fec/generic_conv_decoder",
    Pothos::Callable(&GenericConvolution::make)
        .bind(false, 0));
