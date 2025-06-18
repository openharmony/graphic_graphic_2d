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
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"
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
#ifndef MODIFIER_NG
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float value = GetData<float>();
    uint64_t id = GetData<uint64_t>();
    int64_t time = GetData<int64_t>();
    std::shared_ptr<RSProperty<float>> property = std::make_shared<RSProperty<float>>(value);
    std::shared_ptr<RSPositionZModifier> modifier = std::make_shared<RSPositionZModifier>(property);
    auto animation = std::make_shared<RSRenderAnimation>(id);
    RSModifierManager manager;
    manager.AddModifier(modifier);
    manager.AddAnimation(animation);
    manager.RemoveAnimation(id);
    manager.Animate(time);
    manager.RegisterSpringAnimation(id, id);
    manager.UnregisterSpringAnimation(id, id);
    manager.QuerySpringAnimation(id);
    manager.JudgeAnimateWhetherSkip(id, time, time);
    manager.SetDisplaySyncEnable(true);
    manager.FlushStartAnimation(time);
    manager.GetAnimation(id);
#endif
    return true;
}

bool TestHasUIAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSModifierManager manager;
    manager.HasUIRunningAnimation();
    manager.Draw();
    manager.GetFrameRateRange();
    manager.IsDisplaySyncEnabled();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::TestModifierManager(data, size);
    OHOS::Rosen::TestHasUIAnimation(data, size);
    return 0;
}