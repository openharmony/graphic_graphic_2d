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

#ifndef RENDER_SERVICE_DRAWABLE_RS_SURFACE_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_SURFACE_RENDER_NODE_DRAWABLE_H

#ifndef ROSEN_CROSS_PLATFORM
#include <ibuffer_consumer_listener.h>
#include <iconsumer_surface.h>
#include <surface.h>
#endif
#include <mutex>
#include "platform/drawing/rs_surface.h"

#include "common/rs_common_def.h"
#include "drawable/rs_render_node_drawable.h"
#include "params/rs_surface_render_params.h"
#include "params/rs_display_render_params.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_surface_render_node.h"
#include "feature/uifirst/rs_draw_window_cache.h"
#include "feature/uifirst/rs_sub_thread_cache.h"

namespace OHOS::Rosen {
class RSRenderThreadParams;
class RSSurfaceRenderNode;
class RSSurfaceRenderParams;
namespace DrawableV2 {
class RSDisplayRenderNodeDrawable;
class RSRcdSurfaceRenderNodeDrawable;

// remove this when rcd node is replaced by common hardware composer node in OH 6.0 rcd refactoring
class RSSurfaceRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSSurfaceRenderNodeDrawable() = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;
    bool CheckIfSurfaceSkipInMirror(const RSSurfaceRenderParams& surfaceParams);
    void SetVirtualScreenWhiteListRootId(const std::unordered_set<NodeId>& whiteList, NodeId id);
    void ResetVirtualScreenWhiteListRootId(NodeId id);

    const std::string& GetName() const
    {
        return name_;
    }

    std::string GetDebugInfo()
    {
        std::string res = "pid_";
        res.append("_name_");
        res.append(name_);
        return res;
    }

    // uifirst
    RsSubThreadCache& GetRsSubThreadCache()
    {
        return subThreadCache_;
    }

    Drawing::Matrix GetGravityMatrix(float imgWidth, float imgHeight);

    const Occlusion::Region& GetVisibleDirtyRegion() const;
    void SetVisibleDirtyRegion(const Occlusion::Region& region);
    void SetAlignedVisibleDirtyRegion(const Occlusion::Region& region);
    void SetGlobalDirtyRegion(Occlusion::Region region);
    const Occlusion::Region& GetGlobalDirtyRegion() const;
    void SetDirtyRegionAlignedEnable(bool enable);
    void SetDirtyRegionBelowCurrentLayer(Occlusion::Region& region);
    std::shared_ptr<RSDirtyRegionManager> GetSyncDirtyManager() const override;

    GraphicColorGamut GetAncestorDisplayColorGamut(const RSSurfaceRenderParams& surfaceParams);
    void DealWithSelfDrawingNodeBuffer(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);

    void SetUIExtensionNeedToDraw(bool needToDraw) override
    {
        uiExtensionNeedToDraw_ = needToDraw;
    }

    bool UIExtensionNeedToDraw() const override
    {
        return uiExtensionNeedToDraw_;
    }

    bool PrepareOffscreenRender();
    void FinishOffscreenRender(const Drawing::SamplingOptions& sampling);
    bool IsHardwareEnabled();

    bool IsScbWindowType() const
    {
        return surfaceWindowType_ == SurfaceWindowType::SYSTEM_SCB_WINDOW ||
               surfaceWindowType_ == SurfaceWindowType::SCB_DESKTOP ||
               surfaceWindowType_ == SurfaceWindowType::SCB_WALLPAPER ||
               surfaceWindowType_ == SurfaceWindowType::SCB_SCREEN_LOCK ||
               surfaceWindowType_ == SurfaceWindowType::SCB_NEGATIVE_SCREEN ||
               surfaceWindowType_ == SurfaceWindowType::SCB_DROPDOWN_PANEL;
    }

#ifndef ROSEN_CROSS_PLATFORM
    sptr<IConsumerSurface> GetConsumerOnDraw() const
    {
        return consumerOnDraw_;
    }
    void RegisterDeleteBufferListenerOnSync(sptr<IConsumerSurface> consumer) override;
#endif

    bool IsHardwareEnabledTopSurface() const;
    void UpdateSurfaceDirtyRegion(std::shared_ptr<RSPaintFilterCanvas>& canvas);

private:
    explicit RSSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    void OnGeneralProcess(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams,
        RSRenderThreadParams& uniParams, bool isSelfDrawingSurface);
    bool IsVisibleRegionEqualOnPhysicalAndVirtual(RSSurfaceRenderParams& surfaceParams);
    void CaptureSurface(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);

