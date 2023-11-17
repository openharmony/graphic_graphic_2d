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

#include "rs_surface_frame_ohos_vulkan.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSSurfaceFrameOhosVulkan::RSSurfaceFrameOhosVulkan(sk_sp<SkSurface> surface, int32_t width,
    int32_t height, int32_t bufferAge)
    : surface_(surface), width_(width), height_(height), bufferAge_(bufferAge)
{
}

RSSurfaceFrameOhosVulkan::RSSurfaceFrameOhosVulkan(sk_sp<SkSurface> surface, int32_t width, int32_t height)
    : surface_(surface), width_(width), height_(height)
{
}

void RSSurfaceFrameOhosVulkan::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
#ifdef RS_ENABLE_VK
    std::vector<SkIRect> skIRects;
    SkIRect skIRect = {left, top, width, height};
    skIRects.push_back(skIRect);
    surface_->setDrawingArea(skIRects);
#endif
}

void RSSurfaceFrameOhosVulkan::SetDamageRegion(const std::vector<RectI>& rects)
{
#ifdef RS_ENABLE_VK
    std::vector<SkIRect> skIRects;
    for (auto &rect : rects) {
        SkIRect skIRect = {rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()};
        skIRects.push_back(skIRect);
    }

    surface_->setDrawingArea(skIRects);
#endif
}

SkCanvas* RSSurfaceFrameOhosVulkan::GetCanvas()
{
    return surface_ != nullptr ? surface_->getCanvas() : nullptr;
}

sk_sp<SkSurface> RSSurfaceFrameOhosVulkan::GetSurface()
{
    return surface_;
}

int32_t RSSurfaceFrameOhosVulkan::GetReleaseFence() const
{
    return releaseFence_;
}

void RSSurfaceFrameOhosVulkan::SetReleaseFence(const int32_t& fence)
{
    releaseFence_ = fence;
}

int32_t RSSurfaceFrameOhosVulkan::GetBufferAge() const
{
    return bufferAge_;
}
} // namespace Rosen
} // namespace OHOS
