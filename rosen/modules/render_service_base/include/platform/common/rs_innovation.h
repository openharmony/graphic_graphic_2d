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
#ifdef RS_ENABLE_GL
        GetParallelRenderingFunc();
#endif
    }

    static void CloseInnovationSo()
    {
        if (innovationHandle) {
            ResetParallelCompositionFunc();
            ResetOcclusionCullingFunc();
            ResetQosVsyncFunc();
#ifdef RS_ENABLE_GL
            ResetParallelRenderingFunc();
#endif
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
    static inline void* _s_checkForSerialForced = nullptr;

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

#ifdef RS_ENABLE_GL
    // parallel rendering
    static bool GetParallelRenderingEnabled()
    {
        return _s_parallelRenderingLoaded;
    }

    static inline bool _s_parallelRenderingLoaded = false;

    static inline void* _s_loadReassignment = nullptr;
    static inline void* _s_bfsLoadAssignment = nullptr;
    static inline void* _s_createEglShareContext = nullptr;
    static inline void* _s_intersects = nullptr;
    static inline void* _s_createShareContextPSurface = nullptr;
    static inline void* _s_createSharedContext = nullptr;
    static inline void* _s_loadBalancePushTask = nullptr;
    static inline void* _s_setSubThreadRenderLoad = nullptr;
    static inline void* _s_updateLoadCall = nullptr;
    static inline void* _s_loadBalanceBasedOnLoadCostingCall = nullptr;
    static inline void* _s_loadBalanceBasedOnLoadNumCall = nullptr;
    static inline void* _s_loadBalanceCall = nullptr;
    static inline void* _s_clearLoad = nullptr;
    static inline void* _s_wrapAndPushSuperTaskRegister = nullptr;
    static inline void* _s_getSubContextRegister = nullptr;
    static inline void* _s_canvasPrepareRegister = nullptr;
    static inline void* _s_subDrawRegister = nullptr;
    static inline void* _s_flushAndSubmitFuncRegister = nullptr;
    static inline void* _s_wrapAndPushSuperTask = nullptr;
    static inline void* _s_pushTask = nullptr;
    static inline void* _s_createFence = nullptr;
    static inline void* _s_getSubContext = nullptr;
    static inline void* _s_canvasPrepare = nullptr;
    static inline void* _s_subDraw = nullptr;
    static inline void* _s_subMainThread = nullptr;
    static inline void* _s_setCoreLevel = nullptr;
    static inline void* _s_setSubRenderThreadNum = nullptr;
#endif
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

    static void ResetParallelCompositionFunc()
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

#ifdef RS_ENABLE_GL
    static void GetParallelRenderingFunc()
    {
        if (!innovationHandle) return;
        _s_loadReassignment = dlsym(innovationHandle, "LoadReassignment");
        _s_bfsLoadAssignment = dlsym(innovationHandle, "BfsLoadAssignment");
        _s_createEglShareContext = dlsym(innovationHandle, "CreateEglShareContext");
        _s_intersects = dlsym(innovationHandle, "Intersects");
        _s_createShareContextPSurface = dlsym(innovationHandle, "CreateShareContextPSurface");
        _s_createSharedContext = dlsym(innovationHandle, "CreateSharedContext");
        _s_loadBalancePushTask = dlsym(innovationHandle, "LoadBalancePushTask");
        _s_setSubThreadRenderLoad = dlsym(innovationHandle, "SetSubThreadRenderLoad");
        _s_updateLoadCall = dlsym(innovationHandle, "UpdateLoadCall");
        _s_loadBalanceBasedOnLoadCostingCall = dlsym(innovationHandle, "LoadBalanceBasedOnLoadCostingCall");
        _s_loadBalanceBasedOnLoadNumCall = dlsym(innovationHandle, "LoadBalanceBasedOnLoadNumCall");
        _s_loadBalanceCall = dlsym(innovationHandle, "LoadBalanceCall");
        _s_clearLoad = dlsym(innovationHandle, "ClearLoad");
        _s_wrapAndPushSuperTaskRegister = dlsym(innovationHandle, "WrapAndPushSuperTaskRegister");
        _s_getSubContextRegister = dlsym(innovationHandle, "GetSubContextRegister");
        _s_canvasPrepareRegister = dlsym(innovationHandle, "CanvasPrepareRegister");
        _s_subDrawRegister = dlsym(innovationHandle, "SubDrawRegister");
        _s_flushAndSubmitFuncRegister = dlsym(innovationHandle, "FlushAndSubmitFuncRegister");
        _s_wrapAndPushSuperTask = dlsym(innovationHandle, "WrapAndPushSuperTask");
        _s_pushTask = dlsym(innovationHandle, "PushTask");
        _s_createFence = dlsym(innovationHandle, "CreateFence");
        _s_getSubContext = dlsym(innovationHandle, "GetSubContext");
        _s_canvasPrepare = dlsym(innovationHandle, "CanvasPrepare");
        _s_subDraw = dlsym(innovationHandle, "SubDraw");
        _s_subMainThread = dlsym(innovationHandle, "SubMainThread");
        _s_setCoreLevel = dlsym(innovationHandle, "SetCoreLevel");
        _s_setSubRenderThreadNum = dlsym(innovationHandle, "SetSubRenderThreadNum");

        _s_parallelRenderingLoaded = (_s_loadReassignment != nullptr) &&
            (_s_bfsLoadAssignment != nullptr) &&
            (_s_createEglShareContext != nullptr) &&
            (_s_createSharedContext != nullptr) && (_s_intersects != nullptr) &&
            (_s_createShareContextPSurface != nullptr) &&
            (_s_pushTask != nullptr) && (_s_setCoreLevel != nullptr) &&
            (_s_loadBalanceBasedOnLoadCostingCall != nullptr) &&
            (_s_loadBalanceBasedOnLoadNumCall != nullptr) &&
            (_s_getSubContextRegister!= nullptr) &&
            (_s_getSubContext!= nullptr) && (_s_createFence != nullptr) &&
            (_s_wrapAndPushSuperTask != nullptr) &&
            (_s_wrapAndPushSuperTaskRegister != nullptr) &&
            (_s_canvasPrepareRegister != nullptr) &&
            (_s_canvasPrepare != nullptr) && (_s_subDrawRegister != nullptr) &&
            (_s_subDraw != nullptr) && (_s_subMainThread != nullptr) &&
            (_s_clearLoad != nullptr) && (_s_flushAndSubmitFuncRegister != nullptr) &&
            (_s_loadBalancePushTask != nullptr) && (_s_setSubThreadRenderLoad != nullptr) &&
            (_s_loadBalanceCall != nullptr) && (_s_updateLoadCall != nullptr) &&
            (_s_setSubRenderThreadNum != nullptr);
    }

    static void ResetParallelRenderingFunc()
    {
        if (!innovationHandle) {
            return;
        }
        _s_loadReassignment = nullptr;
        _s_bfsLoadAssignment = nullptr;
        _s_createEglShareContext = nullptr;
        _s_createShareContextPSurface = nullptr;
        _s_intersects = nullptr;
        _s_createSharedContext = nullptr;
        _s_loadBalancePushTask = nullptr;
        _s_updateLoadCall = nullptr;
        _s_loadBalanceBasedOnLoadNumCall = nullptr;
        _s_loadBalanceCall = nullptr;
        _s_wrapAndPushSuperTask = nullptr;
        _s_pushTask = nullptr;
        _s_loadBalanceBasedOnLoadCostingCall = nullptr;
        _s_createFence = nullptr;
        _s_wrapAndPushSuperTaskRegister = nullptr;
        _s_getSubContextRegister = nullptr;
        _s_getSubContext = nullptr;
        _s_canvasPrepareRegister = nullptr;
        _s_canvasPrepare = nullptr;
        _s_subDrawRegister = nullptr;
        _s_subDraw = nullptr;
        _s_subMainThread = nullptr;
        _s_flushAndSubmitFuncRegister = nullptr;
        _s_setSubThreadRenderLoad = nullptr;
        _s_clearLoad = nullptr;
        _s_setCoreLevel = nullptr;
        _s_setSubRenderThreadNum = nullptr;
    }
#endif
};
}
}

#endif // RENDER_SERVICE_BASE_COMMON_RS_INNOVATION_H