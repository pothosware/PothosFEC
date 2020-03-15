// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "GenericConvolutionBase.hpp"
#include "Utility.hpp"

#include <Pothos/Callable.hpp>
#include <Pothos/Exception.hpp>

#include <algorithm>
#include <cstring>

GenericConvolutionBase::GenericConvolutionBase():
    ConvolutionBase(nullptr)
{
    std::memset(&_convCode, 0, sizeof(_convCode));
    _pConvCode = &_convCode;

    this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolutionBase, setN));
    this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolutionBase, setK));
    this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolutionBase, setLength));
    this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolutionBase, setRecursiveGeneratorPolynomial));
    this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolutionBase, setGeneratorPolynomial));
    this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolutionBase, setPuncture));
    this->registerCall(this, POTHOS_FCN_TUPLE(GenericConvolutionBase, setTerminationType));

    this->registerSignal("NChanged");
    this->registerSignal("KChanged");
    this->registerSignal("lengthChanged");
    this->registerSignal("recursiveGeneratorPolynomialChanged");
    this->registerSignal("generatorPolynomialChanged");
    this->registerSignal("punctureChanged");
    this->registerSignal("terminationTypeChanged");

    this->setN(2);
    this->setK(5);
    this->setLength(224);
    this->setRecursiveGeneratorPolynomial(0);
    this->setGeneratorPolynomial({});
    this->setPuncture({});
    this->setTerminationType("Flush");
}

GenericConvolutionBase::~GenericConvolutionBase() {}

void GenericConvolutionBase::setN(int n)
{
    if((n < 2) || (n > 4))
    {
        throw Pothos::InvalidArgumentException("N must be in range [2,4]");
    }

    _pConvCode->n = n;
}

void GenericConvolutionBase::setK(int k)
{
    if((k != 5) && (k != 7))
    {
        throw Pothos::InvalidArgumentException("K must be 5 or 7");
    }

    _pConvCode->k = k;
}

void GenericConvolutionBase::setLength(int length)
{
    if(length < 1)
    {
        throw Pothos::InvalidArgumentException("Length must be positive");
    }

    _pConvCode->len = length;
}

void GenericConvolutionBase::setRecursiveGeneratorPolynomial(int rgen)
{
    if(rgen < 0)
    {
        throw Pothos::InvalidArgumentException("RGen must be >= 0");
    }

    _pConvCode->rgen = rgen;
}

void GenericConvolutionBase::setGeneratorPolynomial(const std::vector<unsigned>& gen)
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
}

void GenericConvolutionBase::setPuncture(const std::vector<int>& puncture)
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
}

void GenericConvolutionBase::setTerminationType(const std::string& terminationType)
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
}
