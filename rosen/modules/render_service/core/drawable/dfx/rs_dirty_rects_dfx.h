/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#pragma once

#include <string>

#include "system/rs_system_parameters.h"

#include "common/rs_occlusion_region.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
namespace OHOS::Rosen {

class RSDirtyRectsDfx {
public:
    RSDirtyRectsDfx(std::shared_ptr<RSDisplayRenderNode> targetNode) : targetNode_(targetNode) {}
    ~RSDirtyRectsDfx() = default;

    enum class RSPaintStyle { FILL, STROKE };
    void OnDraw(std::shared_ptr<RSPaintFilterCanvas> canvas);
    void SetDirtyRegion(Occlusion::Region& dirtyRegion)
    {
        dirtyRegion_ = dirtyRegion;
    }

private:
    Occlusion::Region dirtyRegion_;
    std::shared_ptr<RSDisplayRenderNode> targetNode_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;

    void DrawDirtyRectForDFX(const RectI& dirtyRect, const Drawing::Color color, const RSPaintStyle fillType,
        float alpha, int edgeWidth = 6) const;
    bool DrawDetailedTypesOfDirtyRegionForDFX(RSSurfaceRenderNode& node) const;
    void DrawSurfaceOpaqueRegionForDFX(RSSurfaceRenderNode& node) const;

    void DrawDirtyRegionForDFX(std::vector<RectI> dirtyRects) const;
    void DrawCacheRegionForDFX(std::vector<RectI> cacheRects) const;
    void DrawAllSurfaceDirtyRegionForDFX() const;
    void DrawAllSurfaceOpaqueRegionForDFX() const;
    void DrawTargetSurfaceDirtyRegionForDFX() const;
    void DrawTargetSurfaceVisibleRegionForDFX() const;
    void DrawAndTraceSingleDirtyRegionTypeForDFX(
        RSSurfaceRenderNode& node, DirtyRegionType dirtyType, bool isDrawn = true) const;

    // dfx check if surface name is in dfx target list
    inline bool CheckIfSurfaceTargetedForDFX(std::string nodeName) const
    {
        auto surfaceName = RSUniRenderThread::Instance().GetRSRenderThreadParams()->dfxTargetSurfaceNames_;
        return (std::find(surfaceName.begin(), surfaceName.end(), nodeName) != surfaceName.end());
    }
};
} // namespace OHOS::Rosen
