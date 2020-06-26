// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConvolutionBase.hpp"
#include "Utility.hpp"

#include <Pothos/Callable.hpp>
#include <Pothos/Exception.hpp>

#include <algorithm>
#include <cstring>

static bool isVectorEmptyOrZeros(const std::vector<unsigned>& vec)
{
    if(vec.empty()) return true;

    auto nonZeroIter = std::find_if(vec.begin(), vec.end(), [](unsigned v){return (v != 0);});
    return (nonZeroIter == vec.end());
}

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

        // Note: defaults come from GSM XCCH.
        this->_setN(2);
        this->_setK(5);
        this->_setLength(224);
        this->_setRGen(0);
        this->_setGen({023, 033});
        this->_setPuncture({});
        this->_setTerminationType("Flush");
    }

    ~GenericConvolution() {}

    void setN(int n)
    {
        Poco::FastMutex::ScopedLock lock(_convCodeMutex);

        if((n < 2) || (n > 4))
        {
            throw Pothos::InvalidArgumentException("N must be in range [2,4]");
        }

        int oldN = 0;
        try
        {
            this->_setN(n, &oldN);
            this->_getEncodeSize();
        }
        catch(const Pothos::Exception& ex)
        {
            this->_setN(oldN);
            this->_getEncodeSize();
            throw;
        }

        this->emitSignal("NChanged", n);
    }

    void setK(int k)
    {
        Poco::FastMutex::ScopedLock lock(_convCodeMutex);

        if((k != 5) && (k != 7))
        {
            throw Pothos::InvalidArgumentException("K must be 5 or 7");
        }

        int oldK = 0;
        try
        {
            this->_setK(k, &oldK);
            this->_getEncodeSize();
        }
        catch(const Pothos::Exception& ex)
        {
            this->_setK(oldK);
            this->_getEncodeSize();
            throw;
        }

        this->emitSignal("KChanged", k);
    }

    void setLength(int length)
    {
        Poco::FastMutex::ScopedLock lock(_convCodeMutex);

        if(length < 1)
        {
            throw Pothos::InvalidArgumentException("Length must be positive");
        }

        int oldLength = 0;
        try
        {
            this->_setLength(length, &oldLength);
            this->_getEncodeSize();
        }
        catch(const Pothos::Exception& ex)
        {
            this->_setLength(oldLength);
            this->_getEncodeSize();
            throw;
        }

        this->emitSignal("lengthChanged", length);
    }

    void setRGen(unsigned rgen)
    {
        Poco::FastMutex::ScopedLock lock(_convCodeMutex);

        if(rgen > 0)
        {
            if(::CONV_TERM_TAIL_BITING == _pConvCode->term)
            {
                throw Pothos::InvalidArgumentException(
                          "Cannot set RGen to a positive value "
                          "when termination is set to Tail-biting.");
            }
            else if(isVectorEmptyOrZeros(this->_gen()))
            {
                throw Pothos::InvalidArgumentException(
                          "Cannot set RGen to a positive value "
                          "when gen is empty or zero-only.");
            }
        }

        unsigned oldRGen = 0;
        try
        {
            this->_setRGen(rgen, &oldRGen);
            this->_getEncodeSize();
        }
        catch(const Pothos::Exception& ex)
        {
            this->_setRGen(oldRGen);
            this->_getEncodeSize();
            throw;
        }

        this->emitSignal("rgenChanged", rgen);
    }

    void setGen(const std::vector<unsigned>& gen)
    {
        Poco::FastMutex::ScopedLock lock(_convCodeMutex);

        if(gen.size() > 4)
        {
            throw Pothos::InvalidArgumentException("Gen must be of size 0-4");
        }
        else if(isVectorEmptyOrZeros(gen) && (_pConvCode->rgen > 0))
        {
            throw Pothos::InvalidArgumentException(
                      "Cannot set gen to an empty or all-zeros "
                      "value when RGen is positive.");
        }

        std::vector<unsigned> oldGen;
        try
        {
            this->_setGen(gen, &oldGen);
            this->_getEncodeSize();
        }
        catch(const Pothos::Exception& ex)
        {
            this->_setGen(oldGen);
            this->_getEncodeSize();
            throw;
        }

        this->emitSignal("genChanged", gen);
    }

    void setPuncture(const std::vector<int>& puncture)
    {
        Poco::FastMutex::ScopedLock lock(_convCodeMutex);

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

        std::vector<int> oldPuncture;
        try
        {
            this->_setPuncture(puncture, &oldPuncture);
            this->_getEncodeSize();
        }
        catch(const Pothos::Exception& ex)
        {
            this->_setPuncture(oldPuncture);
            this->_getEncodeSize();
            throw;
        }

        this->emitSignal("punctureChanged", puncture);
    }

    void setTerminationType(const std::string& terminationType)
    {
        Poco::FastMutex::ScopedLock lock(_convCodeMutex);

        if(("Tail-biting" == terminationType) && (_pConvCode->rgen > 0))
        {
            throw Pothos::InvalidArgumentException(
                      "Cannot set termination to Tail-biting"
                      "when RGen is positive.");
        }

        std::string oldTerminationType;
        try
        {
            this->_setTerminationType(terminationType, &oldTerminationType);
            this->_getEncodeSize();
        }
        catch(const Pothos::Exception& ex)
        {
            this->_setTerminationType(oldTerminationType);
            this->_getEncodeSize();
            throw;
        }

        this->emitSignal("terminationTypeChanged", terminationType);
    }

