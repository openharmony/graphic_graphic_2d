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
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include "EGL/egl.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context_base.h"
#endif
#include "rs_parallel_hardware_composer.h"
#include "rs_parallel_sub_thread.h"
#include "rs_parallel_pack_visitor.h"
#include "rs_parallel_task_manager.h"
#include "pipeline/parallel_render/rs_render_task.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_base_render_node.h"
#include "common/rs_rect.h"
#include "common/rs_vector4.h"

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
    PROCESS_TASK,
    CALC_COST_TASK,
    COMPOSITION_TASK
};

class RSParallelRenderManager {
public:
    static RSParallelRenderManager *Instance();
    void SetParallelMode(bool parallelMode);
    bool GetParallelMode() const;
    bool GetParallelModeSafe() const;
#ifdef NEW_RENDER_CONTEXT
    void StartSubRenderThread(uint32_t threadNum, std::shared_ptr<RenderContextBase> context,
        std::shared_ptr<DrawingContext> drawingContext);
#else
    void StartSubRenderThread(uint32_t threadNum, RenderContext *context);
#endif
    void EndSubRenderThread();
    void CopyVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node);
    void CopyPrepareVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node);
    void CopyCalcCostVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node, bool isNeedCalc,
        bool doAnimate, bool isOpDropped);
    void PackRenderTask(RSSurfaceRenderNode &node, TaskType type = TaskType::PROCESS_TASK);
    void PackParallelCompositionTask(std::shared_ptr<RSNodeVisitor> visitor,
                                     const std::shared_ptr<RSBaseRenderNode> node);
    void LoadBalanceAndNotify(TaskType type = TaskType::PROCESS_TASK);
    void MergeRenderResult(RSPaintFilterCanvas& canvas);
    void SetFrameSize(int width, int height);
    void GetFrameSize(int &width, int &height) const;
    void SubmitSuperTask(uint32_t taskIndex, std::unique_ptr<RSSuperRenderTask> superRenderTask);
    void SubmitCompositionTask(uint32_t taskIndex, std::unique_ptr<RSCompositionTask> compositionTask);
    void SubMainThreadNotify(int threadIndex);
    void WaitSubMainThread(uint32_t threadIndex);
    void SubMainThreadWait(uint32_t threadIndex);
    void WaitCalcCostEnd();
    void WaitCompositionEnd();
    void UpdateNodeCost(RSDisplayRenderNode& node);
    bool IsNeedCalcCost() const;
    int32_t GetCost(RSRenderNode &node) const;
    int32_t GetSelfDrawNodeCost() const;
    void StartTiming(uint32_t subMainThreadIdx);
    void StopTimingAndSetRenderTaskCost(
        uint32_t subMainThreadIdx, uint64_t loadId, TaskType type = TaskType::PROCESS_TASK);
    bool ParallelRenderExtEnabled();
    void TryEnableParallelRendering();
    void ReadySubThreadNumIncrement();
    void CommitSurfaceNum(int surfaceNum);
    void WaitPrepareEnd(RSUniRenderVisitor &visitor);
    TaskType GetTaskType() const;
    RSUniRenderVisitor* GetUniVisitor() const
    {
        return uniVisitor_;
    }

    std::shared_ptr<RSUniRenderVisitor> GetUniParallelCompositionVisitor() const
    {
        return uniParallelCompositionVisitor_;
    }

    bool IsDoAnimate() const
    {
        return doAnimate_;
    }

    bool IsOpDropped() const
    {
        return isOpDropped_;
    }

    bool IsSecurityDisplay() const
    {
        return isSecurityDisplay_;
    }

    ParallelStatus GetParallelRenderingStatus() const;
    void WorkSerialTask(RSSurfaceRenderNode &node);
    void LockFlushMutex()
    {
        flushMutex_.lock();
    }
    void UnlockFlushMutex()
    {
        flushMutex_.unlock();
    }
    uint32_t GetParallelThreadNumber() const;
    void AddSelfDrawingSurface(unsigned int subThreadIndex, bool isRRect, RectF rect,
        Vector4f cornerRadius = {0.f, 0.f, 0.f, 0.f});
    void ClearSelfDrawingSurface(RSPaintFilterCanvas& canvas, unsigned int subThreadIndex);
    void AddAppWindowNode(uint32_t surfaceIndex, std::shared_ptr<RSSurfaceRenderNode> appNode);
    const std::map<uint32_t, std::vector<std::shared_ptr<RSSurfaceRenderNode>>>& GetAppWindowNodes() const
    {
        return appWindowNodesMap_;
    }

    // Use for Vulkan
    void WaitProcessEnd();
    void InitDisplayNodeAndRequestFrame(
        const std::shared_ptr<RSBaseRenderEngine> renderEngine, const ScreenInfo screenInfo);
    void ProcessParallelDisplaySurface(RSUniRenderVisitor &visitor);
    void ReleaseBuffer();
    void NotifyUniRenderFinish();
    std::shared_ptr<RSDisplayRenderNode> GetParallelDisplayNode(uint32_t subMainThreadIdx);
    std::unique_ptr<RSRenderFrame> GetParallelFrame(uint32_t subMainThreadIdx);
    void SetFilterSurfaceRenderNode(RSSurfaceRenderNode& node)
    {
        filterSurfaceRenderNodes_.push_back(node.ReinterpretCastTo<RSSurfaceRenderNode>());
    }
    void ClearFilterSurfaceRenderNode()
    {
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>().swap(filterSurfaceRenderNodes_);
    }
    uint32_t GetFilterSurfaceRenderNodeCount()
    {
        return filterSurfaceRenderNodes_.size();
    }
    void ProcessFilterSurfaceRenderNode();

