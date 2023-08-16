/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos/rs_render_surface_ohos.h"

#include "include/core/SkSurface.h"

#include "utils/log.h"
#include "rs_trace.h"
#include "window.h"

#if !defined(NEW_SKIA)
#include "memory/rs_tag_tracker.h"
#endif

#include "render_backend_utils.h"

namespace OHOS {
namespace Rosen {
RSRenderSurfaceOhos::RSRenderSurfaceOhos(const sptr<Surface>& surface,
    const std::shared_ptr<DrawingContext>& drawingContext)
{
    drawingContext_ = drawingContext;
    frame_ = std::make_shared<RSRenderSurfaceFrame>();
    std::shared_ptr<FrameConfig> frameConfig = std::make_shared<FrameConfig>();
    std::shared_ptr<SurfaceConfig> surfaceConfig = std::make_shared<SurfaceConfig>();
    std::shared_ptr<EGLState> eglState = std::make_shared<EGLState>();
    surfaceConfig->producer = surface;
    frame_->frameConfig = frameConfig;
    frame_->surfaceConfig = surfaceConfig;
    frame_->eglState = eglState;
}

RSRenderSurfaceOhos::~RSRenderSurfaceOhos()
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to ~RSRenderSurfaceOhos, frame_ is invalid");
        return;
    }
    std::shared_ptr<SurfaceConfig> surfaceConfig = frame_->surfaceConfig;
    if (surfaceConfig->nativeWindow != nullptr) {
        DestoryNativeWindow(surfaceConfig->nativeWindow);
        surfaceConfig->nativeWindow = nullptr;
    }
    if (renderContext_ != nullptr) {
        renderContext_->DestroySurface(frame_);
    }
    frame_ = nullptr;
    drawingContext_ = nullptr;
    renderContext_ = nullptr;
}

bool RSRenderSurfaceOhos::IsValid() const
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to check surface is valid, frame_ is invalid");
        return false;
    }
    return frame_->surfaceConfig->producer != nullptr;
}

sptr<Surface> RSRenderSurfaceOhos::GetSurfaceOhos() const
{
    if (!IsValid()) {
        LOGE("Failed to get ohos render surface, render surface is invalid");
        return nullptr;
    }
    return frame_->surfaceConfig->producer;
}

uint32_t RSRenderSurfaceOhos::GetQueueSize() const
{
    sptr<Surface> producer = GetSurfaceOhos();
    if (producer == nullptr) {
        LOGE("Failed to get queue size, render surface is invalid");
        return 0;
    }
    return producer->GetQueueSize();
}

std::shared_ptr<RSRenderSurfaceFrame> RSRenderSurfaceOhos::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC)
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to request frame, renderContext_ is nullptr");
        return nullptr;
    }
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to request frame, frame_ is invalid");
        return nullptr;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    frameConfig->width = width;
    frameConfig->height = height;
    frameConfig->uiTimestamp = uiTimestamp;
    frameConfig->useAFBC = useAFBC;
    frameConfig->skSurface = nullptr;

    std::shared_ptr<SurfaceConfig> surfaceConfig = frame_->surfaceConfig;
    if (surfaceConfig->producer == nullptr) {
        LOGE("Failed to request frame, surfaceConfig->producer is invalid");
        return nullptr;
    }
    bool isSuccess = renderContext_->CreateSurface(frame_);
    if (!isSuccess) {
        LOGE("Failed to request frame, create surface by renderContext_ failed");
        return nullptr;
    }
    LOGD("Request frame successfully, width is %{public}d, height is %{public}d", width, height);
    return frame_;
}

bool RSRenderSurfaceOhos::FlushFrame(uint64_t uiTimestamp)
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to flush frame, renderContext_ is nullptr");
        return false;
    }
    // gles render flush
    RenderType renderType = renderContext_->GetRenderType();
    if (renderType == RenderType::GLES) {
        RenderFrame();
    }
    renderContext_->SwapBuffers(frame_);
    LOGD("FlushFrame successfully");
    return true;
}

