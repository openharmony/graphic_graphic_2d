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

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "message_parcel.h"
#include "securec.h"

namespace OHOS {
namespace Rosen {
constexpr size_t MAX_SIZE = 4;
constexpr size_t MAX_DATA = 52;
const std::u16string RENDERSERVICECONNECTION_INTERFACE_TOKEN = u"ohos.rosen.RenderServiceConnection";

namespace {
const uint8_t* data_ = nullptr;
size_t size_ = 0;
size_t pos;
} // namespace

/*
 * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (data_ == nullptr || objectSize > size_ - pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    pos += objectSize;
    return object;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    
    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteInterfaceToken(RENDERSERVICECONNECTION_INTERFACE_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSRenderServiceConnectionStub> connectionStub =
        new RSRenderServiceConnection(0, nullptr, RSMainThread::Instance(), nullptr, token->AsObject(), nullptr);
    for (uint32_t code = 0; code < MAX_DATA; ++code) {
        connectionStub->OnRemoteRequest(code, datas, reply, option);
    }
    return true;
}
} // ROSEN
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}