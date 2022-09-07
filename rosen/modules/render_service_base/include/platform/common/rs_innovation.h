/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_INNOVATION_H
#define RENDER_SERVICE_BASE_COMMON_RS_INNOVATION_H
#include <dlfcn.h>
#include <parameters.h>
namespace OHOS {
namespace Rosen {
class RSInnovation {
public:
    ~RSInnovation() = default;
    static void OpenInnovationSo()
    {
        innovationHandle = dlopen("libgraphic_innovation.z.so", RTLD_NOW);
        GetParallelCompositionFunc();
        GetOcclusionCullingFunc();
        GetQosVSyncFunc();
    }

    static void CloseInnovationSo()
    {
        if (innovationHandle) {
            ResetParallelCompositionFunc();
            ResetOcclusionCullingFunc();
            ResetQosVsyncFunc();
            dlclose(innovationHandle);
        }
    }

    static inline void* innovationHandle = nullptr;

    // parallel composition
    static bool GetParallelCompositionEnabled()
    {
        return _s_parallelCompositionLoaded &&
            std::atoi((system::GetParameter("rosen.parallelcomposition.enabled", "0")).c_str()) != 0;
    }

    static inline bool _s_parallelCompositionLoaded = false;
    static inline void* _s_createParallelSyncSignal = nullptr;
    static inline void* _s_signalCountDown = nullptr;
    static inline void* _s_signalAwait = nullptr;
    static inline void* _s_assignTask = nullptr;
    static inline void* _s_removeStoppedThreads = nullptr;

    // occlusion culling
    static void UpdateOcclusionCullingSoEnabled()
    {
        _s_occlusionCullingSoEnabled =
            std::atoi((system::GetParameter("rosen.occlusion.so.enabled", "0")).c_str()) != 0;
    }
    static inline bool _s_occlusionCullingFuncLoaded = false;
    static inline bool _s_occlusionCullingSoEnabled = false;
    static inline void* _s_regionOpFromSo = nullptr;

    // qos vsync
    static bool UpdateQosVsyncEnabled()
    {
        return _s_qosVsyncFuncLoaded &&
            (std::atoi((system::GetParameter("rosen.qos_vsync.enabled", "0")).c_str()) != 0);
    }
    static inline bool _s_qosVsyncFuncLoaded = false;
    static inline void* _s_createRSQosService = nullptr;
    static inline void* _s_qosThreadStart = nullptr;
    static inline void* _s_qosThreadStop = nullptr;
    static inline void* _s_qosSetBoundaryRate = nullptr;
    static inline void* _s_qosOnRSVisibilityChangeCB = nullptr;
    static inline void* _s_qosRegisteFuncCB = nullptr;
    static inline void* _s_qosOnRSResetPid = nullptr;

private:
    RSInnovation() = default;

    static void GetParallelCompositionFunc()
    {
        if (innovationHandle) {
            _s_createParallelSyncSignal = dlsym(innovationHandle, "CreateParallelSyncSignal");
            _s_signalCountDown = dlsym(innovationHandle, "SignalCountDown");
            _s_signalAwait = dlsym(innovationHandle, "SignalAwait");
            _s_assignTask = dlsym(innovationHandle, "AssignTask");
            _s_removeStoppedThreads = dlsym(innovationHandle, "RemoveStoppedThreads");
            _s_parallelCompositionLoaded =
                (_s_createParallelSyncSignal != nullptr) &&
                (_s_signalCountDown != nullptr) &&
                (_s_signalAwait != nullptr) &&
                (_s_assignTask != nullptr) &&
                (_s_removeStoppedThreads != nullptr);
        }
    }

    static void ResetParallelCompositionFunc()
    {
        if (_s_parallelCompositionLoaded) {
            _s_parallelCompositionLoaded = false;
            _s_createParallelSyncSignal = nullptr;
            _s_signalCountDown = nullptr;
            _s_signalAwait = nullptr;
            _s_assignTask = nullptr;
            _s_removeStoppedThreads = nullptr;
        }
    }

    static void GetOcclusionCullingFunc()
    {
        if (innovationHandle) {
            _s_regionOpFromSo = dlsym(innovationHandle, "RegionOpFromSO");
            _s_occlusionCullingFuncLoaded = (_s_regionOpFromSo != nullptr);
        }
    }

    static void ResetOcclusionCullingFunc()
    {
        if (_s_occlusionCullingFuncLoaded) {
            _s_regionOpFromSo = nullptr;
        }
    }
    static void GetQosVSyncFunc()
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

    static void ResetQosVsyncFunc()
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
};
}
}

#endif // RENDER_SERVICE_BASE_COMMON_RS_INNOVATION_H