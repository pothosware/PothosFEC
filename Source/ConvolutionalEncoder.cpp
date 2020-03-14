// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Utility.hpp"

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>

extern "C"
{
#include <turbofec/conv.h>
}

class ConvolutionalEncoderBase: public Pothos::Block
{
public:
    ConvolutionalEncoderBase():
        Pothos::Block(),
        _pConvCode(nullptr)
    {
        this->setupInput(0, "uint8");
        this->setupOutput(0, "uint8");
    }

    void work() override
    {
        if(!_pConvCode)
        {
            throw Pothos::AssertionViolationException("ConvolutionalEncoderBase::work() called without setting _pConvCode");
        }

        const auto elems = this->workInfo().minElements;
        if(elems < static_cast<size_t>(_pConvCode->len))
        {
            return;
        }

        auto* input = this->input(0);
        auto* output = this->output(0);

        int encodeRet = ::lte_conv_encode(
                            _pConvCode,
                            input->buffer(),
                            output->buffer());
        throwOnErrCode(encodeRet);

        if(encodeRet > 0)
        {
            input->consume(_pConvCode->len);
            output->produce(encodeRet);
        }
    }

protected:
    lte_conv_code* _pConvCode;
};
