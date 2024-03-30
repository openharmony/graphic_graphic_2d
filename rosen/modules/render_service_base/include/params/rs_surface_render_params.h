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
    GraphicMatrix matrix;
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

    void SetNeedSubmitSubThread(bool needSubmitSubThread)
    {
        processed = false;
        needSubmitSubThread_ = needSubmitSubThread;
    }

    bool GetNeedSubmitSubThread()
    {
        return needSubmitSubThread_;
    }
    bool processed = false;

    void SetUifirstNodeEnableParam(bool isUifirst)
    {
        if (uiFirstFlag_ == isUifirst) {
            return;
        }
        uiFirstFlag_ = isUifirst;
        needSync_ = true;
    }

    bool GetUifirstNodeEnableParam()
    {
        return uiFirstFlag_;
    }

    void SetIsParentUifirstNodeEnableParam(bool isUifirstParent)
    {
        if (uiFirstParentFlag_ == isUifirstParent) {
            return;
        }
        uiFirstParentFlag_ = isUifirstParent;
        needSync_ = true;
    }

    bool GetParentUifirstNodeEnableParam()
    {
        return uiFirstParentFlag_;
    }
    void SetOcclusionVisible(bool visible);
    bool GetOcclusionVisible() const;

    void SetIsTransparent(bool isTransparent);
    bool GetIsTransparent() const;

    void SetOldDirtyInSurface(const RectI& oldDirtyInSurface);
    RectI GetOldDirtyInSurface() const;

    void SetVisibleRegion(const Occlusion::Region& visibleRegion);
    Occlusion::Region GetVisibleRegion() const;

    void SetOccludedByFilterCache(bool val);
    bool GetOccludedByFilterCache() const;

    void SetLayerInfo(const RSLayerInfo& layerInfo);
    RSLayerInfo& GetLayerInfo();
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
    bool needSubmitSubThread_ = false; // UI First
    bool isMainThreadNode_ = false; //UI First
    bool uiFirstFlag_ = false;
    bool uiFirstParentFlag_ = false;
    Color backgroundColor_ = RgbPalette::Transparent();

    RectI oldDirtyInSurface_;
    RectI absDrawRect_;
    RRect rrect_;

    bool occlusionVisible_ = false;
    Occlusion::Region visibleRegion_;
    bool isOccludedByFilterCache_ = false;
    RSLayerInfo layerInfo_;
    bool isHardwareEnabled_ = false;
    bool isLastFrameHardwareEnabled_ = false;

    friend class RSSurfaceRenderNode;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
