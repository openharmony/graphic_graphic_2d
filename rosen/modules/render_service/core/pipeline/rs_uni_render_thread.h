/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_THREAD_H
#define RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_THREAD_H

#include <memory>
#include <string>
#include <vector>

#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_context.h"
#include "params/rs_render_thread_params.h"
#ifdef RES_SCHED_ENABLE
#include "vsync_system_ability_listener.h"
#endif

namespace OHOS {
namespace Rosen {
namespace DrawableV2 {
class RSRenderNodeDrawable;
class RSDisplayRenderNodeDrawable;
}

class RSUniRenderThread {
public:
    using Callback = std::function<void()>;
    static RSUniRenderThread& Instance();

    // disable copy and move
    RSUniRenderThread(const RSUniRenderThread&) = delete;
    RSUniRenderThread& operator=(const RSUniRenderThread&) = delete;
    RSUniRenderThread(RSUniRenderThread&&) = delete;
    RSUniRenderThread& operator=(RSUniRenderThread&&) = delete;

    void Start();
    void InitGrContext();
    void RenderFrames();
    void Sync(std::unique_ptr<RSRenderThreadParams>&& stagingRenderThreadParams);
    void PostTask(const std::function<void()>& task);
    void RemoveTask(const std::string& name);
    void PostRTTask(const std::function<void()>& task);
    void PostImageReleaseTask(const std::function<void()>& task);
    void RunImageReleaseTask();
    void ClearResource();
    void PostTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
        AppExecFwk::EventQueue::Priority priority = AppExecFwk::EventQueue::Priority::HIGH);
    void PostSyncTask(const std::function<void()>& task);
    bool IsIdle() const;
    void Render();
    void ReleaseSelfDrawingNodeBuffer();
    std::shared_ptr<RSBaseRenderEngine> GetRenderEngine() const;
    void NotifyDisplayNodeBufferReleased();
    bool WaitUntilDisplayNodeBufferReleased(DrawableV2::RSDisplayRenderNodeDrawable& displayNodeDrawable);

    uint64_t GetCurrentTimestamp() const;
    int64_t GetActualTimestamp() const;
    uint64_t GetVsyncId() const;
    bool GetForceRefreshFlag() const;
    uint32_t GetPendingScreenRefreshRate() const;
    uint64_t GetPendingConstraintRelativeTime() const;

    void PurgeCacheBetweenFrames();
    void ClearMemoryCache(ClearMemoryMoment moment, bool deeply, pid_t pid = -1);
    void DefaultClearMemoryCache();
    void PostClearMemoryTask(ClearMemoryMoment moment, bool deeply, bool isDefaultClean);
    void MemoryManagementBetweenFrames();
    void FlushGpuMemoryInWaitQueueBetweenFrames();
    void SuppressGpuCacheBelowCertainRatioBetweenFrames();
    void ResetClearMemoryTask(const std::unordered_map<NodeId, bool>&& ids, bool isDoDirectComposition = false);
    void SetDefaultClearMemoryFinished(bool isFinished);
    bool IsDefaultClearMemoryFinished();
    bool GetClearMemoryFinished() const;
    void SetClearMemoryFinished();
    bool GetClearMemDeeply() const;
    void SetClearMoment(ClearMemoryMoment moment);
    ClearMemoryMoment GetClearMoment() const;
    uint32_t GetRefreshRate() const;
    void DumpMem(DfxString& log);
    void TrimMem(std::string& dumpString, std::string& type);
    std::shared_ptr<Drawing::Image> GetWatermarkImg();
    uint64_t GetFrameCount() const
    {
        return frameCount_;
    }
    void IncreaseFrameCount()
    {
        frameCount_++;
    }
    bool GetWatermarkFlag() const;

    bool IsCurtainScreenOn() const;
    bool IsColorFilterModeOn() const;
    bool IsHighContrastTextModeOn() const;

    static void SetCaptureParam(const CaptureParam& param);
    static CaptureParam& GetCaptureParam();
    static void ResetCaptureParam();
    static bool IsInCaptureProcess();
    std::vector<NodeId>& GetDrawStatusVec()
    {
        return curDrawStatusVec_;
    }
    const std::unique_ptr<RSRenderThreadParams>& GetRSRenderThreadParams() const
    {
        return RSRenderThreadParamsManager::Instance().GetRSRenderThreadParams();
    }

    void RenderServiceTreeDump(std::string& dumpString);
    void ReleaseSurface();
    void AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface);

