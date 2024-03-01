/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_surface_frame_ohos_raster.h"
#include <cstdint>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSSurfaceFrameOhosRaster::RSSurfaceFrameOhosRaster(int32_t width, int32_t height)
{
    requestConfig_.width = width;
    requestConfig_.height = height;
    flushConfig_.damage.w = width;
    flushConfig_.damage.h = height;
}

void RSSurfaceFrameOhosRaster::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    flushConfig_.damage.x = left;
    flushConfig_.damage.y = top;
    flushConfig_.damage.w = width;
    flushConfig_.damage.h = height;
}

void RSSurfaceFrameOhosRaster::SetDamageRegion(const std::vector<RectI> &rects)
{
    // currently not support SetDamageRegion
    ROSEN_LOGE("currently not support SetDamageRegion");
}

int32_t RSSurfaceFrameOhosRaster::GetBufferAge() const
{
    // currently could not get real buffer age, so return invalid age
    // should be supported by surface
    ROSEN_LOGD("RSSurfaceFrameOhosRaster::GetBufferAge return -1");
    return -1;
}

Drawing::Canvas* RSSurfaceFrameOhosRaster::GetCanvas()
{
    if (buffer_ == nullptr || buffer_->GetWidth() <= 0 || buffer_->GetHeight() <= 0) {
        ROSEN_LOGW("buffer is invalid");
        return nullptr;
    }
    if (surface_ == nullptr) {
        CreateSurface();
    }
    return surface_->GetCanvas().get();
}

std::shared_ptr<Drawing::Surface> RSSurfaceFrameOhosRaster::GetSurface()
{
    if (buffer_ == nullptr || buffer_->GetWidth() <= 0 || buffer_->GetHeight() <= 0) {
        ROSEN_LOGW("buffer is invalid");
        return nullptr;
    }
    if (surface_ == nullptr) {
        CreateSurface();
    }
    return surface_;
}

void RSSurfaceFrameOhosRaster::CreateSurface()
{
    auto addr = static_cast<uint32_t*>(buffer_->GetVirAddr());
    if (addr == nullptr) {
        ROSEN_LOGW("buffer addr is invalid");
        return;
    }
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    bitmap.Build(buffer_->GetWidth(), buffer_->GetHeight(), format);
    bitmap.SetPixels(addr);
    surface_ = std::make_shared<Drawing::Surface>();
    surface_->Bind(bitmap);
}

int32_t RSSurfaceFrameOhosRaster::GetReleaseFence() const
{
    return releaseFence_;
}

void RSSurfaceFrameOhosRaster::SetReleaseFence(const int32_t& fence)
{
    releaseFence_ = fence;
}

} // namespace Rosen
} // namespace OHOS