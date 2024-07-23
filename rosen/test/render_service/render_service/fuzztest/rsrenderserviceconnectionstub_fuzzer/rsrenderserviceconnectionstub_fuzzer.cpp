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
} // Rosen
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoOnRemoteRequest(data, size);
    return 0;
}