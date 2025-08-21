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

#include "rs_custom_modifier_fuzzer.h"

#include "modifier_ng/custom/rs_background_style_modifier.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "modifier_ng/custom/rs_foreground_style_modifier.h"
#include "modifier_ng/custom/rs_node_modifier.h"
#include "modifier_ng/custom/rs_transition_style_modifier.h"

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

bool RSBackgroundStyleModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSBackgroundStyleModifier>();
    ModifierNG::RSDrawingContext context {nullptr, GetData<float>(), GetData<float>()};
    modifier->Draw(context);
    return true;
}

bool RSContentStyleModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    ModifierNG::RSDrawingContext context {nullptr, GetData<float>(), GetData<float>()};
    modifier->Draw(context);

    return true;
}

bool RSForegroundStyleModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSForegroundStyleModifier>();
    ModifierNG::RSDrawingContext context {nullptr, GetData<float>(), GetData<float>()};
    modifier->Draw(context);

    return true;
}

bool RSNodeModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSNodeModifier>();
    ModifierNG::RSDrawingContext context {nullptr, GetData<float>(), GetData<float>()};
    modifier->Draw(context);
    return true;
}

bool RSTransitionStyleModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSTransitionStyleModifier>();
    ModifierNG::RSDrawingContext context {nullptr, GetData<float>(), GetData<float>()};
    modifier->Draw(context);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::g_data = data;
    OHOS::Rosen::g_size = size;
    OHOS::Rosen::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::RSBackgroundStyleModifierFuzzTest(data, size);
    OHOS::Rosen::RSContentStyleModifierFuzzTest(data, size);
    OHOS::Rosen::RSForegroundStyleModifierFuzzTest(data, size);
    OHOS::Rosen::RSNodeModifierFuzzTest(data, size);
    OHOS::Rosen::RSTransitionStyleModifierFuzzTest(data, size);
    return 0;
}
