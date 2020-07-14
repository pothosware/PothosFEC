// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <Pothos/Proxy.hpp>

namespace FECTests
{
    void testCoderOutputLength(
        const Pothos::Proxy& coder,
        size_t numInputElems,
        size_t expectedNumOutputElems);

    void testCoderSymmetry(
        const Pothos::Proxy& encoder,
        const Pothos::Proxy& decoder,
        size_t numElems);
}