void RSRenderSurfaceOhos::SetUiTimeStamp(uint64_t uiTimestamp)
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to set ui timestamp, renderContext_ is nullptr");
        return;
    }

    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to set ui timestamp, frame_ is invalid");
        return;
    }
    RenderType renderType = renderContext_->GetRenderType();
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    std::shared_ptr<SurfaceConfig> surfaceConfig = frame_->surfaceConfig;
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    if (renderType == RenderType::GLES || renderType == RenderType::VULKAN) {
        NativeWindowHandleOpt(surfaceConfig->nativeWindow, SET_UI_TIMESTAMP, duration);
    } else {
        if (frameConfig->buffer == nullptr) {
            LOGE("Failed to set ui timestamp, frameConfig buffer is nullptr");
            return;
        }
        if (frameConfig->buffer->GetExtraData() == nullptr) {
            LOGE("Failed to set ui timestamp, frame_->buffer_->GetExtraData is nullptr");
            return;
        }
        GSError ret = frameConfig->buffer->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
        if (ret != GSERROR_OK) {
            LOGE("Failed to set ui timestamp, frame buffer get ExtraSet failed");
        }
    }
}

void RSRenderSurfaceOhos::SetDamageRegion(const std::vector<RectI>& rects)
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to set damage region, renderContext_ is nullptr");
        return;
    }
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to set damage region, frame_ is invalid");
        return;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    frameConfig->damageRects = rects;
    renderContext_->DamageFrame(frame_);
}

int32_t RSRenderSurfaceOhos::GetBufferAge()
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to set damage region, renderContext_ is nullptr");
        return 0;
    }
    return renderContext_->GetBufferAge();
}

void RSRenderSurfaceOhos::ClearBuffer()
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to clear buffer, renderContext_ is nullptr");
        return;
    }
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to clear buffer, frame_ is invalid");
        return;
    }
    std::shared_ptr<SurfaceConfig> surfaceConfig = frame_->surfaceConfig;
    DestoryNativeWindow(surfaceConfig->nativeWindow);
    surfaceConfig->nativeWindow = nullptr;
    renderContext_->MakeCurrent();
    renderContext_->DestroySurface(frame_);
    if (!IsValid()) {
        LOGE("Failed to clear buffer, render surface is invalid");
        return;
    }
    surfaceConfig->producer->GoBackground();
    LOGD("Clear buffer successfully");
}

SkCanvas* RSRenderSurfaceOhos::GetCanvas()
{
    sk_sp<SkSurface> skSurface = GetSurface();
    if (skSurface == nullptr) {
        LOGE("Failed to get canvas, skSurface is nullptr");
        return nullptr;
    }
    return skSurface->getCanvas();
}

sk_sp<SkSurface> RSRenderSurfaceOhos::GetSurface()
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to get surface, frame_ is invalid");
        return nullptr;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    if (frameConfig->skSurface == nullptr) {
        if (drawingContext_ == nullptr) {
            LOGE("Failed to get surface, drawingContext_ is nullptr");
            return nullptr;
        }
        drawingContext_->SetUpDrawingContext();
        frameConfig->skSurface = drawingContext_->AcquireSurface(frame_);
    }
    return frameConfig->skSurface;
}

GraphicColorGamut RSRenderSurfaceOhos::GetColorSpace()
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to get color space, frame_ is invalid");
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    return frameConfig->colorSpace;
}

void RSRenderSurfaceOhos::SetColorSpace(GraphicColorGamut colorSpace)
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to set color space, frame_ is invalid");
        return;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    frameConfig->colorSpace = colorSpace;
}

void RSRenderSurfaceOhos::SetSurfaceBufferUsage(uint64_t usage)
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to set surface buffer usage, frame_ is invalid");
        return;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    frameConfig->bufferUsage = usage;
}

void RSRenderSurfaceOhos::SetSurfacePixelFormat(uint64_t pixelFormat)
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to set suface pixel format, frame_ is invalid");
        return;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    frameConfig->pixelFormat = pixelFormat;
}

void RSRenderSurfaceOhos::SetReleaseFence(const int32_t& fence)
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to set release fence, frame_ is invalid");
        return;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    frameConfig->releaseFence = fence;
}

int32_t RSRenderSurfaceOhos::GetReleaseFence() const
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to get release fence, frame_ is invalid");
        return -1;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    return frameConfig->releaseFence;
}

void RSRenderSurfaceOhos::RenderFrame()
{
    if (!RenderBackendUtils::IsValidFrame(frame_)) {
        LOGE("Failed to render frame, frame_ is invalid");
        return;
    }
    std::shared_ptr<FrameConfig> frameConfig = frame_->frameConfig;
    if (frameConfig->skSurface == nullptr) {
        LOGE("Failed to render frame, frameConfig skSurface is nullptr");
        return;
    }
    RS_TRACE_FUNC();
    if (frameConfig->skSurface->getCanvas() != nullptr) {
        LOGD("Render frame successfully");
        frameConfig->skSurface->getCanvas()->flush();
    } else {
        LOGE("Failed to render frame, canvas is nullptr");
    }
}
}
}