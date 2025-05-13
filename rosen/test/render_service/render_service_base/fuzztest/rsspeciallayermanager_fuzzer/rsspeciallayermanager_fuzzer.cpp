/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rsspeciallayermanager_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>

#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "common/rs_common_def.h"
#include "common/rs_special_layer_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size) {
        return object;
    }
    auto remainSize = g_size - g_pos;
    if (objectSize <= remainSize) {
        if (memcpy_s(&object, objectSize, g_data + g_pos, objectSize) != EOK) {
            return {};
        }
    } else {
        if (memcpy_s(&object, remainSize, g_data + g_pos, remainSize) != EOK) {
            return {};
        }
        if (memcpy_s(reinterpret_cast<uint8_t*>(&object) + remainSize, objectSize - remainSize,
            g_data, objectSize - remainSize) != EOK) {
            return {};
        }
    }
    g_pos += objectSize;
    g_pos %= g_size;
    return object;
}

void AddTestFunction(std::vector<std::function<void(RSSpecialLayerManager&)>>& testFunctions,
    std::function<void(RSSpecialLayerManager&)> testFunction)
{
    constexpr uint8_t maxRunTimes{10};
    auto runTimes = (GetData<uint8_t>() % maxRunTimes);
    for (uint8_t i = 0; i <= runTimes; ++i) {
        testFunctions.emplace_back(testFunction);
    }
}
} // namespace

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // prepare test functions
    std::vector<std::function<void(RSSpecialLayerManager&)>> testFunctions;
    AddTestFunction(testFunctions, [](RSSpecialLayerManager& rsSpecialLayerManager){
        rsSpecialLayerManager.Set(GetData<uint32_t>(), GetData<bool>());
    });
    AddTestFunction(testFunctions, [](RSSpecialLayerManager& rsSpecialLayerManager){
        rsSpecialLayerManager.Find(GetData<uint32_t>());
    });
    AddTestFunction(testFunctions, [](RSSpecialLayerManager& rsSpecialLayerManager){
        auto type = GetData<uint32_t>();
        rsSpecialLayerManager.Set(type, GetData<bool>());
        rsSpecialLayerManager.Find(type);
    });
    AddTestFunction(testFunctions, [](RSSpecialLayerManager& rsSpecialLayerManager){
        rsSpecialLayerManager.Get();
    });
    AddTestFunction(testFunctions, [](RSSpecialLayerManager& rsSpecialLayerManager){
        rsSpecialLayerManager.AddIds(GetData<uint32_t>(), GetData<NodeId>());
    });
    AddTestFunction(testFunctions, [](RSSpecialLayerManager& rsSpecialLayerManager){
        rsSpecialLayerManager.RemoveIds(GetData<uint32_t>(), GetData<NodeId>());
    });
    AddTestFunction(testFunctions, [](RSSpecialLayerManager& rsSpecialLayerManager){
        auto type = GetData<uint32_t>();
        auto id = GetData<NodeId>();
        rsSpecialLayerManager.AddIds(type, id);
        rsSpecialLayerManager.RemoveIds(type, id);
    });

    // run test functions
    std::mt19937 g(GetData<uint_fast32_t>());
    std::shuffle(testFunctions.begin(), testFunctions.end(), g);

    RSSpecialLayerManager rsSpecialLayerManager;
    for (const auto& func : testFunctions) {
        func(rsSpecialLayerManager);
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
