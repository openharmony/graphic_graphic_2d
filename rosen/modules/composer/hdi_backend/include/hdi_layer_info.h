/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HDI_BACKEND_HDI_LAYER_INFO_H
#define HDI_BACKEND_HDI_LAYER_INFO_H

#include <string>
#include "iconsumer_surface.h"
#include <surface.h>
#include <sync_fence.h>
#include "hdi_log.h"
#include "hdi_display_type.h"

namespace OHOS {
namespace Rosen {
static const std::map<GraphicTransformType, std::string> TransformTypeStrs = {
    {GRAPHIC_ROTATE_NONE,                    "0 <no rotation>"},
    {GRAPHIC_ROTATE_90,                      "1 <rotation by 90 degrees>"},
    {GRAPHIC_ROTATE_180,                     "2 <rotation by 180 degrees>"},
    {GRAPHIC_ROTATE_270,                     "3 <rotation by 270 degrees>"},
    {GRAPHIC_FLIP_H,                         "4 <flip horizontally>"},
    {GRAPHIC_FLIP_V,                         "5 <flip vertically>"},
    {GRAPHIC_FLIP_H_ROT90,                   "6 <flip horizontally and rotate 90 degrees>"},
    {GRAPHIC_FLIP_V_ROT90,                   "7 <flip vertically and rotate 90 degrees>"},
    {GRAPHIC_FLIP_H_ROT180,                  "8 <flip horizontally and rotate 180 degrees>"},
    {GRAPHIC_FLIP_V_ROT180,                  "9 <flip vertically and rotate 180 degrees>"},
    {GRAPHIC_FLIP_H_ROT270,                  "10 <flip horizontally and rotate 270 degrees>"},
    {GRAPHIC_FLIP_V_ROT270,                  "11 <flip vertically and rotate 270 degrees>"},
    {GRAPHIC_ROTATE_BUTT,                    "12 <uninitialized>"},
};

static const std::map<GraphicCompositionType, std::string> CompositionTypeStrs = {
    {GRAPHIC_COMPOSITION_CLIENT,             "0 <client composistion>"},
    {GRAPHIC_COMPOSITION_DEVICE,             "1 <device composistion>"},
    {GRAPHIC_COMPOSITION_CURSOR,             "2 <cursor composistion>"},
    {GRAPHIC_COMPOSITION_VIDEO,              "3 <video composistion>"},
    {GRAPHIC_COMPOSITION_DEVICE_CLEAR,       "4 <device clear composistion>"},
    {GRAPHIC_COMPOSITION_CLIENT_CLEAR,       "5 <client clear composistion>"},
    {GRAPHIC_COMPOSITION_TUNNEL,             "6 <tunnel composistion>"},
    {GRAPHIC_COMPOSITION_BUTT,               "7 <uninitialized>"},
    {GRAPHIC_COMPOSITION_SOLID_COLOR,        "8 <layercolor composition>"},
};

static const std::map<GraphicBlendType, std::string> BlendTypeStrs = {
    {GRAPHIC_BLEND_NONE,                     "0 <No blending>"},
    {GRAPHIC_BLEND_CLEAR,                    "1 <CLEAR blending>"},
    {GRAPHIC_BLEND_SRC,                      "2 <SRC blending>"},
    {GRAPHIC_BLEND_SRCOVER,                  "3 <SRC_OVER blending>"},
    {GRAPHIC_BLEND_DSTOVER,                  "4 <DST_OVER blending>"},
    {GRAPHIC_BLEND_SRCIN,                    "5 <SRC_IN blending>"},
    {GRAPHIC_BLEND_DSTIN,                    "6 <DST_IN blending>"},
    {GRAPHIC_BLEND_SRCOUT,                   "7 <SRC_OUT blending>"},
    {GRAPHIC_BLEND_DSTOUT,                   "8 <DST_OUT blending>"},
    {GRAPHIC_BLEND_SRCATOP,                  "9 <SRC_ATOP blending>"},
    {GRAPHIC_BLEND_DSTATOP,                  "10 <DST_ATOP blending>"},
    {GRAPHIC_BLEND_ADD,                      "11 <ADD blending>"},
    {GRAPHIC_BLEND_XOR,                      "12 <XOR blending>"},
    {GRAPHIC_BLEND_DST,                      "13 <DST blending>"},
    {GRAPHIC_BLEND_AKS,                      "14 <AKS blending>"},
    {GRAPHIC_BLEND_AKD,                      "15 <AKD blending>"},
    {GRAPHIC_BLEND_BUTT,                     "16 <Uninitialized>"},
};

class HdiLayerInfo {
public:
    HdiLayerInfo() = default;
    virtual ~HdiLayerInfo() = default;

