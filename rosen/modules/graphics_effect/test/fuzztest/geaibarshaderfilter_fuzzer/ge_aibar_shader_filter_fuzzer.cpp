
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

#include "ge_aibar_shader_filter_fuzzer.h"
#include "ge_aibar_shader_filter.h"
#include "get_object.h"

namespace OHOS {
namespace Rosen {

std::string GetDescriptionFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    Drawing::GEAIBarShaderFilterParams params { 2.0, 2.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> shaderFilter =
        std::make_unique<GEAIBarShaderFilter>(params);
    std::string res = shaderFilter->GetDescription();
    return res;
}

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

    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image { nullptr };
    geAIBarShaderFilter->ProcessImage(canvas, image, {0.0, 0.0, 100.0, 100.0}, {0.0, 0.0, 100.0, 100.0});
    image = std::make_shared<Drawing::Image>();
    auto res = geAIBarShaderFilter->ProcessImage(canvas, image, src, dst);
    return res;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::GetDescriptionFuzzTest(data, size);
    OHOS::Rosen::ProcessImageFuzzTest(data, size);
    return 0;
}
