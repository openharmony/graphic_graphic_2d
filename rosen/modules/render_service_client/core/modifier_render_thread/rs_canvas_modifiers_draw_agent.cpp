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

#include "common/rs_common_utils.h"

namespace OHOS::Rosen {
RSCanvasModifiersDrawAgent::RSCanvasModifiersDrawAgent(std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    canvasModifiersDraw_ = std::make_shared<RSCanvasModifiersDraw>(weakRenderInterface);
}

void RSCanvasModifiersDrawAgent::SetCacheDir(const std::string& cacheDir)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = canvasModifiersDraw_;
    canvasModifiersDraw_->PostTask([weakCanvasModifiersDraw, path = cacheDir] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        CHECK_NULL_VOID(canvasModifiersDraw);
        canvasModifiersDraw->SetCacheDir(path);
    });
}

void RSCanvasModifiersDrawAgent::OnNodeCreate(NodeId nodeId, GraphicColorGamut colorSpace)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = canvasModifiersDraw_;
    canvasModifiersDraw_->PostTask([weakCanvasModifiersDraw, nodeId, colorSpace] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        CHECK_NULL_VOID(canvasModifiersDraw);
        canvasModifiersDraw->OnNodeCreate(nodeId, colorSpace);
    });
}

void RSCanvasModifiersDrawAgent::OnNodeRelease(NodeId nodeId)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = canvasModifiersDraw_;
    canvasModifiersDraw_->PostTask([weakCanvasModifiersDraw, nodeId] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        CHECK_NULL_VOID(canvasModifiersDraw);
        canvasModifiersDraw->OnNodeRelease(nodeId);
    });
}

void RSCanvasModifiersDrawAgent::OnNodeStateChanged(NodeId nodeId, RSNodeState state)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = canvasModifiersDraw_;
    canvasModifiersDraw_->PostTask([weakCanvasModifiersDraw, nodeId, state] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        CHECK_NULL_VOID(canvasModifiersDraw);
        canvasModifiersDraw->OnNodeStateChanged(nodeId, state);
    });
}

void RSCanvasModifiersDrawAgent::ResetSurface(NodeId nodeId, int width, int height, bool sizeOutOfGpuLimit)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = canvasModifiersDraw_;
    canvasModifiersDraw_->PostTask([weakCanvasModifiersDraw, nodeId, width, height, sizeOutOfGpuLimit] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        CHECK_NULL_VOID(canvasModifiersDraw);
        canvasModifiersDraw->ResetSurface(nodeId, width, height, sizeOutOfGpuLimit);
    });
}

bool RSCanvasModifiersDrawAgent::GetBitmap(NodeId nodeId, Drawing::Bitmap& bitmap)
{
    bool ret = false;
    canvasModifiersDraw_->PostSyncTask([canvasModifiersDraw = canvasModifiersDraw_, nodeId, &bitmap, &ret] {
        ret = canvasModifiersDraw->GetBitmap(nodeId, bitmap);
    });
    return ret;
}

bool RSCanvasModifiersDrawAgent::GetPixelMap(NodeId nodeId, std::shared_ptr<Media::PixelMap> pixelMap,
    const Drawing::Rect* rect, Drawing::DrawCmdListPtr drawCmdList)
{
    bool ret = false;
    canvasModifiersDraw_->PostSyncTask(
        [canvasModifiersDraw = canvasModifiersDraw_, nodeId, pixelMap, rect, drawCmdList, &ret] {
            ret = canvasModifiersDraw->GetPixelMap(nodeId, pixelMap, rect, drawCmdList);
        });
    return ret;
}

void RSCanvasModifiersDrawAgent::UpdateCanvasContent(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = canvasModifiersDraw_;
    canvasModifiersDraw_->PostTask([weakCanvasModifiersDraw, nodeId, drawCmdList] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        CHECK_NULL_VOID(canvasModifiersDraw);
        canvasModifiersDraw->UpdateCanvasContent(nodeId, drawCmdList);
    });
}

void RSCanvasModifiersDrawAgent::SubmitAndCollectCanvasBuffers()
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = canvasModifiersDraw_;
    canvasModifiersDraw_->PostTask([weakCanvasModifiersDraw] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        CHECK_NULL_VOID(canvasModifiersDraw);
        canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    });
}

std::unique_ptr<RSCommand> RSCanvasModifiersDrawAgent::SyncAndWait()
{
    std::unique_ptr<RSCommand> command = nullptr;
    canvasModifiersDraw_->PostSyncTask(
        [canvasModifiersDraw = canvasModifiersDraw_, &command] { command = canvasModifiersDraw->SyncAndWait(); });
    return command;
}

void RSCanvasModifiersDrawAgent::WaitAllTasksFinish()
{
    if (canvasModifiersDraw_->RemoveCleanFreeBuffersTask()) {
        canvasModifiersDraw_->PostSyncTask(
            [canvasModifiersDraw = canvasModifiersDraw_] { canvasModifiersDraw->ReleaseResource(); });
    }
}

void RSCanvasModifiersDrawAgent::Destroy()
{
    canvasModifiersDraw_->Destroy();
}
} // namespace OHOS::Rosen
