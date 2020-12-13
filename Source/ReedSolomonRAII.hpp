// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

extern "C"
{
#include "libfec/fec.h"
}

#include <cassert>
#include <memory>

// Default values (taken from GNU Radio)
static constexpr int DefaultSymSize = 8;
static constexpr int DefaultGFPoly = 0x11D;
static constexpr int DefaultFCR = 0;
static constexpr int DefaultPrim = 1;
static constexpr int DefaultPad = 0;
static constexpr int DefaultN = 255;
static constexpr int DefaultK = 239;

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
            if(_rs) _freeFcn(_rs);
        }

        inline void encode(
            T* data,
            T* parity) const
        {
            assert(_rs);
            _encodeFcn(_rs, data, parity);
        }

        inline int decode(
            T* data,
            int* erasPos,
            int noEras) const
        {
            assert(_rs);
            return _decodeFcn(_rs, data, erasPos, noEras);
        }

        inline int symSize() const
        {
            return _symSize;
        }

        inline int gfPoly() const
        {
            return _gfPoly;
        }

        inline int fcr() const
        {
            return _fcr;
        }

        inline int prim() const
        {
            return _prim;
        }

        inline int nRoots() const
        {
            return _nroots;
        }

        inline int pad() const
        {
            return _pad;
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
