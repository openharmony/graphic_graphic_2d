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

#ifndef RS_SUB_THREAD_CACHE_H
#define RS_SUB_THREAD_CACHE_H

#ifndef ROSEN_CROSS_PLATFORM
#include <ibuffer_consumer_listener.h>
#include <iconsumer_surface.h>
#include <surface.h>
#endif
#include "platform/drawing/rs_surface.h"

#include "common/rs_common_def.h"
#include "drawable/rs_render_node_drawable.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_surface_render_node.h"
#include "feature/uifirst/rs_draw_window_cache.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RsSubThreadCache {
public:
    RsSubThreadCache();
    ~RsSubThreadCache()
    {
        ClearCacheSurfaceInThread();
    }
    RsSubThreadCache(const RsSubThreadCache&) = delete;
    RsSubThreadCache& operator=(const RsSubThreadCache&) = delete;

    struct UIFirstParams {
        uint32_t submittedSubThreadIndex_ = INT_MAX;
        std::atomic<CacheProcessStatus> cacheProcessStatus_ = CacheProcessStatus::UNKNOWN;
        std::atomic<bool> isNeedSubmitSubThread_ = true;
    };

    void SetNodeId(NodeId nodeId)
    {
        nodeId_ = nodeId;
    }

    NodeId GetNodeId() const
    {
        return nodeId_;
    }

    RSDrawWindowCache& GetRSDrawWindowCache()
    {
        return drawWindowCache_;
    }

    void SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus);
    CacheProcessStatus GetCacheSurfaceProcessedStatus() const;

    // UIFirst
    bool BufferFormatNeedUpdate(std::shared_ptr<Drawing::Surface> cacheSurface, bool isNeedFP16);
    void UpdateCompletedCacheSurface();
    void ClearCacheSurfaceInThread();
    void ClearCacheSurfaceOnly();
    void UpdateCacheSurfaceInfo(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable);
    std::shared_ptr<Drawing::Surface> GetCacheSurface(uint32_t threadIndex);
    bool NeedInitCacheSurface(RSSurfaceRenderParams* surfaceParams);
    std::shared_ptr<Drawing::Image> GetCompletedImage(RSPaintFilterCanvas& canvas, uint32_t threadIndex,
        bool isUIFirst);
    using ClearCacheSurfaceFunc =
        std::function<void(std::shared_ptr<Drawing::Surface>&&,
        std::shared_ptr<Drawing::Surface>&&, uint32_t, uint32_t)>;
    void InitCacheSurface(Drawing::GPUContext* grContext,
        std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable = nullptr,
        ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX, bool isNeedFP16 = false);

    void ResetUifirst(bool isOnlyClearCache = false);

    void ResetWindowCache()
    {
        drawWindowCache_.ClearCache();
    }

    bool HasCachedTexture() const;

    float GetCacheSurfaceAlphaInfo()
    {
        return cacheSurfaceInfo_.alpha;
    }

    int GetCacheSurfaceProcessedNodes()
    {
        return cacheSurfaceInfo_.processedNodeCount;
    }

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

    void SetTaskFrameCount(uint64_t frameCount);

    uint64_t GetTaskFrameCount() const;

    void SetSubThreadSkip(bool isSubThreadSkip);
    bool IsSubThreadSkip() const
    {
        return isSubThreadSkip_;
    }

    void ProcessSurfaceSkipCount();
    void ResetSurfaceSkipCount();
    int32_t GetSurfaceSkipCount() const;
    int32_t GetSurfaceSkipPriority();
    bool IsHighPostPriority();
    void SetHighPostPriority(bool postPriority);

    uint32_t GetUifirstPostOrder() const;
    void SetUifirstPostOrder(uint32_t order);

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

    void SetHDRPresent(bool hasHdrPresent)
    {
        hasHdrPresent_ = hasHdrPresent;
    }

    bool GetHDRPresent() const
    {
        return hasHdrPresent_;
    }

    int GetTotalProcessedSurfaceCount() const;
    void TotalProcessedSurfaceCountInc(RSPaintFilterCanvas& canvas);
    void ClearTotalProcessedSurfaceCount();
    inline bool CheckCacheSurface()
    {
        return cacheSurface_ ? true : false;
    }
    bool DealWithUIFirstCache(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable, RSPaintFilterCanvas& canvas,
        RSSurfaceRenderParams& surfaceParams, RSRenderThreadParams& uniParams);
    bool DrawCacheSurface(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable, RSPaintFilterCanvas& canvas,
        const Vector2f& boundSize, uint32_t threadIndex = UNI_MAIN_THREAD_INDEX, bool isUIFirst = false);

    // uifirst dirtyRegion
    std::shared_ptr<RSDirtyRegionManager> GetSyncUifirstDirtyManager() const;
    bool UpdateCacheSurfaceDirtyManager(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
        bool hasCompleteCache, bool isLastFrameSkip);
    void UpdateUifirstDirtyManager(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable);
    void SetUifirstDirtyRegion(Drawing::Region dirtyRegion);
    Drawing::Region GetUifirstDirtyRegion() const;
    bool CalculateUifirstDirtyRegion(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
        Drawing::RectI& dirtyRect);
    bool GetCurDirtyRegionWithMatrix(const Drawing::Matrix& matrix,
        Drawing::RectF& latestDirtyRect, Drawing::RectF& absDrawRect);
    bool MergeUifirstAllSurfaceDirtyRegion(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
        Drawing::RectI& dirtyRects);
    void SetUifrstDirtyEnableFlag(bool dirtyEnableFlag);
    bool GetUifrstDirtyEnableFlag() const;
    void PushDirtyRegionToStack(RSPaintFilterCanvas& canvas, Drawing::Region& resultRegion);
    bool IsCacheValid() const;
    void UifirstDirtyRegionDfx(Drawing::Canvas& canvas, Drawing::RectI& surfaceDrawRect);
    bool IsDirtyRecordCompletated();
    void UpdateDirtyRecordCompletatedState(bool isCompletate);
    void UpadteAllSurfaceUifirstDirtyEnableState(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
        bool isEnableDirtyRegion);

    void SubDraw(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable, Drawing::Canvas& canvas);

    void SetCacheBehindWindowData(const std::shared_ptr<RSPaintFilterCanvas::CacheBehindWindowData>& data);
    void SetCacheCompletedBehindWindowData(const std::shared_ptr<RSPaintFilterCanvas::CacheBehindWindowData>& data);
    void ResetCacheBehindWindowData();
    void ResetCacheCompletedBehindWindowData();
    void DrawBehindWindowBeforeCache(RSPaintFilterCanvas& canvas,
        const Drawing::scalar px = 0.f, const Drawing::scalar py = 0.f);
