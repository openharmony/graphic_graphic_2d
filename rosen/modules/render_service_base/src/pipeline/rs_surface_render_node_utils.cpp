/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "pipeline/rs_surface_render_node_utils.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool RSSurfaceRenderNodeUtils::IntersectHwcDamageWith(const RSSurfaceRenderNode& hwcNode, const RectI& rect)
{
    if (!hwcNode.IsHardwareEnabledType()) {
        ROSEN_LOGD("RSSurfaceRenderNode::IntersectHwcDamageWith: not hardware enabled type.");
        return false;
    }

    auto surfaceHandler = hwcNode.GetRSSurfaceHandler();
    if (!surfaceHandler || !surfaceHandler->IsCurrentFrameBufferConsumed()) {
        ROSEN_LOGD("RSSurfaceRenderNode::IntersectHwcDamageWith: no buffer or the buffer has not been consumed.");
        return false;
    }

    auto geoPtr = hwcNode.GetRenderProperties().GetBoundsGeometry();
#ifndef ROSEN_CROSS_PLATFORM
    auto region = surfaceHandler->GetDamageRegion();
    RectF realRect = RectF(region.x, region.y, region.w, region.h);
#else
    auto region = geoPtr->GetAbsRect();
    RectF realRect = RectF(region.left_, region.top_, region.width_, region.height_);
#endif
    auto dirtyRect = geoPtr->MapRect(realRect, hwcNode.GetBufferRelMatrix());
    RS_OPTIONAL_TRACE_FMT("RSSurfaceRenderNode::IntersectHwcDamageWith: dirtyRect: [%d,%d,%d,%d] rect: [%d,%d,%d,%d]",
        dirtyRect.GetLeft(), dirtyRect.GetTop(), dirtyRect.GetWidth(), dirtyRect.GetHeight(),
        rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
    return dirtyRect.Intersect(rect);
}

} // namespace Rosen
} // namespace OHOS
