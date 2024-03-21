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

#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "drawable/rs_render_node_drawable.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_context.h"
#include "params/rs_render_thread_params.h"
#ifdef RES_SCHED_ENABLE
#include "vsync_system_ability_listener.h"
#endif

namespace OHOS {
namespace Rosen {
class RSUniRenderThread {
public:
    static RSUniRenderThread& Instance();

    // disable copy and move
    RSUniRenderThread(const RSUniRenderThread&) = delete;
    RSUniRenderThread& operator=(const RSUniRenderThread&) = delete;
    RSUniRenderThread(RSUniRenderThread&&) = delete;
    RSUniRenderThread& operator=(RSUniRenderThread&&) = delete;

    void Start();
    void InitGrContext();
    void RenderFrames();
    void Sync(std::unique_ptr<RSRenderThreadParams>& stagingRenderThreadParams);
    void PostTask(const std::function<void()>& task);
    void PostTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
        AppExecFwk::EventQueue::Priority priority = AppExecFwk::EventQueue::Priority::IDLE);
    void PostSyncTask(const std::function<void()>& task);
    void Render();
    std::shared_ptr<RSBaseRenderEngine> GetRenderEngine() const;
    void NotifyDisplayNodeBufferReleased();
    bool WaitUntilDisplayNodeBufferReleased(std::shared_ptr<RSSurfaceHandler> surfaceHandler);

    uint64_t GetTimestamp();

    void ClearMemoryCache(ClearMemoryMoment moment, bool deeply);
    bool GetClearMemoryFinished() const;
    bool GetClearMemDeeply() const;
    void SetClearMoment(ClearMemoryMoment moment);
    ClearMemoryMoment GetClearMoment() const;
    uint32_t GetRefreshRate() const;
    void DumpMem(DfxString& log);
    void TrimMem(std::string& dumpString, std::string& type);
    std::shared_ptr<Drawing::Image> GetWatermarkImg();
    bool GetWatermarkFlag();

    std::vector<NodeId>& GetDrawStatusVec()
    {
        return curDrawStatusVec_;
    }
    const std::unique_ptr<RSRenderThreadParams>& GetRSRenderThreadParams()
    {
        return renderThreadParams_;
    }

    static void SetIsInCapture(bool flag);
    static bool GetIsInCapture();

    void RenderServiceTreeDump(std::string& dumpString) const;

private:
    RSUniRenderThread();
    ~RSUniRenderThread() noexcept;

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;

    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    std::shared_ptr<RSContext> context_;
    std::unique_ptr<RSRenderNodeDrawable> rootNodeDrawable_;
    std::vector<NodeId> curDrawStatusVec_;
    std::unique_ptr<RSRenderThreadParams> renderThreadParams_ = nullptr; // sync from main thread
#ifdef RES_SCHED_ENABLE
    void SubScribeSystemAbility();
    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
#endif
    // used for blocking renderThread before displayNode has no freed buffer to request
    mutable std::mutex displayNodeBufferReleasedMutex_;
    bool displayNodeBufferReleased_ = false;
    // used for stalling renderThread before displayNode has no freed buffer to request
    std::condition_variable displayNodeBufferReleasedCond_;

    // Those variable is used to manage memory.
    bool clearMemoryFinished_ = true;
    bool clearMemDeeply_ = false;
    DeviceType deviceType_ = DeviceType::PHONE;
    static thread_local bool isInCaptureFlag_; // true if in capture mode
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_THREAD_H
