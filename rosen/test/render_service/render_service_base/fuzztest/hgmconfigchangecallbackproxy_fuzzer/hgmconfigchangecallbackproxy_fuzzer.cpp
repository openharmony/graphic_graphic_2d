/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hgmconfigchangecallbackproxy_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iostream>
#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "ipc_callbacks/hgm_config_change_callback_proxy.h"
#include "transaction/rs_hgm_config_data.h"
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
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    int32_t refreshRateMode = GetData<int32_t>();
    int32_t refreshRate = GetData<int32_t>();
    auto configData = std::make_shared<RSHgmConfigData>();
    float ppi = GetData<float>();
    float xDpi = GetData<float>();
    float yDpi = GetData<float>();

    configData->SetPpi(ppi);
    configData->SetXDpi(xDpi);
    configData->SetYDpi(yDpi);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSHgmConfigChangeCallbackProxy rsHgmConfigChangeCallbackProxy(remoteObject);

    rsHgmConfigChangeCallbackProxy.OnHgmConfigChanged(configData);
    rsHgmConfigChangeCallbackProxy.OnHgmRefreshRateModeChanged(refreshRateMode);
    rsHgmConfigChangeCallbackProxy.OnHgmRefreshRateUpdate(refreshRate);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
