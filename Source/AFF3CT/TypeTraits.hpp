// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <aff3ct.hpp>

template <typename T>
struct AFF3CTTypeTraits
{
};

template<>
struct AFF3CTTypeTraits<B_8>
{
    using R = R_8;
    using Q = Q_8;
};

template<>
struct AFF3CTTypeTraits<B_16>
{
    using R = R_16;
    using Q = Q_16;
};

template<>
struct AFF3CTTypeTraits<B_32>
{
    using R = R_32;
    using Q = Q_32;
};

template<>
struct AFF3CTTypeTraits<B_64>
{
    using R = R_64;
    using Q = Q_64;
};
