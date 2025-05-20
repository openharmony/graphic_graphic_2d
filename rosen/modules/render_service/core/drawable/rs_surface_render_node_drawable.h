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
#ifdef NEW_RENDER_CONTEXT
#include "rs_render_surface.h"
#else
#include "platform/drawing/rs_surface.h"
#endif

#include "common/rs_common_def.h"
#include "drawable/rs_render_node_drawable.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_base_render_engine.h"
#include "params/rs_display_render_params.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_draw_window_cache.h"

namespace OHOS::Rosen {
class RSRenderThreadParams;
class RSSurfaceRenderNode;
class RSSurfaceRenderParams;
namespace DrawableV2 {
class RSDisplayRenderNodeDrawable;
struct UIFirstParams {
    uint32_t submittedSubThreadIndex_ = INT_MAX;
    std::atomic<CacheProcessStatus> cacheProcessStatus_ = CacheProcessStatus::UNKNOWN;
    std::atomic<bool> isNeedSubmitSubThread_ = true;
};
class RSSurfaceRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSSurfaceRenderNodeDrawable() override;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;
    bool CheckIfSurfaceSkipInMirror(const RSSurfaceRenderParams& surfaceParams);
    void SetVirtualScreenWhiteListRootId(const std::unordered_set<NodeId>& whiteList, NodeId id);
    void ResetVirtualScreenWhiteListRootId(NodeId id);

    void SubDraw(Drawing::Canvas& canvas);

    void UpdateCacheSurface();
    void SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus);
    CacheProcessStatus GetCacheSurfaceProcessedStatus() const;

    const std::string& GetName() const
    {
        return name_;
    }

    // UIFirst
    void SetSubmittedSubThreadIndex(uint32_t index)
    {
        uiFirstParams.submittedSubThreadIndex_ = index;
    }

    uint32_t GetSubmittedSubThreadIndex() const
    {
        return uiFirstParams.submittedSubThreadIndex_;
    }

    std::string GetDebugInfo()
    {
        std::string res = "pid_";
        res.append("_name_");
        res.append(name_);
        return res;
    }

    void UpdateCompletedCacheSurface();
    void ClearCacheSurfaceInThread();
    void ClearCacheSurface(bool isClearCompletedCacheSurface = true);

    std::shared_ptr<Drawing::Surface> GetCacheSurface(uint32_t threadIndex, bool needCheckThread,
        bool releaseAfterGet = false);
    bool NeedInitCacheSurface();
    std::shared_ptr<Drawing::Image> GetCompletedImage(RSPaintFilterCanvas& canvas, uint32_t threadIndex,
        bool isUIFirst);
    using ClearCacheSurfaceFunc =
        std::function<void(std::shared_ptr<Drawing::Surface>&&,
        std::shared_ptr<Drawing::Surface>&&, uint32_t, uint32_t)>;
    void InitCacheSurface(Drawing::GPUContext* grContext, ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX, bool isHdrOn = false);

    void ResetUifirst(bool isNotClearCompleteCacheSurface)
    {
        if (isNotClearCompleteCacheSurface) {
            ClearCacheSurfaceOnly();
        } else {
            ClearCacheSurfaceInThread();
        }
        drawWindowCache_.ClearCache();
    }

    bool IsCurFrameStatic(DeviceType deviceType);

    Vector2f GetGravityTranslate(float imgWidth, float imgHeight);

    bool HasCachedTexture() const;

    void SetTextureValidFlag(bool isValid);
    void SetCacheSurfaceNeedUpdated(bool isCacheSurfaceNeedUpdate)
    {
        isCacheSurfaceNeedUpdate_ = isCacheSurfaceNeedUpdate;
    }

