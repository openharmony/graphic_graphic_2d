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

#include "rscmodifier_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <malloc.h>
#include <securec.h>

#include "animation/rs_animation.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_modifier_manager_map.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

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

bool DoGetPropertyId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    modifier->GetPropertyId();
    return true;
}

bool DoSet(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    float value1 = GetData<float>();
    prop->Set(value1);
    std::static_pointer_cast<RSAnimatableProperty<float>>(modifier->GetProperty())->Get();
    return true;
}

bool DoAttachProperty(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    modifier->AttachProperty(prop);
    std::unordered_map<int32_t, std::shared_ptr<RSModifierManager>>().swap(
        RSModifierManagerMap::Instance()->managerMap_);

    return true;
}

bool DoSetDirty(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    bool dirty = GetData<bool>();
    modifier->SetDirty(dirty);
    std::unordered_map<int32_t, std::shared_ptr<RSModifierManager>>().swap(
        RSModifierManagerMap::Instance()->managerMap_);

    return true;
}

bool DoResetRSNodeExtendModifierDirty(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    modifier->ResetRSNodeExtendModifierDirty();

    return true;
}

bool DoGetProperty(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    modifier->GetProperty();
    modifier->GetRenderProperty();

    return true;
}

bool DoGetModifierType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    modifier->GetModifierType();

    return true;
}

bool DoAttachAndDetachNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    auto node = std::make_shared<RSNode>(true);
    modifier->AttachToNode(node);
    modifier->DetachFromNode();

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGetPropertyId(data, size);
    OHOS::Rosen::DoSet(data, size);
    OHOS::Rosen::DoAttachProperty(data, size);
    OHOS::Rosen::DoSetDirty(data, size);
    OHOS::Rosen::DoResetRSNodeExtendModifierDirty(data, size);
    OHOS::Rosen::DoGetProperty(data, size);
    OHOS::Rosen::DoGetModifierType(data, size);
    OHOS::Rosen::DoAttachAndDetachNode(data, size);
    return 0;
}

