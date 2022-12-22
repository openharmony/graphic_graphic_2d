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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_TASK_MANAGER_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_TASK_MANAGER_H

#include "rs_render_task.h"
#include <map>
#include <vector>
#include <memory>

namespace OHOS {
namespace Rosen {
class RSParallelTaskManager {
public:
    RSParallelTaskManager();
    ~RSParallelTaskManager();
    void Initialize(uint32_t threadNum);
    void PushRenderTask(std::unique_ptr<RSRenderTask> renderTask);
    void LBCalcAndSubmitSuperTask(std::shared_ptr<RSBaseRenderNode> displayNode);
    int GetTaskNum();
    void Reset();
    void SetSubThreadRenderTaskLoad(uint32_t threadIdx, uint64_t loadId, float cost);
    bool GetParallelRenderExtEnable()
    {
        return isParallelRenderExtEnabled_;
    }

private:
    void SubmitSuperTask();
    std::vector<uint32_t> LoadBalancing();
    std::vector<std::unique_ptr<RSRenderTask>> renderTaskList_;
    std::unique_ptr<RSSuperRenderTask> cachedSuperRenderTask_;
    std::vector<std::unique_ptr<RSSuperRenderTask>> superRenderTaskList_;
    std::map<int, float> evalTaskCost_;
    int threadNum_;
    int taskNum_;
    bool isParallelRenderExtEnabled_;
    int* loadBalance_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_TASK_MANAGER_H