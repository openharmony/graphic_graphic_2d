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

#include "rscommonhook_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "pixel_map.h"

#include "common/rs_common_hook.h"
#include "common/rs_common_tools.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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

bool DoSetVideoSurfaceFlag(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    bool videoSurfaceFlag = GetData<bool>();
    RsCommonHook::Instance().SetVideoSurfaceFlag(videoSurfaceFlag);
    RsCommonHook::Instance().GetVideoSurfaceFlag();
    return true;
}

bool DoSetHardwareEnabledByHwcnodeBelowSelfInAppFlag(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    bool hardwareEnabledByHwcnodeSkippedFlag = GetData<bool>();
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(hardwareEnabledByHwcnodeSkippedFlag);
    RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag();
    return true;
}

bool DoSetHardwareEnabledByBackgroundAlphaFlag(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    bool hardwareEnabledByBackgroundAlphaSkippedFlag = GetData<bool>();
    bool isWhiteListForSolidColorLayerFlag = GetData<bool>();
    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(hardwareEnabledByBackgroundAlphaSkippedFlag);
    RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag();
    RsCommonHook::Instance().SetIsWhiteListForSolidColorLayerFlag(isWhiteListForSolidColorLayerFlag);
    RsCommonHook::Instance().GetIsWhiteListForSolidColorLayerFlag();
    auto callback = [](FrameRateRange &range) {
        range.preferred_ = RANGE_MAX_REFRESHRATE;
    };
    RsCommonHook::Instance().SetComponentPowerFpsFunc(callback);
    FrameRateRange range;
    RsCommonHook::Instance().GetComponentPowerFps(range);
    return true;
}

bool DoSetAdaptiveColorGamutEnable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    bool isAdaptiveColorGamutEnable = GetData<bool>();
    RsCommonHook::Instance().SetAdaptiveColorGamutEnable(isAdaptiveColorGamutEnable);
    RsCommonHook::Instance().IsAdaptiveColorGamutEnabled();
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSetVideoSurfaceFlag(data, size);
    OHOS::Rosen::DoSetHardwareEnabledByHwcnodeBelowSelfInAppFlag(data, size);
    OHOS::Rosen::DoSetHardwareEnabledByBackgroundAlphaFlag(data, size);
    return 0;
}
