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

#include "rsuidisplaysoloist_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "feature/hyper_graphic_manager/rs_ui_display_soloist.h"

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

bool DoStart(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    SoloistIdType id = GetData<uint32_t>();
    RSDisplaySoloistManager::GetInstance().Start(id);
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

    SoloistIdType id = GetData<uint32_t>();
    RSDisplaySoloistManager::GetInstance().Stop(id);
    return true;
}

bool DoRemoveSoloist(const uint8_t* data, size_t size)
{
        if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    SoloistIdType id = GetData<uint32_t>();
    RSDisplaySoloistManager::GetInstance().RemoveSoloist(id);
    return true;
}

bool DoInsertOnVsyncCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    SoloistIdType id = GetData<uint32_t>();
    auto callback = [](long long timestamp, long long targetTimestamp, void* callbackData) {
        (void)timestamp;
        (void)targetTimestamp;
        (void)callbackData;
    };
    bool useNullCallback = GetData<bool>();
    if (useNullCallback) {
        RSDisplaySoloistManager::GetInstance().InsertOnVsyncCallback(id, nullptr, nullptr);
    } else {
        RSDisplaySoloistManager::GetInstance().InsertOnVsyncCallback(id, callback, nullptr);
    }
    return true;
}

bool DoInsertFrameRateRange(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    SoloistIdType id = GetData<uint32_t>();
    FrameRateRange frameRateRange;
    frameRateRange.min_ = GetData<int32_t>();
    frameRateRange.max_ = GetData<int32_t>();
    frameRateRange.preferred_ = GetData<int32_t>();
    frameRateRange.type_ = GetData<uint32_t>();
    RSDisplaySoloistManager::GetInstance().InsertFrameRateRange(id, frameRateRange);
    return true;
}

bool DoInsertUseExclusiveThreadFlag(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    SoloistIdType id = GetData<uint32_t>();
    bool useExclusiveThread = GetData<bool>();
    RSDisplaySoloistManager::GetInstance().InsertUseExclusiveThreadFlag(id, useExclusiveThread);
    return true;
}

bool DoSetVSyncRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    int32_t vsyncRate = GetData<int32_t>();
    RSDisplaySoloistManager::GetInstance().SetVSyncRate(vsyncRate);
    return true;
}

bool DoSetMainFrameRateLinkerEnable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    bool enabled = GetData<bool>();
    RSDisplaySoloistManager::GetInstance().SetMainFrameRateLinkerEnable(enabled);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoStart(data, size);
    OHOS::Rosen::DoStop(data, size);
    OHOS::Rosen::DoRemoveSoloist(data, size);
    OHOS::Rosen::DoInsertOnVsyncCallback(data, size);
    OHOS::Rosen::DoInsertFrameRateRange(data, size);
    OHOS::Rosen::DoInsertUseExclusiveThreadFlag(data, size);
    OHOS::Rosen::DoSetVSyncRate(data, size);
    OHOS::Rosen::DoSetMainFrameRateLinkerEnable(data, size);
    return 0;
}
