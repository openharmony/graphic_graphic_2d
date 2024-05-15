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

#include "drawable/rs_render_node_shadow_drawable.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::DrawableV2 {

RSRenderNodeShadowDrawable::RSRenderNodeShadowDrawable(
    std::shared_ptr<const RSRenderNode> node, std::shared_ptr<RSRenderNodeDrawableAdapter> nodeDrawable)
    : RSRenderNodeDrawableAdapter(std::move(node)), nodeDrawable_(std::move(nodeDrawable))
{
    // tell associated RSRenderNodeDrawableAdapter to skip shadow, as we take care of drawing shadow
    // Planning: directly set skip type (a RT flag!) here may cause shadow display error in some cases
    nodeDrawable_->SetSkip(SkipType::SKIP_SHADOW);
}

void RSRenderNodeShadowDrawable::Draw(Drawing::Canvas& canvas)
{
    // rect is not directly used, make a dummy rect
    static const Drawing::Rect rect;

    auto shadowIndex = nodeDrawable_->drawCmdIndex_.shadowIndex_;
    if (shadowIndex == -1) {
        return;
    }

    const auto& params = nodeDrawable_->GetRenderParams();
    if (!params) {
        RS_LOGE("params is nullptr");
        return;
    }
    if (!params->GetShouldPaint()) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    
    // PLANNING: add support for op drop

    for (auto i = 0; i <= shadowIndex; i++) {
        nodeDrawable_->drawCmdList_[i](&canvas, &rect);
    }
}

void RSRenderNodeShadowDrawable::DumpDrawableTree(int32_t depth, std::string& out) const
{
    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    RSRenderNode::DumpNodeType(nodeType_, out);
    out += "[" + std::to_string(nodeId_) + "] Draw Shadow Only\n";
}

void RSRenderNodeShadowDrawable::OnDetach()
{
    nodeDrawable_->SetSkip(SkipType::NONE);
}
} // namespace OHOS::Rosen::DrawableV2
