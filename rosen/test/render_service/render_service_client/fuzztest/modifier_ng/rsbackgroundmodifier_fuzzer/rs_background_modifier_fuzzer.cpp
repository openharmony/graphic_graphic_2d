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

#include "rs_background_modifier_fuzzer.h"
#include "modifier_ng/background/rs_background_color_modifier.h"
#include "modifier_ng/background/rs_background_image_modifier.h"
#include "modifier_ng/background/rs_background_shader_modifier.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>


namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

bool BackgroundColorModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBackgroundColorModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundColorModifier>();

    Vector4f brightnessRates(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    modifier->SetBgBrightnessRates(brightnessRates);
    modifier->GetBgBrightnessRates();

    float brightnessFract = GetData<float>();
    modifier->SetBgBrightnessFract(brightnessFract);
    modifier->GetBgBrightnessFract();

    return true;
}

bool BackgroundImageModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBackgroundImageModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundImageModifier>();

    Vector4f innerRect(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    modifier->SetBgImageInnerRect(innerRect);
    modifier->GetBgImageInnerRect();

    float width = GetData<float>();
    modifier->SetBgImageWidth(width);
    modifier->GetBgImageWidth();

    float height = GetData<float>();
    modifier->SetBgImageHeight(height);
    modifier->GetBgImageHeight();

    float positionX = GetData<float>();
    modifier->SetBgImagePositionX(positionX);
    modifier->GetBgImagePositionX();

    float positionY = GetData<float>();
    modifier->SetBgImagePositionY(positionY);
    modifier->GetBgImagePositionY();

    Vector4f dstRect(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    modifier->SetBgImageDstRect(dstRect);
    modifier->GetBgImageDstRect();

    return true;
}

bool BackgroundShaderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBackgroundShaderModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundShaderModifier>();

    float progress = GetData<float>();
    modifier->SetBackgroundShaderProgress(progress);
    modifier->GetBackgroundShaderProgress();

    std::vector<float> param = {GetData<float>()};
    modifier->SetComplexShaderParam(param);
    modifier->GetComplexShaderParam();

    return true;
}


} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::BackgroundColorModifierFuzzTest(data, size);
    OHOS::Rosen::BackgroundImageModifierFuzzTest(data, size);
    OHOS::Rosen::BackgroundShaderModifierFuzzTest(data, size);
    return 0;
}