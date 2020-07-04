// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <Pothos/Util/ExceptionForErrorCode.hpp>

static inline void throwOnErrCode(int errCode)
{
    if(errCode < 0) throw Pothos::Util::ErrnoException<>(errCode);
}
