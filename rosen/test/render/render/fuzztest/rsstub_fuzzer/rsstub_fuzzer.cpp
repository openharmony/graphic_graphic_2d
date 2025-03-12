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

#include "rsrenderservicestub_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <message_option.h>
#include <message_parcel.h>
#include <system_ability_definition.h>
#include <securec.h>

#include "platform/ohos/rs_irender_service.h"
#include "platform/ohos/rs_irender_service_ipc_interface_code_access_verifier.h"
#include "platform/ohos/rs_render_service_proxy.h"
#include "pipeline/main_thread/rs_render_service.h"
#include "transaction/rs_render_service_stub.h"

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

bool RSStubFuzztest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    // get data
    uint32_t code = GetData<uint32_t>();
    sptr<RSRenderServiceStub> stub = new RSRenderService();
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    stub->OnRemoteRequest(code, data1, reply, option);
    return true;
}

bool RSStubFuzztest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    // get data
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::CREATE_CONNECTION);
    sptr<RSRenderServiceStub> stub = new RSRenderService();
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIRenderService::GetDescriptor());
    stub->OnRemoteRequest(code, data1, reply, option);
    return true;
}

bool RSStubFuzztest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    // get data
    uint32_t code = GetData<uint32_t>();
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIRenderService> renderService = iface_cast<RSRenderServiceProxy>(remoteObject);
    sptr<RSRenderServiceStub> stub = new RSRenderService();
    sptr<RSIConnectionToken>  token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSIRenderServiceConnection> conn = renderService->CreateConnection(token);
    MessageOption option;
    MessageParcel data1;
    MessageParcel reply;
    data1.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    stub->OnRemoteRequest(code, data1, reply, option);
    return true;
}

bool RSStubFuzztest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    // get data
    uint32_t code = GetData<uint32_t>();
    sptr<RSRenderServiceStub> stub = new RSRenderService();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIRenderService::GetDescriptor());
    data1.WriteRemoteObject(token->AsObject());
    stub->OnRemoteRequest(code, data1, reply, option);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSStubFuzztest001(data, size);
    OHOS::Rosen::RSStubFuzztest002(data, size);
    OHOS::Rosen::RSStubFuzztest003(data, size);
    OHOS::Rosen::RSStubFuzztest004(data, size);
    return 0;
}