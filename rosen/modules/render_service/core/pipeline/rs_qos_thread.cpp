/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_qos_thread.h"

#include <vector>

#include "vsync_distributor.h"
#include "platform/common/rs_innovation.h"

namespace OHOS::Rosen {
std::once_flag RSQosThread::flag_;
RSQosThread* RSQosThread::instance_ = nullptr;
sptr<VSyncDistributor> RSQosThread::appVSyncDistributor_ = nullptr;

RSQosThread* RSQosThread::GetInstance()
{
    std::call_once(flag_, &RSQosThread::Init);
    return instance_;
}

void RSQosThread::Init()
{
    instance_ = new RSQosThread();
    if (!RSInnovation::_s_qosVsyncFuncLoaded) {
        return;
    }

    using CreateQosServiceFunc = void* (*)();
    using QosSetBoundaryRateFunc = void* (*)(int, int);
    using QosSetRegisteFuncCBFunc =
        void* (*)(std::function<void(uint32_t, int)>,
                  std::function<void(std::vector<std::pair<uint32_t, int> >&)>);

    auto CreateQosService = (CreateQosServiceFunc)RSInnovation::_s_createRSQosService;
    auto QosSetBoundaryRate = (QosSetBoundaryRateFunc)RSInnovation::_s_qosSetBoundaryRate;
    auto QosRegisteFuncCB = (QosSetRegisteFuncCBFunc)RSInnovation::_s_qosRegisteFuncCB;

    QosSetBoundaryRate(MIN_RATE, MAX_RATE);
    QosRegisteFuncCB(SetQosVSyncRate, GetQosVSyncRateInfos);
    CreateQosService();
}

void RSQosThread::ThreadStart()
{
    if (!RSInnovation::_s_qosVsyncFuncLoaded) {
        return;
    }

    using QosTreadStartFunc = void* (*)();
    auto QosTreadStart = (QosTreadStartFunc)RSInnovation::_s_qosThreadStart;
    QosTreadStart();
}

void RSQosThread::ThreadStop()
{
    if (!RSInnovation::_s_qosVsyncFuncLoaded) {
        return;
    }

    using QosTreadStopFunc = void* (*)();
    auto QosTreadStop = (QosTreadStopFunc)RSInnovation::_s_qosThreadStop;
    QosTreadStop();
}

void RSQosThread::GetQosVSyncRateInfos(std::vector<std::pair<uint32_t, int32_t>>& appsRateVec)
{
    if (appVSyncDistributor_ == nullptr) {
        return;
    }

    appVSyncDistributor_->GetQosVSyncRateInfos(appsRateVec);
}

void RSQosThread::SetQosVSyncRate(uint32_t pid, int32_t rate)
{
    if (appVSyncDistributor_ == nullptr) {
        return;
    }

    appVSyncDistributor_->SetQosVSyncRate(pid, rate);
}

void RSQosThread::ResetQosPid()
{
    using QosOnRSResetPidFunc = void* (*)();

    auto QosOnRSResetPid = (QosOnRSResetPidFunc)RSInnovation::_s_qosOnRSResetPid;
    QosOnRSResetPid();
}

void RSQosThread::OnRSVisibilityChangeCB(std::map<uint32_t, bool>& pidVisMap)
{
    if (!qosCal_) {
        return;
    }

    using QosOnRSVisibilityChangeCBFunc = void (*)(std::map<uint32_t, bool>&);

    auto QosOnRSVisibilityChangeCB = (QosOnRSVisibilityChangeCBFunc)RSInnovation::_s_qosOnRSVisibilityChangeCB;
    QosOnRSVisibilityChangeCB(pidVisMap);
}
} // amespace OHOS::Rosen