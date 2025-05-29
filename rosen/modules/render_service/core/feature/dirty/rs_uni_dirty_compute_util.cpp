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

#include "rs_uni_dirty_compute_util.h"

#include <cstdint>
#include <memory>
#include <parameter.h>
#include <parameters.h>
#include <string>

#include "common/rs_optional_trace.h"
#include "common/rs_rectangles_merger.h"
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
std::vector<RectI> RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty(
    DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, ScreenInfo& screenInfo, RSDisplayRenderParams& params)
{
    Occlusion::Region damageRegions;
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    // update all child surfacenode history
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            RS_LOGI("GetCurrentFrameVisibleDirty surfaceNodeDrawable is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (!surfaceParams || !surfaceDirtyManager) {
            RS_LOGI("RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty node(%{public}" PRIu64") params or "
                "dirty manager is nullptr", surfaceNodeDrawable->GetId());
            continue;
        }
        if (!surfaceParams->IsAppWindow() || surfaceParams->GetDstRect().IsEmpty()) {
            continue;
        }
        // for cross-display surface, only consider the dirty region on the first display (use global dirty for others).
        if (surfaceParams->IsFirstLevelCrossNode() &&
            !RSUniRenderThread::Instance().GetRSRenderThreadParams()->IsFirstVisitCrossNodeDisplay()) {
            continue;
        }
        auto visibleRegion = surfaceParams->GetVisibleRegion();
        auto surfaceCurrentFrameDirtyRegion = surfaceDirtyManager->GetCurrentFrameDirtyRegion();
        Occlusion::Region currentFrameDirtyRegion { Occlusion::Rect {
            surfaceCurrentFrameDirtyRegion.left_, surfaceCurrentFrameDirtyRegion.top_,
            surfaceCurrentFrameDirtyRegion.GetRight(), surfaceCurrentFrameDirtyRegion.GetBottom() } };
        Occlusion::Region damageRegion = currentFrameDirtyRegion.And(visibleRegion);
        damageRegions.OrSelf(damageRegion);
    }
    auto rects = RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(damageRegions, screenInfo);
    RectI rect = displayDrawable.GetSyncDirtyManager()->GetDirtyRegionFlipWithinSurface();
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }
    return rects;
}

std::vector<RectI> RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(
    const Occlusion::Region &region, ScreenInfo& screenInfo)
{
    const std::vector<Occlusion::Rect>& rects = region.GetRegionRects();
    std::vector<RectI> retRects;
    for (const Occlusion::Rect& rect : rects) {
        // origin transformation
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            retRects.emplace_back(RectI(rect.left_, rect.top_,
                rect.right_ - rect.left_, rect.bottom_ - rect.top_));
        } else {
            retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.bottom_,
                rect.right_ - rect.left_, rect.bottom_ - rect.top_));
        }
#else
        retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.bottom_,
            rect.right_ - rect.left_, rect.bottom_ - rect.top_));
#endif
    }
    RS_LOGD("ScreenIntersectDirtyRects size %{public}d %{public}s", region.GetSize(), region.GetRegionInfo().c_str());
    return retRects;
}

std::vector<RectI> RSUniDirtyComputeUtil::GetFilpDirtyRects(
    const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return srcRects;
    }
#endif

    return FilpRects(srcRects, screenInfo);
}

std::vector<RectI> RSUniDirtyComputeUtil::FilpRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo)
{
    std::vector<RectI> retRects;
    for (const RectI& rect : srcRects) {
        retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.top_ - rect.height_,
            rect.width_, rect.height_));
    }
    return retRects;
}

GraphicIRect RSUniDirtyComputeUtil::IntersectRect(const GraphicIRect& first, const GraphicIRect& second)
{
    int left = std::max(first.x, second.x);
    int top = std::max(first.y, second.y);
    int right = std::min(first.x + first.w, second.x + second.w);
    int bottom = std::min(first.y + first.h, second.y + second.h);
    int width = right - left;
    int height = bottom - top;

    if (width <= 0 || height <= 0) {
        return GraphicIRect { 0, 0, 0, 0 };
    } else {
        return GraphicIRect { left, top, width, height };
    }
}
} // namespace Rosen
} // namespace OHOS