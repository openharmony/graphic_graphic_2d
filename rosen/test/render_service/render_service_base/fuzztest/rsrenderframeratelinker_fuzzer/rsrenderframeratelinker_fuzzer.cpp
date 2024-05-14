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

#include "rsrenderframeratelinker_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "pipeline/rs_render_frame_rate_linker.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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
bool DoGetId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint64_t id = GetData<uint64_t>();
    RSRenderFrameRateLinker rsRenderFrameRateLinker(id);
    rsRenderFrameRateLinker.GetId();
    return true;
}
bool DoSetExpectedRange(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint64_t id = GetData<uint64_t>();
    int min = GetData<int>();
    int max = GetData<int>();
    int preferred = GetData<int>();
    FrameRateRange range(min, max, preferred);
    RSRenderFrameRateLinker rsRenderFrameRateLinker(id);
    rsRenderFrameRateLinker.SetExpectedRange(range);
    return true;
}
bool DoGetExpectedRange(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint64_t id = GetData<uint64_t>();
    RSRenderFrameRateLinker rsRenderFrameRateLinker(id);
    rsRenderFrameRateLinker.GetExpectedRange();
    return true;
}
bool DoSetFrameRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint64_t id = GetData<uint64_t>();
    uint32_t rate = GetData<uint32_t>();
    RSRenderFrameRateLinker rsRenderFrameRateLinker(id);
    rsRenderFrameRateLinker.SetFrameRate(rate);
    return true;
}
bool DoGetFrameRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint64_t id = GetData<uint64_t>();
    RSRenderFrameRateLinker rsRenderFrameRateLinker(id);
    rsRenderFrameRateLinker.GetFrameRate();
    return true;
}

bool DoGenerateId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSRenderFrameRateLinker::GenerateId();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGetId(data, size);
    OHOS::Rosen::DoSetExpectedRange(data, size);
    OHOS::Rosen::DoGetExpectedRange(data, size);
    OHOS::Rosen::DoSetFrameRate(data, size);
    OHOS::Rosen::DoGetFrameRate(data, size);
    OHOS::Rosen::DoGenerateId(data, size);
    return 0;
}
