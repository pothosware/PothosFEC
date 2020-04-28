// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

extern "C"
{
#include "errnoname.h"
}

#include "Utility.hpp"

#include <Pothos/Exception.hpp>

#include <cstring>

// TODO: move this into PothosCore(?)
static std::string errnoName(int errCode)
{
    return errCode ? std::string(::errnoname(-errCode)) : 0;
}

void throwOnErrCode(int errCode)
{
    if(errCode < 0)
    {
        throw Pothos::RuntimeException(errnoName(errCode)+": "+std::strerror(-errCode));
    }
}
