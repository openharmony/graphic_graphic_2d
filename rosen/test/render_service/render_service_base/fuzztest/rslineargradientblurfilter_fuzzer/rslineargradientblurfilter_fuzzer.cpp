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

#include "rslineargradientblurfilter_fuzzer.h"

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
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_linear_gradient_blur_filter.h"
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
bool DoPostProcess(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);

    int32_t width1 = GetData<int32_t>();
    int32_t height1 = GetData<int32_t>();
    Canvas canvas(width1, height1);
    float alpha = GetData<float>();
    RSPaintFilterCanvas filterCanvas(&canvas, alpha);
    filter->PostProcess(filterCanvas);
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

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);

    filter->GetDescription();
    filter->GetDetailedDescription();
    RSLinearGradientBlurFilter::MakeHorizontalMeanBlurEffect();
    RSLinearGradientBlurFilter::MakeVerticalMeanBlurEffect();
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

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image;
    int value1 = 8;
    linearGradientBlurPara->direction_ = (GradientDirection)value1;
    image = std::make_shared<Drawing::Image>();
    filter->DrawImageRect(canvas, image, src, dst);
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

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);

    auto image = std::make_shared<Image>();
    filter->PreProcess(image);
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

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);

    std::shared_ptr<RSDrawingFilterOriginal> other = nullptr;
    filter->Compose(other);
    return true;
}

bool DoSetGeometry(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);

    int32_t width1 = GetData<int32_t>();
    int32_t height1 = GetData<int32_t>();
    Canvas canvas(width1, height1);
    float geoWidth = GetData<float>();
    float geoHeight = GetData<float>();
    filter->SetGeometry(canvas, geoWidth, geoHeight);
    return true;
}
bool DoIsOffscreenCanvas(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);
    bool isOffscreenCanvas = GetData<bool>();
    filter->IsOffscreenCanvas(isOffscreenCanvas);
    return true;
}

bool DoTransformGradientBlurDirection(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint8_t direction = GetData<uint8_t>();
    uint8_t directionBias = GetData<uint8_t>();
    RSLinearGradientBlurFilter::TransformGradientBlurDirection(direction, directionBias);
    return true;
}
bool DoComputeScale(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float width1 = GetData<float>();
    float height1 = GetData<float>();
    bool useMaskAlgorithm = GetData<bool>();
    RSLinearGradientBlurFilter::ComputeScale(width1, height1, useMaskAlgorithm);
    return true;
}
bool DoCalcDirectionBias(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Matrix mat;
    RSLinearGradientBlurFilter::CalcDirectionBias(mat);
    return true;
}
bool DoGetGradientDirectionPoints(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Point points[2];
    float x = GetData<float>();
    float y = GetData<float>();
    float z = GetData<float>();
    float k = GetData<float>();
    Point point1(x, y);
    Point point2(z, k);
    points[0] = point1;
    points[1] = point2;

    int l = GetData<int>();
    int t = GetData<int>();
    int r = GetData<int>();
    int b = GetData<int>();
    RectI value(l, t, r, b);
    Rect clipBounds(value);

    int dir = GetData<int>();

    GradientDirection direction = (GradientDirection)dir;
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, direction);
    return true;
}
bool DoMakeAlphaGradientShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int l = GetData<int>();
    int t = GetData<int>();
    int r = GetData<int>();
    int b = GetData<int>();
    Rect clipBounds(l, t, r, b);

    std::vector<std::pair<float, float>> fractionStops;
    float value1 = GetData<float>();
    float value2 = GetData<float>();
    float blurRadius = GetData<float>();
    fractionStops.push_back(std::make_pair(value1, value2));
    int value = GetData<int>();
    GradientDirection direction = (GradientDirection)value;
    auto para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);

    uint8_t directionBias = GetData<uint8_t>();
    RSLinearGradientBlurFilter::MakeAlphaGradientShader(clipBounds, para, directionBias);
    return true;
}
bool DoDrawMaskLinearGradientBlur(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    std::shared_ptr<Drawing::Image> imagef = nullptr;
    Drawing::Canvas canvas;
    Drawing::Rect dst = { 1.f, 1.f, 1.f, 1.f };
    auto image = std::make_shared<Drawing::Image>();
    auto blurFilter =
        std::static_pointer_cast<RSDrawingFilterOriginal>(linearGradientBlurPara->LinearGradientBlurFilter_);
    auto alphaGradientShader = std::make_shared<Drawing::ShaderEffect>();
    filter->DrawMaskLinearGradientBlur(imagef, canvas, blurFilter, alphaGradientShader, dst);
    return true;
}
bool DoMakeMaskLinearGradientBlurShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto srcImageShader = std::make_shared<ShaderEffect>();
    auto gradientShader = std::make_shared<ShaderEffect>();
    RSLinearGradientBlurFilter::MakeMaskLinearGradientBlurShader(srcImageShader, gradientShader);
    return true;
}
bool DoDrawMeanLinearGradientBlur(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    Drawing::Canvas canvas;
    Drawing::Rect dst = { 1.f, 1.f, 1.f, 1.f };
    std::shared_ptr<Drawing::Image> image = nullptr;
    auto alphaGradientShader = std::make_shared<Drawing::ShaderEffect>();
    filter->DrawMeanLinearGradientBlur(image, canvas, 0.f, alphaGradientShader, dst);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoPostProcess(data, size);
    OHOS::Rosen::DoOtherFunc(data, size);
    OHOS::Rosen::DoDrawImageRect(data, size);
    OHOS::Rosen::DoPreProcess(data, size);
    OHOS::Rosen::DoCompose(data, size);
    OHOS::Rosen::DoSetGeometry(data, size);
    OHOS::Rosen::DoIsOffscreenCanvas(data, size);
    OHOS::Rosen::DoTransformGradientBlurDirection(data, size);
    OHOS::Rosen::DoComputeScale(data, size);
    OHOS::Rosen::DoCalcDirectionBias(data, size);
    OHOS::Rosen::DoGetGradientDirectionPoints(data, size);
    OHOS::Rosen::DoMakeAlphaGradientShader(data, size);
    OHOS::Rosen::DoDrawMaskLinearGradientBlur(data, size);
    OHOS::Rosen::DoMakeMaskLinearGradientBlurShader(data, size);
    OHOS::Rosen::DoDrawMeanLinearGradientBlur(data, size);
    return 0;
}
