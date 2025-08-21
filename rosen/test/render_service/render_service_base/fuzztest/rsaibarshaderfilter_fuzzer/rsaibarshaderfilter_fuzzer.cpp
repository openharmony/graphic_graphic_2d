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

#include "rsaibarshaderfilter_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iostream>
#include <securec.h>
#include <unistd.h>

#include "draw/canvas.h"
#include "ge_render.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_render_aibar_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
int g_number = 6;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

/*
* get a string from DATA
*/
std::string GetStringFromData(int strlen)
{
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        cstr[i] = GetData<char>();
    }
    std::string str(cstr);
    return str;
}

bool DoGenerateGEVisualEffect()
{
    auto geContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    std::string name = GetStringFromData(5); // Get a string of length 5
    Drawing::DrawingPaintType type = GetData<Drawing::DrawingPaintType>();
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    geContainer->AddToChainedFilter(visualEffect);
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    rsAIBarShaderFilter->GenerateGEVisualEffect(geContainer);
    return true;
}

bool DoIsAiInvertCoefValid()
{
    std::vector<float> aiInvertCoef;
    aiInvertCoef.reserve(g_number);
    for (int i = 0; i < g_number; i++) {
        float value = GetData<float>();
        aiInvertCoef.push_back(value);
    }
    RSAIBarShaderFilter::IsAiInvertCoefValid(aiInvertCoef);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return 0;
    }
    // initialize
    OHOS::Rosen::DATA = data;
    OHOS::Rosen::g_size = size;
    OHOS::Rosen::g_pos = 0;
    
    /* Run your code on data */
    OHOS::Rosen::DoGenerateGEVisualEffect();
    OHOS::Rosen::DoIsAiInvertCoefValid();
    return 0;
}