    enum class LayerMask {
        LAYER_MASK_NORMAL = 0,
        LAYER_MASK_HBM_SYNC = 1,   // enable fingerprint
    };

    /* rs create and set/get layer info begin */
    static std::shared_ptr<HdiLayerInfo> CreateHdiLayerInfo()
    {
        return std::make_shared<HdiLayerInfo>();
    }

    void SetSurface(const sptr<IConsumerSurface> &surface)
    {
        cSurface_ = surface;
    }

    void SetBuffer(const sptr<SurfaceBuffer> &sbuffer, const sptr<SyncFence> &acquireFence)
    {
        sbuffer_ = sbuffer;
        acquireFence_ = acquireFence;
    }

    void SetPreBuffer(const sptr<SurfaceBuffer> &buffer)
    {
        pbuffer_ = buffer;
    }

    void SetZorder(int32_t zOrder)
    {
        zOrder_ = static_cast<uint32_t>(zOrder);
    }

    void SetAlpha(const GraphicLayerAlpha &alpha)
    {
        layerAlpha_ = alpha;
    }

    void SetTransform(GraphicTransformType type)
    {
        transformType_ = type;
    }

    void SetCompositionType(GraphicCompositionType type)
    {
        compositionType_ = type;
    }

    void SetVisibleRegions(const std::vector<GraphicIRect> &visibleRegions)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        visibleRegions_ = visibleRegions;
    }

    void SetDirtyRegions(const std::vector<GraphicIRect> &dirtyRegions)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        dirtyRegions_ = dirtyRegions;
    }

    void SetBlendType(GraphicBlendType type)
    {
        blendType_ = type;
    }

    void SetCropRect(const GraphicIRect &crop)
    {
        cropRect_ = crop;
    }

    void SetPreMulti(bool preMulti)
    {
        preMulti_ = preMulti;
    }

    void SetLayerSize(const GraphicIRect &layerRect)
    {
        layerRect_ = layerRect;
    }

    void SetBoundSize(const GraphicIRect &boundRect)
    {
        boundRect_ = boundRect;
    }

    void SetLayerAdditionalInfo(void *info)
    {
        additionalInfo_ = info;
    }

    void* GetLayerAdditionalInfo()
    {
        return additionalInfo_;
    }

    void SetLayerColor(GraphicLayerColor layerColor)
    {
        layerColor_ = layerColor;
    }

    void SetColorTransform(const std::vector<float> &matrix)
    {
        colorTransformMatrix_ = matrix;
    }

    void SetColorDataSpace(GraphicColorDataSpace colorSpace)
    {
        colorSpace_ = colorSpace;
    }

    void SetMetaData(const std::vector<GraphicHDRMetaData> &metaData)
    {
        metaData_ = metaData;
    }

    void SetMetaDataSet(const GraphicHDRMetaDataSet &metaDataSet)
    {
        metaDataSet_ = metaDataSet;
    }

    void SetMatrix(GraphicMatrix matrix)
    {
        matrix_ = matrix;
    }

    void SetGravity(int32_t gravity)
    {
        gravity_ = gravity;
    }

    void SetUniRenderFlag(bool isUniRender)
    {
        isUniRender_ = isUniRender;
    }

    void SetTunnelHandleChange(bool change)
    {
        tunnelHandleChange_ = change;
    }

    void SetTunnelHandle(const sptr<SurfaceTunnelHandle> &handle)
    {
        tunnelHandle_ = handle;
    }

    bool IsSupportedPresentTimestamp() const
    {
        return IsSupportedPresentTimestamp_;
    }

    const GraphicPresentTimestamp &GetPresentTimestamp()
    {
        return presentTimestamp_;
    }

    /* rs create and set/get layer info end */

    /* hdiLayer get layer info begin */
    sptr<IConsumerSurface> GetSurface() const
    {
        return cSurface_;
    }

    sptr<SurfaceBuffer> GetBuffer() const
    {
        return sbuffer_;
    }

    sptr<SurfaceBuffer> GetPreBuffer() const
    {
        return pbuffer_;
    }