private:
    lte_conv_code _convCode; // For base class to point to
    std::vector<int> _puncture; // For _convCode's pointer to point to

    inline void _setN(int N, int* oldNOut = nullptr)
    {
        if(oldNOut) *oldNOut = _pConvCode->n;
        _pConvCode->n = N;
    }

    inline void _setK(int K, int* oldKOut = nullptr)
    {
        if(oldKOut) *oldKOut = _pConvCode->k;
        _pConvCode->k = K;
    }

    inline void _setLength(int length, int* oldLengthOut = nullptr)
    {
        if(oldLengthOut) *oldLengthOut = _pConvCode->len;
        _pConvCode->len = length;
    }

    inline void _setRGen(unsigned rgen, unsigned* oldRGenOut = nullptr)
    {
        if(oldRGenOut) *oldRGenOut = _pConvCode->rgen;
        _pConvCode->rgen = rgen;
    }

    void _setGen(
        const std::vector<unsigned>& gen,
        std::vector<unsigned>* oldGenOut = nullptr)
    {
        if(oldGenOut) *oldGenOut = this->_gen();

        // This is stored as a static array of size 4.
        std::memset(_pConvCode->gen, 0, sizeof(_pConvCode->gen));
        if(!gen.empty())
        {
            std::memcpy(
                _pConvCode->gen,
                gen.data(),
                (gen.size() * sizeof(unsigned)));
        }

        _genArrLength = gen.size();
    }

    void _setPuncture(
        const std::vector<int>& puncture,
        std::vector<int>* oldPunctureOut = nullptr)
    {
        if(oldPunctureOut) *oldPunctureOut = this->_punctureFunc();

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

    void _setTerminationType(
        const std::string& terminationType,
        std::string* oldTerminationTypeOut = nullptr)
    {
        if(oldTerminationTypeOut) *oldTerminationTypeOut = this->_terminationType();

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
};

/*
 * |PothosDoc Generic Convolution Encoder
 *
 * |category /FEC/Convolution
 * |keywords N K gen recursive termination gsm lte
 * |factory /fec/generic_conv_encoder()
 * |setter setN(N)
 * |setter setK(K)
 * |setter setLength(length)
 * |setter setRGen(rgen)
 * |setter setGen(gen)
 * |setter setPuncture(puncture)
 * |setter setTerminationType(terminationType)
 *
 * |param N[Rate] 2, 3, 4 (corresponding to 1/2, 1/3, 1/4)
 * |widget SpinBox(minimum=2,maximum=4)
 * |default 2
 * |preview enable
 *
 * |param K[Constraint Length] 5 or 7
 * |widget ComboBox(editable=False)
 * |option 5
 * |option 7
 * |default 5
 * |preview enable
 *
 * |param length[Length] Length of blocks to convolve
 * |widget SpinBox(minimum=1)
 * |default 224
 * |preview enable
 *
 * |param rgen[RGen] Recursive generator polynomial
 * |widget SpinBox(minimum=0,base=8)
 * |default 0
 * |preview enable
 *
 * |param gen[Gen] Generator polynomial (length 0-4)
 * |widget LineEdit()
 * |default [0o23,0o33]
 * |preview enable
 *
 * |param puncture[Puncture] Note: all values must be positive.
 * |widget LineEdit()
 * |default []
 * |preview enable
 *
 * |param terminationType[Termination Type]
 * |widget ComboBox(editable=False)
 * |option [Flush] "Flush"
 * |option [Tail-biting] "Tail-biting"
 * |default "Flush"
 * |preview enable
 */
static Pothos::BlockRegistry registerGenericConvolutionEncoder(
    "/fec/generic_conv_encoder",
    Pothos::Callable(&GenericConvolution::make)
        .bind(true, 0));

/*
 * |PothosDoc Generic Convolution Decoder
 *
 * |category /FEC/Convolution
 * |keywords N K gen recursive termination gsm lte
 * |factory /fec/generic_conv_decoder()
 * |setter setN(N)
 * |setter setK(K)
 * |setter setLength(length)
 * |setter setRGen(rgen)
 * |setter setGen(gen)
 * |setter setPuncture(puncture)
 * |setter setTerminationType(terminationType)
 *
 * |param N[Rate] 2, 3, 4 (corresponding to 1/2, 1/3, 1/4)
 * |widget SpinBox(minimum=2,maximum=4)
 * |default 2
 * |preview enable
 *
 * |param K[Constraint Length] 5 or 7
 * |widget ComboBox(editable=False)
 * |option 5
 * |option 7
 * |default 5
 * |preview enable
 *
 * |param length[Length] Length of blocks to convolve
 * |widget SpinBox(minimum=1)
 * |default 224
 * |preview enable
 *
 * |param rgen[RGen] Recursive generator polynomial
 * |widget SpinBox(minimum=0,base=8)
 * |default 0
 * |preview enable
 *
 * |param gen[Gen] Generator polynomial (length 0-4)
 * |widget LineEdit()
 * |default [0o23,0o33]
 * |preview enable
 *
 * |param puncture[Puncture] Note: all values must be positive.
 * |widget LineEdit()
 * |default []
 * |preview enable
 *
 * |param terminationType[Termination Type]
 * |widget ComboBox(editable=False)
 * |option [Flush] "Flush"
 * |option [Tail-biting] "Tail-biting"
 * |default "Flush"
 * |preview enable
 */
static Pothos::BlockRegistry registerGenericConvolutionDecoder(
    "/fec/generic_conv_decoder",
    Pothos::Callable(&GenericConvolution::make)
        .bind(false, 0));
