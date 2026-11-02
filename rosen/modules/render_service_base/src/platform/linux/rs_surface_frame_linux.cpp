/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * the distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_surface_frame_linux.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSurfaceFrameLinux::RSSurfaceFrameLinux(int32_t width, int32_t height)
    : width_(width), height_(height)
{
    addr_ = std::make_unique<uint32_t[]>(width_ * height_);
}

void RSSurfaceFrameLinux::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
}

int32_t RSSurfaceFrameLinux::GetBufferAge() const
{
    return -1;
}

Drawing::Canvas* RSSurfaceFrameLinux::GetCanvas()
{
    if (surface_ == nullptr) {
        CreateSurface();
    }
    if (surface_ == nullptr) {
        return nullptr;
    }
    return surface_->GetCanvas().get();
}

std::shared_ptr<Drawing::Surface> RSSurfaceFrameLinux::GetSurface()
{
    if (surface_ == nullptr) {
        CreateSurface();
    }
    return surface_;
}

void RSSurfaceFrameLinux::SetRenderContext(std::shared_ptr<RenderContext> context)
{
    renderContext_ = context;
}

void RSSurfaceFrameLinux::CreateSurface()
{
    if (addr_ == nullptr) {
        ROSEN_LOGW("RSSurfaceFrameLinux::CreateSurface buffer is nullptr");
        return;
    }
    bitmap_ = std::make_shared<Drawing::Bitmap>();
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap_->Build(width_, height_, format);
    bitmap_->SetPixels(addr_.get());
    surface_ = std::make_shared<Drawing::Surface>();
    surface_->Bind(*bitmap_);
}
} // namespace Rosen
} // namespace OHOS
