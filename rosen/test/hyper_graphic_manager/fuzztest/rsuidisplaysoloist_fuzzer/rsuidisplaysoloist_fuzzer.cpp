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
const uint8_t DO_SOLOIST_ID_CREATE = 8;
const uint8_t DO_SOLOIST_IS_COMMON_DIVISOR = 9;
const uint8_t DO_SOLOIST_FIND_REFRESH_RATE_FACTORS = 10;
const uint8_t DO_SOLOIST_SET_VSYNC_RATE = 13;
const uint8_t DO_SOLOIST_SET_CALLBACK_AND_TRIGGER = 14;
const uint8_t DO_SOLOIST_ON_VSYNC = 15;
const uint8_t DO_MANAGER_ON_VSYNC = 16;
const uint8_t DO_MANAGER_VSYNC_CALLBACK_INNER = 17;
const uint8_t DO_MANAGER_GETTERS = 18;
const uint8_t DO_MANAGER_FLUSH_FRAME_RATE = 19;
const uint8_t DO_MANAGER_ON_VSYNC_TIMEOUT = 20;
const uint8_t DO_SOLOIST_FLUSH_FRAME_RATE = 21;
const uint8_t DO_SOLOIST_SET_SUB_FRAME_RATE_LINKER_ENABLE = 22;
const uint8_t DO_SOLOIST_ON_VSYNC_TIMEOUT = 23;
const uint8_t DO_MANAGER_DISPATCH_SOLOIST_CALLBACK = 24;
const uint8_t TARGET_SIZE = 25;

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

void DoSoloistIdCreate(FuzzedDataProvider& fdp)
{
    (void)fdp;
    std::shared_ptr<SoloistId> soloistId = SoloistId::Create();
    if (soloistId) {
        soloistId->GetId();
    }
}

void DoSoloistIsCommonDivisor(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloist soloist(id);
    int32_t expectedRate = fdp.ConsumeIntegral<int32_t>();
    int32_t vsyncRate = fdp.ConsumeIntegral<int32_t>();
    soloist.IsCommonDivisor(expectedRate, vsyncRate);
}

void DoSoloistFindRefreshRateFactors(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloist soloist(id);
    int32_t refreshRate = fdp.ConsumeIntegral<int32_t>() % 360;
    soloist.FindRefreshRateFactors(refreshRate);
}

void DoSoloistSetVSyncRate(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloist soloist(id);
    int32_t vsyncRate = fdp.ConsumeIntegral<int32_t>();
    soloist.SetVSyncRate(vsyncRate);
}

void DoSoloistSetCallbackAndTrigger(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloist soloist(id);
    bool useNullCallback = fdp.ConsumeBool();
    if (useNullCallback) {
        soloist.SetCallback(nullptr, nullptr);
    } else {
        auto callback = [](long long timestamp, long long targetTimestamp, void* data) {
            (void)timestamp;
            (void)targetTimestamp;
            (void)data;
        };
        soloist.SetCallback(callback, nullptr);
    }
    soloist.TriggerCallback();
}

void DoSoloistOnVsync(FuzzedDataProvider& fdp)
{
    TimestampType timestamp = fdp.ConsumeIntegral<int64_t>();
    bool useNullClient = fdp.ConsumeBool();
    if (useNullClient) {
        RSDisplaySoloist::OnVsync(timestamp, nullptr);
    } else {
        SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
        RSDisplaySoloist soloist(id);
        soloist.useExclusiveThread_ = false;
        RSDisplaySoloist::OnVsync(timestamp, &soloist);
    }
}

void DoManagerOnVsync(FuzzedDataProvider& fdp)
{
    TimestampType timestamp = fdp.ConsumeIntegral<int64_t>();
    bool useNullClient = fdp.ConsumeBool();
    if (useNullClient) {
        RSDisplaySoloistManager::OnVsync(timestamp, nullptr);
    } else {
        RSDisplaySoloistManager::OnVsync(timestamp, &RSDisplaySoloistManager::GetInstance());
    }
}

void DoManagerVsyncCallbackInner(FuzzedDataProvider& fdp)
{
    auto& manager = RSDisplaySoloistManager::GetInstance();
    bool testInactive = fdp.ConsumeBool();
    if (testInactive) {
        manager.managerStatus_ = ActiveStatus::INACTIVE;
        manager.VsyncCallbackInner(fdp.ConsumeIntegral<int64_t>());
        manager.managerStatus_ = ActiveStatus::ACTIVE;
    } else {
        SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
        std::unique_lock<std::mutex> lock(manager.dataUpdateMtx_);
        manager.idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
        manager.idToSoloistMap_[id]->subStatus_ = ActiveStatus::NEED_REMOVE;
        lock.unlock();
        manager.VsyncCallbackInner(fdp.ConsumeIntegral<int64_t>());
        lock.lock();
        manager.idToSoloistMap_.erase(id);
        lock.unlock();
    }
}