    Drawing::Region CalculateVisibleDirtyRegion(RSSurfaceRenderParams& surfaceParams,
        RSSurfaceRenderNodeDrawable& surfaceDrawable, bool isOffscreen) const;
    Drawing::Region GetSurfaceDrawRegion() const;
    void SetSurfaceDrawRegion(const Drawing::Region& region);
    void CrossDisplaySurfaceDirtyRegionConversion(
        const RSRenderThreadParams& uniParam, const RSSurfaceRenderParams& surfaceParam, RectI& surfaceDirtyRect) const;
    bool HasCornerRadius(const RSSurfaceRenderParams& surfaceParams) const;
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::SURFACE_NODE, OnGenerate>;
    static Registrar instance_;

    bool CheckDrawAndCacheWindowContent(RSSurfaceRenderParams& surfaceParams,
        RSRenderThreadParams& uniParams) const;
    void PreprocessUnobscuredUEC(RSPaintFilterCanvas& canvas);

    void EnableGpuOverDrawDrawBufferOptimization(Drawing::Canvas& canvas, RSSurfaceRenderParams* surfaceParams);

    // dirty manager
    void UpdateDisplayDirtyManager(int32_t bufferage, bool useAlignedDirtyRegion = false);

    void DrawSelfDrawingNodeBuffer(RSPaintFilterCanvas& canvas,
        const RSSurfaceRenderParams& surfaceParams, BufferDrawParam& params);

    // Draw cloneNode
    bool DrawCloneNode(RSPaintFilterCanvas& canvas, RSRenderThreadParams& uniParam,
        RSSurfaceRenderParams& surfaceParams, bool isCapture = false);

    // Watermark
    void DrawWatermark(RSPaintFilterCanvas& canvas, const RSSurfaceRenderParams& surfaceParams);

    bool RecordTimestamp(NodeId id, uint32_t seqNum);

    bool DrawCacheImageForMultiScreenView(RSPaintFilterCanvas& canvas, const RSSurfaceRenderParams& surfaceParams);

    void ClipHoleForSelfDrawingNode(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);
    void DrawBufferForRotationFixed(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);

    std::string name_;
    RSSurfaceNodeType surfaceNodeType_ = RSSurfaceNodeType::DEFAULT;
    SurfaceWindowType surfaceWindowType_ = SurfaceWindowType::DEFAULT_WINDOW;
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IBufferConsumerListener> consumerListener_ = nullptr;
#endif
    std::shared_ptr<RSSurface> surface_ = nullptr;
    bool surfaceCreated_ = false;
    bool uiExtensionNeedToDraw_ = false;

    RSPaintFilterCanvas* curCanvas_ = nullptr;
    std::shared_ptr<Drawing::Surface> offscreenSurface_ = nullptr; // temporary holds offscreen surface
    int releaseCount_ = 0;
    static constexpr int MAX_RELEASE_FRAME = 10;
    RSPaintFilterCanvas* canvasBackup_ = nullptr; // backup current canvas before offscreen rende
    std::shared_ptr<RSPaintFilterCanvas> offscreenCanvas_ = nullptr;
    int maxRenderSize_ = 0;
    std::unique_ptr<RSAutoCanvasRestore> arc_ = nullptr;

#ifndef ROSEN_CROSS_PLATFORM
    sptr<IConsumerSurface> consumerOnDraw_ = nullptr;
#endif

    // dirty manager
    std::shared_ptr<RSDirtyRegionManager> syncDirtyManager_ = nullptr;
    Occlusion::Region visibleDirtyRegion_;
    Occlusion::Region alignedVisibleDirtyRegion_;
    bool isDirtyRegionAlignedEnable_ = false;
    Occlusion::Region globalDirtyRegion_;

    // if a there a dirty layer under transparent clean layer, transparent layer should refreshed
    Occlusion::Region dirtyRegionBelowCurrentLayer_;
    bool dirtyRegionBelowCurrentLayerIsEmpty_ = false;

    friend class OHOS::Rosen::RSDrawWindowCache;
    bool vmaCacheOff_ = false;

    static inline bool isInRotationFixed_ = false;
    bool lastGlobalPositionEnabled_ = false;
    RsSubThreadCache subThreadCache_;
    friend class RsSubThreadCache;

    Drawing::Region curSurfaceDrawRegion_ {};
    mutable std::mutex drawRegionMutex_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_SURFACE_RENDER_NODE_DRAWABLE_H
