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

#ifndef RENDER_SERVICE_DRAWABLE_DFX_RS_DIRTY_RECTS_DFX_H
#define RENDER_SERVICE_DRAWABLE_DFX_RS_DIRTY_RECTS_DFX_H

#include <string>

#include "system/rs_system_parameters.h"

#include "common/rs_occlusion_region.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_screen_render_params.h"
#include "params/rs_render_params.h"
#include "params/rs_render_thread_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_surface_render_node.h"
namespace OHOS::Rosen {

class RSDirtyRectsDfx {
public:
    explicit RSDirtyRectsDfx(DrawableV2::RSScreenRenderNodeDrawable& targetDrawable)
        : targetDrawable_(targetDrawable), screenParams_(targetDrawable.GetRenderParams())
    {}
    ~RSDirtyRectsDfx() = default;

    enum class RSPaintStyle { FILL, STROKE };
    void OnDraw(RSPaintFilterCanvas& canvas);
    void OnDrawVirtual(RSPaintFilterCanvas& canvas);
    void SetDirtyRegion(Occlusion::Region& dirtyRegion)
    {
        dirtyRegion_ = dirtyRegion;
    }

    void SetMergedDirtyRegion(const Occlusion::Region& mergedDirtyRegion)
    {
        mergedDirtyRegion_ = mergedDirtyRegion;
    }

    void SetVirtualDirtyRects(const std::vector<RectI>& virtualDirtyRects, const ScreenInfo& screenInfo)
    {
        virtualDirtyRects_ = virtualDirtyRects;
        screenInfo_ = screenInfo;
    }

private:
    Occlusion::Region dirtyRegion_;
    Occlusion::Region mergedDirtyRegion_;
    std::vector<RectI> virtualDirtyRects_;
    ScreenInfo screenInfo_;
    const DrawableV2::RSScreenRenderNodeDrawable& targetDrawable_;
    const std::unique_ptr<RSRenderParams>& screenParams_;

    void DrawDirtyRectForDFX(RSPaintFilterCanvas& canvas, RectI dirtyRect, const Drawing::Color color,
        const RSPaintStyle fillType, int edgeWidth = 6, bool isTextOutsideRect = false) const;
    bool DrawDetailedTypesOfDirtyRegionForDFX(RSPaintFilterCanvas& canvas,
        DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable) const;
#ifdef RS_ENABLE_GPU
    void DrawSurfaceOpaqueRegionForDFX(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams) const;
#endif
    void DrawHwcRegionForDFX(RSPaintFilterCanvas& canvas) const;

    void DrawDirtyRegionForDFX(RSPaintFilterCanvas& canvas, const std::vector<RectI>& dirtyRects) const;
    void DrawAllSurfaceDirtyRegionForDFX(RSPaintFilterCanvas& canvas) const;
    void DrawMergedAndAllDirtyRegionForDFX(RSPaintFilterCanvas& canvas) const;
    void DrawAllSurfaceOpaqueRegionForDFX(RSPaintFilterCanvas& canvas) const;
    void DrawTargetSurfaceDirtyRegionForDFX(RSPaintFilterCanvas& canvas) const;
    void DrawTargetSurfaceVisibleRegionForDFX(RSPaintFilterCanvas& canvas) const;
    void DrawAndTraceSingleDirtyRegionTypeForDFX(RSPaintFilterCanvas& canvas,
        DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, DirtyRegionType dirtyType, bool isDrawn = true) const;
    void DrawDirtyRegionInVirtual(RSPaintFilterCanvas& canvas) const;

    // dfx check if surface name is in dfx target list
    inline bool CheckIfSurfaceTargetedForDFX(std::string nodeName) const
    {
        auto surfaceName = RSUniRenderThread::Instance().GetRSRenderThreadParams()->dfxTargetSurfaceNames_;
        return (std::find(surfaceName.begin(), surfaceName.end(), nodeName) != surfaceName.end());
    }
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_DFX_RS_DIRTY_RECTS_DFX_H
