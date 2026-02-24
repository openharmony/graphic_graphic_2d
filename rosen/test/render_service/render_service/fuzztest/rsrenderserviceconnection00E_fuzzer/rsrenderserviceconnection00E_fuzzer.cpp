/*
* Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "rsrenderserviceconnection00E_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unordered_map>
#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/main_thread/rs_main_thread.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "transaction/rs_marshalling_helper.h"
#include "message_parcel.h"
#include "securec.h"
#include <iservice_registry.h>
#include <system_ability_definition.h>

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
const uint8_t DO_REGISTER_TYPEFACE = 0;
const uint8_t DO_UNREGISTER_TYPEFACE = 1;
const uint8_t DO_NEED_REGISTER_TYPEFACE = 2;
const uint8_t DO_REGISTER_SHARED_TYPEFACE = 3;
const uint8_t TARGET_SIZE = 4;

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

void DoRegisterTypeface()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    uint64_t uniqueId = static_cast<NodeId>(g_pid) << 32;
    uint32_t hash = GetData<uint32_t>();
    option.SetFlags(MessageOption::TF_SYNC);
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(uniqueId);
    dataParcel.WriteUint32(hash);
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    RSMarshallingHelper::Marshalling(dataParcel, typeface);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SHARED_TYPEFACE);
    MessageParcel dataParcel1;
    MessageParcel replyParcel1;
    MessageOption option1;
    uint64_t uniqueId1 = static_cast<NodeId>(g_pid) << 32 | GetData<uint32_t>();
    uint32_t size = GetData<uint32_t>();
    auto ashmem = Ashmem::CreateAshmem("test", size);
    if (ashmem != nullptr) {
        ashmem->MapReadAndWriteAshmem();
        option.SetFlags(MessageOption::TF_SYNC);
        dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        Drawing::SharedTypeface sharedTypeface;
        sharedTypeface.id_ = uniqueId1;
        sharedTypeface.size_ = size;
        sharedTypeface.index_ = GetData<uint32_t>();
        sharedTypeface.fd_ = ashmem->GetAshmemFd();
        RSMarshallingHelper::Marshalling(dataParcel, sharedTypeface);
        toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    }
}

void DoRegisterSharedTypeface()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SHARED_TYPEFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    Drawing::SharedTypeface sharedTypeface;
    // id is pid(int32_t) | unique id(random uint32_t)
    sharedTypeface.id_ = (static_cast<NodeId>(g_pid) << 32) | GetData<uint32_t>();
    sharedTypeface.size_ = GetData<uint32_t>();
    sharedTypeface.fd_ = GetData<int32_t>();
    sharedTypeface.hash_ = GetData<uint32_t>();
    RSMarshallingHelper::Marshalling(dataParcel, sharedTypeface);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoUnRegisterTypeface()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    uint64_t uniqueId = static_cast<NodeId>(g_pid) << 32;
    option.SetFlags(MessageOption::TF_SYNC);
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(uniqueId);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

// DO NOTHING
void DoNeedRegisterTypeface()
{}
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
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_REGISTER_TYPEFACE:
            OHOS::Rosen::DoRegisterTypeface();
            break;
        case OHOS::Rosen::DO_UNREGISTER_TYPEFACE:
            OHOS::Rosen::DoUnRegisterTypeface();
            break;
        case OHOS::Rosen::DO_NEED_REGISTER_TYPEFACE:
            OHOS::Rosen::DoRegisterTypeface();
            break;
        case OHOS::Rosen::DO_REGISTER_SHARED_TYPEFACE:
            OHOS::Rosen::DoRegisterSharedTypeface();
            break;
        default:
            return -1;
    }
    return 0;
}