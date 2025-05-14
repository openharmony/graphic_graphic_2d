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

#include "rsrenderframeratelinkermap_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "pipeline/rs_render_frame_rate_linker.h"
#include "pipeline/rs_render_frame_rate_linker_map.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"

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
bool DoRegisterFrameRateLinker(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    auto linkerPtr = std::make_shared<RSRenderFrameRateLinker>(id);
    RSRenderFrameRateLinkerMap rsRenderFrameRateLinkerMap;
    rsRenderFrameRateLinkerMap.RegisterFrameRateLinker(linkerPtr);
    return true;
}
bool DoUnregisterFrameRateLinker(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    RSRenderFrameRateLinkerMap rsRenderFrameRateLinkerMap;
    rsRenderFrameRateLinkerMap.UnregisterFrameRateLinker(id);
    return true;
}
bool DoFilterFrameRateLinkerByPid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    int pid = GetData<int>();
    RSRenderFrameRateLinkerMap rsRenderFrameRateLinkerMap;
    rsRenderFrameRateLinkerMap.FilterFrameRateLinkerByPid(pid);
    return true;
}
bool DoGetFrameRateLinker(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    RSRenderFrameRateLinkerMap rsRenderFrameRateLinkerMap;
    rsRenderFrameRateLinkerMap.GetFrameRateLinker(id);
    return true;
}
bool DoRegisterFrameRateLinkerExpectedFpsUpdateCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    int listenerPid = GetData<int>();
    uint32_t dstPid = GetData<uint32_t>();
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> rsIFrameRateLinkerExpectedFpsUpdateCallback =
        iface_cast<RSIFrameRateLinkerExpectedFpsUpdateCallback>(remoteObject);
    RSRenderFrameRateLinkerMap rsRenderFrameRateLinkerMap;
    rsRenderFrameRateLinkerMap.RegisterFrameRateLinkerExpectedFpsUpdateCallback(listenerPid,
        dstPid, rsIFrameRateLinkerExpectedFpsUpdateCallback);

    return true;
}
bool DoUnRegisterExpectedFpsUpdateCallbackByListener(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    int listenerPid = GetData<int>();
    RSRenderFrameRateLinkerMap rsRenderFrameRateLinkerMap;
    rsRenderFrameRateLinkerMap.UnRegisterExpectedFpsUpdateCallbackByListener(listenerPid);

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRegisterFrameRateLinker(data, size);
    OHOS::Rosen::DoUnregisterFrameRateLinker(data, size);
    OHOS::Rosen::DoFilterFrameRateLinkerByPid(data, size);
    OHOS::Rosen::DoGetFrameRateLinker(data, size);
    OHOS::Rosen::DoRegisterFrameRateLinkerExpectedFpsUpdateCallback(data, size);
    OHOS::Rosen::DoUnRegisterExpectedFpsUpdateCallbackByListener(data, size);
    return 0;
}