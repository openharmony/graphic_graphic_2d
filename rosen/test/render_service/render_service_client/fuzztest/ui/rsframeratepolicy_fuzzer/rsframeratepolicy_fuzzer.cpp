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

#include "rsframeratepolicy_fuzzer.h"

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
constexpr size_t STR_LEN = 10;
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

std::string GetStringFromData(int strlen)
{
    if (strlen <= 0) {
        return "fuzz";
    }
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        char tmp = GetData<char>();
        if (tmp == '\0') {
            tmp = '1';
        }
        cstr[i] = tmp;
    }
    std::string str(cstr);
    return str;
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
    std::shared_ptr<RSHgmConfigData> rsHgmConfigData = std::make_shared<RSHgmConfigData>();

    float ppi = GetData<float>();
    float xDpi = GetData<float>();
    float yDpi = GetData<float>();

    rsHgmConfigData->GetConfigData();
    rsHgmConfigData->GetPpi();
    rsHgmConfigData->GetXDpi();
    rsHgmConfigData->GetYDpi();

    rsHgmConfigData->SetPpi(ppi);
    rsHgmConfigData->SetXDpi(xDpi);
    rsHgmConfigData->SetYDpi(yDpi);

    AnimDynamicItem item;
    item.animType = GetStringFromData(STR_LEN);
    item.animName = GetStringFromData(STR_LEN);
    item.minSpeed = GetData<int32_t>();
    item.maxSpeed = GetData<int32_t>();
    item.preferredFps = GetData<int32_t>();
    rsHgmConfigData->AddAnimDynamicItem(item);
    std::string pageName = GetStringFromData(STR_LEN);
    rsHgmConfigData->AddPageName(pageName);
    instance->HgmConfigChangeCallback(rsHgmConfigData);
    std::string scene = item.animType;
    float speed = GetData<float>();
    instance->GetPreferredFps(scene, speed);
    scene = GetStringFromData(STR_LEN);
    speed = GetData<float>();
    instance->GetPreferredFps(scene, speed);
    return true;
}

bool DoGetExpectedFrameRate(const uint8_t* data, size_t size)
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
    std::vector<RSPropertyUnit> units = {
        RSPropertyUnit::UNKNOWN,
        RSPropertyUnit::PIXEL_POSITION,
        RSPropertyUnit::PIXEL_SIZE,
        RSPropertyUnit::RATIO_SCALE,
        RSPropertyUnit::ANGLE_ROTATION,
    };

    for (const auto& unit : units) {
        instance->GetExpectedFrameRate(unit, 1.f);
    }

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
    instance->GetRefreshRateModeName();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGetPreferredFps(data, size);
    OHOS::Rosen::DoGetExpectedFrameRate(data, size);
    OHOS::Rosen::DoHgmRefreshRateModeChangeCallback(data, size);
    return 0;
}

