/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "rsrenderserviceconnectionstub_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"

namespace OHOS {
namespace Rosen {
constexpr size_t MAX_SIZE = 4;
DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");

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

bool DoOnRemoteRequest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = fdp.ConsumeIntegralInRange<uint32_t>(0, 100);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetScreenGamutMap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    
    FuzzedDataProvider fdp(data, size);
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenGamutMap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    
    FuzzedDataProvider fdp(data, size);
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenHDRCapability(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    
    FuzzedDataProvider fdp(data, size);
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    
    FuzzedDataProvider fdp(data, size);
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetBitmap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    
    FuzzedDataProvider fdp(data, size);
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetAppWindowNum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    
    FuzzedDataProvider fdp(data, size);
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoShowWatermark(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    
    FuzzedDataProvider fdp(data, size);
    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetScreenPowerStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetScreenActiveMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenActiveMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetRefreshRateMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenSupportedRefreshRates(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenSupportedModes(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenCapability(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetMemoryGraphic(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
    auto newPid = getpid();
    
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoCreateVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    auto newPid = getpid();
    
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoRemoveVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN);
    auto newPid = getpid();
    
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenData(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA);
    auto newPid = getpid();
    
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenBacklight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
    auto newPid = getpid();
    
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetScreenBacklight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT);
    auto newPid = getpid();
    
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoGetScreenColorGamut(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT);
    auto newPid = getpid();
    
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetScreenColorGamut(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT);
    auto newPid = getpid();
    
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetAncoForceDoDirect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT);
    auto newPid = getpid();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::vector<uint8_t> subData =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, fdp.remaining_bytes()));
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBuffer(subData.data(), subData.size());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}
} // Rosen
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoOnRemoteRequest(data, size);
    OHOS::Rosen::DoSetScreenGamutMap(data, size);
    OHOS::Rosen::DoGetScreenGamutMap(data, size);
    OHOS::Rosen::DoGetScreenHDRCapability(data, size);
    OHOS::Rosen::DoGetScreenType(data, size);
    OHOS::Rosen::DoGetBitmap(data, size);
    OHOS::Rosen::DoSetAppWindowNum(data, size);
    OHOS::Rosen::DoShowWatermark(data, size);
    OHOS::Rosen::DoSetScreenPowerStatus(data, size);
    OHOS::Rosen::DoSetScreenActiveMode(data, size);
    OHOS::Rosen::DoGetScreenActiveMode(data, size);
    OHOS::Rosen::DoSetRefreshRateMode(data, size);
    OHOS::Rosen::DoGetScreenSupportedRefreshRates(data, size);
    OHOS::Rosen::DoGetScreenSupportedModes(data, size);
    OHOS::Rosen::DoGetScreenCapability(data, size);
    OHOS::Rosen::DoGetMemoryGraphic(data, size);
    OHOS::Rosen::DoCreateVirtualScreen(data, size);
    OHOS::Rosen::DoRemoveVirtualScreen(data, size);
    OHOS::Rosen::DoGetScreenData(data, size);
    OHOS::Rosen::DoGetScreenBacklight(data, size);
    OHOS::Rosen::DoSetScreenBacklight(data, size);
    OHOS::Rosen::DoGetScreenColorGamut(data, size);
    OHOS::Rosen::DoSetScreenColorGamut(data, size);

    return 0;
}