/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "vsync_manager_agent.h"

namespace OHOS {
namespace Rosen {
RSVsyncManagerAgent::RSVsyncManagerAgent(sptr<VSyncGenerator> vsyncGenerator,
    sptr<VSyncDistributor> rsVsyncDistributor)
: vsyncGenerator_(vsyncGenerator), rsVsyncDistributor_(rsVsyncDistributor)
{

}

RSVsyncManagerAgent::~RSVsyncManagerAgent()
{

}

void RSVsyncManagerAgent::ForceRsDVsync(const std::string& sceneId)
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->ForceRsDVsync(sceneId);
}

void RSVsyncManagerAgent::DVSyncUpdate(uint64_t dvsyncTime, uint64_t vsyncTime)
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->DVSyncUpdate(dvsyncTime, vsyncTime);
}

void RSVsyncManagerAgent::SetTaskEndWithTime(uint64_t time)
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->SetTaskEndWithTime(time);
}

void RSVsyncManagerAgent::NotifyPackageEvent(const std::vector<std::string>& packageList)
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->NotifyPackageEvent(packageList);
}

void RSVsyncManagerAgent::SetBufferInfo(uint64_t id, const std::string &name, uint32_t queueSize,
    int32_t bufferCount, int64_t lastConsumeTime, bool isUrgent)
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->SetBufferInfo(id, name, queueSize, bufferCount, lastConsumeTime, isUrgent);
}

uint64_t RSVsyncManagerAgent::GetRealTimeOffsetOfDvsync(int64_t time)
{
    if (rsVsyncDistributor_ == nullptr) {
        return 0;
    }
    return rsVsyncDistributor_->GetRealTimeOffsetOfDvsync(time);
}

void RSVsyncManagerAgent::SetHardwareTaskNum(uint32_t num)
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->SetHardwareTaskNum(num);
}

uint64_t RSVsyncManagerAgent::CheckVsyncReceivedAndGetRelTs(uint64_t timestamp)
{
    if (rsVsyncDistributor_ == nullptr) {
        return timestamp;
    }
    return rsVsyncDistributor_->CheckVsyncReceivedAndGetRelTs(timestamp);
}

void RSVsyncManagerAgent::PrintConnectionsStatus()
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->PrintConnectionsStatus();
}

void RSVsyncManagerAgent::PrintGeneratorStatus()
{
    if (vsyncGenerator_ == nullptr) {
        return;
    }
    vsyncGenerator_->PrintGeneratorStatus();
}

void RSVsyncManagerAgent::SetFrameIsRender(bool isRender)
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->SetFrameIsRender(isRender);
}

int64_t RSVsyncManagerAgent::GetUiCommandDelayTime()
{
    if (rsVsyncDistributor_ == nullptr) {
        return 0;
    }
    return rsVsyncDistributor_->GetUiCommandDelayTime();
}

void RSVsyncManagerAgent::SetVSyncTimeUpdated()
{
    if (rsVsyncDistributor_ == nullptr) {
        return;
    }
    rsVsyncDistributor_->SetVSyncTimeUpdated();
}

bool RSVsyncManagerAgent::NeedUpdateVSyncTime(int32_t& pid)
{
    if (rsVsyncDistributor_ == nullptr) {
        return false;
    }
    return rsVsyncDistributor_->NeedUpdateVSyncTime(pid);
}

int64_t RSVsyncManagerAgent::GetLastUpdateTime()
{
    if (rsVsyncDistributor_ == nullptr) {
        return 0;
    }
    return rsVsyncDistributor_->GetLastUpdateTime();
}

bool RSVsyncManagerAgent::IsUiDvsyncOn()
{
    if (rsVsyncDistributor_ == nullptr) {
        return false;
    }
    return rsVsyncDistributor_->IsUiDvsyncOn();
}

bool RSVsyncManagerAgent::GetWaitForDvsyncFrame()
{
    return waitForDVSyncFrame_.load();
}

void RSVsyncManagerAgent::SetWaitForDvsyncFrame(bool wait)
{
    waitForDVSyncFrame_.store(wait);
}

} // namespace Rosen
} // namespace OHOS

