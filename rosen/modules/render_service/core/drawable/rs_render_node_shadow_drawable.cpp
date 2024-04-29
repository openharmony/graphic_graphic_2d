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
#include "pipeline/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::DrawableV2 {
RSRenderNodeShadowDrawable::Registrar RSRenderNodeShadowDrawable::instance_;

RSRenderNodeShadowDrawable::Ptr RSRenderNodeShadowDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSRenderNodeShadowDrawable(std::move(node));
}

void RSRenderNodeShadowDrawable::Draw(Drawing::Canvas& canvas)
{
    // rect is not directly used, make a dummy rect
    static Drawing::Rect rect;

    auto shadowIndex = drawCmdIndex_.shadowIndex_;
    if (shadowIndex == -1) {
        return;
    }

    const auto& params = GetRenderParams();
    if (!params) {
        RS_LOGE("params is nullptr");
        return;
    }
    if (!params->GetShouldPaint()) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas, RSRenderNodeDrawable::GetParentSurfaceMatrix());
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if ((!uniParam || uniParam->IsOpDropped()) && QuickReject(canvas, params->GetLocalDrawRect())) {
        return;
    }

    SetSkip(SkipType::NONE);
    DrawRangeImpl(canvas, rect, 0, shadowIndex + 1);
}

void RSRenderNodeShadowDrawable::DumpDrawableTree(int32_t depth, std::string& out) const
{
    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    RSRenderNode::DumpNodeType(nodeType_, out);
    out += "[" + std::to_string(nodeId_) + "] Draw Shadow Only\n";
}
} // namespace OHOS::Rosen::DrawableV2
