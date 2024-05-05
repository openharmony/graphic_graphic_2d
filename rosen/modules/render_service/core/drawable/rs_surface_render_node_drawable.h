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

#include "common/rs_common_def.h"
#include "drawable/rs_render_node_drawable.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {
class RSRenderThreadParams;
class RSSurfaceRenderNode;
class RSSurfaceRenderParams;
namespace DrawableV2 {
struct UIFirstParams {
    uint32_t submittedSubThreadIndex_ = INT_MAX;
    std::atomic<CacheProcessStatus> cacheProcessStatus_ = CacheProcessStatus::WAITING;
    std::atomic<bool> isNeedSubmitSubThread_ = true;
};
class RSSurfaceRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSSurfaceRenderNodeDrawable() override;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;
    bool EnableRecordingOptimization(RSRenderParams& params);

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
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);

    void ResetUifirst()
    {
        ClearCacheSurfaceInThread();
    }

    bool IsCurFrameStatic(DeviceType deviceType);

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

    void DealWithSelfDrawingNodeBuffer(RSSurfaceRenderNode& surfaceNode,
        RSPaintFilterCanvas& canvas, const RSSurfaceRenderParams& surfaceParams);

private:
    explicit RSSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    void CacheImgForCapture(RSPaintFilterCanvas& canvas, std::shared_ptr<RSDisplayRenderNode> curDisplayNode);
    bool DealWithUIFirstCache(RSSurfaceRenderNode& surfaceNode, RSPaintFilterCanvas& canvas,
        RSSurfaceRenderParams& surfaceParams, RSRenderThreadParams& uniParams);

    void CaptureSurface(RSSurfaceRenderNode& surfaceNode,
        RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams);

    void MergeDirtyRegionBelowCurSurface(RSRenderThreadParams* uniParam,
        RSSurfaceRenderParams* surfaceParams,
        std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        Drawing::Region& region);
    Drawing::Region CalculateVisibleRegion(RSRenderThreadParams* uniParam,
        RSSurfaceRenderParams* surfaceParams, std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
        bool isOffscreen) const;
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::SURFACE_NODE, OnGenerate>;
    static Registrar instance_;

    std::string name_;
    bool DrawUIFirstCache(RSPaintFilterCanvas& rscanvas, bool canSkipWait);
    bool CheckIfNeedResetRotate(RSPaintFilterCanvas& canvas);
    NodeId FindInstanceChildOfDisplay(std::shared_ptr<RSRenderNode> node);
    void DrawUIFirstDfx(RSPaintFilterCanvas& canvas, MultiThreadCacheType enableType,
        RSSurfaceRenderParams& surfaceParams, bool drawCacheSuccess);

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
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_SURFACE_RENDER_NODE_DRAWABLE_H
