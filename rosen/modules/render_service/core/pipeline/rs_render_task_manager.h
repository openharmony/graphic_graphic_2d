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
#ifdef RS_ENABLE_GL
#ifndef RS_RENDER_TASK_MANAGER_H
#define RS_RENDER_TASK_MANAGER_H
#include <chrono>
#include <cstdint>
#include <ratio>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES3/gl32.h>
#include "platform/drawing/rs_surface.h"
#include "include/core/SkSurface.h"
#include "include/core/SkImage.h"
#include "platform/common/rs_innovation.h"
#include "rs_render_task.h"

namespace OHOS {
namespace Rosen {
class RSSurface;
class RSRenderSuperTask;
class RSRenderTask;

class Timer {
    static const size_t maxQueueSize = 100;
    double totalMs = 0;
    std::queue<double> durationMsQueue;
    std::chrono::steady_clock::time_point tp;
public:
    Timer()
    {
        UpdateTp();
    }
    void UpdateTp()
    {
        tp = std::chrono::high_resolution_clock::now();
    }
    void Push(double durMs)
    {
        if (durationMsQueue.size() == maxQueueSize) {
            totalMs -= durationMsQueue.front();
            durationMsQueue.pop();
        }
        totalMs += durMs;
        durationMsQueue.push(durMs);
    }

    void MeasureNow(const std::chrono::steady_clock::time_point& start)
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationMs = now - start;
        Push(durationMs.count());
    }

    void MeasureLastDuration()
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationMs = now - tp;
        tp = now;
        Push(durationMs.count());
    }

    double GetAverageTime()
    {
        if (durationMsQueue.size()) {
            return totalMs / durationMsQueue.size();
        } else {
            return 0;
        }
    }
};

class RSRenderTaskTexture {
public:
    RSRenderTaskTexture(int idx, int offsetX, int offsetY);
    ~RSRenderTaskTexture();

    int GetTaskIdx()
    {
        return taskIdx_;
    }

    void SetTaskIdx(int idx)
    {
        taskIdx_ = idx;
    }

    sk_sp<SkImage> GetTexture()
    {
        return texture_;
    }

    void SetTexture(sk_sp<SkImage> texture);
    void WaitTexture(int textureIndex);
    bool CreateFence();
    bool WaitReleaseFence();
    int GetOffSetX()
    {
        return offsetX_;
    }
    int GetOffSetY()
    {
        return offsetY_;
    }
private:
    int taskIdx_;
    mutable std::mutex mutex_;
    mutable std::condition_variable cv_;
    bool isFinished_;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLSyncKHR eglSync_ = EGL_NO_SYNC_KHR;
    sk_sp<SkImage> texture_ = nullptr;
    int offsetX_;
    int offsetY_;
};

class RSRenderTaskManager {
    struct SubRenderThreadLoad {
        float costing {0.f};
    };

    struct LoadInfo {
        float costing {0.f};
        bool isLastFrame {false};
        int orderedIdx {0};
        float costingAvgFactor {1.0f};
    };

    struct RenderLoad {
        using LoadId = uint64_t;
        using ThreadIdx = uint32_t;
        uint32_t subRenderThreadNum {3};
        float totalRenderCosting {0.f};
        float avgRenderCosting {0.f};
        std::unordered_map<LoadId, LoadInfo> loadInfoMap;
        std::unordered_map<ThreadIdx, SubRenderThreadLoad> threadLoadMap;
        bool isWindowUpdated {false};
        std::vector<uint32_t> loadNumSum {};
        std::vector<uint32_t> lastFrameLoadNumSum {};
        std::vector<float> loadCostingVec {};
        std::vector<LoadId> orderedLoadIds {};
        bool isReassigned {false};
        float threadCostingDist {0.f};
        float errorFactor {0.2f};
        bool isUseAvgCosting = true;
    };

public:
    RSRenderTaskManager();
    ~RSRenderTaskManager();

    void InitTaskManager();
    void SetSubRenderThreadNum(uint32_t num)
    {
        renderLoad_.subRenderThreadNum = bMainThreadUsed_ ? num + 1 : num;
    }

    void PushTask(std::unique_ptr<RSRenderTask> &&task);
    void LoadBalancePushTask(std::unique_ptr<RSRenderTask> &&task);
    void WrapAndPushSuperTask();
    void SaveTexture(int taskIdx, sk_sp<SkImage> texture);
    void MergeTextures(SkCanvas* canvas);
    void DeleteTextures();
    void CreateTaskFence(int32_t taskIdx);
    void WaitReleaseTaskFence(int32_t taskIdx);
    void SetSubThreadRenderLoad(uint32_t threadIdx, uint64_t loadId, float costing);
    bool GetEnableLoadBalance();
    void LoadBalance();
    void UninitTaskManager();
    std::unique_ptr<RSRenderSuperTask> GetTask(int threadIndex);
    void ManagerCallBackRegister();
    void RegisWrapAndPushSuperTaskCallBack();

    bool EnableParallerRendering()
    {
        return true;
    }

    std::queue<RSSurfaceRenderNode *> GetLoadForMainThread() const
    {
        return surfaceNodesQueue_;
    }

    bool GetMainThreadUsed() const
    {
        return bMainThreadUsed_;
    }

    Timer timer;

private:
    void ResetMainThreadRenderingParam();
    void ResetLoadBalanceParam();

    std::queue<RSSurfaceRenderNode *> surfaceNodesQueue_;
    bool bMainThreadUsed_ {false};
    bool bMainThreadLoadAssigned_ {false};
    using LoadReassignmentFunc = void(*)(std::vector<float> &, uint32_t, std::vector<uint32_t> &);
    int superTaskNum_ = 0;
    int pushTaskCount_ = 0;
    std::vector<std::unique_ptr<RSRenderSuperTask>> taskList_;
    std::vector<std::unique_ptr<RSRenderTaskTexture>> taskTextures_ = {};
    mutable std::mutex mutex_;
    mutable std::condition_variable cv_;
    std::unique_ptr<RSRenderSuperTask> cachedSuperTask_;
    std::unordered_map<uint64_t, std::unique_ptr<RSRenderTask>> taskLoad_;
    RenderLoad renderLoad_;
};
}
}
#endif
#endif // RS_RENDER_TASK_MANAGER_H