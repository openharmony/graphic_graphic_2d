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
    const auto& drawCmdList_ = renderNode_->drawCmdList_;
    if (drawCmdList_.empty()) {
        return;
    }
    auto& renderParams = renderNode_->GetRenderParams();
    Drawing::Rect bounds = renderParams ? renderParams->GetBounds() : Drawing::Rect(0, 0, 0, 0);

    DrawRangeImpl(canvas, bounds, 0, renderNode_->drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawable::DrawBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, 0, renderNode_->drawCmdIndex_.backgroundEndIndex_);
}

void RSRenderNodeDrawable::DrawContent(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    auto index = renderNode_->drawCmdIndex_.contentIndex_;
    if (index == -1) {
        return;
    }
    renderNode_->drawCmdList_[index](&canvas, &rect);
}

void RSRenderNodeDrawable::DrawChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const {
    renderNode_->drawCmdList_[renderNode_->drawCmdIndex_.childrenIndex_](&canvas, &rect);
    auto index = renderNode_->drawCmdIndex_.childrenIndex_;
    if (index == -1) {
        return;
    }
    renderNode_->drawCmdList_[index](&canvas, &rect);
}

void RSRenderNodeDrawable::DrawForeground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const {
    DrawRangeImpl(canvas, rect, renderNode_->drawCmdIndex_.foregroundBeginIndex_, renderNode_->drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawable::DrawRangeImpl(
    Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t start, int8_t end) const
{
    const auto& drawCmdList_ = renderNode_->drawCmdList_;
    for (auto i = start; i < end; i++) {
        drawCmdList_[i](&canvas, &rect);
    }
}

} // namespace OHOS::Rosen
