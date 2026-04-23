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

#include "params/rs_canvas_drawing_render_params.h"
#include "platform/common/rs_log.h"
namespace OHOS::Rosen {
RSCanvasDrawingRenderParams::RSCanvasDrawingRenderParams(NodeId id) : RSRenderParams(id) {}

void RSCanvasDrawingRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    RSRenderParams::OnSync(target);
    if (!canvasDrawingNodeSurfaceChanged_) {
        return;
    }
    auto targetParams = static_cast<RSCanvasDrawingRenderParams*>(target.get());
    targetParams->canvasDrawingNodeSurfaceChanged_ = true;
    targetParams->surfaceParams_.width = surfaceParams_.width;
    targetParams->surfaceParams_.height = surfaceParams_.height;
    targetParams->surfaceParams_.colorSpace = surfaceParams_.colorSpace;
    targetParams->canvasDrawingResetSurfaceIndex_ = canvasDrawingResetSurfaceIndex_.load();
    if (GetParamsType() == RSRenderParamsType::RS_PARAM_OWNED_BY_DRAWABLE) {
        return;
    }
    canvasDrawingNodeSurfaceChanged_ = false;
}

void RSCanvasDrawingRenderParams::SetCanvasDrawingSurfaceChanged(bool changeFlag)
{
    if (changeFlag) {
        needSync_ = true;
    }
    canvasDrawingNodeSurfaceChanged_ = changeFlag;
}

void RSCanvasDrawingRenderParams::SetCanvasDrawingResetSurfaceIndex(uint32_t index)
{
    if (index == canvasDrawingResetSurfaceIndex_) {
        return;
    }
    canvasDrawingResetSurfaceIndex_.store(index);
    needSync_ = true;
}

void RSCanvasDrawingRenderParams::SetCanvasDrawingSurfaceParams(int width, int height, GraphicColorGamut colorSpace)
{
    surfaceParams_.width = width;
    surfaceParams_.height = height;
    surfaceParams_.colorSpace = colorSpace;
}
} // namespace OHOS::Rosen