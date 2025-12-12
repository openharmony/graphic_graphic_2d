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

#include "feature/hyper_graphic_manager/rs_ui_display_soloist.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

// class RSC_EXPORT SoloistId

std::shared_ptr<SoloistId> SoloistId::Create()
{
    std::shared_ptr<SoloistId> soloistId = std::make_shared<SoloistId>();
    return soloistId;
}

SoloistIdType SoloistId::GenerateId()
{
    static std::atomic<SoloistIdType> currentId_ = 0;
    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    return currentId;
}

SoloistIdType SoloistId::GetId() const
{
    return id_;
}

SoloistId::SoloistId() : id_(GenerateId()) {}

SoloistId::~SoloistId() {}

// class RSC_EXPORT RSDisplaySoloist

RSDisplaySoloist::RSDisplaySoloist(SoloistIdType instanceId)
    : instanceId_(instanceId)
{
    vsyncTimeoutTaskName_ = TIME_OUT_TASK + std::to_string(instanceId_);
}

void RSDisplaySoloist::OnVsync(TimestampType timestamp, void* client)
{
    auto soloist = static_cast<RSDisplaySoloist*>(client);
    if (soloist && soloist->useExclusiveThread_) {
        soloist->VsyncCallbackInner(timestamp);
    }
}

void RSDisplaySoloist::VsyncCallbackInner(TimestampType timestamp)
{
#ifdef RS_ENABLE_GPU
    {
        std::lock_guard<std::mutex> lock(mtx_);
        hasRequestedVsync_ = false;
        subVsyncHandler_->RemoveTask(vsyncTimeoutTaskName_);
    }
    if (subStatus_ != ActiveStatus::ACTIVE) {
        SetSubFrameRateLinkerEnable(false);
        return;
    }

    if (JudgeWhetherSkip(timestamp)) {
        TriggerCallback();
    }

    if (callback_.first) {
        RequestNextVSync();
    }
    FlushFrameRate(frameRateRange_.preferred_);
#endif
}

void RSDisplaySoloist::TriggerCallback()
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    if (callback_.first) {
        RS_TRACE_NAME_FMT("SubDisplaySoloistId: %d, RefreshRate: %d, FrameRateRange: {%d, %d, %d}, "
            "drawFPS: %d, rate: %d, count: %d", instanceId_, GetVSyncRate(),
            frameRateRange_.min_, frameRateRange_.max_, frameRateRange_.preferred_, drawFPS_,
            currRate_, currCnt_);
        ROSEN_LOGD("SubDisplaySoloistId: %{public}d, RefreshRate: %{public}d, "
            "FrameRateRange: {%{public}d, %{public}d, %{public}d}, "
            "drawFPS: %{public}d, rate: %{public}d, count: %{public}d", instanceId_, GetVSyncRate(),
            frameRateRange_.min_, frameRateRange_.max_, frameRateRange_.preferred_, drawFPS_,
            currRate_, currCnt_);
        callback_.first(timestamp_, targetTimestamp_, callback_.second);
    }
}

void RSDisplaySoloist::SetCallback(DisplaySoloistOnFrameCallback cb, void* params)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callback_ = {cb, params};
}

void RSDisplaySoloist::Init()
{
#ifdef RS_ENABLE_GPU
    if (useExclusiveThread_ && (!subReceiver_ || !hasInitVsyncReceiver_)) {
        if (!subVsyncHandler_) {
            subVsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(
                AppExecFwk::EventRunner::Create("OS_" + std::to_string(instanceId_) + "_SubDisplaySoloist"));
        }
        auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
        frameRateLinker_ = OHOS::Rosen::RSFrameRateLinker::Create();
        while (!subReceiver_) {
            subReceiver_ = rsClient.CreateVSyncReceiver(std::to_string(instanceId_)+"_SubDisplaySoloist",
                frameRateLinker_->GetId(), subVsyncHandler_);
        }
        subReceiver_->Init();
        subStatus_ = ActiveStatus::ACTIVE;
        hasInitVsyncReceiver_ = true;
    }
#endif
}

