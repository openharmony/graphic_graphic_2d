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

#include "rsaibarfilter_fuzzer.h"

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
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_aibar_filter.h"
namespace OHOS {
namespace Rosen {
using namespace Drawing;
auto rsAIBarFilter = std::make_shared<RSAIBarFilter>();

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
int g_number = 6;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoCompose(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSDrawingFilterOriginal> blurFilter = nullptr;
    rsAIBarFilter->Compose(blurFilter);
    return true;
}

bool DoPreProcess(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto image = std::make_shared<Image>();
    rsAIBarFilter->PreProcess(image);
    return true;
}
bool DoPostProcess(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    float alpha = GetData<float>();
    RSPaintFilterCanvas filterCanvas(&canvas, alpha);
    rsAIBarFilter->PostProcess(filterCanvas);
    return true;
}
bool DoOtherFunc(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    rsAIBarFilter->GetDescription();
    rsAIBarFilter->GetDetailedDescription();
    rsAIBarFilter->CanSkipFrame();
    RSAIBarFilter::GetAiInvertCoef();
    return true;
}
bool DoIsAiInvertCoefValid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<float> aiInvertCoef;
    aiInvertCoef.reserve(g_number);
    for (int i = 0; i < g_number; i++) {
        float value = GetData<float>();
        aiInvertCoef.push_back(value);
    }
    RSAIBarFilter::IsAiInvertCoefValid(aiInvertCoef);
    return true;
}
bool DoMakeBinarizationShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto imageShader = std::make_shared<ShaderEffect>();
    RSAIBarFilter::MakeBinarizationShader(imageShader);
    return true;
}
} // namespace Rosen
} // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCompose(data, size);
    OHOS::Rosen::DoPreProcess(data, size);
    OHOS::Rosen::DoPostProcess(data, size);
    OHOS::Rosen::DoOtherFunc(data, size);
    OHOS::Rosen::DoIsAiInvertCoefValid(data, size);
    OHOS::Rosen::DoMakeBinarizationShader(data, size);
    return 0;
}