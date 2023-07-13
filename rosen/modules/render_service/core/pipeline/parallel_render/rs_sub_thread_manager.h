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
#include "pipeline/parallel_render/rs_render_task.h"
#include "pipeline/rs_base_render_node.h"
#include "render_context/render_context.h"

namespace OHOS::Rosen {
class RSSubThreadManager {
public:
    static RSSubThreadManager *Instance();
    void Start(RenderContext *context);
    void PostTask(const std::function<void()>& task, uint32_t threadIndex);
    void WaitNodeTask(uint64_t nodeId);
    void NodeTaskNotify(uint64_t nodeId);
    void SubmitSubThreadTask(const std::shared_ptr<RSDisplayRenderNode>& node,
        const std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
    void ResetSubThreadGrContext();
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
    bool needResetContext_ = false;
};
}
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_MANAGER_H