void RSDisplaySoloist::RequestNextVSync()
{
#ifdef RS_ENABLE_GPU
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (destroyed_) {
            return;
        }
        Init();
        if (hasRequestedVsync_) {
            return;
        }
        if (subVsyncHandler_) {
            subVsyncHandler_->RemoveTask(vsyncTimeoutTaskName_);
            ROSEN_LOGD("%{public}s SubDisplaySoloistId: %{public}d PostTimeoutTask", __func__, instanceId_);
            subVsyncHandler_->PostTask(vsyncTimeoutCallback_, vsyncTimeoutTaskName_, TIME_OUT_MILLISECONDS);
        }
    }

    if (subReceiver_ && useExclusiveThread_) {
        subReceiver_->RequestNextVSync(subFrameCallback_);
        hasRequestedVsync_ = true;
    }
#endif
}

void RSDisplaySoloist::OnVsyncTimeOut()
{
    ROSEN_LOGD("%{public}s SubDisplaySoloistId: %{public}d Vsync time out", __func__, instanceId_);
    std::lock_guard<std::mutex> lock(mtx_);
    hasRequestedVsync_ = false;
}

void RSDisplaySoloist::FlushFrameRate(int32_t rate)
{
    if (frameRateLinker_ && frameRateLinker_->IsEnable()) {
        FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, rate, DISPLAY_SOLOIST_FRAME_RATE_TYPE};
        frameRateLinker_->UpdateFrameRateRangeImme(range);
    }
}

void RSDisplaySoloist::SetSubFrameRateLinkerEnable(bool enabled)
{
    if (frameRateLinker_) {
        if (!enabled) {
            FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, FRAME_RATE_0};
            frameRateLinker_->UpdateFrameRateRangeImme(range);
        }
        frameRateLinker_->SetEnable(enabled);
    }
}

bool RSDisplaySoloist::JudgeWhetherSkip(TimestampType timestamp)
{
    int32_t vsyncRate = GetVSyncRate();
    drawFPS_ = FindMatchedRefreshRate(vsyncRate, frameRateRange_.preferred_);
    if (drawFPS_ < frameRateRange_.min_ || drawFPS_ > frameRateRange_.max_) {
        drawFPS_ = SearchMatchedRate(frameRateRange_, vsyncRate);
    }

    if (drawFPS_ == 0) {
        return false;
    }

    int32_t currRate = vsyncRate / drawFPS_;
    int64_t currVsyncPeriod = GetVSyncPeriod();
    timestamp_ = timestamp;
    targetTimestamp_ = timestamp + currRate * currVsyncPeriod;

    if (currRate != currRate_) {
        currCnt_ = 0;
    }
    currRate_ = currRate;

    bool isSkip = false;
    if (currCnt_ == 0) {
        isSkip = true;
    }

    if ((currRate_ > 0) && (currRate_ - currCnt_) == 1) {
        currCnt_ = -1;
    }
    currCnt_++;

    return isSkip;
}

bool RSDisplaySoloist::IsCommonDivisor(int32_t expectedRate, int32_t vsyncRate)
{
    if (expectedRate == 0 || vsyncRate == 0) {
        return false;
    }

    int32_t n =  vsyncRate / expectedRate;
    if (expectedRate * n == vsyncRate) {
        return true;
    }
    return false;
}

std::vector<int32_t> RSDisplaySoloist::FindRefreshRateFactors(int32_t refreshRate)
{
    std::vector<int32_t> refreshRateFactors;
    for (int32_t i = 1; i * i <= refreshRate; ++i) {
        if (refreshRate % i == 0) {
            refreshRateFactors.emplace_back(i);
            if (i != refreshRate / i) {
                refreshRateFactors.emplace_back(refreshRate / i);
            }
        }
    }
    sort(refreshRateFactors.begin(), refreshRateFactors.end());
    return refreshRateFactors;
}

void RSDisplaySoloist::FindAllRefreshRateFactors()
{
    std::set<int32_t> allFactors;
    for (const auto& refreshRate : REFRESH_RATE_LIST) {
        std::vector<int32_t> factors = FindRefreshRateFactors(refreshRate);
        allFactors.insert(factors.begin(), factors.end());
    }
    REFRESH_RATE_FACTORS.clear();
    std::copy(allFactors.begin(), allFactors.end(), std::back_inserter(REFRESH_RATE_FACTORS));
    return;
}

int32_t RSDisplaySoloist::FindAccurateRefreshRate(int32_t approximateRate)
{
    if (REFRESH_RATE_FACTORS.empty()) {
        ROSEN_LOGE("%{public}s REFRESH_RATE_FACTORS is Empty.", __func__);
        return 0;
    }
    auto it = std::lower_bound(REFRESH_RATE_FACTORS.begin(), REFRESH_RATE_FACTORS.end(), approximateRate);
    if (it == REFRESH_RATE_FACTORS.begin()) {
        return *it;
    } else if (it == REFRESH_RATE_FACTORS.end()) {
        // Indicate the end element of vector.
        return *(it - 1);
    }
    return std::abs(*it - approximateRate) < std::abs(*(it - 1) - approximateRate) ? *it : *(it - 1);
}

