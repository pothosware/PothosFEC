// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <Pothos/Framework.hpp>

extern "C"
{
#include <turbofec/conv.h>
}

#include <string>
#include <vector>

class ConvolutionalEncoderBase: public Pothos::Block
{
public:
    ConvolutionalEncoderBase(lte_conv_code* pConvCode);

    int N() const;

    int K() const;

    int length() const;

    unsigned recursiveGeneratorPolynomial() const;

    std::vector<unsigned> generatorPolynomial() const;

    std::vector<int> puncture() const;

    std::string terminationType() const;

    void work() override;

protected:
    lte_conv_code* _pConvCode;
    size_t _genArrLength;
};
