// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TestUtility.hpp"

#include <nlohmann/json.hpp>

#include <Pothos/Plugin.hpp>
#include <Pothos/Testing.hpp>

#include <string>

using namespace FECTests;

POTHOS_TEST_BLOCK("/fec/tests", test_module_info)
{
    const auto moduleInfo = getAndCallPlugin<std::string>("/devices/fec/info");
    POTHOS_TEST_FALSE(moduleInfo.empty());

    // Make sure this is valid JSON. This will throw if the JSON is invalid.
    nlohmann::json json(moduleInfo);
}