    uint32_t GetZorder() const
    {
        return zOrder_;
    }

    sptr<SyncFence> GetAcquireFence() const
    {
        return acquireFence_;
    }

    const GraphicLayerAlpha &GetAlpha()
    {
        return layerAlpha_;
    }

    GraphicTransformType GetTransformType() const
    {
        return transformType_;
    }

    GraphicCompositionType GetCompositionType() const
    {
        return compositionType_;
    }

    const std::vector<GraphicIRect> &GetVisibleRegions()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return visibleRegions_;
    }

    const std::vector<GraphicIRect> &GetDirtyRegions()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return dirtyRegions_;
    }

    GraphicBlendType GetBlendType() const
    {
        return blendType_;
    }

    const GraphicIRect &GetCropRect()
    {
        return cropRect_;
    }

    const GraphicIRect &GetLayerSize()
    {
        return layerRect_;
    }

    GraphicIRect GetBoundSize() const
    {
        return boundRect_;
    }

    GraphicMatrix GetMatrix() const
    {
        return matrix_;
    }

    int32_t GetGravity() const
    {
        return gravity_;
    }

    bool GetUniRenderFlag() const
    {
        return isUniRender_;
    }

    bool IsPreMulti() const
    {
        return preMulti_;
    }

    const std::vector<float> &GetColorTransform()
    {
        return colorTransformMatrix_;
    }

    GraphicColorDataSpace GetColorDataSpace() const
    {
        return colorSpace_;
    }

    GraphicLayerColor GetLayerColor() const
    {
        return layerColor_;
    }

    std::vector<GraphicHDRMetaData> &GetMetaData()
    {
        return metaData_;
    }

    GraphicHDRMetaDataSet &GetMetaDataSet()
    {
        return metaDataSet_;
    }

    bool GetTunnelHandleChange() const
    {
        return tunnelHandleChange_;
    }

    sptr<SurfaceTunnelHandle> GetTunnelHandle() const
    {
        return tunnelHandle_;
    }

    void SetIsSupportedPresentTimestamp(bool isSupported)
    {
        IsSupportedPresentTimestamp_ = isSupported;
    }

    void SetPresentTimestamp(const GraphicPresentTimestamp &timestamp)
    {
        presentTimestamp_ = timestamp;
    }

    void CopyLayerInfo(const std::shared_ptr<HdiLayerInfo> &layerInfo)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        zOrder_ = layerInfo->GetZorder();
        layerRect_ = layerInfo->GetLayerSize();
        boundRect_ = layerInfo->GetBoundSize();
        visibleRegions_ = layerInfo->GetVisibleRegions();
        dirtyRegions_ = layerInfo->GetDirtyRegions();
        cropRect_ = layerInfo->GetCropRect();
        matrix_ = layerInfo->GetMatrix();
        gravity_ = layerInfo->GetGravity();
        layerAlpha_ = layerInfo->GetAlpha();
        transformType_ = layerInfo->GetTransformType();
        compositionType_ = layerInfo->GetCompositionType();
        blendType_ = layerInfo->GetBlendType();
        colorTransformMatrix_ = layerInfo->GetColorTransform();
        colorSpace_ = layerInfo->GetColorDataSpace();
        layerColor_ = layerInfo->GetLayerColor();
        metaData_ = layerInfo->GetMetaData();
        metaDataSet_ = layerInfo->GetMetaDataSet();
        tunnelHandle_ = layerInfo->GetTunnelHandle();
        tunnelHandleChange_ = layerInfo->GetTunnelHandleChange();
        sbuffer_ = layerInfo->GetBuffer();
        pbuffer_= layerInfo->GetPreBuffer();
        acquireFence_ = layerInfo->GetAcquireFence();
        preMulti_ = layerInfo->IsPreMulti();
    }

    void Dump(std::string &result) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (TransformTypeStrs.find(transformType_) != TransformTypeStrs.end() &&
            CompositionTypeStrs.find(compositionType_) != CompositionTypeStrs.end() &&
            BlendTypeStrs.find(blendType_) != BlendTypeStrs.end()) {
            result += " zOrder = " + std::to_string(zOrder_) +
                ", visibleNum = " + std::to_string(visibleRegions_.size()) +
                ", transformType = " + TransformTypeStrs.at(transformType_) +
                ", compositionType = " + CompositionTypeStrs.at(compositionType_) +
                ", blendType = " + BlendTypeStrs.at(blendType_) +
                ", layerAlpha = [enGlobalAlpha(" + std::to_string(layerAlpha_.enGlobalAlpha) + "), enPixelAlpha(" +
                std::to_string(layerAlpha_.enPixelAlpha) + "), alpha0(" +
                std::to_string(layerAlpha_.alpha0) + "), alpha1(" +
                std::to_string(layerAlpha_.alpha1) + "), gAlpha(" +
                std::to_string(layerAlpha_.gAlpha) + ")].\n";
        }
        result += " layerRect = [" + std::to_string(layerRect_.x) + ", " +
            std::to_string(layerRect_.y) + ", " +
            std::to_string(layerRect_.w) + ", " +
            std::to_string(layerRect_.h) + "], ";
        result += "cropRect = [" + std::to_string(cropRect_.x) + ", " +
            std::to_string(cropRect_.y) + ", " +
            std::to_string(cropRect_.w) + ", " +
            std::to_string(cropRect_.h) + "],";
        for (decltype(visibleRegions_.size()) i = 0; i < visibleRegions_.size(); i++) {
            result += "visibleRegions[" + std::to_string(i) + "] = [" +
                std::to_string(visibleRegions_[i].x) + ", " +
                std::to_string(visibleRegions_[i].y) + ", " +
                std::to_string(visibleRegions_[i].w) + ", " +
                std::to_string(visibleRegions_[i].h) + "], ";
        }
        for (decltype(dirtyRegions_.size()) i = 0; i < dirtyRegions_.size(); i++) {
            result += "dirtyRegions[" + std::to_string(i) + "] = [" +
                std::to_string(dirtyRegions_[i].x) + ", " +
                std::to_string(dirtyRegions_[i].y) + ", " +
                std::to_string(dirtyRegions_[i].w) + ", " +
                std::to_string(dirtyRegions_[i].h) + "], ";
        }
        if (cSurface_ != nullptr) {
            cSurface_->Dump(result);
        }
    }

    RosenError SetLayerMaskInfo(LayerMask mask)
    {
        switch (mask) {
            case LayerMask::LAYER_MASK_NORMAL:
            case LayerMask::LAYER_MASK_HBM_SYNC:
                break;
            default:
                HLOGE("Invalid argument [mask:%{public}d]", static_cast<int32_t>(mask));
                return ROSEN_ERROR_INVALID_ARGUMENTS;
        }

        layerMask_ = mask;
        return ROSEN_ERROR_OK;
    }

    LayerMask GetLayerMaskInfo()
    {
        return layerMask_;
    }
    /* hdiLayer get layer info end */

