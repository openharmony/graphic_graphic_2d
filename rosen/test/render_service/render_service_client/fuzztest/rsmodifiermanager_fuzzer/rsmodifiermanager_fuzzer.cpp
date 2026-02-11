/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "rsmodifiermanager_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iostream>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "common/rs_common_def.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_modifier_manager_map.h"
#include "modifier/rs_property.h"
#include "ui/rs_canvas_node.h"

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

bool TestModifierManager(const uint8_t* data, size_t size)
{
    return true;
}

bool TestRemoveAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto rsModifierManager = RSModifierManagerMap::Instance()->GetModifierManager();
    AnimationId keyId = GetData<AnimationId>();
    rsModifierManager->RemoveAnimation(keyId);
    return true;
}

bool TestJudgeAnimateWhetherSkip(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto rsModifierManager = RSModifierManagerMap::Instance()->GetModifierManager();
    AnimationId animId = GetData<AnimationId>();
    int64_t time = GetData<int64_t>();
    int64_t vsyncPeriod = GetData<int64_t>();
    rsModifierManager->JudgeAnimateWhetherSkip(animId, time, vsyncPeriod);
    return true;
}

bool TestRegisterSpringAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto rsModifierManager = RSModifierManagerMap::Instance()->GetModifierManager();
    PropertyId propertyId = GetData<PropertyId>();
    AnimationId animationId = GetData<AnimationId>();
    rsModifierManager->RegisterSpringAnimation(propertyId, animationId);
    rsModifierManager->UnregisterSpringAnimation(propertyId, animationId);
    return true;
}

bool TestQuerySpringAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto rsModifierManager = RSModifierManagerMap::Instance()->GetModifierManager();
    PropertyId propertyId = GetData<PropertyId>();
    rsModifierManager->QuerySpringAnimation(propertyId);
    return true;
}

bool TestMoveModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto rsModifierManager = RSModifierManagerMap::Instance()->GetModifierManager();
    auto dstRsModifierManager = std::make_shared<RSModifierManager>();
    NodeId id = GetData<NodeId>();
    rsModifierManager->MoveModifier(dstRsModifierManager, id);
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
    OHOS::Rosen::TestModifierManager(data, size);
    OHOS::Rosen::TestRemoveAnimation(data, size);
    OHOS::Rosen::TestJudgeAnimateWhetherSkip(data, size);
    OHOS::Rosen::TestRegisterSpringAnimation(data, size);
    OHOS::Rosen::TestQuerySpringAnimation(data, size);
    OHOS::Rosen::TestMoveModifier(data, size);
    return 0;
}