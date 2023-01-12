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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_RENDER_MANAGER_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_RENDER_MANAGER_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include "EGL/egl.h"
#include "rs_parallel_sub_thread.h"
#include "rs_parallel_pack_visitor.h"
#include "rs_parallel_task_manager.h"
#include "pipeline/parallel_render/rs_render_task.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderVisitor;
class RSSurfaceRenderNode;
class RSDisplayRenderNode;

enum class ParallelRenderType {
    DRAW_IMAGE = 0,
    FLUSH_ONE_BUFFER
};

enum class ParallelStatus {
    OFF = 0,
    ON,
    FIRSTFLUSH,
    WAITFIRSTFLUSH
};

enum class TaskType {
    PREPARE_TASK = 0,
    PROCESS_TASK
};

class RSParallelRenderManager {
public:
    static RSParallelRenderManager *Instance();
    void SetParallelMode(bool parallelMode);
    bool GetParallelMode() const;
    bool GetParallelModeSafe() const;
    void StartSubRenderThread(uint32_t threadNum, RenderContext *context);
    void EndSubRenderThread();
    void CopyVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node);
    void CopyPrepareVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node);
    void PackRenderTask(RSSurfaceRenderNode &node, TaskType type = TaskType::PROCESS_TASK);
    void LoadBalanceAndNotify(TaskType type = TaskType::PROCESS_TASK);
    void MergeRenderResult(std::shared_ptr<SkCanvas> canvas);
    void SetFrameSize(int height, int width);
    void GetFrameSize(int &height, int &width);
    void SubmitSuperTask(int taskIndex, std::unique_ptr<RSSuperRenderTask> superRenderTask);
    void SubMainThreadNotify(int threadIndex);
    void WaitSubMainThread(uint32_t threadIndex);
    void SubMainThreadWait(uint32_t threadIndex);
    void SetRenderTaskCost(uint32_t subMainThreadIdx, uint64_t loadId, float cost,
        TaskType type = TaskType::PROCESS_TASK);
    bool ParallelRenderExtEnabled();
    void TryEnableParallelRendering();
    void ReadySubThreadNumIncrement();
    void CommitSurfaceNum(int surfaceNum);
    void WaitPrepareEnd(RSUniRenderVisitor &visitor);
    TaskType GetTaskType();
    RSUniRenderVisitor* GetUniVisitor()
    {
        return uniVisitor_;
    }
    ParallelStatus GetParallelRenderingStatus() const;
    void WorkSerialTask(RSSurfaceRenderNode &node);

private:
    RSParallelRenderManager() = default;
    ~RSParallelRenderManager() = default;
    RSParallelRenderManager(const RSParallelRenderManager &) = delete;
    RSParallelRenderManager(const RSParallelRenderManager &&) = delete;
    RSParallelRenderManager &operator = (const RSParallelRenderManager &) = delete;
    RSParallelRenderManager &operator = (const RSParallelRenderManager &&) = delete;
    void DrawImageMergeFunc(std::shared_ptr<SkCanvas> canvas);
    void FlushOneBufferFunc();

    std::shared_ptr<RSParallelPackVisitor> packVisitor_;
    std::shared_ptr<RSParallelPackVisitor> packVisitorPrepare_;
    std::vector<std::unique_ptr<RSParallelSubThread>> threadList_;
    RSParallelTaskManager processTaskManager_;
    RSParallelTaskManager prepareTaskManager_;
    int height_;
    int width_;
    std::vector<uint8_t> flipCoin_;
    std::mutex parallelRenderMutex_;
    std::mutex cvParallelRenderMutex_;
    std::condition_variable cvParallelRender_;
    RenderContext *renderContext_;
    ParallelRenderType renderType_ = ParallelRenderType::DRAW_IMAGE;
    std::shared_ptr<RSBaseRenderNode> displayNode_ = nullptr;

    uint32_t expectedSubThreadNum_ = 0;
    std::atomic<uint32_t> readySubThreadNum_ = 0;
    bool firstFlush_ = false;
    bool parallelMode_ = false;
    RSUniRenderVisitor *uniVisitor_ = nullptr;
    TaskType taskType_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_RENDER_MANAGER_H