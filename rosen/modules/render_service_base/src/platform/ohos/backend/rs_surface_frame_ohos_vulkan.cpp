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
#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {

RSSurfaceFrameOhosVulkan::RSSurfaceFrameOhosVulkan(std::shared_ptr<Drawing::Surface> surface, int32_t width,
    int32_t height, int32_t bufferAge)
    : surface_(surface), width_(width), height_(height), bufferAge_(bufferAge)
{
}

void RSSurfaceFrameOhosVulkan::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    RS_TRACE_FUNC();
    std::vector<Drawing::RectI> rects;
    Drawing::RectI rect = {left, top, left + width, top + height};
    rects.push_back(rect);
    surface_->SetDrawingArea(rects);
#endif
}

void RSSurfaceFrameOhosVulkan::SetDamageRegion(const std::vector<RectI>& rects)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    RS_TRACE_FUNC();
    std::vector<Drawing::RectI> iRects;
    for (auto &rect : rects) {
        Drawing::RectI iRect = {rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()};
        iRects.push_back(iRect);
    }
    surface_->SetDrawingArea(iRects);
#endif
}

Drawing::Canvas* RSSurfaceFrameOhosVulkan::GetCanvas()
{
    return surface_ != nullptr ? surface_->GetCanvas().get() : nullptr;
}

std::shared_ptr<Drawing::Surface> RSSurfaceFrameOhosVulkan::GetSurface()
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
