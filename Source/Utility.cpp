// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Utility.hpp"

#include <Pothos/Exception.hpp>

#include <cstring>

void throwOnErrCode(int errCode)
{
    if(errCode)
    {
        throw Pothos::RuntimeException(std::strerror(errCode));
    }
}
