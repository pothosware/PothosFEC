// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include <Pothos/Exception.hpp>
#include <Pothos/Framework.hpp>

extern "C"
{
#include <turbofec/conv.h>
}

class LTEConvEncoderBase: public Pothos::Block
{
public:
    LTEConvEncoderBase():
        Pothos::Block(),
        _pLTEConvCode(nullptr)
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

        if(!_pLTEConvCode)
        {
            throw Pothos::AssertionViolationException("LTEConvEncoderBase::work() called without setting _pLTEConvCode");
        }

        auto* input = this->input(0);
        auto* output = this->output(0);

        _pLTEConvCode->len = static_cast<int>(elems);
        ::lte_conv_encode(
            _pLTEConvCode,
            input->buffer(),
            output->buffer());

        input->consume(elems);
        output->produce(elems);
    }

protected:
    lte_conv_code* _pLTEConvCode;
};
