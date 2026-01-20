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

#include "rsrendermodifierng_fuzzer.h"

#include "pipeline/rs_render_node.h"
#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/rs_render_modifier_ng.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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

bool DoAttachPropertyFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    ModifierId id = GetData<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderProperty<bool>>();
    ModifierNG::RSPropertyType type = GetData<ModifierNG::RSPropertyType>();
    modifier->AttachProperty(type, property);
    return true;
}

bool DoDetachPropertyFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    ModifierId id = GetData<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    ModifierNG::RSPropertyType type = GetData<ModifierNG::RSPropertyType>();
    modifier->DetachProperty(type);
    return true;
}

bool DoApplyLegacyPropertyFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    ModifierId id = GetData<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    RSProperties properties;
    properties.SetAlpha(GetData<float>());
    properties.SetAlphaOffscreen(GetData<bool>());
    modifier->ApplyLegacyProperty(properties);
    return true;
}

bool DoFindPropertyTypeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    ModifierId id = GetData<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderProperty<bool>>();
    modifier->FindPropertyType(property);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::DATA = data;
    OHOS::Rosen::g_size = size;
    OHOS::Rosen::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::DoAttachPropertyFuzzTest(data, size);
    OHOS::Rosen::DoDetachPropertyFuzzTest(data, size);
    OHOS::Rosen::DoApplyLegacyPropertyFuzzTest(data, size);
    OHOS::Rosen::DoFindPropertyTypeFuzzTest(data, size);
    return 0;
}