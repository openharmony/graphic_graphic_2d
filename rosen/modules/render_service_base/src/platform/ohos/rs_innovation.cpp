/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "platform/common/rs_innovation.h"

#include <dlfcn.h>
#include <parameters.h>

namespace OHOS {
namespace Rosen {
void RSInnovation::OpenInnovationSo()
{
    innovationHandle = dlopen("libgraphic_innovation.z.so", RTLD_NOW);
    GetParallelCompositionFunc();
    GetOcclusionCullingFunc();
    GetQosVSyncFunc();
}

void RSInnovation::CloseInnovationSo()
{
    if (innovationHandle) {
        ResetParallelCompositionFunc();
        ResetOcclusionCullingFunc();
        ResetQosVsyncFunc();
        dlclose(innovationHandle);
    }
}

// parallel composition
bool RSInnovation::GetParallelCompositionEnabled()
{
    return _s_parallelCompositionLoaded &&
        std::atoi((system::GetParameter("rosen.parallelcomposition.enabled", "0")).c_str()) != 0;
}

// occlusion culling
void RSInnovation::UpdateOcclusionCullingSoEnabled()
{
    _s_occlusionCullingSoEnabled =
        std::atoi((system::GetParameter("rosen.occlusion.so.enabled", "0")).c_str()) != 0;
}

bool RSInnovation::UpdateQosVsyncEnabled()
{
    return _s_qosVsyncFuncLoaded &&
        (std::atoi((system::GetParameter("rosen.qos_vsync.enabled", "0")).c_str()) != 0);
}

void RSInnovation::GetParallelCompositionFunc()
{
    if (innovationHandle) {
        _s_createParallelSyncSignal = dlsym(innovationHandle, "CreateParallelSyncSignal");
        _s_signalCountDown = dlsym(innovationHandle, "SignalCountDown");
        _s_signalAwait = dlsym(innovationHandle, "SignalAwait");
        _s_assignTask = dlsym(innovationHandle, "AssignTask");
        _s_removeStoppedThreads = dlsym(innovationHandle, "RemoveStoppedThreads");
        _s_checkForSerialForced = dlsym(innovationHandle, "CheckForSerialForced");
        _s_parallelCompositionLoaded =
            (_s_createParallelSyncSignal != nullptr) &&
            (_s_signalCountDown != nullptr) &&
            (_s_signalAwait != nullptr) &&
            (_s_assignTask != nullptr) &&
            (_s_removeStoppedThreads != nullptr) &&
            (_s_checkForSerialForced != nullptr);
    }
}

void RSInnovation::ResetParallelCompositionFunc()
{
    if (_s_parallelCompositionLoaded) {
        _s_parallelCompositionLoaded = false;
        _s_createParallelSyncSignal = nullptr;
        _s_signalCountDown = nullptr;
        _s_signalAwait = nullptr;
        _s_assignTask = nullptr;
        _s_removeStoppedThreads = nullptr;
        _s_checkForSerialForced = nullptr;
    }
}

void RSInnovation::GetOcclusionCullingFunc()
{
    if (innovationHandle) {
        _s_regionOpFromSo = dlsym(innovationHandle, "RegionOpFromSO");
        _s_occlusionCullingFuncLoaded = (_s_regionOpFromSo != nullptr);
    }
}

void RSInnovation::ResetOcclusionCullingFunc()
{
    if (_s_occlusionCullingFuncLoaded) {
        _s_regionOpFromSo = nullptr;
    }
}

void RSInnovation::GetQosVSyncFunc()
{
    if (innovationHandle) {
        _s_createRSQosService = dlsym(innovationHandle, "CreateRSQosService");
        _s_qosThreadStart = dlsym(innovationHandle, "QosThreadStart");
        _s_qosThreadStop = dlsym(innovationHandle, "QosThreadStop");
        _s_qosSetBoundaryRate = dlsym(innovationHandle, "QosSetBoundaryRate");
        _s_qosOnRSVisibilityChangeCB = dlsym(innovationHandle, "QosOnRSVisibilityChangeCB");
        _s_qosRegisteFuncCB = dlsym(innovationHandle, "QosRegisteFuncCB");
        _s_qosOnRSResetPid = dlsym(innovationHandle, "QosOnRSResetPid");
        _s_qosVsyncFuncLoaded = (_s_createRSQosService != nullptr) &&
                                (_s_qosThreadStart != nullptr) &&
                                (_s_qosThreadStop != nullptr) &&
                                (_s_qosSetBoundaryRate != nullptr) &&
                                (_s_qosOnRSVisibilityChangeCB != nullptr) &&
                                (_s_qosRegisteFuncCB != nullptr) &&
                                (_s_qosOnRSResetPid != nullptr);
    }
}

void RSInnovation::ResetQosVsyncFunc()
{
    if (_s_qosVsyncFuncLoaded) {
        _s_qosVsyncFuncLoaded = false;
        _s_createRSQosService = nullptr;
        _s_qosThreadStart = nullptr;
        _s_qosThreadStop = nullptr;
        _s_qosSetBoundaryRate = nullptr;
        _s_qosOnRSVisibilityChangeCB = nullptr;
        _s_qosRegisteFuncCB = nullptr;
        _s_qosOnRSResetPid = nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS
