// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

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
        const auto elems = this->workInfo().minElements;
        if(0 == elems)
        {
            return;
        }

        if(!_pConvCode)
        {
            throw Pothos::AssertionViolationException("ConvolutionalEncoderBase::work() called without setting _pConvCode");
        }

        auto* input = this->input(0);
        auto* output = this->output(0);

        _pConvCode->len = static_cast<int>(elems);
        ::lte_conv_encode(
            _pConvCode,
            input->buffer(),
            output->buffer());

        input->consume(elems);
        output->produce(elems);
    }

protected:
    lte_conv_code* _pConvCode;
};
