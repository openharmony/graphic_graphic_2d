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

#include "common/rs_common_def.h"
#include "drawable/rs_misc_drawable.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSRenderNodeDrawable::Registrar RSRenderNodeDrawable::instance_;

RSRenderNodeDrawable::RSRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawableAdapter(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSRenderNodeDrawable(std::move(node));
}

void RSRenderNodeDrawable::Draw(Drawing::Canvas& canvas) const
{
    if (UNLIKELY(RSUniRenderThread::GetIsInCapture())) {
        OnCapture(canvas);
    } else {
        OnDraw(canvas);
    }
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

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas) const
{
    RSRenderNodeDrawable::OnDraw(canvas);
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

void RSRenderNodeDrawable::DrawChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    renderNode_->drawCmdList_[renderNode_->drawCmdIndex_.childrenIndex_](&canvas, &rect);
    auto index = renderNode_->drawCmdIndex_.childrenIndex_;
    if (index == -1) {
        return;
    }
    renderNode_->drawCmdList_[index](&canvas, &rect);
}

void RSRenderNodeDrawable::DrawForeground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, renderNode_->drawCmdIndex_.foregroundBeginIndex_, renderNode_->drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawable::DrawShadow(Drawing::Canvas& canvas) const
{
    auto index = renderNode_->drawCmdIndex_.shadowIndex_;
    if (index == -1) {
        return;
    }
    auto& renderParams = renderNode_->GetRenderParams();
    Drawing::Rect bounds = renderParams ? renderParams->GetBounds() : Drawing::Rect(0, 0, 0, 0);
    DrawRangeImpl(canvas, bounds, 0, renderNode_->drawCmdIndex_.shadowIndex_ + 1);
}

void RSRenderNodeDrawable::DrawWithoutShadow(Drawing::Canvas& canvas) const
{
    auto& renderParams = renderNode_->GetRenderParams();
    Drawing::Rect bounds = renderParams ? renderParams->GetBounds() : Drawing::Rect(0, 0, 0, 0);

    auto index = renderNode_->drawCmdIndex_.shadowIndex_;
    if (index == -1) { // no shadowindex, do the same as OnDraw
        DrawRangeImpl(canvas, bounds, 0, renderNode_->drawCmdIndex_.endIndex_);
        return;
    }
    DrawRangeImpl(canvas, bounds, 0, renderNode_->drawCmdIndex_.shadowIndex_);
    DrawRangeImpl(canvas, bounds, renderNode_->drawCmdIndex_.shadowIndex_ + 1, renderNode_->drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawable::DrawRangeImpl(
    Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t start, int8_t end) const
{
    const auto& drawCmdList_ = renderNode_->drawCmdList_;
    for (auto i = start; i < end; i++) {
        drawCmdList_[i](&canvas, &rect);
    }
}

void RSRenderNodeDrawable::DumpDrawableTree(int32_t depth, std::string &out) const
{
    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    renderNode_->DumpNodeType(out);
    out += "[" + std::to_string(renderNode_->GetId()) + "]";
    renderNode_->DumpSubClassNode(out);
    out += ", DrawableVec:[" + DumpDrawableVec() + "]";
    out += "\n";

    auto childrenDrawable = std::static_pointer_cast<RSChildrenDrawable>(
        renderNode_->drawableVec_[static_cast<int32_t>(RSDrawableSlot::CHILDREN)]);
    if (childrenDrawable) {
        for (const auto& renderNodeDrawable : childrenDrawable->childrenDrawableVec_) {
            renderNodeDrawable->DumpDrawableTree(depth + 1, out);
        }
    }
}

std::string RSRenderNodeDrawable::DumpDrawableVec() const
{
    const auto & drawableVec = renderNode_->drawableVec_;
    std::string str;
    for (uint i = 0; i < drawableVec.size(); ++i) {
        if (drawableVec[i]) {
            str += std::to_string(i) + ", ";
        }
    }
    if (str.length() > 2) {
        str.pop_back();
        str.pop_back();
    }

    return str;
}

} // namespace OHOS::Rosen
