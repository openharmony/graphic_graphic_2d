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
#include <surface.h>
#include <surface_buffer.h>
#include <sync_fence.h>

#include "surface_type.h"
#include "display_type.h"

namespace OHOS {
namespace Rosen {
static const std::map<GraphicTransformType, std::string> TransformTypeStrs = {
    {GRAPHIC_ROTATE_NONE,                    "0 <no rotation>"},
    {GRAPHIC_ROTATE_90,                      "1 <rotation by 90 degrees>"},
    {GRAPHIC_ROTATE_180,                     "2 <rotation by 180 degrees>"},
    {GRAPHIC_ROTATE_270,                     "3 <rotation by 270 degrees>"},
    {GRAPHIC_ROTATE_BUTT,                    "4 <uninitialized>"},
};

static const std::map<CompositionType, std::string> CompositionTypeStrs = {
    {COMPOSITION_CLIENT,             "0 <client composistion>"},
    {COMPOSITION_DEVICE,             "1 <device composistion>"},
    {COMPOSITION_CURSOR,             "2 <cursor composistion>"},
    {COMPOSITION_VIDEO,              "3 <video composistion>"},
    {COMPOSITION_DEVICE_CLEAR,       "4 <device clear composistion>"},
    {COMPOSITION_CLIENT_CLEAR,       "5 <client clear composistion>"},
    {COMPOSITION_TUNNEL,             "6 <tunnel composistion>"},
    {COMPOSITION_BUTT,               "7 <uninitialized>"},
};

static const std::map<BlendType, std::string> BlendTypeStrs = {
    {BLEND_NONE,                     "0 <No blending>"},
    {BLEND_CLEAR,                    "1 <CLEAR blending>"},
    {BLEND_SRC,                      "2 <SRC blending>"},
    {BLEND_SRCOVER,                  "3 <SRC_OVER blending>"},
    {BLEND_DSTOVER,                  "4 <DST_OVER blending>"},
    {BLEND_SRCIN,                    "5 <SRC_IN blending>"},
    {BLEND_DSTIN,                    "6 <DST_IN blending>"},
    {BLEND_SRCOUT,                   "7 <SRC_OUT blending>"},
    {BLEND_DSTOUT,                   "8 <DST_OUT blending>"},
    {BLEND_SRCATOP,                  "9 <SRC_ATOP blending>"},
    {BLEND_DSTATOP,                  "10 <DST_ATOP blending>"},
    {BLEND_ADD,                      "11 <ADD blending>"},
    {BLEND_XOR,                      "12 <XOR blending>"},
    {BLEND_DST,                      "13 <DST blending>"},
    {BLEND_AKS,                      "14 <AKS blending>"},
    {BLEND_AKD,                      "15 <AKD blending>"},
    {BLEND_BUTT,                     "16 <Uninitialized>"},
};

class HdiLayerInfo {
public:
    HdiLayerInfo() = default;
    virtual ~HdiLayerInfo() = default;

    /* rs create and set/get layer info begin */
    static std::shared_ptr<HdiLayerInfo> CreateHdiLayerInfo()
    {
        return std::make_shared<HdiLayerInfo>();
    }

    void SetSurface(const sptr<Surface> &surface)
    {
        cSurface_ = surface;
    }

    void SetBuffer(const sptr<SurfaceBuffer> &sbuffer, const sptr<SyncFence>& acquireFence)
    {
        sbuffer_ = sbuffer;
        acquireFence_ = acquireFence;
    }

    void SetZorder(int32_t zOrder)
    {
        zOrder_ = static_cast<uint32_t>(zOrder);
    }

    void SetAlpha(const LayerAlpha &alpha)
    {
        layerAlpha_ = alpha;
    }

    void SetTransform(GraphicTransformType type)
    {
        transformType_ = type;
    }

    void SetCompositionType(CompositionType type)
    {
        compositionType_ = type;
    }

    void SetVisibleRegion(uint32_t num, const IRect &visibleRegion)
    {
        visibleNum_ = num;
        visibleRegion_ = visibleRegion;
    }

    void SetDirtyRegion(const IRect &dirtyRegion)
    {
        dirtyRegion_ = dirtyRegion;
    }

    void SetBlendType(BlendType type)
    {
        blendType_ = type;
    }

    void SetCropRect(const IRect &crop)
    {
        cropRect_ = crop;
    }

    void SetPreMulti(bool preMulti)
    {
        preMulti_ = preMulti;
    }

    void SetLayerSize(const IRect &layerRect)
    {
        layerRect_ = layerRect;
    }

    void SetLayerAdditionalInfo(void *info)
    {
        additionalInfo_ = info;
    }

    void* GetLayerAdditionalInfo()
    {
        return additionalInfo_;
    }

    void SetColorTransform(const float *matrix)
    {
        colorTransformMatrix_ = const_cast<float *>(matrix);
    }

    void SetColorDataSpace(ColorDataSpace colorSpace)
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

    const GraphicPresentTimestamp& GetPresentTimestamp()
    {
        return presentTimestamp_;
    }

    /* rs create and set/get layer info end */

    /* hdiLayer get layer info begin */
    sptr<Surface> GetSurface() const
    {
        return cSurface_;
    }