private:
    void ClearCacheSurface(bool isClearCompletedCacheSurface = true);
    bool DrawUIFirstCache(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable, RSPaintFilterCanvas& rscanvas,
        bool canSkipWait);
    bool DrawUIFirstCacheWithStarting(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
        RSPaintFilterCanvas& rscanvas, NodeId id);
    void DrawUIFirstDfx(RSPaintFilterCanvas& canvas, MultiThreadCacheType enableType,
        RSSurfaceRenderParams& surfaceParams, bool drawCacheSuccess);

    NodeId nodeId_ = 0;
    // Cache in RT
    RSDrawWindowCache drawWindowCache_;

    // UIFIRST
    UIFirstParams uiFirstParams;
    bool isCacheValid_ = false;
    bool isCacheCompletedValid_ = false;
    ClearCacheSurfaceFunc clearCacheSurfaceFunc_ = nullptr;
    uint32_t cacheSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    uint32_t completedSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    mutable std::recursive_mutex completeResourceMutex_; // only lock complete Resource
    struct CacheSurfaceInfo {
        int processedSurfaceCount = -1;
        int processedNodeCount = -1;
        float alpha = -1.f;
    };
    CacheSurfaceInfo cacheSurfaceInfo_;
    CacheSurfaceInfo cacheCompletedSurfaceInfo_;
    std::shared_ptr<Drawing::Surface> cacheSurface_ = nullptr;
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface_ = nullptr;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::BackendTexture cacheBackendTexture_;
    Drawing::BackendTexture cacheCompletedBackendTexture_;
    std::atomic<bool> isCacheSurfaceNeedUpdate_ = false;
#endif
#ifdef RS_ENABLE_VK
    NativeBufferUtils::VulkanCleanupHelper* cacheCleanupHelper_ = nullptr;
    NativeBufferUtils::VulkanCleanupHelper* cacheCompletedCleanupHelper_ = nullptr;
#endif
    std::atomic<bool> isTextureValid_ = false;
    pid_t lastFrameUsedThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    NodePriorityType priority_ = NodePriorityType::MAIN_PRIORITY;
    uint64_t frameCount_ = 0;
    bool isSubThreadSkip_ = false;
    int32_t isSurfaceSkipCount_ = 0;
    int32_t isSurfaceSkipPriority_ = 0;
    bool isHighPostPriority_ = false;
    // hdr
    bool hasHdrPresent_ = false;
    GraphicColorGamut targetColorGamut_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    ScreenId screenId_ = INVALID_SCREEN_ID;
    uint32_t uifirstPostOrder_ = 0;
    static inline std::atomic<int> totalProcessedSurfaceCount_ = 0;

    std::shared_ptr<RSDirtyRegionManager> syncUifirstDirtyManager_ = nullptr;
    bool isDirtyRecordCompletated_ = false;
    Drawing::Region uifirstDirtyRegion_;
    bool uifrstDirtyEnableFlag_ = false;
    Drawing::Region uifirstMergedDirtyRegion_;
    std::shared_ptr<RSPaintFilterCanvas::CacheBehindWindowData> cacheBehindWindowData_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas::CacheBehindWindowData> cacheCompletedBehindWindowData_ = nullptr;
};
} // DrawableV2
} // OHOS::Rosen
#endif // RS_SUB_THREAD_CACHE_H