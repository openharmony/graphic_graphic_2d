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

#include "rs_background_render_modifier_fuzzer.h"
#include "modifier_ng/background/rs_background_color_render_modifier.h"
#include "modifier_ng/background/rs_background_image_render_modifier.h"
#include "modifier_ng/background/rs_background_shader_render_modifier.h"

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

bool RSBackgroundColorRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBackgroundColorRenderModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundColorRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}

bool RSBackgroundImageRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBackgroundImageRenderModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundImageRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}

bool RSBackgroundShaderRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBackgroundShaderRenderModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundShaderRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}


} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSBackgroundColorRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSBackgroundImageRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSBackgroundShaderRenderModifierFuzzTest(data, size);
    return 0;
}