    sptr<SurfaceBuffer> GetBuffer() const
    {
        return sbuffer_;
    }

    uint32_t GetZorder() const
    {
        return zOrder_;
    }

    sptr<SyncFence> GetAcquireFence() const
    {
        return acquireFence_;
    }
    
    /* const */ LayerAlpha& GetAlpha()
    {
        return layerAlpha_;
    }

    GraphicTransformType GetTransformType() const
    {
        return transformType_;
    }

    CompositionType GetCompositionType() const
    {
        return compositionType_;
    }

    uint32_t GetVisibleNum() const
    {
        return visibleNum_;
    }

    /* const */ IRect& GetVisibleRegion()
    {
        return visibleRegion_;
    }

    /* const */ IRect& GetDirtyRegion()
    {
        return dirtyRegion_;
    }

    BlendType GetBlendType() const
    {
        return blendType_;
    }

    /* const */ IRect& GetCropRect()
    {
        return cropRect_;
    }

    /* const */ IRect& GetLayerSize()
    {
        return layerRect_;
    }

    bool IsPreMulti() const
    {
        return preMulti_;
    }

    float* GetColorTransform() const
    {
        return colorTransformMatrix_;
    }

    ColorDataSpace GetColorDataSpace() const
    {
        return colorSpace_;
    }

    std::vector<GraphicHDRMetaData>& GetMetaData()
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
        zOrder_ = layerInfo->GetZorder();
        visibleNum_ = layerInfo->GetVisibleNum();
        layerRect_ = layerInfo->GetLayerSize();
        visibleRegion_ = layerInfo->GetVisibleRegion();
        dirtyRegion_ = layerInfo->GetDirtyRegion();
        cropRect_ = layerInfo->GetCropRect();
        layerAlpha_ = layerInfo->GetAlpha();
        transformType_ = layerInfo->GetTransformType();
        compositionType_ = layerInfo->GetCompositionType();
        blendType_ = layerInfo->GetBlendType();
        colorTransformMatrix_ = layerInfo->GetColorTransform();
        colorSpace_ = layerInfo->GetColorDataSpace();
        metaData_ = layerInfo->GetMetaData();
        metaDataSet_ = layerInfo->GetMetaDataSet();
        tunnelHandle_ = layerInfo->GetTunnelHandle();
        tunnelHandleChange_ = layerInfo->GetTunnelHandleChange();
        sbuffer_ = layerInfo->GetBuffer();
        acquireFence_ = layerInfo->GetAcquireFence();
        preMulti_ = layerInfo->IsPreMulti();
    }

    void Dump(std::string &result) const
    {
        if (TransformTypeStrs.find(transformType_) != TransformTypeStrs.end() &&
            CompositionTypeStrs.find(compositionType_) != CompositionTypeStrs.end() &&
            BlendTypeStrs.find(blendType_) != BlendTypeStrs.end()) {
            result += " zOrder = " + std::to_string(zOrder_) +
                ", visibleNum = " + std::to_string(visibleNum_) +
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
        result += "visibleRegion = [" + std::to_string(visibleRegion_.x) + ", " +
            std::to_string(visibleRegion_.y) + ", " +
            std::to_string(visibleRegion_.w) + ", " +
            std::to_string(visibleRegion_.h) + "], ";
        result += "dirtyRegion = [" + std::to_string(dirtyRegion_.x) + ", " +
            std::to_string(dirtyRegion_.y) + ", " +
            std::to_string(dirtyRegion_.w) + ", " +
            std::to_string(dirtyRegion_.h) + "], ";
        result += "cropRect = [" + std::to_string(cropRect_.x) + ", " +
            std::to_string(cropRect_.y) + ", " +
            std::to_string(cropRect_.w) + ", " +
            std::to_string(cropRect_.h) + "].\n";
        if (cSurface_ != nullptr) {
            cSurface_->Dump(result);
        }
    }
    /* hdiLayer get layer info end */

private:
    uint32_t zOrder_ = 0;
    uint32_t visibleNum_ = 0;
    IRect layerRect_;
    IRect visibleRegion_;
    IRect dirtyRegion_;
    IRect cropRect_;
    LayerAlpha layerAlpha_;
    GraphicTransformType transformType_ = GraphicTransformType::GRAPHIC_ROTATE_BUTT;
    CompositionType compositionType_;
    BlendType blendType_;
    float *colorTransformMatrix_ = nullptr;
    ColorDataSpace colorSpace_ = ColorDataSpace::COLOR_DATA_SPACE_UNKNOWN;
    std::vector<GraphicHDRMetaData> metaData_;
    GraphicHDRMetaDataSet metaDataSet_;
    sptr<SurfaceTunnelHandle> tunnelHandle_ = nullptr;
    bool tunnelHandleChange_ = false;
    bool IsSupportedPresentTimestamp_ = false;
    GraphicPresentTimestamp presentTimestamp_ = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};

    void *additionalInfo_ = nullptr;
    sptr<Surface> cSurface_ = nullptr;
    sptr<SyncFence> acquireFence_ = SyncFence::INVALID_FENCE;
    sptr<SurfaceBuffer> sbuffer_ = nullptr;
    bool preMulti_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_LAYER_INFO_H