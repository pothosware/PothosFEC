// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "ConvolutionBase.hpp"
#include "Utility.hpp"

class GenericConvolutionBase: public ConvolutionBase
{
public:
    GenericConvolutionBase();

    virtual ~GenericConvolutionBase();

    void setN(int n);

    void setK(int k);

    void setLength(int length);

    void setRecursiveGeneratorPolynomial(int rgen);

    void setGeneratorPolynomial(const std::vector<unsigned>& gen);

    void setPuncture(const std::vector<int>& puncture);

    void setTerminationType(const std::string& terminationType);

private:
    lte_conv_code _convCode; // For the base class to point at

    std::vector<int> _puncture;
};
