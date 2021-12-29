/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "platform/drawing/rs_platform_canvas.h"

#include "display_type.h"
#include "include/core/SkImageInfo.h"

#include "platform/common/rs_log.h"

namespace OHOS{
namespace Rosen{
PlatformCanvas::PlatformCanvas()
{
    config_ = {
        .width = 0,
        .height = 0,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
}

void PlatformCanvas::SetSurface(OHOS::Surface* surface)
{
    sptr<Surface> surfacePtr(surface);
    if (surface_ != surfacePtr) {
        surface_ = surfacePtr;
        if (surface_ != nullptr) {
            surface_->SetQueueSize(5); // Set buffer size to 5
        }
    }
}

SkCanvas* PlatformCanvas::AcquireCanvas()
{
    if (sk_surface_ == nullptr || sk_surface_->width() != config_.width || sk_surface_->height() != config_.height) {
        SkImageInfo image_info = SkImageInfo::Make(
            config_.width, config_.height, kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
        sk_surface_ = SkSurface::MakeRaster(image_info);
    }

    SkCanvas* canvas = sk_surface_->getCanvas();
    canvas->resetMatrix();
    return canvas;
}

void PlatformCanvas::FlushBuffer()
{
    if (surface_ == nullptr || sk_surface_ == nullptr) {
        ROSEN_LOGW("PlatformCanvas::FlushBuffer fail");
        return;
    }
    sk_surface_->getCanvas()->flush();

    int32_t releaseFence;
    if (enable_) {
        std::unique_lock<std::mutex> lock(mutex_);
        SurfaceError ret = surface_->RequestBuffer(buffer_, releaseFence, config_);
        if (ret != SURFACE_ERROR_OK) {
            ROSEN_LOGW("sync request buffer failed");
        }
    }
    if (buffer_ == nullptr) {
        ROSEN_LOGW("buffer is null");
    }

    auto addr = static_cast<uint32_t*>(buffer_->GetVirAddr());
    if (addr == nullptr || buffer_->GetWidth() <= 0 || buffer_->GetHeight() <= 0) {
        return;
    }
    SkImageInfo info = SkImageInfo::Make(
            buffer_->GetWidth(), buffer_->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    auto canvas = SkCanvas::MakeRasterDirect(info, addr, buffer_->GetSize() / buffer_->GetHeight());

    sk_surface_->draw(canvas.get(), 0.0f, 0.0f, nullptr);

    BufferFlushConfig flushConfig = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = config_.width,
            .h = config_.height,
        },
    };
    if (enable_) {
        std::unique_lock<std::mutex> lock(mutex_);
        SurfaceError ret = surface_->FlushBuffer(buffer_, -1, flushConfig);
        if (ret != SURFACE_ERROR_OK) {
            ROSEN_LOGW("sync flush buffer failed");
        }
    }
}

void PlatformCanvas::SetSurfaceSize(int width, int height)
{
    int pixelBase = 16;
    config_.width = (width % pixelBase == 0) ? width : ((width / pixelBase + 1) * pixelBase);
    config_.height = height;
}

void PlatformCanvas::Disable()
{
    std::unique_lock<std::mutex> lock(mutex_);
    enable_ = false;
}
} // namespace Rosen
} // namespace OHOS
