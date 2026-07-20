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
#include <mutex>
#include <unordered_map>

#include "event_handler.h"
#include "refbase.h"
#include "surface_buffer.h"

#include "common/rs_common_def.h"
#include "platform/drawing/rs_surface.h"
#include "transaction/rs_buffer_transaction.h"
#include "transaction/rs_render_interface.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
namespace TaskDetail {
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
} // namespace TaskDetail

struct DestroySemaphoreInfo;

class RSC_EXPORT RSCanvasModifiersDrawable {
public:
    RSCanvasModifiersDrawable() = default;
    ~RSCanvasModifiersDrawable() = default;

private:
    void Reset();
    void CreateProducerSurface(std::weak_ptr<RSRenderInterface> weakRenderInterface, const std::string& cacheDir);
    void ReleaseProducerSurface(std::weak_ptr<RSRenderInterface> weakRenderInterface);
    DestroySemaphoreInfo* ResetSurface(int width, int height, bool sizeOutOfGpuLimit, GraphicColorGamut colorSpace);
    DestroySemaphoreInfo* UpdateContent(Drawing::DrawCmdListPtr drawCmdList, bool forceFlushBuffer);
    DestroySemaphoreInfo* Draw();
    std::unique_ptr<RSSurfaceFrame> RequestBufferAndDrawHistory();
    void Playback(const Drawing::DrawCmdListPtr& cmdList);
    DestroySemaphoreInfo* FlushSurfaceWithSemaphore();

    sptr<SurfaceBuffer> OnFlushBuffer();
    void OnDirtyBufferCollected(int64_t lastFlushBufferTime);
    int32_t GetFenceFd();

    bool IsFree(int64_t now, int64_t maxDuration);
    void CleanBuffer();

    bool GetPixelMap(std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
        Drawing::DrawCmdListPtr drawCmdList, const std::string& cacheDir);

    bool GetBitmap(Drawing::Bitmap& bitmap, const std::string& cacheDir);

    std::shared_ptr<Drawing::Image> GetImage(
        const Drawing::BitmapFormat& bitmapFormat, std::shared_ptr<Drawing::GPUContext> gpuContext);

    NodeId nodeId_ = 0;
    int width_ = 0;
    int height_ = 0;
    bool forceFlushBuffer_ = false;
    bool needResetCanvas_ = false;
    int64_t lastFlushBufferTime_ = 0;
    RSNodeState nodeState_ = RSNodeState::ACTIVE;
    std::shared_ptr<RSSurface> producerSurface_ = nullptr;
    std::shared_ptr<Drawing::Surface> drawingSurface_ = nullptr;
    std::unique_ptr<RSSurfaceFrame> surfaceFrame_ = nullptr;
    VkSemaphore semaphore_ = VK_NULL_HANDLE;
    std::unique_ptr<std::vector<Drawing::DrawCmdListPtr>> drawCmdListCache_ = nullptr;

    friend class RSCanvasModifiersDraw;
};

class RSC_EXPORT RSCanvasModifiersDraw : public std::enable_shared_from_this<RSCanvasModifiersDraw> {
public:
    RSCanvasModifiersDraw() = default;
    ~RSCanvasModifiersDraw() = default;
    RSCanvasModifiersDraw(const RSCanvasModifiersDraw&) = delete;
    RSCanvasModifiersDraw(const RSCanvasModifiersDraw&&) = delete;
    RSCanvasModifiersDraw& operator=(const RSCanvasModifiersDraw&) = delete;
    RSCanvasModifiersDraw& operator=(const RSCanvasModifiersDraw&&) = delete;

private:
    // Thread-related methods
    void StartThread();
    void WaitAllTasksFinish();
    void Destroy();
    void PostTask(const std::function<void()>& task, const std::string& name = std::string(), int64_t delayTime = 0);
    void PostSyncTask(const std::function<void()>& task);
    void RemoveTask(const std::string& name);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = TaskDetail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([scheduledTask_(std::move(scheduledTask))]() { scheduledTask_->Run(); });
        return std::move(taskFuture);
    }
    // End of thread-related methods

    void SetCacheDir(const std::string& cacheDir);

    void QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight);

    void OnNodeCreate(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface);

    void OnNodeRelease(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface);

    void OnNodeStateChanged(NodeId nodeId, RSNodeState nodeState);

    void ResetSurface(NodeId nodeId, int width, int height, bool sizeOutOfGpuLimit, GraphicColorGamut colorSpace);

    bool GetBitmap(NodeId nodeId, Drawing::Bitmap& bitmap);

    bool GetPixelMap(NodeId nodeId, std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
        Drawing::DrawCmdListPtr drawCmdList);

    void UpdateCanvasContent(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList);

    void SubmitAndCollectCanvasBuffers();

    void AppendTransactionConfig(NodeId nodeId, sptr<SurfaceBuffer> buffer, int fenceFd);

    void DestroyCanvasSemaphore();

    void SwapTransactionConfigList(std::vector<RSTransactionConfig>& transactionConfigList);

    void CleanFreeBuffers(int64_t delayTime, bool immediately);

    void CleanFreeBuffersImmediately();

    void DoCleanFreeBuffers(int64_t maxDuration);

    // Thread-related members
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::atomic<bool> threadStarted_ = false;
    std::atomic<bool> threadDestroyed_ = false;
    // End of thread-related members

    std::string cacheDir_;

    std::unordered_map<NodeId, RSCanvasModifiersDrawable> drawableMap_;

    std::vector<DestroySemaphoreInfo*> canvasNewSemaphoreInfos_;

    std::vector<DestroySemaphoreInfo*> canvasExpiredSemaphoreInfos_;

    std::vector<RSTransactionConfig> transactionConfigList_;

    friend class RSCanvasModifiersDrawAgent;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_H