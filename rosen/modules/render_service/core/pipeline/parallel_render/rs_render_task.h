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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_RENDER_TASK_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_RENDER_TASK_H

#include <cstdint>
#include <memory>
#include <queue>
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"

#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
namespace OHOS {
namespace Rosen {
class RSRenderTaskBase {
public:
    explicit RSRenderTaskBase(std::shared_ptr<RSBaseRenderNode> node) : node_(node), loadId_(node->GetId()) {}
    virtual ~RSRenderTaskBase() = default;
    void SetIdx(uint64_t idx)
    {
        loadId_ = idx;
    }
    uint64_t GetIdx() const
    {
        return loadId_;
    }
    std::shared_ptr<RSBaseRenderNode> GetNode() const
    {
        return node_;
    }

private:
    std::shared_ptr<RSBaseRenderNode> node_;
    uint64_t loadId_;
};

class RSRenderTask : public RSRenderTaskBase {
public:
    enum class RenderNodeStage {
        PREPARE = 0,
        PROCESS,
        CACHE,
        CALC_COST
    };
    explicit RSRenderTask(RSSurfaceRenderNode &node, RenderNodeStage stage)
        : RSRenderTaskBase(node.shared_from_this()), stage_(stage) {}
    ~RSRenderTask() override {};
    RenderNodeStage GetNodeStage() const
    {
        return stage_;
    }
private:
    RenderNodeStage stage_;
};

class RSSuperRenderTask : public RSRenderTaskBase {
public:
    explicit RSSuperRenderTask(RSDisplayRenderNode &node)
        : RSRenderTaskBase(node.shared_from_this()) {}
    explicit RSSuperRenderTask(std::shared_ptr<RSBaseRenderNode> node, uint64_t frameCount = 0)
        : RSRenderTaskBase(node), frameCount_(frameCount) {}
    ~RSSuperRenderTask() override;

    void AddTask(std::unique_ptr<RSRenderTask> &&task);

    int GetTaskSize() const
    {
        return tasks_.size();
    }

    uint64_t GetFrameCount() const
    {
        return frameCount_;
    }

    std::shared_ptr<RSBaseRenderNode> GetSurfaceNode();
    std::unique_ptr<RSRenderTask> GetNextRenderTask();

private:
    std::queue<std::unique_ptr<RSRenderTask>> tasks_;
    uint64_t frameCount_ = 0;
};

class RSCompositionTask : public RSRenderTaskBase {
public:
    explicit RSCompositionTask(std::shared_ptr<RSDisplayRenderNode> node) : RSRenderTaskBase(node) {}
    ~RSCompositionTask() override {};
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_RENDER_TASK_H