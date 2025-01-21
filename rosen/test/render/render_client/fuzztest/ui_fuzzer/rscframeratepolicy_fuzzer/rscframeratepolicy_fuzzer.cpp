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

#include "rscframeratepolicy_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "feature/hyper_graphic_manager/rs_frame_rate_policy.h"
#include "transaction/rs_hgm_config_data.h"
#include "modifier/rs_modifier_type.h"

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

bool DoGetInstance(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    auto instance = RSFrameRatePolicy::GetInstance();
    instance->RegisterHgmConfigChangeCallback();
    return true;
}

bool DoRegisterHgmConfigChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    instance->RegisterHgmConfigChangeCallback();
    return true;
}

bool DoGetPreferredFps(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    std::string scene = "test scene";
    float speed = GetData<float>();
    instance->GetPreferredFps(scene, speed);
    return true;
}

bool DoGetRefreshRateModeName(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    instance->GetRefreshRateModeName();
    return true;
}

bool DoGetExpectedFrameRate001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    RSPropertyUnit unit = RSPropertyUnit::PIXEL_POSITION;
    instance->GetExpectedFrameRate(unit, 1.f);
    return true;
}

bool DoGetExpectedFrameRate002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    RSPropertyUnit unit = RSPropertyUnit::PIXEL_SIZE;
    instance->GetExpectedFrameRate(unit, 1.f);
    return true;
}

bool DoGetExpectedFrameRate003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    RSPropertyUnit unit = RSPropertyUnit::RATIO_SCALE;
    instance->GetExpectedFrameRate(unit, 1.f);
    return true;
}

bool DoGetExpectedFrameRate004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    RSPropertyUnit unit = RSPropertyUnit::ANGLE_ROTATION;
    instance->GetExpectedFrameRate(unit, 1.f);
    return true;
}

bool DoGetExpectedFrameRate005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    RSPropertyUnit unit = RSPropertyUnit::UNKNOWN;
    instance->GetExpectedFrameRate(unit, 1.f);
    return true;
}

bool DoHgmConfigChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    std::shared_ptr<RSHgmConfigData> configData = std::make_shared<RSHgmConfigData>();
    AnimDynamicItem item;
    item.animType = "animType";
    item.animName = "animName";
    configData->AddAnimDynamicItem(item);
    instance->HgmConfigChangeCallback(configData);
    return true;
}

bool DoHgmRefreshRateModeChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    RSFrameRatePolicy* instance = RSFrameRatePolicy::GetInstance();
    int32_t refreshRateMode = GetData<int32_t>();
    instance->HgmRefreshRateModeChangeCallback(refreshRateMode);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGetInstance(data, size);
    OHOS::Rosen::DoRegisterHgmConfigChangeCallback(data, size);
    OHOS::Rosen::DoGetPreferredFps(data, size);
    OHOS::Rosen::DoGetRefreshRateModeName(data, size);
    OHOS::Rosen::DoGetExpectedFrameRate001(data, size);
    OHOS::Rosen::DoGetExpectedFrameRate002(data, size);
    OHOS::Rosen::DoGetExpectedFrameRate003(data, size);
    OHOS::Rosen::DoGetExpectedFrameRate004(data, size);
    OHOS::Rosen::DoGetExpectedFrameRate005(data, size);
    OHOS::Rosen::DoHgmConfigChangeCallback(data, size);
    OHOS::Rosen::DoHgmRefreshRateModeChangeCallback(data, size);
    return 0;
}

