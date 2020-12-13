// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ReedSolomonRAII.hpp"
#include "Utility.hpp"

#include <Pothos/Framework.hpp>

#include <cstring>

//
// Interface
//

template <typename T>
class ReedSolomonBase: public Pothos::Block
{
    public:
        using BaseClass = ReedSolomonBase<T>;
        using RAII = ReedSolomonRAII<T>;

        ReedSolomonBase();
        virtual ~ReedSolomonBase() = default;

        int symbolSize() const;
        int gfPoly() const;
        int FCR() const;
        int primitive() const;
        int N() const;
        int K() const;

        void setSymbolSize(int symbolSize);
        void setGFPoly(int gfPoly);
        void setFCR(int fcr);
        void setPrimitive(int primitive);
        void setN(int N);
        void setK(int K);

        // To force using a subclass
        virtual void work() = 0;

    protected:
        void _resetRSUPtr(
            int symSize,
            int gfPoly,
            int fcr,
            int prim,
            int nroots);

        int _N;
        int _K;
        typename RAII::UPtr _rsUPtr;
};

template <typename T>
class ReedSolomonEncoder: public ReedSolomonBase<T>
{
    public:
        ReedSolomonEncoder(): ReedSolomonBase<T>(){};
        virtual ~ReedSolomonEncoder() = default;

        void work() override;
};

template <typename T>
class ReedSolomonDecoder: public ReedSolomonBase<T>
{
    public:
        ReedSolomonDecoder(): ReedSolomonBase<T>(){};
        virtual ~ReedSolomonDecoder() = default;

        void work() override;
};

//
// Implementation
//

#define ASSERT_PTR(ptr) if(!ptr) throw Pothos::AssertionViolationException("Null " #ptr);

template <typename T>
ReedSolomonBase<T>::ReedSolomonBase():
    _N(DefaultN),
    _K(DefaultK),
    _rsUPtr(RAII::makeUPtr(
        DefaultSymSize,
        DefaultGFPoly,
        DefaultFCR,
        DefaultPrim,
        (_N - _K),
        DefaultPad))
{
    ASSERT_PTR(_rsUPtr);

    static const auto dtype = Pothos::DType(typeid(T));

    this->setupInput(0, dtype);
    this->setupOutput(0, dtype);

    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, symbolSize));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, gfPoly));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, FCR));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, primitive));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, N));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, K));

    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, setSymbolSize));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, setGFPoly));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, setFCR));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, setPrimitive));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, setN));
    this->registerCall(this, POTHOS_FCN_TUPLE(BaseClass, setK));
}

template <typename T>
int ReedSolomonBase<T>::symbolSize() const
{
    ASSERT_PTR(_rsUPtr);
    return _rsUPtr->symSize();
}

template <typename T>
int ReedSolomonBase<T>::gfPoly() const
{
    ASSERT_PTR(_rsUPtr);
    return _rsUPtr->gfPoly();
}

template <typename T>
int ReedSolomonBase<T>::FCR() const
{
    ASSERT_PTR(_rsUPtr);
    return _rsUPtr->fcr();
}

template <typename T>
int ReedSolomonBase<T>::primitive() const
{
    ASSERT_PTR(_rsUPtr);
    return _rsUPtr->prim();
}

template <typename T>
int ReedSolomonBase<T>::N() const
{
    return _N;
}

template <typename T>
int ReedSolomonBase<T>::K() const
{
    return _K;
}

template <typename T>
void ReedSolomonBase<T>::setSymbolSize(int symbolSize)
{
    ASSERT_PTR(_rsUPtr);
    _resetRSUPtr(
        symbolSize,
        _rsUPtr->gfPoly(),
        _rsUPtr->fcr(),
        _rsUPtr->prim(),
        (_N - _K));
}

template <typename T>
void ReedSolomonBase<T>::setGFPoly(int gfPoly)
{
    ASSERT_PTR(_rsUPtr);
    _resetRSUPtr(
        _rsUPtr->symSize(),
        gfPoly,
        _rsUPtr->fcr(),
        _rsUPtr->prim(),
        (_N - _K));
}

template <typename T>
void ReedSolomonBase<T>::setFCR(int fcr)
{
    ASSERT_PTR(_rsUPtr);
    _resetRSUPtr(
        _rsUPtr->symSize(),
        _rsUPtr->gfPoly(),
        fcr,
        _rsUPtr->prim(),
        (_N - _K));
}

