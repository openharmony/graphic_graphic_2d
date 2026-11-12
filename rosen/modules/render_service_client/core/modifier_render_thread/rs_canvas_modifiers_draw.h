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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_H

#include <future>
#include <unordered_map>

#include "event_handler.h"
#include "refbase.h"
#include "surface_buffer.h"

#include "common/rs_common_def.h"
#include "platform/drawing/rs_surface.h"
#include "transaction/rs_render_interface.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
struct DestroySemaphoreInfo;
namespace CanvasTaskDetail {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> scheduledTask(new ScheduledTask(std::forward<Task&&>(task)));
        return std::make_pair(scheduledTask, scheduledTask->task_.get_future());
    }

    void Run()
    {
        task_();
    }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() override = default;

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace CanvasTaskDetail

class RSC_EXPORT RSCanvasModifiersContext {
public:
    RSCanvasModifiersContext() = delete;
    RSCanvasModifiersContext(std::weak_ptr<RSRenderInterface> weakRenderInterface);
    ~RSCanvasModifiersContext() = default;

private:
    std::shared_ptr<Drawing::GPUContext> GetGpuContext();

    void ReleaseGpuContext();

    void CleanGpuCache();

    void GetGpuCacheLimits();

    void RestoreGpuCacheLimits();

    std::string cacheDir_;

    std::shared_ptr<Drawing::GPUContext> gpuContext_ = nullptr;

    std::shared_ptr<RenderContext> renderContext_ = nullptr;

    static inline size_t maxGpuResourceBytes_ = 0;

    bool needRestoreGpuCacheLimit_ = false;

    std::weak_ptr<RSRenderInterface> weakRenderInterface_;

    friend class RSCanvasModifiersDraw;
    friend class RSCanvasModifiersDrawable;
};

class RSC_EXPORT RSCanvasModifiersDrawable {
public:
    RSCanvasModifiersDrawable() = default;
    ~RSCanvasModifiersDrawable() = default;

private:
    void Reset();
    void CreateProducerSurface(
        std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext, GraphicColorGamut colorSpace);
    void ReleaseProducerSurface(std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext);
    void ResetSurface(int width, int height, bool sizeOutOfGpuLimit);
    void UpdateContent(Drawing::DrawCmdListPtr drawCmdList, bool forceFlushBuffer);
    void Draw(bool forceFlushBuffer);
    bool RequestBufferAndDrawHistory(int width, int height, bool needResetCanvas);
    void Playback(const Drawing::DrawCmdListPtr& cmdList);

    sptr<SurfaceBuffer> GetBuffer(bool needFlushBuffer);
    void OnDirtyBufferCollected(int64_t lastBufferExpiredTime);

    bool IsFree(int64_t now, int64_t maxDuration);
    void CleanBuffer();

    bool GetPixelMap(std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
        Drawing::DrawCmdListPtr drawCmdList, std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext);

    bool GetBitmap(Drawing::Bitmap& bitmap, std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext);

    std::shared_ptr<Drawing::Image> GetImage(
        const Drawing::BitmapFormat& bitmapFormat, std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext);

    NodeId nodeId_ = 0;
    int width_ = 0;
    int height_ = 0;
    bool hasDrawn_ = false;
    bool bufferRequested_ = false;
    bool bufferDirty_ = false;
    int64_t lastBufferExpiredTime_ = 0;
    RSNodeState nodeState_ = RSNodeState::ACTIVE;
    std::shared_ptr<RSSurface> producerSurface_ = nullptr;
    std::shared_ptr<Drawing::Surface> drawingSurface_ = nullptr;
    std::unique_ptr<std::vector<Drawing::DrawCmdListPtr>> drawCmdListCache_ = nullptr;

    friend class RSCanvasModifiersDraw;
};

class RSC_EXPORT RSCanvasModifiersDraw : public std::enable_shared_from_this<RSCanvasModifiersDraw> {
public:
    RSCanvasModifiersDraw() = delete;
    RSCanvasModifiersDraw(std::weak_ptr<RSRenderInterface> weakRenderInterface);
    ~RSCanvasModifiersDraw() = default;
    RSCanvasModifiersDraw(const RSCanvasModifiersDraw&) = delete;
    RSCanvasModifiersDraw(const RSCanvasModifiersDraw&&) = delete;
    RSCanvasModifiersDraw& operator=(const RSCanvasModifiersDraw&) = delete;
    RSCanvasModifiersDraw& operator=(const RSCanvasModifiersDraw&&) = delete;

private:
    // Thread-related methods
    void StartThread();
    void Destroy();
    void PostTask(const std::function<void()>& task, const std::string& name = std::string(), int64_t delayTime = 0);
    void PostSyncTask(const std::function<void()>& task);
    void RemoveTask(const std::string& name);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = CanvasTaskDetail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([scheduledTask_(std::move(scheduledTask))]() { scheduledTask_->Run(); });
        return std::move(taskFuture);
    }
    // End of thread-related methods

    bool RemoveCleanFreeBuffersTask();

    void ReleaseResource();

    void SetCacheDir(const std::string& cacheDir);

    void OnNodeCreate(NodeId nodeId, GraphicColorGamut colorSpace);

    void OnNodeRelease(NodeId nodeId);

    void OnNodeStateChanged(NodeId nodeId, RSNodeState nodeState);

    void ResetSurface(NodeId nodeId, int width, int height, bool sizeOutOfGpuLimit);

    bool GetBitmap(NodeId nodeId, Drawing::Bitmap& bitmap);

    bool GetPixelMap(NodeId nodeId, std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
        Drawing::DrawCmdListPtr drawCmdList);

    void UpdateCanvasContent(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList);

    DestroySemaphoreInfo* FlushSurfaceWithSemaphore(
        VkSemaphore& semaphore, std::shared_ptr<Drawing::Surface> drawingSurface);

    int32_t GetFenceFd(VkSemaphore& semaphore);

    void SubmitAndCollectCanvasBuffers();

    void AppendTransactionConfig(NodeId nodeId, sptr<SurfaceBuffer> buffer, sptr<SyncFence> fence);

    std::unique_ptr<RSCommand> SyncAndWait();

    void CleanFreeBuffers(bool immediately);

    void DoCleanFreeBuffers(int64_t maxDuration);

    // Thread-related members
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::atomic<bool> threadStarted_ = false;
    std::atomic<bool> threadDestroyed_ = false;
    // End of thread-related members

    std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext_ = nullptr;

    DestroySemaphoreInfo* destroySemaphoreInfo_ = nullptr;

    int64_t lastGpuContextActiveTime_ = 0;

    bool needForceSubmit_ = false;

    std::unordered_map<NodeId, RSCanvasModifiersDrawable> drawableMap_;

    std::vector<RSTransactionConfig> transactionConfigList_;

    friend class RSCanvasModifiersDrawAgent;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_H