private:
    RSParallelRenderManager();
    ~RSParallelRenderManager() = default;
    RSParallelRenderManager(const RSParallelRenderManager &) = delete;
    RSParallelRenderManager(const RSParallelRenderManager &&) = delete;
    RSParallelRenderManager &operator = (const RSParallelRenderManager &) = delete;
    RSParallelRenderManager &operator = (const RSParallelRenderManager &&) = delete;
    void DrawImageMergeFunc(RSPaintFilterCanvas& canvas);
    void FlushOneBufferFunc();
    void GetCostFactor();
    void InitAppWindowNodeMap();
    bool DrawImageMergeFuncForRosenDrawing(RSPaintFilterCanvas& canvas, std::shared_ptr<Drawing::Image> texture);
    std::shared_ptr<RSParallelPackVisitor> packVisitor_;
    std::shared_ptr<RSParallelPackVisitor> packVisitorPrepare_;
    std::shared_ptr<RSParallelPackVisitor> calcCostVisitor_;
    std::vector<std::unique_ptr<RSParallelSubThread>> threadList_;
    RSParallelTaskManager processTaskManager_;
    RSParallelTaskManager prepareTaskManager_;
    RSParallelTaskManager calcCostTaskManager_;
    RSParallelTaskManager compositionTaskManager_;
    int width_ = 0;
    int height_ = 0;
    std::vector<uint8_t> flipCoin_;
    std::mutex parallelRenderMutex_;
    std::mutex cvParallelRenderMutex_;
    std::mutex flushMutex_;
    std::condition_variable cvParallelRender_;
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RenderContextBase> renderContext_ = nullptr;
    std::shared_ptr<DrawingContext> drawingContext_ = nullptr;
#else
    RenderContext* renderContext_ = nullptr;
#endif
    ParallelRenderType renderType_ = ParallelRenderType::DRAW_IMAGE;
    std::shared_ptr<RSBaseRenderNode> displayNode_ = nullptr;
    std::shared_ptr<RSDisplayRenderNode> mainDisplayNode_ = nullptr;
    std::shared_ptr<RSBaseRenderNode> baseNode_ = nullptr;

    uint32_t expectedSubThreadNum_ = 0;
    std::atomic<uint32_t> readySubThreadNum_ = 0;
    bool firstFlush_ = false;
    bool parallelMode_ = false;
    RSUniRenderVisitor *uniVisitor_ = nullptr;
    std::shared_ptr<RSUniRenderVisitor> uniParallelCompositionVisitor_ = nullptr;
    TaskType taskType_;
    std::unique_ptr<RSParallelHardwareComposer> parallelHardwareComposer_;
    std::map<uint32_t, std::vector<std::shared_ptr<RSSurfaceRenderNode>>> appWindowNodesMap_;

    std::vector<uint32_t> parallelPolicy_;
    int32_t calcCostCount_ = 0;
    std::map<std::string, int32_t> costFactor_;
    std::map<int64_t, int32_t> imageFactor_;

    bool doAnimate_ = false;
    bool isOpDropped_ = false;
    bool isSecurityDisplay_ = false;

    std::vector<timespec> startTime_;
    std::vector<timespec> stopTime_;

    // Use for Vulkan
    std::vector<std::shared_ptr<RSDisplayRenderNode>> parallelDisplayNodes_;
    std::vector<std::shared_ptr<RSDisplayRenderNode>> backParallelDisplayNodes_;
    std::vector<std::unique_ptr<RSRenderFrame>> parallelFrames_;
    int readyBufferNum_ = 0;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> filterSurfaceRenderNodes_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_RENDER_MANAGER_H