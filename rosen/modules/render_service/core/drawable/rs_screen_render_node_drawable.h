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

#ifndef RENDER_SERVICE_DRAWABLE_RS_SCREEN_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_SCREEN_RENDER_NODE_DRAWABLE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_occlusion_region.h"
#include "drawable/rs_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSScreenRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSScreenRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override {}

    std::shared_ptr<Drawing::Image> GetCacheImgForCapture() const
    {
        return cacheImgForMultiScreenView_;
    }

    void SetCacheImgForCapture(std::shared_ptr<Drawing::Image> cacheImgForCapture)
    {
        if (cacheImgForMultiScreenView_ == cacheImgForCapture) {
            return;
        }
        cacheImgForMultiScreenView_ = cacheImgForCapture;
    }

    const std::shared_ptr<RSSurfaceHandler> GetRSSurfaceHandlerOnDraw() const
    {
        return surfaceHandler_;
    }

    std::shared_ptr<RSSurfaceHandler> GetMutableRSSurfaceHandlerOnDraw()
    {
        return surfaceHandler_;
    }

    const std::vector<RectI>& GetDirtyRects() const
    {
        return dirtyRects_;
    }

    void SetDirtyRects(const std::vector<RectI>& rects)
    {
        dirtyRects_ = rects;
    }

    std::shared_ptr<RSDirtyRegionManager> GetSyncDirtyManager() const override
    {
        return syncDirtyManager_;
    }

    int32_t GetBufferAge();
    void SetDamageRegion(const std::vector<RectI>& rects);
#ifndef ROSEN_CROSS_PLATFORM
    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    sptr<IBufferConsumerListener> GetConsumerListener() const
    {
        return consumerListener_;
    }
#endif
    bool IsSurfaceCreated() const
    {
        return surfaceCreated_;
    }
    std::shared_ptr<RSSurface> GetRSSurface() const
    {
        return surface_;
    }
    void SetVirtualSurface(std::shared_ptr<RSSurface>& virtualSurface, uint64_t pSurfaceUniqueId)
    {
        virtualSurface_ = virtualSurface;
        virtualSurfaceUniqueId_ = pSurfaceUniqueId;
    }
    std::shared_ptr<RSSurface> GetVirtualSurface(uint64_t pSurfaceUniqueId)
    {
        return virtualSurfaceUniqueId_ != pSurfaceUniqueId ? nullptr : virtualSurface_;
    }
    bool SkipFrame(uint32_t refreshRate, ScreenInfo screenInfo);
    bool IsRenderSkipIfScreenOff() const
    {
        return isRenderSkipIfScreenOff_;
    }

    RSRenderNodeDrawableType GetDrawableType() const override
    {
        return RSRenderNodeDrawableType::DISPLAY_NODE_DRAWABLE;
    }

    ScreenRotation GetFirstBufferRotation() const
    {
        return firstBufferRotation_;
    }

    void SetFirstBufferRotation(const ScreenRotation bufferRotation)
    {
        firstBufferRotation_ = bufferRotation;
    }
    void CheckAndUpdateFilterCacheOcclusionFast();

private:
    explicit RSScreenRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    bool CheckScreenNodeSkip(RSScreenRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void ClearCanvasStencil(RSPaintFilterCanvas& canvas, RSScreenRenderParams& params,
         RSRenderThreadParams& uniParam);
    std::unique_ptr<RSRenderFrame> RequestFrame(RSScreenRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void DrawCurtainScreen() const;
    void RemoveClearMemoryTask() const;
    void PostClearMemoryTask() const;
    void SetDisplayNodeSkipFlag(RSRenderThreadParams& uniParam, bool flag);
    void UpdateDisplayDirtyManager(int32_t bufferage, bool useAlignedDirtyRegion = false);
    void SetScreenNodeSkipFlag(RSRenderThreadParams& uniParam, bool flag);
    static void CheckFilterCacheFullyCovered(RSSurfaceRenderParams& surfaceParams, RectI screenRect);
    static void CheckAndUpdateFilterCacheOcclusion(RSScreenRenderParams& params, const ScreenInfo& screenInfo);
    bool HardCursorCreateLayer(std::shared_ptr<RSProcessor> processor);

    void RenderOverDraw();
    bool SkipFrameByInterval(uint32_t refreshRate, uint32_t skipFrameInterval);
    bool SkipFrameByRefreshRate(uint32_t refreshRate, uint32_t expectedRefreshRate);

    void MirrorRedrawDFX(bool mirrorRedraw, ScreenId screenId);

    void CheckHpaeBlurRun(bool isHdrOn);
    
    // hpae offline
    void CheckAndPostAsyncProcessOfflineTask();
    bool ProcessOfflineSurfaceDrawable(const std::shared_ptr<RSProcessor>& processor,
        std::shared_ptr<RSSurfaceRenderNodeDrawable>& surfaceDrawable, bool async);

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::SCREEN_NODE, OnGenerate>;
    static Registrar instance_;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_ = nullptr;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_ = nullptr;
    std::unique_ptr<RSRenderFrame> expandRenderFrame_ = nullptr;
    std::shared_ptr<Drawing::Surface> offscreenSurface_ = nullptr; // temporarily holds offscreen surface
    std::shared_ptr<RSPaintFilterCanvas> canvasBackup_ = nullptr; // backup current canvas before offscreen render
    std::unordered_set<NodeId> currentBlackList_ = {};
    std::unordered_set<NodeType> currentTypeBlackList_ = {};
    std::unordered_set<NodeId> lastBlackList_ = {};
    std::unordered_set<NodeType> lastTypeBlackList_ = {};
    bool curSecExemption_ = false;
    bool lastSecExemption_ = false;
    std::shared_ptr<Drawing::Image> cacheImgForMultiScreenView_ = nullptr;
    int32_t specialLayerType_ = 0;
    bool castScreenEnableSkipWindow_ = false;
    bool isScreenNodeSkip_ = false;
    bool isScreenNodeSkipStatusChanged_ = false;
    Drawing::Matrix lastMatrix_;
    Drawing::Matrix lastMirrorMatrix_;
    bool useFixedOffscreenSurfaceSize_ = false;
    std::shared_ptr<RSScreenRenderNodeDrawable> mirrorSourceDrawable_ = nullptr;
    uint64_t virtualSurfaceUniqueId_ = 0;
    bool resetRotate_ = false;
    // dirty manager
    std::shared_ptr<RSDirtyRegionManager> syncDirtyManager_ = nullptr;
    std::vector<RectI> dirtyRects_;

    // surface create in render thread
    static constexpr uint32_t BUFFER_SIZE = 4;
    bool surfaceCreated_ = false;
    std::shared_ptr<RSSurface> surface_ = nullptr;
    std::shared_ptr<RSSurface> virtualSurface_ = nullptr;
    ScreenRotation firstBufferRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;

    bool isMirrorSLRCopy_ = false;
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IBufferConsumerListener> consumerListener_ = nullptr;
#endif
    int64_t lastRefreshTime_ = 0;
    bool virtualDirtyRefresh_ = false;
    bool enableVisibleRect_ = false;
    Drawing::RectI curVisibleRect_;
    Drawing::RectI lastVisibleRect_;

    bool isRenderSkipIfScreenOff_ = false;

    bool filterCacheOcclusionUpdated_ = false;

    bool hardCursorLastCommitSuccess_ = false;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_SCREEN_RENDER_NODE_DRAWABLE_H