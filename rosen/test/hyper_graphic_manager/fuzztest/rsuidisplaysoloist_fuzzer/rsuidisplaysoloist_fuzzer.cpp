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

#include <fuzzer/FuzzedDataProvider.h>

#include "feature/hyper_graphic_manager/rs_ui_display_soloist.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_START = 0;
const uint8_t DO_STOP = 1;
const uint8_t DO_REMOVE_SOLOIST = 2;
const uint8_t DO_INSERT_ON_VSYNC_CALLBACK = 3;
const uint8_t DO_INSERT_FRAME_RATE_RANGE = 4;
const uint8_t DO_INSERT_USE_EXCLUSIVE_THREAD_FLAG = 5;
const uint8_t DO_SET_VSYNC_RATE = 6;
const uint8_t DO_SET_MAIN_FRAME_RATE_LINKER_ENABLE = 7;
const uint8_t TARGET_SIZE = 8;

void DoStart(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloistManager::GetInstance().Start(id);
    std::unique_lock<std::mutex> lock(RSDisplaySoloistManager::GetInstance().dataUpdateMtx_);
    RSDisplaySoloistManager::GetInstance().idToSoloistMap_.erase(id);
}

void DoStop(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloistManager::GetInstance().Stop(id);
    std::unique_lock<std::mutex> lock(RSDisplaySoloistManager::GetInstance().dataUpdateMtx_);
    RSDisplaySoloistManager::GetInstance().idToSoloistMap_.erase(id);
}

void DoRemoveSoloist(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloistManager::GetInstance().RemoveSoloist(id);
    std::unique_lock<std::mutex> lock(RSDisplaySoloistManager::GetInstance().dataUpdateMtx_);
    RSDisplaySoloistManager::GetInstance().idToSoloistMap_.erase(id);
}

void DoInsertOnVsyncCallback(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    auto callback = [](long long timestamp, long long targetTimestamp, void* data) {
        (void)timestamp;
        (void)targetTimestamp;
        (void)data;
    };
    bool useNullCallback = fdp.ConsumeBool();
    if (useNullCallback) {
        RSDisplaySoloistManager::GetInstance().InsertOnVsyncCallback(id, nullptr, nullptr);
    } else {
        RSDisplaySoloistManager::GetInstance().InsertOnVsyncCallback(id, callback, nullptr);
    }
    std::unique_lock<std::mutex> lock(RSDisplaySoloistManager::GetInstance().dataUpdateMtx_);
    RSDisplaySoloistManager::GetInstance().idToSoloistMap_.erase(id);
}

void DoInsertFrameRateRange(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    FrameRateRange frameRateRange;
    frameRateRange.min_ = fdp.ConsumeIntegral<int32_t>();
    frameRateRange.max_ = fdp.ConsumeIntegral<int32_t>();
    frameRateRange.preferred_ = fdp.ConsumeIntegral<int32_t>();
    frameRateRange.type_ = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloistManager::GetInstance().InsertFrameRateRange(id, frameRateRange);
    std::unique_lock<std::mutex> lock(RSDisplaySoloistManager::GetInstance().dataUpdateMtx_);
    RSDisplaySoloistManager::GetInstance().idToSoloistMap_.erase(id);
}

void DoInsertUseExclusiveThreadFlag(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    bool useExclusiveThread = fdp.ConsumeBool();
    RSDisplaySoloistManager::GetInstance().InsertUseExclusiveThreadFlag(id, useExclusiveThread);
    std::unique_lock<std::mutex> lock(RSDisplaySoloistManager::GetInstance().dataUpdateMtx_);
    RSDisplaySoloistManager::GetInstance().idToSoloistMap_.erase(id);
}

void DoSetVSyncRate(FuzzedDataProvider& fdp)
{
    int32_t vsyncRate = fdp.ConsumeIntegral<int32_t>();
    RSDisplaySoloistManager::GetInstance().SetVSyncRate(vsyncRate);
}

void DoSetMainFrameRateLinkerEnable(FuzzedDataProvider& fdp)
{
    bool enabled = fdp.ConsumeBool();
    RSDisplaySoloistManager::GetInstance().SetMainFrameRateLinkerEnable(enabled);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_START:
            OHOS::Rosen::DoStart(fdp);
            break;
        case OHOS::Rosen::DO_STOP:
            OHOS::Rosen::DoStop(fdp);
            break;
        case OHOS::Rosen::DO_REMOVE_SOLOIST:
            OHOS::Rosen::DoRemoveSoloist(fdp);
            break;
        case OHOS::Rosen::DO_INSERT_ON_VSYNC_CALLBACK:
            OHOS::Rosen::DoInsertOnVsyncCallback(fdp);
            break;
        case OHOS::Rosen::DO_INSERT_FRAME_RATE_RANGE:
            OHOS::Rosen::DoInsertFrameRateRange(fdp);
            break;
        case OHOS::Rosen::DO_INSERT_USE_EXCLUSIVE_THREAD_FLAG:
            OHOS::Rosen::DoInsertUseExclusiveThreadFlag(fdp);
            break;
        case OHOS::Rosen::DO_SET_VSYNC_RATE:
            OHOS::Rosen::DoSetVSyncRate(fdp);
            break;
        case OHOS::Rosen::DO_SET_MAIN_FRAME_RATE_LINKER_ENABLE:
            OHOS::Rosen::DoSetMainFrameRateLinkerEnable(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