template <typename T>
void ReedSolomonBase<T>::setPrimitive(int primitive)
{
    ASSERT_PTR(_rsUPtr);
    _resetRSUPtr(
        _rsUPtr->symSize(),
        _rsUPtr->gfPoly(),
        _rsUPtr->fcr(),
        primitive,
        (_N - _K));
}

template <typename T>
void ReedSolomonBase<T>::setN(int N)
{
    _N = N;
    _resetRSUPtr(
        _rsUPtr->symSize(),
        _rsUPtr->gfPoly(),
        _rsUPtr->fcr(),
        _rsUPtr->prim(),
        (_N - _K));
}

template <typename T>
void ReedSolomonBase<T>::setK(int K)
{
    _K = K;
    _resetRSUPtr(
        _rsUPtr->symSize(),
        _rsUPtr->gfPoly(),
        _rsUPtr->fcr(),
        _rsUPtr->prim(),
        (_N - _K));
}

template <typename T>
void ReedSolomonBase<T>::_resetRSUPtr(
    int symSize,
    int gfPoly,
    int fcr,
    int prim,
    int nroots)
{
    auto newUPtr = RAII::makeUPtr(
                       symSize,
                       gfPoly,
                       fcr,
                       prim,
                       nroots,
                       0);
    ASSERT_PTR(newUPtr);

    _rsUPtr = std::move(newUPtr);
}

template <typename T>
void ReedSolomonEncoder<T>::work()
{
    const auto& workInfo = this->workInfo();

    if((workInfo.minInElements < size_t(this->_K)) ||
       (workInfo.minOutElements < size_t(this->_N))) return;

    auto input = this->input(0);
    auto output = this->output(0);

    const auto numInputBlocks = input->elements() / this->_K;
    const auto numOutputBlocks = output->elements() / this->_N;
    const auto numBlocks = std::min(numInputBlocks, numOutputBlocks);

    const T* buffIn = input->buffer();
    T* buffOut = output->buffer();

    for(size_t blockIndex = 0;
        blockIndex < numBlocks;
        ++blockIndex, buffIn += this->_K, buffOut += this->_N)
    {
        // LibFEC functions operate in-place.
        std::memcpy(buffOut, buffIn, (this->_N * sizeof(T)));
        this->_rsUPtr->encode(buffOut, &buffOut[this->_K]);
    }

    input->consume(numBlocks * this->_K);
    output->produce(numBlocks * this->_N);
}

template <typename T>
void ReedSolomonDecoder<T>::work()
{
    const auto& workInfo = this->workInfo();

    if((workInfo.minInElements < size_t(this->_N)) ||
       (workInfo.minOutElements < size_t(this->_K))) return;

    auto input = this->input(0);
    auto output = this->output(0);

    const auto numInputBlocks = input->elements() / this->_N;
    const auto numOutputBlocks = output->elements() / this->_K;
    const auto numBlocks = std::min(numInputBlocks, numOutputBlocks);

    const T* buffIn = input->buffer();
    T* buffOut = output->buffer();

    for(size_t blockIndex = 0;
        blockIndex < numBlocks;
        ++blockIndex, buffIn += this->_N, buffOut += this->_K)
    {
        // LibFEC functions operate in-place.
        std::memcpy(buffOut, buffIn, (this->_K * sizeof(T)));
        this->_rsUPtr->decode(buffOut, nullptr, 0); // Don't store erasure info
    }

    input->consume(numBlocks * this->_N);
    output->produce(numBlocks * this->_K);
}

//
// Registration
//

static Pothos::Block* makeEncoder(const Pothos::DType& dtype)
{
    #define IfTypeThenEncoder(T) \
        if(Pothos::DType::fromDType(dtype, 1) == Pothos::DType(typeid(T))) \
            return new ReedSolomonEncoder<T>();

    IfTypeThenEncoder(unsigned char)
    IfTypeThenEncoder(int)

    throw Pothos::InvalidArgumentException("Invalid type: "+dtype.name());
}

static Pothos::Block* makeDecoder(const Pothos::DType& dtype)
{
    #define IfTypeThenDecoder(T) \
        if(Pothos::DType::fromDType(dtype, 1) == Pothos::DType(typeid(T))) \
            return new ReedSolomonDecoder<T>();

    IfTypeThenDecoder(unsigned char)
    IfTypeThenDecoder(int)

    throw Pothos::InvalidArgumentException("Invalid type: "+dtype.name());
}

static Pothos::BlockRegistry registerEncoder(
    "/fec/reedsolomon_encoder",
    &makeEncoder);

static Pothos::BlockRegistry registerDecoder(
    "/fec/reedsolomon_decoder",
    &makeDecoder);
