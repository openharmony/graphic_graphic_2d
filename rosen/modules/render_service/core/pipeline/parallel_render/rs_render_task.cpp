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

#include "rs_render_task.h"
#include "pipeline/rs_base_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSuperRenderTask::~RSSuperRenderTask()
{
    while (GetTaskSize() > 0) {
        tasks_.pop();
    }
}

void RSSuperRenderTask::AddTask(std::unique_ptr<RSRenderTask> &&task)
{
    if (task == nullptr) {
        RS_LOGE("task is nullptr");
        return;
    }
    tasks_.push(std::move(task));
}

std::shared_ptr<RSBaseRenderNode> RSSuperRenderTask::GetSurfaceNode()
{
    if (GetTaskSize() == 0) {
        return nullptr;
    }
    auto surfaceNode = tasks_.front()->GetNode();
    tasks_.pop();
    return surfaceNode;
}

std::unique_ptr<RSRenderTask> RSSuperRenderTask::GetNextRenderTask()
{
    if (GetTaskSize() == 0) {
        return std::unique_ptr<RSRenderTask>();
    }
    auto task = std::move(tasks_.front());
    tasks_.pop();
    return task;
}

} // namespace Rosen
} // namespace OHOS