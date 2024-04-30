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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H

#include <string>

#include "common/rs_occlusion_region.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_base_render_node.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_buffer.h"
#include "sync_fence.h"
#endif
#include "surface_type.h"

namespace OHOS::Rosen {
class RSSurfaceRenderNode;
struct RSLayerInfo {
#ifndef ROSEN_CROSS_PLATFORM
    GraphicIRect srcRect;
    GraphicIRect dstRect;
    GraphicIRect boundRect;
    Drawing::Matrix matrix;
    int32_t gravity;
    int32_t zOrder;
    sptr<SurfaceBuffer> buffer;
    sptr<SurfaceBuffer> preBuffer;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    GraphicBlendType blendType;
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    bool operator==(const RSLayerInfo& layerInfo) const
    {
        return (srcRect == layerInfo.srcRect) && (dstRect == layerInfo.dstRect) &&
            (boundRect == layerInfo.boundRect) && (matrix == layerInfo.matrix) && (gravity == layerInfo.gravity) &&
            (zOrder == layerInfo.zOrder) && (buffer == layerInfo.buffer) && (preBuffer == layerInfo.preBuffer) &&
            (acquireFence == layerInfo.acquireFence) && (blendType == layerInfo.blendType) &&
            (transformType == layerInfo.transformType);
    }
#endif
};
class RSB_EXPORT RSSurfaceRenderParams : public RSRenderParams {
public:
    explicit RSSurfaceRenderParams(NodeId id);
    virtual ~RSSurfaceRenderParams() = default;
    bool IsMainWindowType() const
    {
        return isMainWindowType_;
    }
    RSSurfaceNodeType GetSurfaceNodeType() const
    {
        return rsSurfaceNodeType_;
    }
    SelfDrawingNodeType GetSelfDrawingNodeType() const
    {
        return selfDrawingType_;
    }
    void SetAncestorDisplayNode(const RSRenderNode::WeakPtr& ancestorDisplayNode)
    {
        ancestorDisplayNode_ = ancestorDisplayNode;
    }
    RSRenderNode::WeakPtr GetAncestorDisplayNode() const
    {
        return ancestorDisplayNode_;
    }

    float GetAlpha() const
    {
        return alpha_;
    }
    bool IsSpherizeValid() const
    {
        return isSpherizeValid_;
    }
    bool NeedBilinearInterpolation() const
    {
        return needBilinearInterpolation_;
    }
    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }
    const RectI& GetAbsDrawRect() const
    {
        return absDrawRect_;
    }
    const RRect& GetRRect() const
    {
        return rrect_;
    }

    bool GetIsSecurityLayer() const
    {
        return isSecurityLayer_;
    }
    bool GetIsSkipLayer() const
    {
        return isSkipLayer_;
    }
    bool GetIsProtectedLayer() const
    {
        return isProtectedLayer_;
    }
    const std::set<NodeId>& GetSecurityLayerIds() const
    {
        return securityLayerIds_;
    }
    const std::set<NodeId>& GetSkipLayerIds() const
    {
        return skipLayerIds_;
    }
    bool HasSecurityLayer()
    {
        return securityLayerIds_.size() != 0;
    }
    bool HasSkipLayer()
    {
        return skipLayerIds_.size() != 0;
    }
    bool HasProtectedLayer()
    {
        return protectedLayerIds_.size() != 0;
    }

    std::string GetName() const
    {
        return name_;
    }

    void SetUifirstNodeEnableParam(MultiThreadCacheType isUifirst)
    {
        if (uiFirstFlag_ == isUifirst) {
            return;
        }
        uiFirstFlag_ = isUifirst;
        needSync_ = true;
    }

    MultiThreadCacheType GetUifirstNodeEnableParam()
    {
        return uiFirstFlag_;
    }

    void SetBufferClearCacheSet(const std::set<int32_t> bufferCacheSet)
    {
        if (bufferCacheSet.size() > 0) {
            bufferCacheSet_ = bufferCacheSet;
            needSync_ = true;
        }
    }

    const std::set<int32_t> GetBufferClearCacheSet()
    {
        return bufferCacheSet_;
    }

    void SetIsParentUifirstNodeEnableParam(bool isUifirstParent)
    {
        if (uiFirstParentFlag_ == isUifirstParent) {
            return;
        }
        uiFirstParentFlag_ = isUifirstParent;
        needSync_ = true;
    }

