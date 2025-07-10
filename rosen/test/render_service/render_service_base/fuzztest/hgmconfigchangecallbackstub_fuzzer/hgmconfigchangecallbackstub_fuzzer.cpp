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

#include "hgmconfigchangecallbackstub_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iostream>
#include <iservice_registry.h>
#include <message_option.h>
#include <message_parcel.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "ipc_callbacks/hgm_config_change_callback_stub.h"
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

    int32_t rate = GetData<int32_t>();
    uint32_t code = GetData<uint32_t>();
    MessageParcel messageParcel;
    MessageParcel reply;
    MessageOption option;
    class MyRSHgmConfigChangeCallbackStub : public OHOS::Rosen::RSHgmConfigChangeCallbackStub {
    public:
        void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override {}
        void OnHgmRefreshRateModeChanged(int32_t refreshRateModeName) override {}
        void OnHgmRefreshRateUpdate(int32_t refreshRateUpdate) override {}
    };
    MyRSHgmConfigChangeCallbackStub myRSHgmConfigChangeCallbackStub;
    // Set a token that matchees GetDescriptor（）
    messageParcel.WriteInterfaceToken(RSIHgmConfigChangeCallback::GetDescriptor());
    messageParcel.WriteInt32(rate);
    myRSHgmConfigChangeCallbackStub.OnRemoteRequest(code, messageParcel, reply, option);
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
