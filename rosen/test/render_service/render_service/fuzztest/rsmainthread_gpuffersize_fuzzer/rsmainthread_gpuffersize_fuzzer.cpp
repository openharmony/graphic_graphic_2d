/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rsmainthread_gpuffersize_fuzzer.h"

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
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "securec.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/ohos/rs_irender_service.h"

namespace OHOS {
namespace Rosen {

static RSMainThread* mainThread_ = nullptr;

bool RSMainThreadGPUBufferSizeFuzzer::DoGetMaxGpuBufferSize()
{
    if (mainThread_ == nullptr) {
        mainThread_ = RSMainThread::Instance();
    }

    if (mainThread_ == nullptr) {
        return false;
    }

    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    return mainThread_->GetMaxGpuBufferSize(maxWidth, maxHeight);
}

static const uint8_t* DATA = nullptr;
static size_t g_size = 0;
static size_t g_pos;

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
    if (size > RSMainThreadGPUBufferSizeFuzzer::MAX_INPUT_SIZE) {
        return false;
    }
    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }

    OHOS::Rosen::RSMainThreadGPUBufferSizeFuzzer::DoGetMaxGpuBufferSize();

    return 0;
}
