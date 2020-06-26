// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Utility.hpp"

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>
#include <Pothos/Plugin.hpp>

#include <aff3ct.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class AZCWEncoder: public Pothos::Block
{
    using Class = AZCWEncoder<T>;

    public:
        AZCWEncoder(size_t K, size_t N, size_t dimension);
        virtual ~AZCWEncoder() = default;

        size_t K() const;

        size_t N() const;

        void work() override;

    private:
        std::unique_ptr<aff3ct::module::Encoder_AZCW<T>> _azcwEncoderUPtr;
};

template <typename T>
AZCWEncoder<T>::AZCWEncoder(size_t K, size_t N, size_t dimension): Pothos::Block()
{
    //
    // Input validation
    //

    if(0 == K) throw Pothos::RangeException("K must be > 0");
    if(0 == N) throw Pothos::RangeException("N must be > 0");

    //
    // Block setup
    //

    _azcwEncoderUPtr.reset(new aff3ct::module::Encoder_AZCW<T>(
         static_cast<int>(K),
         static_cast<int>(N)));

    this->setupInput(0, Pothos::DType(typeid(T), dimension));
    this->setupOutput(0, Pothos::DType(typeid(T), dimension));

    this->registerCall(this, POTHOS_FCN_TUPLE(Class, K));
    this->registerCall(this, POTHOS_FCN_TUPLE(Class, N));

    this->registerProbe("K");
    this->registerProbe("N");
}

template <typename T>
size_t AZCWEncoder<T>::K() const
{
    return static_cast<size_t>(_azcwEncoderUPtr->get_K());
}

template <typename T>
size_t AZCWEncoder<T>::N() const
{
    return static_cast<size_t>(_azcwEncoderUPtr->get_N());
}

template <typename T>
void AZCWEncoder<T>::work()
{
    const auto elems = this->workInfo().minElements;
    if(0 == elems) return;

    auto input = this->input(0);
    auto output = this->output(0);

    const auto inputLen = this->K();
    const auto outputLen = this->N();

    const auto maxInputFrames = elems / inputLen;
    const auto maxOutputFrames = elems / outputLen;
    const auto numFrames = std::min(maxInputFrames, maxOutputFrames);

    const T* buffIn = input->buffer();
    T* buffOut = output->buffer();

    for(size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex)
    {
        _azcwEncoderUPtr->encode(buffIn, buffOut);

        buffIn += inputLen;
        buffOut += outputLen;
    }

    input->consume(numFrames * inputLen);
    output->produce(numFrames * outputLen);
}

// TODO: how to handle AFF3CT_MULTI_PREC types?

/*
 * |PothosDoc AZCW Encoder
 *
 * |category /FEC/Encoders
 * |category Testers
 * |keywords N K
 * |factory /fec/azcw_encoder(dtype,K,N)
 *
 * |param dtype[Data Type] The block data type.
 * |widget DTypeChooser(int8=1,int16=1,int32=1,int64=1)
 * |default "int8"
 * |preview disable
 *
 * |param K
 * |widget SpinBox(minimum=2)
 * |default 2
 * |preview enable
 *
 * |param N
 * |widget SpinBox(minimum=2)
 * |default 3
 * |preview enable
 */
static Pothos::Block* makeAZCWEncoder(
    const Pothos::DType& dtype,
    size_t K,
    size_t N)
{
#define IfTypeThenReturn(T) \
    if(Pothos::DType::fromDType(dtype, 1) == Pothos::DType(typeid(T))) \
        return new AZCWEncoder<T>(K, N, dtype.dimension());

#ifdef AFF3CT_MULTI_PREC
    IfTypeThenReturn(B_8)
    IfTypeThenReturn(B_16)
    IfTypeThenReturn(B_32)
    IfTypeThenReturn(B_64)
#else
    IfTypeThenReturn(B)
#endif

    throw Pothos::InvalidArgumentException("AZCWEncoder: invalid dtype: "+dtype.name());
}

static Pothos::BlockRegistry registerAZCWEncoder(
    "/fec/azcw_encoder",
    Pothos::Callable(&makeAZCWEncoder));