    bool IsMainLooping() const
    {
        return mainLooping_.load();
    }
    void SetMainLooping(bool isMainLooping)
    {
        mainLooping_.store(isMainLooping);
    }
    bool GetDiscardJankFrames() const
    {
        return discardJankFrames_.load();
    }
    void SetDiscardJankFrames(bool discardJankFrames)
    {
        if (discardJankFrames_.load() != discardJankFrames) {
            discardJankFrames_.store(discardJankFrames);
        }
    }
    bool GetSkipJankAnimatorFrame() const
    {
        return skipJankAnimatorFrame_.load();
    }
    void SetSkipJankAnimatorFrame(bool skipJankAnimatorFrame)
    {
        skipJankAnimatorFrame_.store(skipJankAnimatorFrame);
    }
    void UpdateDisplayNodeScreenId();
    uint32_t GetDynamicRefreshRate() const;
    pid_t GetTid() const
    {
        return tid_;
    }

    void SetAcquireFence(sptr<SyncFence> acquireFence);

    // vma cache
    bool GetVmaOptimizeFlag() const
    {
        return vmaOptimizeFlag_; // global flag
    }
    void SetVmaCacheStatus(bool flag); // dynmic flag

    void SetBlackList(std::unordered_set<NodeId> blackList)
    {
        std::lock_guard<std::mutex> lock(nodeListMutex_);
        blackList_ = blackList;
    }

    const std::unordered_set<NodeId> GetBlackList() const
    {
        std::lock_guard<std::mutex> lock(nodeListMutex_);
        return blackList_;
    }

    void SetWhiteList(const std::unordered_set<NodeId>& whiteList)
    {
        std::lock_guard<std::mutex> lock(nodeListMutex_);
        whiteList_ = whiteList;
    }

    const std::unordered_set<NodeId> GetWhiteList() const
    {
        std::lock_guard<std::mutex> lock(nodeListMutex_);
        return whiteList_;
    }

private:
    RSUniRenderThread();
    ~RSUniRenderThread() noexcept;
    void Inittcache();
    void PerfForBlurIfNeeded();

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;

    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    std::shared_ptr<RSContext> context_;
    std::shared_ptr<DrawableV2::RSRenderNodeDrawable> rootNodeDrawable_;
    std::vector<NodeId> curDrawStatusVec_;

    // used for blocking renderThread before displayNode has no freed buffer to request
    mutable std::mutex displayNodeBufferReleasedMutex_;
    bool displayNodeBufferReleased_ = false;
    // used for stalling renderThread before displayNode has no freed buffer to request
    std::condition_variable displayNodeBufferReleasedCond_;

    // Those variable is used to manage memory.
    bool clearMemoryFinished_ = true;
    bool clearMemDeeply_ = false;
    std::unordered_set<NodeId> nodesNeedToBeClearMemory_;
    DeviceType deviceType_ = DeviceType::PHONE;
    std::mutex mutex_;
    mutable std::mutex clearMemoryMutex_;
    std::queue<std::shared_ptr<Drawing::Surface>> tmpSurfaces_;
    static thread_local CaptureParam captureParam_;
    std::atomic<uint64_t> frameCount_ = 0;

    pid_t tid_ = 0;

    // for statistic of jank frames
    std::atomic_bool mainLooping_ = false;
    std::atomic_bool discardJankFrames_ = false;
    std::atomic_bool skipJankAnimatorFrame_ = false;
    ScreenId displayNodeScreenId_ = 0;
    std::set<pid_t> exitedPidSet_;
    ClearMemoryMoment clearMoment_ = ClearMemoryMoment::NO_CLEAR;
    bool isDefaultCleanTaskFinished_ = true;

    std::vector<Callback> imageReleaseTasks_;
    std::mutex imageReleaseMutex_;
    bool postImageReleaseTaskFlag_ = false;
    int imageReleaseCount_ = 0;

    mutable std::mutex nodeListMutex_;
    std::unordered_set<NodeId> blackList_ = {};
    std::unordered_set<NodeId> whiteList_ = {};

    sptr<SyncFence> acquireFence_ = SyncFence::InvalidFence();

    // vma cache
    bool vmaOptimizeFlag_ = false; // enable/disable vma cache, global flag
    uint32_t vmaCacheCount_ = 0;
    std::mutex vmaCacheCountMutex_;
#ifdef RES_SCHED_ENABLE
    void SubScribeSystemAbility();
    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_THREAD_H
