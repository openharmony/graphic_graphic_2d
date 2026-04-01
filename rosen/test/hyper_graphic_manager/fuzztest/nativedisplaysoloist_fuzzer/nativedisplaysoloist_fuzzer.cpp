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

#include "nativedisplaysoloist_fuzzer.h"

#include "native_display_soloist.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

OH_DisplaySoloist* g_displaySoloist = nullptr;
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

bool DoCreate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    if (g_displaySoloist != nullptr) {
        OH_DisplaySoloist_Destroy(g_displaySoloist);
        g_displaySoloist = nullptr;
    }
    bool useExclusiveThread = GetData<bool>();
    g_displaySoloist = OH_DisplaySoloist_Create(useExclusiveThread);
    return true;
}

bool DoDestroy(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    if (g_displaySoloist != nullptr) {
        OH_DisplaySoloist_Destroy(g_displaySoloist);
        g_displaySoloist = nullptr;
    }
    return true;
}

bool DoStart(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    bool useNullSoloist = GetData<bool>();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist;
    bool useNullCallback = GetData<bool>();
    OH_DisplaySoloist_FrameCallback callback = nullptr;
    if (!useNullCallback) {
        callback = [](long long timestamp, long long targetTimestamp, void* data) {
            (void)timestamp;
            (void)targetTimestamp;
            (void)data;
        };
    }
    OH_DisplaySoloist_Start(soloist, callback, nullptr);
    return true;
}

bool DoStop(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    bool useNullSoloist = GetData<bool>();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist;
    OH_DisplaySoloist_Stop(soloist);
    return true;
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

    bool useNullSoloist = GetData<bool>();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist;

    bool useNullRange = GetData<bool>();
    if (useNullRange) {
        OH_DisplaySoloist_SetExpectedFrameRateRange(soloist, nullptr);
        return;
    }

    DisplaySoloist_ExpectedRateRange range;
    range.min = GetData<int32_t>();
    range.max = GetData<int32_t>();
    range.expected = GetData<int32_t>();
    OH_DisplaySoloist_SetExpectedFrameRateRange(soloist, &range);
    return true;
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoDestroy(data, size);
    OHOS::Rosen::DoStart(data, size);
    OHOS::Rosen::DoStop(data, size);
    OHOS::Rosen::DoSetExpectedFrameRateRange(data, size);
    return 0;
}