int32_t RSDisplaySoloist::FindMatchedRefreshRate(int32_t vsyncRate, int32_t targetRate)
{
    if (targetRate == 0 || targetRate > vsyncRate) {
        return vsyncRate;
    }

    if (IsCommonDivisor(targetRate, vsyncRate)) {
        return targetRate;
    }

    if (!RATE_TO_FACTORS.count(vsyncRate)) {
        RATE_TO_FACTORS[vsyncRate] = FindRefreshRateFactors(vsyncRate);
    }

    std::vector<int32_t> refreshRateFactors = RATE_TO_FACTORS[vsyncRate];
    if (refreshRateFactors.empty()) {
        return 0;
    }
    auto it = std::lower_bound(refreshRateFactors.begin(), refreshRateFactors.end(), targetRate);
    if (it == refreshRateFactors.begin()) {
        return *it;
    } else if (it == refreshRateFactors.end()) {
        // Indicate the end element of vector.
        return *(it - 1);
    }
    return std::abs(*it - targetRate) < std::abs(*(it - 1) - targetRate) ? *it : *(it - 1);
}

int32_t RSDisplaySoloist::SearchMatchedRate(const FrameRateRange& frameRateRange,
    int32_t vsyncRate, int32_t iterCount)
{
    if (vsyncRate != 0 && iterCount >= vsyncRate) {
        return FindMatchedRefreshRate(vsyncRate, frameRateRange.preferred_);
    }

    if (iterCount == 0 || vsyncRate == 0) {
        return vsyncRate;
    }

    int32_t expectedRate = vsyncRate / iterCount;
    if (frameRateRange.min_ <= expectedRate &&
        frameRateRange.max_ >= expectedRate) {
        return FindMatchedRefreshRate(vsyncRate, expectedRate);
    }

    return SearchMatchedRate(frameRateRange, vsyncRate, ++iterCount);
}

int64_t RSDisplaySoloist::GetVSyncPeriod()
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        Init();
    }
    int64_t period = 0;
    if (subReceiver_) {
#ifdef __OHOS__
        subReceiver_->GetVSyncPeriod(period);
#endif
    }

    if (period == 0 && !useExclusiveThread_) {
        RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
        period = soloistManager.GetVSyncPeriod();
    }

    return period;
}

int32_t RSDisplaySoloist::GetVSyncRate()
{
    int64_t vsyncPeriod = GetVSyncPeriod();
    if (vsyncPeriod == 0 && !useExclusiveThread_) {
        RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
        vsyncPeriod = soloistManager.GetVSyncPeriod();
    }
    int32_t approximateRate = static_cast<int32_t>(std::ceil(SECOND_IN_NANO / vsyncPeriod));
    std::call_once(COMPUTE_FACTORS_FLAG, [this] () { FindAllRefreshRateFactors(); });
    int32_t rate = FindAccurateRefreshRate(approximateRate);
    SetVSyncRate(rate);
    if (!useExclusiveThread_) {
        RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
        soloistManager.SetVSyncRate(rate);
    }
    return rate;
}

bool RSDisplaySoloist::SetVSyncRate(int32_t vsyncRate)
{
    if (vsyncRate < 0) {
        return false;
    }

    if (sourceVsyncRate_ == vsyncRate) {
        return false;
    }
    sourceVsyncRate_ = vsyncRate;
    return true;
}

// RSDisplaySoloistManager

RSDisplaySoloistManager& RSDisplaySoloistManager::GetInstance() noexcept
{
    static RSDisplaySoloistManager soloistManager;
    soloistManager.InitVsyncReceiver();
    return soloistManager;
}

RSDisplaySoloistManager::~RSDisplaySoloistManager() noexcept {}

void RSDisplaySoloistManager::InitVsyncReceiver()
{
#ifdef RS_ENABLE_GPU
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [this]() {
        if (!vsyncHandler_) {
            vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(
                AppExecFwk::EventRunner::Create("OS_MainDisplaySoloist"));
        }
        auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
        frameRateLinker_ = OHOS::Rosen::RSFrameRateLinker::Create();
        while (!receiver_) {
            receiver_ = rsClient.CreateVSyncReceiver("MainDisplaySoloist",
                frameRateLinker_->GetId(), vsyncHandler_);
        }

        receiver_->Init();
        managerStatus_ = ActiveStatus::ACTIVE;
    });
