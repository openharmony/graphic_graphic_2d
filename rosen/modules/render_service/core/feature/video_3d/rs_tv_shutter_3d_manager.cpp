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

#include "rs_tv_shutter_3d_manager.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSTvShutter3DManager"

namespace OHOS::Rosen {
RSTvShutter3DManager& RSTvShutter3DManager::Instance()
{
    static RSTvShutter3DManager instance;
    return instance;
}

bool RSTvShutter3DManager::Is3DEnabled(UIMode3D uiMode3D) const
{
    if (uiMode3D != UIMode3D::MODE_SHUTTER_3D) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return offscreenSurface3D_ != nullptr && offscreenCanvas3D_ != nullptr && curCanvasBak_ != nullptr;
}

void RSTvShutter3DManager::Init3DContext(UIMode3D uiMode3D, int32_t width, int32_t height,
    std::shared_ptr<RSPaintFilterCanvas>& curCanvas)
{
    if (uiMode3D != UIMode3D::MODE_SHUTTER_3D) {
        return;
    }

    if (!curCanvas) {
        RS_LOGE("RSTvShutter3DManager::Init3DContext curCanvas is null");
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    Drawing::ImageInfo info = {width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    offscreenSurface3D_ = curCanvas->GetSurface()->MakeSurface(info);
    if (!offscreenSurface3D_) {
        RS_LOGE("RSTvShutter3DManager::Init3DContext failed to create offscreenSurface3D");
        return;
    }

    offscreenCanvas3D_ = std::make_shared<RSPaintFilterCanvas>(offscreenSurface3D_.get());
    if (!offscreenCanvas3D_) {
        RS_LOGE("RSTvShutter3DManager::Init3DContext failed to create offscreenCanvas3D");
        offscreenSurface3D_ = nullptr;
        return;
    }

    curCanvasBak_ = curCanvas;
    curCanvas = offscreenCanvas3D_;

    if (videoDimType_ == VideoDimType::VIDEO_DIM_TYPE_2D) {
        videoDimType_ = VideoDimType::VIDEO_DIM_TYPE_3D_SBS;
    }
}

void RSTvShutter3DManager::Process3DImage(std::shared_ptr<RSPaintFilterCanvas> targetCanvas,
    std::shared_ptr<Drawing::Image> snapshot, VideoDimType type)
{
    if (!targetCanvas || !snapshot) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    int32_t frameWidth = snapshot->GetWidth();
    int32_t frameHeight = snapshot->GetHeight();
    Drawing::SamplingOptions samplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Rect srcFull(0, 0, frameWidth, frameHeight);

    if (type == VideoDimType::VIDEO_DIM_TYPE_3D_SBS) {
        int32_t halfWidth = frameWidth / 2;
        Drawing::Rect dstLeft(0, 0, halfWidth, frameHeight);
        Drawing::Rect dstRight(halfWidth, 0, frameWidth, frameHeight);
        targetCanvas->DrawImageRect(*snapshot, srcFull, dstLeft, samplingOptions,
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        targetCanvas->DrawImageRect(*snapshot, srcFull, dstRight, samplingOptions,
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    } else if (type == VideoDimType::VIDEO_DIM_TYPE_3D_TAB) {
        int32_t halfHeight = frameHeight / 2;
        Drawing::Rect dstTop(0, 0, frameWidth, halfHeight);
        Drawing::Rect dstBottom(0, halfHeight, frameWidth, frameHeight);
        targetCanvas->DrawImageRect(*snapshot, srcFull, dstTop, samplingOptions,
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        targetCanvas->DrawImageRect(*snapshot, srcFull, dstBottom, samplingOptions,
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    }
}

void RSTvShutter3DManager::Release3DContext()
{
    std::lock_guard<std::mutex> lock(mutex_);
    offscreenSurface3D_ = nullptr;
    offscreenCanvas3D_ = nullptr;
    curCanvasBak_ = nullptr;
}

std::shared_ptr<RSPaintFilterCanvas> RSTvShutter3DManager::GetOffscreenCanvas() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return offscreenCanvas3D_;
}

std::shared_ptr<RSPaintFilterCanvas> RSTvShutter3DManager::GetBackupCanvas() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return curCanvasBak_;
}

void RSTvShutter3DManager::SetVideoDimType(VideoDimType type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    videoDimType_ = type;
}

VideoDimType RSTvShutter3DManager::GetVideoDimType() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return videoDimType_;
}

bool RSTvShutter3DManager::ShouldSet3DShutterComposition(UIMode3D uiMode3D, VideoDimType videoDimType) const
{
    return uiMode3D == UIMode3D::MODE_SHUTTER_3D && videoDimType != VideoDimType::VIDEO_DIM_TYPE_2D;
}

void RSTvShutter3DManager::UpdateSurfaceNodeCompositionType(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, UIMode3D uiMode3D)
{
    if (!surfaceNode) {
        return;
    }
    surfaceNode->ResetCompositionType();
    auto videoDimType = surfaceNode->GetVideoDimType();
    if (ShouldSet3DShutterComposition(uiMode3D, videoDimType)) {
        RS_TRACE_NAME_FMT("MODE_SHUTTER_3D name:%s id:%" PRIu64 ", videoDimType: %d",
             surfaceNode->GetName().c_str(), surfaceNode->GetId(), videoDimType);
        surfaceNode->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);
        RS_LOGD("CollectInfoForHardwareComposer set %{public}s Set COMPOSITION_3D_SHUTTER",
            surfaceNode->GetName().c_str());
    }
}

bool RSTvShutter3DManager::Prepare3DForDraw(const RSScreenRenderParams& params,
    const std::shared_ptr<Drawing::Surface>& drSurface, std::shared_ptr<RSPaintFilterCanvas>& curCanvas)
{
    if (!drSurface) {
        RS_LOGE("RSTvShutter3DManager::Prepare3DForDraw drSurface is null");
        return false;
    }
    UIMode3D uiMode3D = params.GetUIMode3D();
    VideoDimType videoDimType = params.GetVideoDimType();
    int32_t width = drSurface->Width();
    int32_t height = drSurface->Height();
    RS_TRACE_NAME_FMT("UX 3D: ui3dMode[%d], videoDimType[%d]", uiMode3D, videoDimType);
    SetVideoDimType(videoDimType);
    if (uiMode3D == UIMode3D::MODE_SHUTTER_3D) {
        Init3DContext(uiMode3D, width, height, curCanvas);
        if (!Is3DEnabled(uiMode3D)) {
            RS_LOGE("RSTvShutter3DManager::Prepare3DForDraw failed to init 3D context");
            return false;
        }
    }
    return true;
}

bool RSTvShutter3DManager::Process3DForFlush(UIMode3D uiMode3D, std::shared_ptr<RSPaintFilterCanvas>& curCanvas)
{
    if (!Is3DEnabled(uiMode3D)) {
        return true;
    }

    auto snapshot = offscreenSurface3D_->GetImageSnapshot();
    if (!snapshot) {
        RS_LOGE("RSTvShutter3DManager::Process3DForFlush failed to get image snapshot");
        Release3DContext();
        return false;
    }

    auto backupCanvas = GetBackupCanvas();
    if (!backupCanvas) {
        RS_LOGE("RSTvShutter3DManager::Process3DForFlush backupCanvas is null");
        Release3DContext();
        return false;
    }

    curCanvas = backupCanvas;
    Process3DImage(curCanvas, snapshot, GetVideoDimType());
    Release3DContext();
    return true;
}
} // namespace OHOS::Rosen