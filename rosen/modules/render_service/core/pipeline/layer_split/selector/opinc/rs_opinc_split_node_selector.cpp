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

#include "rs_opinc_split_node_selector.h"

#include <memory>
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_planner.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_processor.h"

#if defined(ROSEN_OHOS)
namespace OHOS::Rosen {

std::shared_ptr<OpincSplitNodeSelector>& OpincSplitNodeSelector::GetInstance()
{
    static std::shared_ptr<OpincSplitNodeSelector> instance = std::make_shared<OpincSplitNodeSelector>();
    return instance;
}

std::shared_ptr<RSRenderNode> OpincSplitNodeSelector::SelectParentNode()
{
    std::unordered_map<std::shared_ptr<RSRenderNode>, uint64_t> parentAreaMap;
    std::vector<std::shared_ptr<RSRenderNode>> localVec;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        localVec = std::move(opincRenderNodeVec_);
    }

    for (const auto& node : localVec) {
        if (!node) {
            continue;
        }

        if (node && node->GetOpincRootCache().OpincGetRootFlag()) {
            auto geo = node->GetRenderProperties().GetBoundsGeometry();
            if (!geo) {
                continue;
            }
            auto rect = geo->GetAbsRect();
            rect = rect.JoinRect(geo->MapAbsRect(
                node->GetChildrenRect().ConvertTo<float>()));

            uint64_t area = static_cast<uint64_t>(rect.GetWidth()) * rect.GetHeight();
            auto parent = node->GetParent().lock();
            if (parent) {
                parentAreaMap[parent] += area;
            }
        }
    }

    if (parentAreaMap.empty()) {
        return nullptr;
    }

    auto maxIter = std::max_element(parentAreaMap.begin(), parentAreaMap.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

    return maxIter->first;
}

void OpincSplitNodeSelector::RecordSplitNode(std::shared_ptr<RSRenderNode> node)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (node->GetOpincRootCache().OpincGetRootFlag() && node->GetRenderProperties().IsCurGeoDirty()) {
        opincRenderNodeVec_.push_back(node);
    }
}

std::shared_ptr<RSLayerSplitterProcessor> OpincSplitNodeSelector::MakeProcessor()
{
    return std::make_shared<RSOpincLayerSplitterProcessor>();
}

std::shared_ptr<RSLayerSplitterPlanner> OpincSplitNodeSelector::MakePlanner()
{
    return std::make_shared<RSOpincLayerSplitterPlanner>();
}

std::shared_ptr<RSRenderNode> OpincSplitNodeSelector::GetCurrParentNode()
{
    return currParentNode_;
}

std::shared_ptr<RSRenderNode> OpincSplitNodeSelector::GetLastParentNode()
{
    return lastParentNode_;
}

void OpincSplitNodeSelector::SetCurrParentNode(std::shared_ptr<RSRenderNode> currParentNode)
{
    if (currParentNode_) {
        lastParentNode_ = currParentNode_;
    }
    currParentNode_ = currParentNode;
}

} // namespace OHOS::Rosen
#endif