#endif
}

void RSDisplaySoloistManager::RequestNextVSync()
{
#ifdef RS_ENABLE_GPU
    if (receiver_ == nullptr) {
        ROSEN_LOGE("%{public}s, VSyncReceiver is null.", __func__);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (managerStatus_ != ActiveStatus::ACTIVE) {
            return;
        }
        InitVsyncReceiver();
        if (vsyncHandler_) {
            vsyncHandler_->RemoveTask(vsyncTimeoutTaskName_);
            ROSEN_LOGD("%{public}s MainDisplaySoloistManager PostTimeoutTask", __func__);
            vsyncHandler_->PostTask(vsyncTimeoutCallback_, vsyncTimeoutTaskName_, TIME_OUT_MILLISECONDS);
        }
    }

    receiver_->RequestNextVSync(managerFrameCallback_);
#endif
}

void RSDisplaySoloistManager::OnVsync(TimestampType timestamp, void* client)
{
    auto soloistManager = static_cast<RSDisplaySoloistManager*>(client);
    if (soloistManager == nullptr) {
        ROSEN_LOGE("%{public}s, soloistManager is null.", __func__);
        return;
    }
    soloistManager->VsyncCallbackInner(timestamp);
}

void RSDisplaySoloistManager::VsyncCallbackInner(TimestampType timestamp)
{
    if (managerStatus_ != ActiveStatus::ACTIVE) {
        SetMainFrameRateLinkerEnable(false);
        return;
    }

    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    for (auto it = idToSoloistMap_.begin(); it != idToSoloistMap_.end();) {
        if (it->second && it->second->subStatus_ == ActiveStatus::NEED_REMOVE) {
            it = idToSoloistMap_.erase(it);
        } else {
            ++it;
        }
    }
    lock.unlock();

    DispatchSoloistCallback(timestamp);
}

void RSDisplaySoloistManager::DispatchSoloistCallback(TimestampType timestamp)
{
    bool isNeedRequestVSync = false;
    frameRateRange_.Reset();

    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    IdToSoloistMapType idToSoloistMapBackup(idToSoloistMap_);
    lock.unlock();

    for (const auto& [id, displaySoloist] : idToSoloistMapBackup) {
        if (displaySoloist && displaySoloist->useExclusiveThread_) {
            displaySoloist->RequestNextVSync();
            continue;
        }

        if (displaySoloist && displaySoloist->JudgeWhetherSkip(timestamp) &&
            displaySoloist->subStatus_ == ActiveStatus::ACTIVE) {
            displaySoloist->TriggerCallback();
            if (displaySoloist->frameRateRange_.IsValid()) {
                frameRateRange_.Merge(displaySoloist->frameRateRange_);
            }
        }
        bool isActiveSoloist = false;
        if (displaySoloist && displaySoloist->subStatus_ == ActiveStatus::ACTIVE) {
            isActiveSoloist = true;
        }
        isNeedRequestVSync = isNeedRequestVSync || isActiveSoloist;
    }

    if (isNeedRequestVSync && managerStatus_ == ActiveStatus::ACTIVE) {
        RequestNextVSync();
        FlushFrameRate(frameRateRange_.preferred_);
    } else {
        FlushFrameRate(FRAME_RATE_0);
    }
}

void RSDisplaySoloistManager::Start(SoloistIdType id)
{
    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    if (!idToSoloistMap_.count(id)) {
        idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
    }
    managerStatus_ = ActiveStatus::ACTIVE;
    idToSoloistMap_[id]->subStatus_ = ActiveStatus::ACTIVE;
    idToSoloistMap_[id]->SetSubFrameRateLinkerEnable(true);
    idToSoloistMap_[id]->RequestNextVSync();
    lock.unlock();
    RequestNextVSync();
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d.", __func__, id);
    return;
}

void RSDisplaySoloistManager::Stop(SoloistIdType id)
{
    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    if (!idToSoloistMap_.count(id)) {
        return;
    }
    idToSoloistMap_[id]->subStatus_ = ActiveStatus::INACTIVE;
    idToSoloistMap_[id]->SetSubFrameRateLinkerEnable(false);
    idToSoloistMap_[id]->RequestNextVSync();
    lock.unlock();
    RequestNextVSync();
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d.", __func__, id);
    return;
}

