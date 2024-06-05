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

#include "rsblurfilter_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "draw/canvas.h"
#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "render/rs_light_up_effect_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
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
bool DoOtherFunc(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    auto rsBlurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    rsBlurFilter->GetBlurRadiusX();
    rsBlurFilter->GetBlurRadiusY();
    rsBlurFilter->IsValid();
    rsBlurFilter->GetDescription();
    rsBlurFilter->GetDetailedDescription();
    rsBlurFilter->Negate();
    rsBlurFilter->CanSkipFrame();

    return true;
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

    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto composedFilter = blurFilter1->Compose(blurFilter2);
    return true;
}
bool DoAdd(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    auto rsBlurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    rsBlurFilter->Add(rhs);
    rsBlurFilter->Sub(rhs);
    return true;
}
bool DoMultiply(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    auto rsBlurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    float rhs = GetData<float>();
    rsBlurFilter->Multiply(rhs);
    return true;
}
bool DoDrawImageRect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    auto rsBlurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    auto image = std::make_shared<Image>();
    float l = GetData<float>();
    float t = GetData<float>();
    float r = GetData<float>();
    float b = GetData<float>();
    float l2 = GetData<float>();
    float t2 = GetData<float>();
    float r2 = GetData<float>();
    float b2 = GetData<float>();
    Rect src(l, t, r, b);
    Rect dst(l2, t2, r2, b2);
    rsBlurFilter->DrawImageRect(canvas, image, src, dst);
    return true;
}
bool DoSetGreyCoef(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    auto rsBlurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    std::optional<Vector2f> greyCoef;
    rsBlurFilter->SetGreyCoef(greyCoef);
    return true;
}
bool DoIsNearEqual(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    auto rsBlurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    float threshold = GetData<float>();
    rsBlurFilter->IsNearEqual(rhs, threshold);
    return true;
}
bool DoIsNearZero(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    auto rsBlurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    float threshold = GetData<float>();
    rsBlurFilter->IsNearZero(threshold);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoOtherFunc(data, size);
    OHOS::Rosen::DoCompose(data, size);
    OHOS::Rosen::DoAdd(data, size);
    OHOS::Rosen::DoMultiply(data, size);
    OHOS::Rosen::DoDrawImageRect(data, size);
    OHOS::Rosen::DoSetGreyCoef(data, size);
    OHOS::Rosen::DoIsNearEqual(data, size);
    OHOS::Rosen::DoIsNearZero(data, size);
    return 0;
}