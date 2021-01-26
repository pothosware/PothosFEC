// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <aff3ct.hpp>

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>
#include <Pothos/Object/Containers.hpp>

#include <Pothos/Util/TypeInfo.hpp>
#include <Pothos/Util/ExceptionForErrorCode.hpp>

#include <Poco/Format.h>

#include <string>

template <typename SrcType, typename DstType>
static DstType* safeDynamicCast(SrcType* src)
{
    auto* dst = dynamic_cast<DstType*>(src);
    if(!dst)
    {
        throw Pothos::AssertionViolationException(
                  Poco::format(
                      "Cannot dynamic_cast %s to %s",
                      Pothos::Util::typeInfoToString(typeid(SrcType)),
                      Pothos::Util::typeInfoToString(typeid(DstType))));
    }

    return dst;
}

template <typename SrcType, typename DstType>
static inline const DstType* safeDynamicCast(const SrcType* src)
{
    return safeDynamicCast(const_cast<SrcType*>(src));
}

static inline void throwOnErrCode(int errCode)
{
    if(errCode < 0) throw Pothos::Util::ErrnoException<>(errCode);
}

template <typename T>
static inline bool doesDTypeMatch(const Pothos::DType& dtype)
{
    return (Pothos::DType::fromDType(dtype, 1) == Pothos::DType(typeid(T)));
}
