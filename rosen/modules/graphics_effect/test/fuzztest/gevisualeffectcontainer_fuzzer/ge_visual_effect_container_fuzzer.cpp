/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


#include "ge_visual_effect_container_fuzzer.h"
#include "get_object.h"
#include "ge_render.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

bool GEVisualEffectContainerFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    auto veContainer = std::make_shared<GEVisualEffectContainer>();
    int32_t nameLen = GETest::GetPlainData<int32_t>();
    std::string name = GETest::GetStringFromData(nameLen);
    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto visualEffect = std::make_shared<GEVisualEffect>(name, type);
    veContainer->AddToChainedFilter(visualEffect);
    veContainer->GetFilters();
    return true;
}

bool GEVisualEffectContainerFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    auto veContainer = std::make_shared<GEVisualEffectContainer>();
    auto visualEffect = nullptr;
    veContainer->AddToChainedFilter(visualEffect);
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest001(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest002(data, size);
    return 0;
}
