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
#ifndef RS_RENDER_TASK_H
#define RS_RENDER_TASK_H

#include <queue>
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class  RSRenderTaskBase {
public:
    virtual void SetIdx(int idx);
    virtual int GetIdx();
    virtual void Render(RSPaintFilterCanvas& canvas) = 0;
    virtual int GetTaskLoad();
    virtual ~RSRenderTaskBase() {};
};

class RSRenderTask : public RSRenderTaskBase {
public:
    void Render(RSPaintFilterCanvas& canvas) override;
    int GetTaskLoad() override;
    void SetIdx(int idx) override
    {
        idx_ = idx;
    }
    int GetIdx() override
    {
        return idx_;
    }
    RSSurfaceRenderNode *GetSurfaceNode()
    {
        return node_;
    }

    RSRenderTask(int idx, RSSurfaceRenderNode &node);
    explicit RSRenderTask(RSSurfaceRenderNode& node);
    ~RSRenderTask() override {};

private:
    int idx_;
    RSSurfaceRenderNode *node_;
};

class RSRenderSuperTask {
public:
    RSRenderSuperTask();
    explicit RSRenderSuperTask(int idx);
    ~RSRenderSuperTask();

    void AddTask(std::unique_ptr<RSRenderTask> && task);
    void InitalizeSuperTask()
    {
        superTaskIdx_ = 0;
        taskLoad_ = 0;
        while (GetTaskSize() > 0) {
            tasks_.pop();
        }
    }
    int GetSuperTaskLoad()
    {
        return taskLoad_;
    }
    void SetSuperTaskIdx(int idx)
    {
        superTaskIdx_ = idx;
    }
    int GetSuperTaskIdx()
    {
        return superTaskIdx_;
    }
    int GetTaskSize()
    {
        return tasks_.size();
    }
    RSSurfaceRenderNode* GetSurfaceNode()
    {
        if (GetTaskSize() == 0) {
            return nullptr;
        }
        auto surfaceNode = tasks_.front()->GetSurfaceNode();
        tasks_.pop();
        return surfaceNode;
    }
    void Playback(RSPaintFilterCanvas& canvas);

private:
    int superTaskIdx_;
    std::queue<std::unique_ptr<RSRenderTask>> tasks_;
    int taskLoad_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_RENDER_TASK_H
#endif
