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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_MANAGER_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_MANAGER_H

#include "rs_sub_thread.h"

#include <condition_variable>
#include <cstdint>
#include <map>
#include <mutex>
#include "EGL/egl.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/rs_base_render_node.h"
#include "render_context/render_context.h"
#include "rs_filter_sub_thread.h"

namespace OHOS::Rosen {
constexpr char TOPIC_RCD_DISPLAY_SIZE[] = "RCD_UPDATE_DISPLAY_SIZE";
constexpr char TOPIC_RCD_DISPLAY_ROTATION[] = "RCD_UPDATE_DISPLAY_ROTATION";
constexpr char TOPIC_RCD_DISPLAY_NOTCH[] = "RCD_UPDATE_DISPLAY_NOTCH";
class RSSubThreadManager {
public:
    static RSSubThreadManager *Instance();
    void Start(RenderContext *context);
    void StartColorPickerThread(RenderContext* context);
    void StartRCDThread(RenderContext* context);
    void PostTask(const std::function<void()>& task, uint32_t threadIndex, bool isSyncTask = false);
    void WaitNodeTask(uint64_t nodeId);
    void NodeTaskNotify(uint64_t nodeId);
    void SubmitSubThreadTask(const std::shared_ptr<RSDisplayRenderNode>& node,
        const std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
    void ResetSubThreadGrContext();
    void CancelReleaseResourceTask();
    void ReleaseTexture();
    void CancelReleaseTextureTask();
    void ForceReleaseResource();
    void DumpMem(DfxString& log);
    float GetAppGpuMemoryInMB();
    std::vector<MemoryGraphic> CountSubMem(int pid);
    void ReleaseSurface(uint32_t threadIndex) const;
    void AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex);
    std::unordered_map<uint32_t, pid_t> GetReThreadIndexMap() const;
    void ScheduleRenderNodeDrawable(DrawableV2::RSSurfaceRenderNodeDrawable* nodeDrawable);
private:
    RSSubThreadManager() = default;
    ~RSSubThreadManager() = default;
    RSSubThreadManager(const RSSubThreadManager &) = delete;
    RSSubThreadManager(const RSSubThreadManager &&) = delete;
    RSSubThreadManager &operator = (const RSSubThreadManager &) = delete;
    RSSubThreadManager &operator = (const RSSubThreadManager &&) = delete;

    RenderContext* renderContext_ = nullptr;
    uint32_t minLoadThreadIndex_ = 0;
    std::mutex parallelRenderMutex_;
    std::condition_variable cvParallelRender_;
    std::map<uint64_t, uint8_t> nodeTaskState_;
    std::vector<std::shared_ptr<RSSubThread>> threadList_;
    std::unordered_map<pid_t, uint32_t> threadIndexMap_;
    std::unordered_map<uint32_t, pid_t> reThreadIndexMap_;
    std::shared_ptr<RSFilterSubThread> colorPickerThread_ = nullptr;
    bool needResetContext_ = false;
    bool needCancelTask_ = false;
    bool needCancelReleaseTextureTask_ = false;

    bool isRcdServiceRegister_ = false;
};
}
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_MANAGER_H