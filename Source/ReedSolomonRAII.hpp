// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

extern "C"
{
#include "libfec/fec.h"
}

#include <Pothos/Framework.hpp>

#include <Poco/Mutex.h>

#include <memory>

template <typename T>
class ReedSolomonRAII
{
    public:
        using UPtr = std::unique_ptr<ReedSolomonRAII>;
        using InitFcn = void*(*)(int, int, int, int, int, int);
        using EncodeFcn = void(*)(void*, T*, T*);
        using DecodeFcn = int(*)(void*, T*, int*, int);
        using FreeFcn = void(*)(void*);

        static inline UPtr makeUPtr(
            int symSize,
            int gfPoly,
            int fcr,
            int prim,
            int nroots,
            int pad)
        {
            return UPtr(new ReedSolomonRAII<T>(symSize, gfPoly, fcr, prim, nroots, pad));
        }

        ReedSolomonRAII(
            int symSize,
            int gfPoly,
            int fcr,
            int prim,
            int nroots,
            int pad);
        ReedSolomonRAII() = delete;
        ReedSolomonRAII(const ReedSolomonRAII&) = delete;
        ReedSolomonRAII(ReedSolomonRAII&&) = default;
        ReedSolomonRAII& operator=(const ReedSolomonRAII&) = delete;
        ReedSolomonRAII& operator=(ReedSolomonRAII&&) = default;

        virtual ~ReedSolomonRAII()
        {
            _freeFcn(_rs);
        }

        inline void encode(
            T* data,
            T* parity)
        {
            _encodeFcn(_rs, data, parity);
        }

        inline int decode(
            T* data,
            int* erasPos,
            int noEras)
        {
            return _decodeFcn(_rs, data, erasPos, noEras);
        }

    private:
        int _symSize;
        int _gfPoly;
        int _fcr;
        int _prim;
        int _nroots;
        int _pad;

        InitFcn _initFcn;
        EncodeFcn _encodeFcn;
        DecodeFcn _decodeFcn;
        FreeFcn _freeFcn;

        void* _rs;
};

template <>
ReedSolomonRAII<unsigned char>::ReedSolomonRAII(
    int symSize,
    int gfPoly,
    int fcr,
    int prim,
    int nroots,
    int pad
):
    _symSize(symSize),
    _gfPoly(gfPoly),
    _fcr(fcr),
    _prim(prim),
    _nroots(nroots),
    _pad(pad),
    _initFcn(::init_rs_char),
    _encodeFcn(::encode_rs_char),
    _decodeFcn(::decode_rs_char),
    _freeFcn(::free_rs_char),
    _rs(_initFcn(_symSize, _gfPoly, _fcr, _prim, _nroots, _pad))
{
}

template <>
ReedSolomonRAII<int>::ReedSolomonRAII(
    int symSize,
    int gfPoly,
    int fcr,
    int prim,
    int nroots,
    int pad
):
    _symSize(symSize),
    _gfPoly(gfPoly),
    _fcr(fcr),
    _prim(prim),
    _nroots(nroots),
    _pad(pad),
    _initFcn(::init_rs_int),
    _encodeFcn(::encode_rs_int),
    _decodeFcn(::decode_rs_int),
    _freeFcn(::free_rs_int),
    _rs(_initFcn(_symSize, _gfPoly, _fcr, _prim, _nroots, _pad))
{
}
