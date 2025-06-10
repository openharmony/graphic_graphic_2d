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

#include "rslineargradientblurshaderfilter_fuzzer.h"

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
#include "render/rs_gradient_blur_para.h"
#include "render/rs_render_linear_gradient_blur_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
}

/*
* describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
* tips: only support basic type
*/
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

bool DoCreateRSLinearGradientBlurShaderFilter(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    float geoWidth = GetData<float>();
    float geoHeight = GetData<float>();
    float blurRadius = GetData<float>();
    float x1 = GetData<float>();
    float x2 = GetData<float>();
    float y1 = GetData<float>();
    float y2 = GetData<float>();
    std::vector<std::pair<float, float>> fractionStops = {
        std::make_pair(x1, x2), std::make_pair(y1, y2)
    };
    GradientDirection direction = GetData<GradientDirection>();
    auto para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, geoWidth, geoHeight);
    return true;
}

bool DoGenerateGEVisualEffect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    float geoWidth = GetData<float>();
    float geoHeight = GetData<float>();
    float blurRadius = GetData<float>();
    std::vector<std::pair<float, float>> fractionStops = {
        std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0)
    };
    GradientDirection direction = GetData<GradientDirection>();
    auto para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, geoWidth, geoHeight);
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    std::string name = GetStringFromData(0);
    Drawing::DrawingPaintType type = GetData<Drawing::DrawingPaintType>();
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    veContainer->AddToChainedFilter(visualEffect);
    filter->GenerateGEVisualEffect(veContainer);
    return true;
}

bool DoSetGeometry(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    float geoWidth = GetData<float>();
    float geoHeight = GetData<float>();
    float blurRadius = GetData<float>();
    std::vector<std::pair<float, float>> fractionStops = {
        std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0)
    };
    GradientDirection direction = GetData<GradientDirection>();
    auto para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, geoWidth, geoHeight);
    Drawing::Canvas canvas;
    filter->SetGeometry(canvas, geoWidth, geoHeight);
    return true;
}

bool DoIsOffscreenCanvas(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    float geoWidth = GetData<float>();
    float geoHeight = GetData<float>();
    float blurRadius = GetData<float>();
    float x1 = GetData<float>();
    float x2 = GetData<float>();
    float y1 = GetData<float>();
    float y2 = GetData<float>();
    std::vector<std::pair<float, float>> fractionStops = {
        std::make_pair(x1, x2), std::make_pair(y1, y2)
    };
    GradientDirection direction = GetData<GradientDirection>();
    auto para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, geoWidth, geoHeight);
    bool isoff = GetData<bool>();
    filter->IsOffscreenCanvas(isoff);
    return true;
}

float DoGetLinearGradientBlurRadius(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    float geoWidth = GetData<float>();
    float geoHeight = GetData<float>();
    float blurRadius = GetData<float>();
    float x1 = GetData<float>();
    float x2 = GetData<float>();
    float y1 = GetData<float>();
    float y2 = GetData<float>();
    std::vector<std::pair<float, float>> fractionStops = {
        std::make_pair(x1, x2), std::make_pair(y1, y2)
    };
    GradientDirection direction = GetData<GradientDirection>();
    auto para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    const auto filter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, geoWidth, geoHeight);
    return filter->GetLinearGradientBlurRadius();
}
} // namespace Rosen
} // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreateRSLinearGradientBlurShaderFilter(data, size);
    OHOS::Rosen::DoGenerateGEVisualEffect(data, size);
    OHOS::Rosen::DoSetGeometry(data, size);
    OHOS::Rosen::DoIsOffscreenCanvas(data, size);
    OHOS::Rosen::DoGetLinearGradientBlurRadius(data, size);
    return 0;
}

