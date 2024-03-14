/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_render_node_drawable.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSRenderNodeDrawable::Registrar RSRenderNodeDrawable::instance_;

RSRenderNodeDrawable::RSRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : renderNode_(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    if (node == nullptr) {
        return nullptr;
    }
    auto Generator = GeneratorMap.find(node->GetType());
    if (Generator != GeneratorMap.end()) {
        return Generator->second(node);
    }
    return nullptr;
}

/*
* This function will be called recursively many times, and the logic should be as concise as possible.
*/
void RSRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas) const
{
    ReplayDisplayList(canvas, ReplayType::REPLAY_ALL);
}

void RSRenderNodeDrawable::ReplayDisplayList(Drawing::Canvas& canvas, ReplayType type) const
{
    const auto& drawCmdList_ = renderNode_->drawCmdList_;
    if (drawCmdList_.empty()) {
        return;
    }
    auto& renderParams = renderNode_->GetRenderParams();
    Drawing::Rect bounds = renderParams ? renderParams->GetBounds() : Drawing::Rect(0, 0, 0, 0);

    auto drawRange = [&](int8_t begin, int8_t end) {
        for (auto i = begin; i < end; i++) {
            drawCmdList_[i](&canvas, &bounds);
        }
    };

    const auto& drawCmdIndex = renderNode_->displayListIndex_;
    switch (type) {
        case ReplayType::REPLAY_ALL:
            drawRange(0, drawCmdList_.size());
            break;
        case ReplayType::REPLAY_ONLY_SHADOW:
            if (drawCmdIndex.shadowIndex_ != -1) {
                drawRange(0, drawCmdIndex.shadowIndex_);
            }
            break;
        case ReplayType::REPLAY_ALL_EXCEPT_SHADOW:
            if (drawCmdIndex.shadowIndex_ == -1) {
                drawRange(0, drawCmdList_.size());
            } else {
                drawRange(0, drawCmdIndex.shadowIndex_);
                // Skip shadow and replay the rest
                drawRange(drawCmdIndex.shadowIndex_ + 1, drawCmdList_.size());
            }
            break;
        case ReplayType::REPLAY_BG_ONLY:
            drawRange(0, drawCmdIndex.bgEndIndex_);
            break;
        case ReplayType::REPLAY_FG_ONLY:
            drawRange(drawCmdIndex.fgBeginIndex_, drawCmdList_.size());
            break;
        case ReplayType::REPLAY_ONLY_CONTENT:
            drawRange(drawCmdIndex.bgEndIndex_, drawCmdIndex.fgBeginIndex_);
            break;
        default: 
            break;
    }
}

} // namespace OHOS::Rosen