void RSDisplaySoloistManager::RemoveSoloist(SoloistIdType id)
{
    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    if (idToSoloistMap_.count(id)) {
        idToSoloistMap_[id]->subStatus_ = ActiveStatus::NEED_REMOVE;
        idToSoloistMap_[id]->SetSubFrameRateLinkerEnable(false);
        idToSoloistMap_[id]->RequestNextVSync();
        RequestNextVSync();
    }
    lock.unlock();
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d.", __func__, id);
    return;
}

void RSDisplaySoloistManager::InsertOnVsyncCallback(SoloistIdType id, DisplaySoloistOnFrameCallback cb, void* data)
{
    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    if (!idToSoloistMap_.count(id)) {
        idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
    }
    idToSoloistMap_[id]->SetCallback(cb, data);
    lock.unlock();
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d.", __func__, id);
    return;
}

void RSDisplaySoloistManager::InsertFrameRateRange(SoloistIdType id, FrameRateRange frameRateRange)
{
    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    if (!idToSoloistMap_.count(id)) {
        idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
    }
    idToSoloistMap_[id]->frameRateRange_ = frameRateRange;
    lock.unlock();
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d expected:%{public}d.", __func__, id, frameRateRange_.preferred_);
    return;
}

void RSDisplaySoloistManager::InsertUseExclusiveThreadFlag(SoloistIdType id, bool useExclusiveThread)
{
    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    if (!idToSoloistMap_.count(id)) {
        idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
    }
    idToSoloistMap_[id]->useExclusiveThread_ = useExclusiveThread;
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d useExclusiveThread:%{public}d.", __func__, id, useExclusiveThread);
    return;
}

void RSDisplaySoloistManager::FlushFrameRate(int32_t rate)
{
    if (frameRateLinker_ && frameRateLinker_->IsEnable()) {
        FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, rate, DISPLAY_SOLOIST_FRAME_RATE_TYPE};
        frameRateLinker_->UpdateFrameRateRangeImme(range);
    }
}

void RSDisplaySoloistManager::SetMainFrameRateLinkerEnable(bool enabled)
{
    if (frameRateLinker_) {
        if (!enabled) {
            FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, FRAME_RATE_0};
            frameRateLinker_->UpdateFrameRateRangeImme(range);
            managerStatus_ = ActiveStatus::INACTIVE;
        } else {
            managerStatus_ = ActiveStatus::ACTIVE;
        }
        frameRateLinker_->SetEnable(enabled);
    }

    std::unique_lock<std::mutex> lock(dataUpdateMtx_);
    IdToSoloistMapType idToSoloistMapBackup(idToSoloistMap_);
    lock.unlock();

    for (const auto& [id, displaySoloist] : idToSoloistMapBackup) {
        displaySoloist->SetSubFrameRateLinkerEnable(enabled);
    }

    RequestNextVSync();
}

FrameRateRange RSDisplaySoloistManager::GetFrameRateRange()
{
    return frameRateRange_;
}

IdToSoloistMapType RSDisplaySoloistManager::GetIdToSoloistMap()
{
    return idToSoloistMap_;
}

std::shared_ptr<RSFrameRateLinker> RSDisplaySoloistManager::GetFrameRateLinker()
{
    return frameRateLinker_;
}

enum ActiveStatus RSDisplaySoloistManager::GetManagerStatus()
{
    return managerStatus_;
}

int64_t RSDisplaySoloistManager::GetVSyncPeriod() const
{
    int64_t period = 0;
    if (receiver_ == nullptr) {
        ROSEN_LOGE("%{public}s, VSyncReceiver is null.", __func__);
        return period;
    }

#ifdef __OHOS__
    receiver_->GetVSyncPeriod(period);
#endif
    return period;
}

bool RSDisplaySoloistManager::SetVSyncRate(int32_t vsyncRate)
{
    if (vsyncRate < 0) {
        return false;
    }

    if (sourceVsyncRate_ == vsyncRate) {
        return false;
    }
    sourceVsyncRate_ = vsyncRate;
    return true;
}

int32_t RSDisplaySoloistManager::GetVSyncRate() const
{
    return sourceVsyncRate_;
}

void RSDisplaySoloistManager::OnVsyncTimeOut()
{
    ROSEN_LOGD("%{public}s MainDisplaySoloistManager: Vsync time out", __func__);
}

} // namespace Rosen
} // namespace OHOS