    void SetUifirstChildrenDirtyRectParam(const RectI& rect)
    {
        childrenDirtyRect_ = rect;
        needSync_ = true;
    }

    RectI& GetUifirstChildrenDirtyRectParam()
    {
        return childrenDirtyRect_;
    }
    void SetSurfaceCacheContentStatic(bool contentStatic);
    bool GetSurfaceCacheContentStatic() const;
    bool GetPreSurfaceCacheContentStatic() const;

    float GetPositionZ() const;

    void SetSurfaceSubTreeDirty(bool isSubTreeDirty);
    bool GetSurfaceSubTreeDirty() const;

    bool GetParentUifirstNodeEnableParam()
    {
        return uiFirstParentFlag_;
    }
    void SetOcclusionVisible(bool visible);
    bool GetOcclusionVisible() const;

    void SetTransparentRegion(const Occlusion::Region& transparentRegion);
    const Occlusion::Region& GetTransparentRegion() const;

    void SetOldDirtyInSurface(const RectI& oldDirtyInSurface);
    RectI GetOldDirtyInSurface() const;

    void SetVisibleRegion(const Occlusion::Region& visibleRegion);
    Occlusion::Region GetVisibleRegion() const;

    void SetOccludedByFilterCache(bool val);
    bool GetOccludedByFilterCache() const;

    void SetLayerInfo(const RSLayerInfo& layerInfo);
    const RSLayerInfo& GetLayerInfo() const;
    void SetHardwareEnabled(bool enabled);
    bool GetHardwareEnabled() const;
    void SetLastFrameHardwareEnabled(bool enabled);
    bool GetLastFrameHardwareEnabled() const;

#ifndef ROSEN_CROSS_PLATFORM
    void SetBuffer(const sptr<SurfaceBuffer>& buffer);
    sptr<SurfaceBuffer> GetBuffer() const;
    void SetPreBuffer(const sptr<SurfaceBuffer>& preBuffer);
    sptr<SurfaceBuffer>& GetPreBuffer();
    void SetAcquireFence(const sptr<SyncFence>& acquireFence);
    sptr<SyncFence> GetAcquireFence() const;
#endif

    virtual void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    // DFX
    std::string ToString() const override;

protected:
private:
    bool isMainWindowType_ = false;
    RSSurfaceNodeType rsSurfaceNodeType_ = RSSurfaceNodeType::DEFAULT;
    SelfDrawingNodeType selfDrawingType_ = SelfDrawingNodeType::DEFAULT;
    RSRenderNode::WeakPtr ancestorDisplayNode_;

    float alpha_ = 0;
    bool isTransparent_ = false;
    bool isSpherizeValid_ = false;
    bool needBilinearInterpolation_ = false;
    MultiThreadCacheType uiFirstFlag_ = MultiThreadCacheType::NONE;
    bool uiFirstParentFlag_ = false;
    Color backgroundColor_ = RgbPalette::Transparent();

    RectI oldDirtyInSurface_;
    RectI childrenDirtyRect_;
    RectI absDrawRect_;
    RRect rrect_;
    Occlusion::Region transparentRegion_;

    bool surfaceCacheContentStatic_ = false;
    bool preSurfaceCacheContentStatic_ = false;
    bool isSubTreeDirty_ = false;
    float positionZ_ = 0.0f;
    bool occlusionVisible_ = false;
    Occlusion::Region visibleRegion_;
    bool isOccludedByFilterCache_ = false;
    RSLayerInfo layerInfo_;
    bool isHardwareEnabled_ = false;
    bool isLastFrameHardwareEnabled_ = false;
    int32_t releaseInHardwareThreadTaskNum_ = 0;
    bool isSecurityLayer_ = false;
    bool isSkipLayer_ = false;
    bool isProtectedLayer_ = false;
    std::set<NodeId> skipLayerIds_= {};
    std::set<NodeId> securityLayerIds_= {};
    std::set<NodeId> protectedLayerIds_= {};
    std::set<int32_t> bufferCacheSet_ = {};
    std::string name_= "";

    friend class RSSurfaceRenderNode;
    friend class RSUniRenderProcessor;
    friend class RSUniRenderThread;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
