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
#include "rs_render_task.h"

namespace OHOS {
namespace Rosen {
RSRenderTask::RSRenderTask(int idx, RSSurfaceRenderNode &node)
{
    idx_ = idx;
    node_ = &node;
}

RSRenderTask::RSRenderTask(RSSurfaceRenderNode& node)
{
    node_ = &node;
}

int RSRenderTask::GetTaskLoad()
{
    return 0;
}

void RSRenderTask::Render(RSPaintFilterCanvas& canvas)
{
}

RSRenderSuperTask::RSRenderSuperTask()
{
    taskLoad_ = 0;
}

RSRenderSuperTask::RSRenderSuperTask(int idx)
{
    taskLoad_ = 0;
    superTaskIdx_ = idx;
}

RSRenderSuperTask::~RSRenderSuperTask()
{
    while (GetTaskSize() > 0) {
        tasks_.pop();
    }
}

void RSRenderSuperTask::AddTask(std::unique_ptr<RSRenderTask> && task)
{
    taskLoad_ += task->GetTaskLoad();
    tasks_.push(std::move(task));
}

void RSRenderSuperTask::Playback(RSPaintFilterCanvas& canvas)
{
    while (GetTaskSize() > 0) {
        auto task = std::move(tasks_.front());
        if (task != nullptr) {
            task->Render(canvas);
        }
        tasks_.pop();
    }
}
} // namespace Rosen
} // namespace OHOS
#endif