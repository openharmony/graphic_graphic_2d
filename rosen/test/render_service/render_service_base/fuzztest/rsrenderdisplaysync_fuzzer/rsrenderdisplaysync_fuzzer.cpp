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

#include "rsrenderdisplaysync_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "pipeline/rs_render_display_sync.h"

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
bool DoSetExpectedFrameRateRange(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    int min = GetData<int>();
    int max = GetData<int>();
    int preferred = GetData<int>();
    FrameRateRange range(min, max, preferred);
    RSRenderDisplaySync rsRenderDisplaySync(id);
    rsRenderDisplaySync.SetExpectedFrameRateRange(range);
    rsRenderDisplaySync.GetExpectedFrameRange();

    int32_t frameRate = GetData<int32_t>();
    rsRenderDisplaySync.CalcSkipRateCount(frameRate);

    uint64_t timestamp = GetData<uint64_t>();
    int64_t period = GetData<int64_t>();
    bool isDisplaySyncEnabled = GetData<bool>();
    rsRenderDisplaySync.OnFrameSkip(timestamp, period, isDisplaySyncEnabled);
    return true;
}
bool DoSetAnimateResult(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    RSRenderDisplaySync rsRenderDisplaySync(id);

    bool flag1 = GetData<bool>();
    bool flag2 = GetData<bool>();
    bool flag3 = GetData<bool>();
    std::tuple<bool, bool, bool> result(flag1, flag2, flag3);
    rsRenderDisplaySync.SetAnimateResult(result);
    rsRenderDisplaySync.GetAnimateResult();
    return true;
}

bool DoGetNearestFrameRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    int32_t num = GetData<int32_t>();
    int32_t value = GetData<int32_t>();
    std::vector<int32_t> rates;
    rates.push_back(value);
    RSRenderDisplaySync rsRenderDisplaySync(id);
    rsRenderDisplaySync.GetNearestFrameRate(num, rates);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSetExpectedFrameRateRange(data, size);
    OHOS::Rosen::DoSetAnimateResult(data, size);
    OHOS::Rosen::DoGetNearestFrameRate(data, size);
    return 0;
}