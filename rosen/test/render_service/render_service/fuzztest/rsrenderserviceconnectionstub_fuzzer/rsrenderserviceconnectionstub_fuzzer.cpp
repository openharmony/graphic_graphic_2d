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
DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");

sptr<RSIConnectionToken> token_ = nullptr;
sptr<RSRenderServiceConnectionStub> rsConnStub_ = nullptr;
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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    pid_t newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(newPid) << 32;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetAppWindowNum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
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

bool DoDropFrameByPid()
{
    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }

    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteInt32Vector(pidList)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetScreenPowerStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
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

bool DoSetHwcNodeBounds(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    pid_t newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(newPid) << 32;
    dataParcel.WriteUint64(id);
    dataParcel.WriteFloat(1.0f);
    dataParcel.WriteFloat(1.0f);
    dataParcel.WriteFloat(1.0f);
    dataParcel.WriteFloat(1.0f);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetScreenActiveMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
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

bool DoSetScreenActiveRect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT);
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

bool DoGetActiveDirtyRegionInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO);
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

bool DoGetGlobalDirtyRegionInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO);
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

bool DoGetLayerComposeInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO);
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

bool DoGetHwcDisabledReasonInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FuzzedDataProvider fdp(data, size);
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO);
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

bool DoCreateNodeAndSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    pid_t newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(newPid) << 32;
    dataParcel.WriteUint64(id);
    dataParcel.WriteString("SurfaceName");
    dataParcel.WriteUint8(0);
    dataParcel.WriteBool(true);
    dataParcel.WriteBool(true);
    dataParcel.WriteUint8(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoExecuteSynchronousTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    pid_t newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    option.SetFlags(MessageOption::TF_SYNC);
    dataParcel.WriteInterfaceToken(GetDescriptor());
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderPropertyBase>();
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(4e8, property);
    task->Marshalling(dataParcel);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetFreeMultiWindowStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub_ =
        new RSRenderServiceConnection(newPid, nullptr, RSMainThread::Instance(), nullptr, token_->AsObject(), nullptr);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS);
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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto newPid = getpid();
    token_ = new IRemoteStub<RSIConnectionToken>();
    if (token_ == nullptr) {
        return false;
    }
    rsConnStub_ = new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    return true;
}

