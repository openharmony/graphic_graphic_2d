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

#include "modifier_render_thread/rs_canvas_modifiers_draw_agent.h"

namespace OHOS::Rosen {
RSCanvasModifiersDrawAgent::RSCanvasModifiersDrawAgent()
{
    canvasModifiersDraw_ = std::make_shared<RSCanvasModifiersDraw>();
}

RSCanvasModifiersDrawAgent::~RSCanvasModifiersDrawAgent() = default;

void RSCanvasModifiersDrawAgent::SetCacheDir(const std::string& cacheDir)
{
    canvasModifiersDraw_->SetCacheDir(cacheDir);
}

void RSCanvasModifiersDrawAgent::QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    canvasModifiersDraw_->QueryMaxGpuBufferSize(maxWidth, maxHeight);
}

void RSCanvasModifiersDrawAgent::OnNodeCreate(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    canvasModifiersDraw_->OnNodeCreate(nodeId, weakRenderInterface);
}

void RSCanvasModifiersDrawAgent::OnNodeRelease(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    canvasModifiersDraw_->OnNodeRelease(nodeId, weakRenderInterface);
}

void RSCanvasModifiersDrawAgent::OnNodeStateChanged(NodeId nodeId, RSNodeState state)
{
    canvasModifiersDraw_->OnNodeStateChanged(nodeId, state);
}

void RSCanvasModifiersDrawAgent::ResetSurface(
    NodeId nodeId, int width, int height, bool sizeOutOfGpuLimit, GraphicColorGamut colorSpace)
{
    canvasModifiersDraw_->ResetSurface(nodeId, width, height, sizeOutOfGpuLimit, colorSpace);
}

bool RSCanvasModifiersDrawAgent::GetBitmap(NodeId nodeId, Drawing::Bitmap& bitmap)
{
    return canvasModifiersDraw_->GetBitmap(nodeId, bitmap);
}

bool RSCanvasModifiersDrawAgent::GetPixelMap(NodeId nodeId, std::shared_ptr<Media::PixelMap> pixelMap,
    const Drawing::Rect* rect, Drawing::DrawCmdListPtr drawCmdList)
{
    return canvasModifiersDraw_->GetPixelMap(nodeId, pixelMap, rect, drawCmdList);
}

void RSCanvasModifiersDrawAgent::UpdateCanvasContent(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList)
{
    canvasModifiersDraw_->UpdateCanvasContent(nodeId, drawCmdList);
}

void RSCanvasModifiersDrawAgent::SubmitAndCollectCanvasBuffers()
{
    canvasModifiersDraw_->SubmitAndCollectCanvasBuffers();
}

void RSCanvasModifiersDrawAgent::SwapTransactionConfigList(std::vector<RSTransactionConfig>& transactionConfigList)
{
    canvasModifiersDraw_->SwapTransactionConfigList(transactionConfigList);
}

void RSCanvasModifiersDrawAgent::WaitAllTasksFinish()
{
    canvasModifiersDraw_->WaitAllTasksFinish();
}

void RSCanvasModifiersDrawAgent::Destroy()
{
    canvasModifiersDraw_->Destroy();
}
} // namespace OHOS::Rosen
