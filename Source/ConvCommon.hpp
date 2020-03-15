// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <unordered_map>

extern "C"
{
#include <turbofec/conv.h>
}

const std::unordered_map<std::string, const lte_conv_code*>& getConvCodeMap();

const std::unordered_map<std::string, size_t>& getGenArrLengthsMap();

std::string convCodeMapOverlay();

// TODO: conv code field compatibility function