    bool GetCacheSurfaceNeedUpdated() const
    {
        return isCacheSurfaceNeedUpdate_;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    void UpdateBackendTexture();
#endif

    bool DrawCacheSurface(RSPaintFilterCanvas& canvas, const Vector2f& boundSize,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX, bool isUIFirst = false);
    void DrawableCache(std::shared_ptr<Drawing::GPUContext> grContext_);

    void SetLastFrameUsedThreadIndex(pid_t tid)
    {
        lastFrameUsedThreadIndex_ = tid;
    }

    pid_t GetLastFrameUsedThreadIndex()
    {
        return lastFrameUsedThreadIndex_;
    }

    void SetRenderCachePriority(NodePriorityType type)
    {
        priority_ = type;
    }

    NodePriorityType GetRenderCachePriority() const
    {
        return priority_;
    }

    void SetHDRPresent(bool hasHdrPresent)
    {
        hasHdrPresent_ = hasHdrPresent;
    }

    bool GetHDRPresent() const
    {
        return hasHdrPresent_;
    }

    ScreenId GetScreenId() const
    {
        return screenId_;
    }

    void SetScreenId(ScreenId screenId)
    {
        screenId_ = screenId;
    }

    GraphicColorGamut GetTargetColorGamut() const
    {
        return targetColorGamut_;
    }

    void SetTargetColorGamut(GraphicColorGamut colorGamut)
    {
        targetColorGamut_ = colorGamut;
    }

    void SetSubThreadSkip(bool isSubThreadSkip);
    bool IsSubThreadSkip() const
    {
        return isSubThreadSkip_;
    }
    void SetTaskFrameCount(uint64_t frameCount);

    uint64_t GetTaskFrameCount() const;

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
    void ClearCacheSurfaceOnly();

    bool PrepareOffscreenRender();
    void FinishOffscreenRender(const Drawing::SamplingOptions& sampling);
    bool IsHardwareEnabled();
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IConsumerSurface> GetConsumerOnDraw() const
    {
        return consumerOnDraw_;
    }
    void RegisterDeleteBufferListenerOnSync(sptr<IConsumerSurface> consumer) override;
#endif
    bool IsHardwareEnabledTopSurface() const
    {
        return surfaceNodeType_ == RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE && GetName() == "pointer window";
    }

    inline bool CheckCacheSurface()
    {
        return cacheSurface_ ? true : false;
    }
private:
    explicit RSSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    bool DealWithUIFirstCache(
        RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams, RSRenderThreadParams& uniParams);
    void OnGeneralProcess(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams, bool isSelfDrawingSurface);
    void CaptureSurface(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);

    void MergeDirtyRegionBelowCurSurface(RSRenderThreadParams& uniParam, Drawing::Region& region);
    void MergeSubSurfaceNodesDirtyRegionForMainWindow(
        RSSurfaceRenderParams& surfaceParams, Occlusion::Region& surfaceDirtyRegion) const;
    Drawing::Region CalculateVisibleDirtyRegion(RSRenderThreadParams& uniParam, RSSurfaceRenderParams& surfaceParams,
        RSSurfaceRenderNodeDrawable& surfaceDrawable, bool isOffscreen) const;
    bool HasCornerRadius(const RSSurfaceRenderParams& surfaceParams) const;
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::SURFACE_NODE, OnGenerate>;
    static Registrar instance_;

    bool DrawUIFirstCache(RSPaintFilterCanvas& rscanvas, bool canSkipWait);
    bool DrawUIFirstCacheWithStarting(RSPaintFilterCanvas& rscanvas, NodeId id);
    // To be deleted after captureWindow being deleted
    bool CheckIfNeedResetRotate(RSPaintFilterCanvas& canvas);
    NodeId FindInstanceChildOfDisplay(std::shared_ptr<RSRenderNode> node);

    void DrawUIFirstDfx(RSPaintFilterCanvas& canvas, MultiThreadCacheType enableType,
        RSSurfaceRenderParams& surfaceParams, bool drawCacheSuccess);
    void EnableGpuOverDrawDrawBufferOptimization(Drawing::Canvas& canvas, RSSurfaceRenderParams* surfaceParams);

    // dirty manager
    void UpdateDisplayDirtyManager(int32_t bufferage, bool useAlignedDirtyRegion = false);

    // DMA Buffer
    bool DrawUIFirstCacheWithDma(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);
    void DrawDmaBufferWithGPU(RSPaintFilterCanvas& canvas);
    void DrawSelfDrawingNodeBuffer(RSPaintFilterCanvas& canvas,
        const RSSurfaceRenderParams& surfaceParams, BufferDrawParam& params);

    void ClipHoleForSelfDrawingNode(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);
    void DrawBufferForRotationFixed(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);

    std::string name_;
    RSSurfaceNodeType surfaceNodeType_ = RSSurfaceNodeType::DEFAULT;
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IBufferConsumerListener> consumerListener_ = nullptr;
#endif
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> surface_ = nullptr;
#else
    std::shared_ptr<RSSurface> surface_ = nullptr;
#endif
    bool surfaceCreated_ = false;

    // UIFIRST
    UIFirstParams uiFirstParams;
    ClearCacheSurfaceFunc clearCacheSurfaceFunc_ = nullptr;
    uint32_t cacheSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    uint32_t completedSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    mutable std::recursive_mutex completeResourceMutex_; // only lock complete Resource
    std::shared_ptr<Drawing::Surface> cacheSurface_ = nullptr;
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface_ = nullptr;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::BackendTexture cacheBackendTexture_;
    Drawing::BackendTexture cacheCompletedBackendTexture_;
#ifdef RS_ENABLE_VK
    NativeBufferUtils::VulkanCleanupHelper* cacheCleanupHelper_ = nullptr;
    NativeBufferUtils::VulkanCleanupHelper* cacheCompletedCleanupHelper_ = nullptr;
#endif
    std::atomic<bool> isCacheSurfaceNeedUpdate_ = false;
#endif
    std::atomic<bool> isTextureValid_ = false;
    pid_t lastFrameUsedThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    NodePriorityType priority_ = NodePriorityType::MAIN_PRIORITY;
    bool hasHdrPresent_ = false;
    // hdr
    GraphicColorGamut targetColorGamut_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    ScreenId screenId_ = INVALID_SCREEN_ID;
    uint64_t frameCount_ = 0;
    bool isSubThreadSkip_ = false;

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
    bool vmaCacheOff_ = false;

    RSDrawWindowCache drawWindowCache_;
    friend class OHOS::Rosen::RSDrawWindowCache;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_SURFACE_RENDER_NODE_DRAWABLE_H
