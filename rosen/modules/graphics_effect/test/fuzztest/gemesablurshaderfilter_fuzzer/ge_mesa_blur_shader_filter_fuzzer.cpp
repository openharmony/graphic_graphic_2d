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

#include "ge_mesa_blur_shader_filter_fuzzer.h"
#include "ge_mesa_blur_shader_filter.h"
#include "get_object.h"
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {

std::shared_ptr<Drawing::Image> ProcessImageFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    Drawing::Rect src = GETest::GetPlainData<Drawing::Rect>();
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();
    uint32_t radius = GETest::GetPlainData<uint32_t>();
    Drawing::GEMESABlurShaderFilterParams params{radius, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0, 1.f, 1.f};
    std::unique_ptr<GEMESABlurShaderFilter> shaderFilter = std::make_unique<GEMESABlurShaderFilter>(params);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    auto res = shaderFilter->ProcessImage(canvas, image, src, dst);
    return res;
}

std::shared_ptr<Drawing::Image> ScaleAndAddRandomColorFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    Drawing::GEMESABlurShaderFilterParams params {1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto shaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    int imageWidth = 100;
    int imageHeight = 30;
    bmp.Build(imageWidth, imageHeight, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    auto imageBlur = bmp.MakeImage();

    Drawing::Bitmap bmp2;
    bmp2.Build(imageWidth, imageHeight, format);
    bmp2.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp2.MakeImage();

    Drawing::Rect src = GETest::GetPlainData<Drawing::Rect>();
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();
    int width = GETest::GetPlainData<int>();
    int height = GETest::GetPlainData<int>();
    Drawing::Canvas canvas;

    auto res = shaderFilter->ScaleAndAddRandomColor(canvas, image, imageBlur, src, dst, width, height);
    return res;
}

int GetRadiusFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    int radius = fdp.ConsumeIntegral<int32_t>();
    Drawing::GEMESABlurShaderFilterParams params{radius, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto shaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    return shaderFilter->GetRadius();
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::ProcessImageFuzzTest(data, size);
    OHOS::Rosen::ScaleAndAddRandomColorFuzzTest(data, size);
    OHOS::Rosen::GetRadiusFuzzTest(data, size);
    return 0;
}
