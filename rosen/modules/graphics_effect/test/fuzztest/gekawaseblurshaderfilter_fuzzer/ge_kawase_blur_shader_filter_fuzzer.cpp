
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

#include "ge_kawase_blur_shader_filter_fuzzer.h"
#include "ge_kawase_blur_shader_filter.h"
#include "get_object.h"

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

    float fLeft = GETest::GetPlainData<float>();
    float fTop = GETest::GetPlainData<float>();
    float fWidth = GETest::GetPlainData<float>();
    float fHeight = GETest::GetPlainData<float>();
    Drawing::Rect src{fLeft, fTop, fWidth, fHeight};
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();

    Drawing::GEKawaseBlurShaderFilterParams params = {5};
    std::unique_ptr<GEKawaseBlurShaderFilter> kawaseBlurShaderFilter =
        std::make_unique<GEKawaseBlurShaderFilter>(params);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    auto res = kawaseBlurShaderFilter->ProcessImage(canvas, image, src, dst);
    return res;
}

int GetRadiusFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return 0;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    int radius = GETest::GetPlainData<int>();

    Drawing::GEKawaseBlurShaderFilterParams params = {radius};
    std::unique_ptr<GEKawaseBlurShaderFilter> kawaseBlurShaderFilter =
        std::make_unique<GEKawaseBlurShaderFilter>(params);

    int res = kawaseBlurShaderFilter->GetRadius();
    return res;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::ProcessImageFuzzTest(data, size);
    OHOS::Rosen::GetRadiusFuzzTest(data, size);
    return 0;
}
