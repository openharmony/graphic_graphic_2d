/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef RS_OPINC_SPLIT_NODE_SELECTOR_H
#define RS_OPINC_SPLIT_NODE_SELECTOR_H

#include <mutex>
#include "pipeline/layer_split/selector/rs_split_node_selector.h"

#if defined(ROSEN_OHOS)
namespace OHOS::Rosen {

class OpincSplitNodeSelector : public SplitNodeSelector {
public:
    static std::shared_ptr<OpincSplitNodeSelector>& GetInstance();
    std::shared_ptr<RSRenderNode> SelectParentNode() override;
    void RecordSplitNode(std::shared_ptr<RSRenderNode> node) override;
    std::shared_ptr<RSLayerSplitterProcessor> MakeProcessor() override;
    std::shared_ptr<RSLayerSplitterPlanner> MakePlanner() override;
    std::shared_ptr<RSRenderNode> GetCurrParentNode() override;
    std::shared_ptr<RSRenderNode> GetLastParentNode() override;
    void SetCurrParentNode(std::shared_ptr<RSRenderNode> currParentNode) override;

protected:
    std::shared_ptr<RSRenderNode> lastParentNode_;
    std::shared_ptr<RSRenderNode> currParentNode_;
    std::mutex mutex_;
    std::vector<std::shared_ptr<RSRenderNode>> opincRenderNodeVec_;
};

} // namespace OHOS::Rosen
#endif
#endif // RS_OPINC_SPLIT_NODE_SELECTOR_H