bool DoSetVirtualScreenSurface()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint64_t id = GetData<uint64_t>();
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    auto producer = pSurface->GetProducer();
    if (!dataP.WriteRemoteObject(producer->AsObject())) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualScreenResolution()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint64_t id = GetData<uint64_t>();
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    uint32_t width = GetData<uint32_t>();
    if (!dataP.WriteUint32(width)) {
        return false;
    }
    uint32_t height = GetData<uint32_t>();
    if (!dataP.WriteUint32(height)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoGetVirtualScreenResolution()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint64_t id = GetData<uint64_t>();
    if (!dataP.WriteUint64(id)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualScreenStatus()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t screenStatus = GetData<uint64_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUint8(screenStatus)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualScreenBlackList()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blackListVector;
    blackListVector.push_back(nodeId);
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUInt64Vector(blackListVector)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoAddVirtualScreenBlackList()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blackListVector;
    blackListVector.push_back(nodeId);
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUInt64Vector(blackListVector)) {
        return false;
    }
    
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoRemoveVirtualScreenBlackList()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blackListVector;
    blackListVector.push_back(nodeId);
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUInt64Vector(blackListVector)) {
        return false;
    }
    
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetScreenSkipFrameInterval()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t interval = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUint32(interval)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
    auto newPid = getpid();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, nullptr, token->AsObject(), nullptr);
    if (connectionStub == nullptr) {
        return false;
    }
    connectionStub->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualScreenSecurityExemptionList()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> secExemptionListVector;
    secExemptionListVector.push_back(nodeId);
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUInt64Vector(secExemptionListVector)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST);
    auto newPid = getpid();
    sptr<RSScreenManager> screenManager = nullptr;
    if (GetData<bool>()) {
        screenManager = CreateOrGetScreenManager();
    }
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, screenManager, token->AsObject(), nullptr);
    if (connectionStub == nullptr) {
        return false;
    }
    connectionStub->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualScreenVisibleRect()
{
    uint64_t id = GetData<uint64_t>();
    Rect rect = {
        .x = GetData<int32_t>(),
        .y = GetData<int32_t>(),
        .w = GetData<int32_t>(),
        .h = GetData<int32_t>()
    };
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteInt32(rect.x) || !dataP.WriteInt32(rect.y) ||
        !dataP.WriteInt32(rect.w) || !dataP.WriteInt32(rect.h)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT);
    auto newPid = getpid();
    sptr<RSScreenManager> screenManager = nullptr;
    if (GetData<bool>()) {
        screenManager = CreateOrGetScreenManager();
    }
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub =
        new RSRenderServiceConnection(newPid, nullptr, nullptr, screenManager, token->AsObject(), nullptr);
    if (connectionStub == nullptr) {
        return false;
    }
    connectionStub->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetCastScreenEnableSkipWindow()
{
    uint64_t id = GetData<uint64_t>();
    bool enable = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteBool(enable)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetScreenCorrection()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t screenRotation = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUint32(screenRotation)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualMirrorScreenCanvasRotation()
{
    uint64_t id = GetData<uint64_t>();
    bool canvasRotation = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteBool(canvasRotation)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualMirrorScreenScaleMode()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t scaleMode = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUint32(scaleMode)) {
        return false;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoResizeVirtualScreen()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUint32(width)) {
        return false;
    }
    if (!dataP.WriteUint32(height)) {
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetCacheEnabledForRotation()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    bool isEnabled = GetData<bool>();
    if (!dataP.WriteBool(isEnabled)) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualScreenUsingStatus()
{
    bool isVirtualScreenUsingStatus = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    if (!dataP.WriteBool(isVirtualScreenUsingStatus)) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoCreatePixelMapFromSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("FuzzTest");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    if (pSurface == nullptr) {
        return false;
    }

    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    if (!dataP.WriteRemoteObject(pSurface->GetProducer()->AsObject())) {
        return false;
    }
    if (!dataP.WriteInt32(GetData<int32_t>()) || !dataP.WriteInt32(GetData<int32_t>()) ||
        !dataP.WriteInt32(GetData<int32_t>()) || !dataP.WriteInt32(GetData<int32_t>())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoNotifyTouchEvent()
{
    uint32_t touchStatus = GetData<uint32_t>();
    uint32_t touchCnt = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint32(touchStatus)) {
        return false;
    }
    if (!dataP.WriteUint32(touchCnt)) {
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT);
    if (rsConnStub_ == nullptr) {
        return false;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
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
    OHOS::Rosen::DoDropFrameByPid();
    OHOS::Rosen::DoSetScreenPowerStatus(data, size);
    OHOS::Rosen::DoSetHwcNodeBounds(data, size);
    OHOS::Rosen::DoSetScreenActiveMode(data, size);
    OHOS::Rosen::DoSetScreenActiveRect(data, size);
    OHOS::Rosen::DoGetScreenActiveMode(data, size);
    OHOS::Rosen::DoSetRefreshRateMode(data, size);
    OHOS::Rosen::DoGetScreenSupportedRefreshRates(data, size);
    OHOS::Rosen::DoGetScreenSupportedModes(data, size);
    OHOS::Rosen::DoGetScreenCapability(data, size);
    OHOS::Rosen::DoGetMemoryGraphic(data, size);
    OHOS::Rosen::DoGetScreenData(data, size);
    OHOS::Rosen::DoGetScreenBacklight(data, size);
    OHOS::Rosen::DoSetScreenBacklight(data, size);
    OHOS::Rosen::DoGetScreenColorGamut(data, size);
    OHOS::Rosen::DoSetScreenColorGamut(data, size);
    OHOS::Rosen::DoCreateNodeAndSurface(data, size);
    OHOS::Rosen::DoExecuteSynchronousTask(data, size);
    OHOS::Rosen::DoSetFreeMultiWindowStatus(data, size);
    OHOS::Rosen::DoCreateVirtualScreen(data, size);
    OHOS::Rosen::DoRemoveVirtualScreen(data, size);
    if (!OHOS::Rosen::Init(data, size)) {
        return 0;
    }
    OHOS::Rosen::DoSetVirtualScreenSurface();
    OHOS::Rosen::DoSetVirtualScreenResolution();
    OHOS::Rosen::DoGetVirtualScreenResolution();
    OHOS::Rosen::DoSetVirtualScreenStatus();
    OHOS::Rosen::DoSetVirtualScreenBlackList();
    OHOS::Rosen::DoAddVirtualScreenBlackList();
    OHOS::Rosen::DoRemoveVirtualScreenBlackList();
    OHOS::Rosen::DoSetScreenSkipFrameInterval();
    OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList();
    OHOS::Rosen::DoSetVirtualScreenVisibleRect();
    OHOS::Rosen::DoSetCastScreenEnableSkipWindow();
    OHOS::Rosen::DoSetScreenCorrection();
    OHOS::Rosen::DoSetVirtualMirrorScreenCanvasRotation();
    OHOS::Rosen::DoSetVirtualMirrorScreenScaleMode();
    OHOS::Rosen::DoResizeVirtualScreen();
    OHOS::Rosen::DoSetCacheEnabledForRotation();
    OHOS::Rosen::DoSetVirtualScreenUsingStatus();
    OHOS::Rosen::DoCreatePixelMapFromSurface(data, size);
    OHOS::Rosen::DoNotifyTouchEvent();
    return 0;
}