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
#include <cstdint>
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
    void PushCompositionTask(std::unique_ptr<RSCompositionTask> compositionTask);
    void LBCalcAndSubmitSuperTask(std::shared_ptr<RSBaseRenderNode> displayNode);
    void LBCalcAndSubmitCompositionTask(std::shared_ptr<RSBaseRenderNode> baseNode);
    uint32_t GetTaskNum();
    void Reset();
    void SetSubThreadRenderTaskLoad(uint32_t threadIdx, uint64_t loadId, float cost);
    void UpdateNodeCost(RSDisplayRenderNode& node, std::vector<uint32_t>& parallelPolicy) const;
    void LoadParallelPolicy(std::vector<uint32_t>& parallelPolicy);
    void GetCostFactor(std::map<std::string, int32_t>& costFactor, std::map<int64_t, int32_t>& imageFactor) const;
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
    std::vector<std::unique_ptr<RSCompositionTask>> compositionTaskList_;
    std::map<int, float> evalTaskCost_;
    uint32_t threadNum_;
    uint32_t taskNum_;
    bool isParallelRenderExtEnabled_;
    int* loadBalance_;
    std::vector<uint32_t> parallelPolicy_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_TASK_MANAGER_H