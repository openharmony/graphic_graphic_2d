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

#include "rsrenderserviceconnection011_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>
#include <unordered_map>

#include "message_parcel.h"
#include "securec.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "ipc_callbacks/rs_first_frame_commit_callback_stub.h"

namespace OHOS {
namespace Rosen {
auto g_pid = getpid();
auto screenManagerPtr_ = RSScreenManager::GetInstance();
auto mainThread_ = RSMainThread::Instance();
sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();

DVSyncFeatureParam dvsyncParam;
auto generator = CreateVSyncGenerator();
auto appVSyncController = new VSyncController(generator, 0);
sptr<VSyncDistributor> appVSyncDistributor_ = new VSyncDistributor(appVSyncController, "app", dvsyncParam);
sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = new RSClientToServiceConnection(
    g_pid, nullptr, mainThread_, screenManagerPtr_, token_->AsObject(), appVSyncDistributor_);
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

class CustomTestFirstFrameCommitCallback : public RSFirstFrameCommitCallbackStub {
public:
    explicit CustomTestFirstFrameCommitCallback(const FirstFrameCommitCallback &callback) : cb_(callback)
    {}
    ~CustomTestFirstFrameCommitCallback() override{};

    void OnFirstFrameCommit(uint64_t screenId, int64_t timestamp) override
    {
        if (cb_ != nullptr) {
            cb_(screenId, timestamp);
        }
    }

private:
    FirstFrameCommitCallback cb_;
};

void DoRegisterFirstFrameCommitCallback()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ON_FIRST_FRAME_COMMIT);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint64_t screenId = GetData<uint64_t>();
    int64_t timestamp = GetData<int64_t>();
    auto callback = [&screenId, &timestamp](uint64_t screenId_, int64_t timestamp_) {
        screenId = screenId_;
        timestamp = timestamp_;
    };
    sptr<CustomTestFirstFrameCommitCallback> rsIFirstFrameCommitCallback =
        new CustomTestFirstFrameCommitCallback(callback);

    bool readRemoteObject = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(readRemoteObject);
    dataParcel.WriteRemoteObject(rsIFirstFrameCommitCallback->AsObject());
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    OHOS::Rosen::DoRegisterFirstFrameCommitCallback();
    return 0;
}