private:
    uint32_t zOrder_ = 0;
    GraphicIRect layerRect_;
    GraphicIRect boundRect_; // node's bound width and height related to this layer, used for uni render redraw
    std::vector<GraphicIRect> visibleRegions_;
    std::vector<GraphicIRect> dirtyRegions_;
    GraphicIRect cropRect_;
    GraphicMatrix matrix_; // matrix used for uni render redraw
    int32_t gravity_; // used for uni render redraw
    bool isUniRender_ = false; // true for uni render layer (DisplayNode)
    GraphicLayerAlpha layerAlpha_;
    GraphicTransformType transformType_ = GraphicTransformType::GRAPHIC_ROTATE_BUTT;
    GraphicCompositionType compositionType_;
    GraphicBlendType blendType_;
    std::vector<float> colorTransformMatrix_;
    GraphicLayerColor layerColor_;
    GraphicColorDataSpace colorSpace_ = GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    std::vector<GraphicHDRMetaData> metaData_;
    GraphicHDRMetaDataSet metaDataSet_;
    sptr<SurfaceTunnelHandle> tunnelHandle_ = nullptr;
    bool tunnelHandleChange_ = false;
    bool IsSupportedPresentTimestamp_ = false;
    GraphicPresentTimestamp presentTimestamp_ = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};

    void *additionalInfo_ = nullptr;
    sptr<IConsumerSurface> cSurface_ = nullptr;
    sptr<SyncFence> acquireFence_ = SyncFence::INVALID_FENCE;
    sptr<SurfaceBuffer> sbuffer_ = nullptr;
    sptr<SurfaceBuffer> pbuffer_ = nullptr;
    bool preMulti_ = false;
    LayerMask layerMask_ = LayerMask::LAYER_MASK_NORMAL;
    mutable std::mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_LAYER_INFO_H