void DoManagerGetters(FuzzedDataProvider& fdp)
{
    (void)fdp;
    auto& manager = RSDisplaySoloistManager::GetInstance();
    manager.GetFrameRateRange();
    manager.GetIdToSoloistMap();
    manager.GetFrameRateLinker();
    manager.GetManagerStatus();
    manager.GetVSyncPeriod();
    manager.GetVSyncRate();
}

void DoManagerFlushFrameRate(FuzzedDataProvider& fdp)
{
    int32_t rate = fdp.ConsumeIntegral<int32_t>();
    RSDisplaySoloistManager::GetInstance().FlushFrameRate(rate);
}

void DoManagerOnVsyncTimeout(FuzzedDataProvider& fdp)
{
    (void)fdp;
    RSDisplaySoloistManager::GetInstance().OnVsyncTimeOut();
}

void DoSoloistFlushFrameRate(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloist soloist(id);
    int32_t rate = fdp.ConsumeIntegral<int32_t>();
    soloist.FlushFrameRate(rate);
}

void DoSoloistSetSubFrameRateLinkerEnable(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloist soloist(id);
    bool enabled = fdp.ConsumeBool();
    soloist.SetSubFrameRateLinkerEnable(enabled);
}

void DoSoloistOnVsyncTimeout(FuzzedDataProvider& fdp)
{
    SoloistIdType id = fdp.ConsumeIntegral<uint32_t>();
    RSDisplaySoloist soloist(id);
    soloist.OnVsyncTimeOut();
}

void DoManagerDispatchSoloistCallback(FuzzedDataProvider& fdp)
{
    TimestampType timestamp = fdp.ConsumeIntegral<int64_t>();
    RSDisplaySoloistManager::GetInstance().DispatchSoloistCallback(timestamp);
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
        case OHOS::Rosen::DO_SOLOIST_ID_CREATE:
            OHOS::Rosen::DoSoloistIdCreate(fdp);
            break;
        case OHOS::Rosen::DO_SOLOIST_IS_COMMON_DIVISOR:
            OHOS::Rosen::DoSoloistIsCommonDivisor(fdp);
            break;
        case OHOS::Rosen::DO_SOLOIST_FIND_REFRESH_RATE_FACTORS:
            OHOS::Rosen::DoSoloistFindRefreshRateFactors(fdp);
            break;
        case OHOS::Rosen::DO_SOLOIST_SET_VSYNC_RATE:
            OHOS::Rosen::DoSoloistSetVSyncRate(fdp);
            break;
        case OHOS::Rosen::DO_SOLOIST_SET_CALLBACK_AND_TRIGGER:
            OHOS::Rosen::DoSoloistSetCallbackAndTrigger(fdp);
            break;
        case OHOS::Rosen::DO_SOLOIST_ON_VSYNC:
            OHOS::Rosen::DoSoloistOnVsync(fdp);
            break;
        case OHOS::Rosen::DO_MANAGER_ON_VSYNC:
            OHOS::Rosen::DoManagerOnVsync(fdp);
            break;
        case OHOS::Rosen::DO_MANAGER_VSYNC_CALLBACK_INNER:
            OHOS::Rosen::DoManagerVsyncCallbackInner(fdp);
            break;
        case OHOS::Rosen::DO_MANAGER_GETTERS:
            OHOS::Rosen::DoManagerGetters(fdp);
            break;
        case OHOS::Rosen::DO_MANAGER_FLUSH_FRAME_RATE:
            OHOS::Rosen::DoManagerFlushFrameRate(fdp);
            break;
        case OHOS::Rosen::DO_MANAGER_ON_VSYNC_TIMEOUT:
            OHOS::Rosen::DoManagerOnVsyncTimeout(fdp);
            break;
        case OHOS::Rosen::DO_SOLOIST_FLUSH_FRAME_RATE:
            OHOS::Rosen::DoSoloistFlushFrameRate(fdp);
            break;
        case OHOS::Rosen::DO_SOLOIST_SET_SUB_FRAME_RATE_LINKER_ENABLE:
            OHOS::Rosen::DoSoloistSetSubFrameRateLinkerEnable(fdp);
            break;
        case OHOS::Rosen::DO_SOLOIST_ON_VSYNC_TIMEOUT:
            OHOS::Rosen::DoSoloistOnVsyncTimeout(fdp);
            break;
        case OHOS::Rosen::DO_MANAGER_DISPATCH_SOLOIST_CALLBACK:
            OHOS::Rosen::DoManagerDispatchSoloistCallback(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}