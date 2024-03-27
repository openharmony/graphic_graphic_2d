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
#include "surface_buffer.h"
#include "surface_type.h"
#include "sync_fence.h"
namespace OHOS::Rosen {
class RSSurfaceRenderNode;
struct RSLayerInfo {
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
};
class RSB_EXPORT RSSurfaceRenderParams : public RSRenderParams {
public:
    explicit RSSurfaceRenderParams();
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
    Gravity GetFrameGravity() const
    {
        return frameGravity_;
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

    void SetOcclusionVisible(bool visible);
    bool GetOcclusionVisible() const;

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

    void SetBuffer(const sptr<SurfaceBuffer>& buffer);
    sptr<SurfaceBuffer> GetBuffer() const;
    void SetPreBuffer(const sptr<SurfaceBuffer>& preBuffer);
    sptr<SurfaceBuffer>& GetPreBuffer();
    void SetAcquireFence(const sptr<SyncFence>& acquireFence);
    sptr<SyncFence> GetAcquireFence() const;

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
    bool isSpherizeValid_ = false;
    bool needBilinearInterpolation_ = false;
    Gravity frameGravity_ = Gravity::CENTER;
    Color backgroundColor_ = RgbPalette::Transparent();

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
