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

namespace OHOS {
namespace Rosen {
RSRenderSurfaceOhos::RSRenderSurfaceOhos(const sptr<Surface>& producer,
    const std::shared_ptr<DrawingContext>& drawingContext)
{
    frame_ = std::make_shared<RSRenderSurfaceFrame>();
    frame_->producer_ = producer;
    drawingContext_ = drawingContext;
}

RSRenderSurfaceOhos::~RSRenderSurfaceOhos()
{
    frame_ = nullptr;
    drawingContext_ = nullptr;
    renderContext_ = nullptr;
}
bool RSRenderSurfaceOhos::IsValid()
{
    if (frame_ == nullptr) {
        LOGE("Failed to get queue size, frame_ is nullptr");
        return false;
    }
    return frame_->producer_ != nullptr;
}

sptr<Surface> RSRenderSurfaceOhos::GetSurfaceOhos() const
{
    if (!IsValid()) {
        LOGE("Failed to get ohos render surface, render surface is invalid");
        return nullptr;
    }
    return frame_->producer_;
}

uint32_t RSRenderSurfaceOhos::GetQueueSize() const
{
    if (!IsValid()) {
        LOGE("Failed to get queue size, render surface is invalid");
        return 0;
    }
    return frame_->producer_->GetQueueSize();
}

std::shared_ptr<RSRenderSurfaceFrame> RSRenderSurfaceOhos::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC)
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to request frame, renderContext_ is nullptr");
        return nullptr;
    }

    if (frame_ == nullptr) {
        LOGE("Failed to request frame, frame_ is nullptr");
        return nullptr;
    }
    
    frame_->width_ = width;
    frame_->height_ = height;
    frame_->uiTimestamp_ = uiTimestamp;
    
    bool isSuccess = renderContext_->CreateSurface(frame_);
    if (!isSuccess) {
        LOGE("Failed to request frame, create surface by renderContext_ failed");
        return nullptr;
    }

    LOGD("Request frame successfully, width is %d, height is %d", width, height);
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
    LOGD("Flush frame successfully");
    return true;
}

void RSRenderSurfaceOhos::SetUiTimeStamp(uint64_t uiTimestamp)
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to set ui timestamp, renderContext_ is nullptr");
        return;
    }

    RenderType renderType = renderContext_->GetRenderType();
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    if (renderType == RenderType::GLES || renderType == RenderType::VULKAN) {
        NativeWindowHandleOpt(frame_->nativeWindow_, SET_UI_TIMESTAMP, duration);
    } else {
        if (frame_ == nullptr || frame_->buffer_ == nullptr) {
            LOGE("Failed to set ui timestamp, frame buffer is nullptr");
            return;
        }
        if (frame_->buffer_->GetExtraData() == nullptr) {
            LOGE("Failed to set ui timestamp, frame_->buffer_->GetExtraData is nullptr");
            return;
        }
        GSError ret = frame_->buffer_->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
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

    if (frame_ == nullptr) {
        LOGE("Failed to set damage region, frame_ is nullptr");
        return;
    }
    frame_->damageRects = rects;
    renderContext_->DamageFrame(frame_);
}

int32_t RSRenderSurfaceOhos::GetBufferAge()
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to set damage region, renderContext_ is nullptr");
        return;
    }
    return renderContext_->GetBufferAge();
}

void RSRenderSurfaceOhos::ClearBuffer()
{
    if (renderContext_ == nullptr) {
        LOGE("Failed to set clear buffer, renderContext_ is nullptr");
        return;
    }
    DestoryNativeWindow(frame_->nativeWindow_);
    renderContext_->MakeCurrent();
    renderContext_->DestroySurface();
    if (!IsValid()) {
        LOGE("Failed to clear buffer, render surface is invalid");
        return;
    }
    frame_->producer_->GoBackground();
}

SkCanvas* RSRenderSurfaceOhos::GetCanvas()
{
    sk_sp<SkSurface> skSurface = GetSurface();
    if (skSurface == nullptr) {
        LOGE("Failed to get canvas, skSurface is nullptr")
        return nullptr;
    }
    return skSurface->getCanvas();
}

sk_sp<SkSurface> RSRenderSurfaceOhos::GetSurface()
{
    if (frame_ == nullptr) {
        LOGE("Failed to get surface, frame_ is nullptr");
        return nullptr;
    }
    if (frame_->skSurface_ == nullptr) {
        if (drawingContext_ == nullptr) {
            LOGE("Failed to get surface, drawingContext_ is nullptr");
            return nullptr;
        }
        drawingContext_->SetUpDrawingContext();
        frame_->skSurface_ = drawingContext_->AcquireSurface(frame_);
    }
    return frame_->skSurface_;
}

ColorGamut RSRenderSurfaceOhos::GetColorSpace()
{
    if (frame_ == nullptr) {
        LOGE("Failed to get color space, frame_ is nullptr");
        return ColorGamut::COLOR_GAMUT_SRGB;
    }
    return frame_->colorSpace_;
}

void RSRenderSurfaceOhos::SetColorSpace(ColorGamut colorSpace)
{
    if (frame_ == nullptr) {
        LOGE("Failed to set color space, frame_ is nullptr");
        return;
    }
    frame_->colorSpace_ = colorSpace;
}

void RSRenderSurfaceOhos::SetSurfaceBufferUsage(uint64_t usage)
{
    if (frame_ == nullptr) {
        LOGE("Failed to set suface buffer usage, frame_ is nullptr");
        return;
    }
    frame_->bufferUsage_ = usage;
}

void RSRenderSurfaceOhos::SetSurfacePixelFormat(uint64_t pixelFormat)
{
    if (frame_ == nullptr) {
        LOGE("Failed to set suface pixel format, frame_ is nullptr");
        return;
    }
    frame_->pixelFormat_ = pixelFormat;
}

void RSRenderSurfaceOhos::SetReleaseFence(const int32_t& fence)
{
    if (frame_ == nullptr) {
        LOGE("Failed to set release fence, frame_ is nullptr");
        return;
    }
    frame_->releaseFence_ = fence;
}

int32_t RSRenderSurfaceOhos::GetReleaseFence() const
{
    if (frame_ == nullptr) {
        LOGE("Failed to get release fence, frame_ is nullptr");
        return -1;
    }
    return frame_->releaseFence_;
}

void RSRenderSurfaceOhos::RenderFrame()
{
    if (frame_ == nullptr) {
        LOGE("Failed to render frame, frame_ is nullptr");
        return;
    }
    if (frame_->skSurface_ == nullptr) {
        LOGE("Failed to render frame, frame_->skSurface is nullptr");
        return;
    }
    RS_TRACE_FUNC();
    if (frame_->skSurface_->getCanvas() != nullptr) {
        frame_->skSurface_->getCanvas()->flush();
    } else {
        LOGE("Failed to render frame, canvas is nullptr");
    }
}
}
}