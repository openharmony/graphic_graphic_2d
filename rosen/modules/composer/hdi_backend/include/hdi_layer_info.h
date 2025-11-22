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
#include "graphic_error.h"
#include "hdi_display_type.h"
#include "common/rs_anco_type.h"

namespace OHOS {
namespace Rosen {
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

    void SetType(const GraphicLayerType& layerType)
    {
        layerType_ = layerType;
    }

    GraphicLayerType GetType() const
    {
        return layerType_;
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

    void SetLayerColor(GraphicLayerColor layerColor)
    {
        layerColor_ = layerColor;
    }

    void SetBackgroundColor(GraphicLayerColor backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    void SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo)
    {
        drmCornerRadiusInfo_ = drmCornerRadiusInfo;
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

    void SetTunnelLayerId(const uint64_t &tunnelLayerId)
    {
        tunnelLayerId_ = tunnelLayerId;
    }

    uint64_t GetTunnelLayerId() const
    {
        return tunnelLayerId_;
    }

    void SetTunnelLayerProperty(uint32_t tunnelLayerProperty)
    {
        tunnelLayerProperty_ = tunnelLayerProperty;
    }

    uint32_t GetTunnelLayerProperty() const
    {
        return tunnelLayerProperty_;
    }

    bool IsSupportedPresentTimestamp() const
    {
        return isSupportedPresentTimestamp_;
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

    void SetWindowsName(std::vector<std::string>& windowsName)
    {
        windowsName_ = windowsName;
    }

    const std::vector<std::string>& GetWindowsName()
    {
        return windowsName_;
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

    GraphicLayerColor GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const std::vector<float>& GetCornerRadiusInfoForDRM() const
    {
        return drmCornerRadiusInfo_;
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
        isSupportedPresentTimestamp_ = isSupported;
    }

    void SetPresentTimestamp(const GraphicPresentTimestamp &timestamp)
    {
        presentTimestamp_ = timestamp;
    }

    float GetSdrNit() const
    {
        return sdrNit_;
    }

    float GetDisplayNit() const
    {
        return displayNit_;
    }

    float GetBrightnessRatio() const
    {
        return brightnessRatio_;
    }

    void SetSdrNit(float sdrNit)
    {
        sdrNit_ = sdrNit;
    }

    void SetDisplayNit(float displayNit)
    {
        displayNit_ = displayNit;
    }

    void SetBrightnessRatio(float brightnessRatio)
    {
        brightnessRatio_ = brightnessRatio;
    }

    const std::vector<float>& GetLayerLinearMatrix() const
    {
        return layerLinearMatrix_;
    }

    void SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix)
    {
        layerLinearMatrix_ = layerLinearMatrix;
    }

    // source crop tuning
    int32_t GetLayerSourceTuning() const
    {
        return layerSource_;
    }

    void SetLayerSourceTuning(int32_t layerSouce)
    {
        layerSource_ = layerSouce;
    }

    void SetRotationFixed(bool rotationFixed)
    {
        rotationFixed_ = rotationFixed;
    }

    bool GetRotationFixed() const
    {
        return rotationFixed_;
    }

    void SetLayerArsr(bool arsrTag)
    {
        arsrTag_ = arsrTag;
    }

    bool GetLayerArsr() const
    {
        return arsrTag_;
    }

    void SetLayerCopybit(bool copybitTag)
    {
        copybitTag_ = copybitTag;
    }

    bool GetLayerCopybit() const
    {
        return copybitTag_;
    }

    void SetNeedBilinearInterpolation(bool need)
    {
        needBilinearInterpolation_ = need;
    }

    bool GetNeedBilinearInterpolation() const
    {
        return needBilinearInterpolation_;
    }

    void SetIsMaskLayer(bool isMaskLayer)
    {
        isMaskLayer_ = isMaskLayer;
    }

    bool IsMaskLayer() const
    {
        return isMaskLayer_;
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
        sdrNit_ = layerInfo->GetSdrNit();
        displayNit_ = layerInfo->GetDisplayNit();
        brightnessRatio_ = layerInfo->GetBrightnessRatio();
        layerLinearMatrix_ = layerInfo->GetLayerLinearMatrix();
        layerSource_ = layerInfo->GetLayerSourceTuning();
        rotationFixed_ = layerInfo->GetRotationFixed();
        arsrTag_ = layerInfo->GetLayerArsr();
        copybitTag_ = layerInfo->GetLayerCopybit();
        needBilinearInterpolation_ = layerInfo->GetNeedBilinearInterpolation();
        tunnelLayerId_ = layerInfo->GetTunnelLayerId();
        tunnelLayerProperty_ = layerInfo->GetTunnelLayerProperty();
        ancoFlags_ = layerInfo->GetAncoFlags();
        ignoreAlpha_ = layerInfo->GetIgnoreAlpha();
    }

    RosenError SetLayerMaskInfo(LayerMask mask)
    {
        switch (mask) {
            case LayerMask::LAYER_MASK_NORMAL:
            case LayerMask::LAYER_MASK_HBM_SYNC:
                break;
            default:
                return ROSEN_ERROR_INVALID_ARGUMENTS;
        }

        layerMask_ = mask;
        return ROSEN_ERROR_OK;
    }

    LayerMask GetLayerMaskInfo()
    {
        return layerMask_;
    }

    inline uint64_t GetNodeId()
    {
        return nodeId_;
    }

    void SetNodeId(uint64_t nodeId)
    {
        nodeId_ = nodeId;
    }

    // only anco use these interfaces
    void SetAncoFlags(const uint32_t ancoFlags) { ancoFlags_ = ancoFlags; }
    uint32_t GetAncoFlags() const { return ancoFlags_; }
    bool IsAncoNative() const
    {
        constexpr uint32_t ANCO_NATIVE_NODE_FLAG = static_cast<uint32_t>(AncoFlags::ANCO_NATIVE_NODE);
        return (ancoFlags_ & ANCO_NATIVE_NODE_FLAG) == ANCO_NATIVE_NODE_FLAG;
    }
    void SetAncoSrcRect(const GraphicIRect& ancoSrcRect) { ancoSrcRect_ = ancoSrcRect; }
    const GraphicIRect& GetAncoSrcRect() const { return ancoSrcRect_; }

    // hpae offline: while creating layer, use srcRect & dstRect instead of bounds to create redraw metrix
    void SetUseDeviceOffline(bool useOffline) { useDeviceOffline_ = useOffline; }
    bool GetUseDeviceOffline() const { return useDeviceOffline_; }

    void SetIgnoreAlpha(bool ignoreAlpha)
    {
        ignoreAlpha_ = ignoreAlpha;
    }

    bool GetIgnoreAlpha()
    {
        return ignoreAlpha_;
    }
    /* hdiLayer get layer info end */

private:
    uint32_t zOrder_ = 0;
    GraphicLayerType layerType_ = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
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
    GraphicLayerColor backgroundColor_;
    GraphicColorDataSpace colorSpace_ = GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    std::vector<GraphicHDRMetaData> metaData_;
    GraphicHDRMetaDataSet metaDataSet_;
    sptr<SurfaceTunnelHandle> tunnelHandle_ = nullptr;
    std::vector<std::string> windowsName_;
    bool tunnelHandleChange_ = false;
    bool isSupportedPresentTimestamp_ = false;
    GraphicPresentTimestamp presentTimestamp_ = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};

    sptr<IConsumerSurface> cSurface_ = nullptr;
    sptr<SyncFence> acquireFence_ = SyncFence::InvalidFence();
    sptr<SurfaceBuffer> sbuffer_ = nullptr;
    sptr<SurfaceBuffer> pbuffer_ = nullptr;
    bool preMulti_ = false;
    bool needBilinearInterpolation_ = false;
    LayerMask layerMask_ = LayerMask::LAYER_MASK_NORMAL;
    mutable std::mutex mutex_;
    float sdrNit_ = 500.0f; // default sdr nit
    float displayNit_ = 500.0f; // default luminance for sdr
    float brightnessRatio_ = 1.0f; // default ratio for sdr
    std::vector<float> layerLinearMatrix_
        = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}; // matrix for linear colorspace
    uint64_t nodeId_ = 0;
    uint64_t tunnelLayerId_ = 0;
    uint32_t tunnelLayerProperty_ = 0;
    int32_t layerSource_ = 0; // default layer source tag
    bool rotationFixed_ = false;
    bool arsrTag_ = true;
    bool copybitTag_ = false;
    std::vector<float> drmCornerRadiusInfo_;
    bool isMaskLayer_ = false;
    uint32_t ancoFlags_ = 0;
    GraphicIRect ancoSrcRect_ {-1, -1, -1, -1};
    // hpae offline
    bool useDeviceOffline_ = false;
    bool ignoreAlpha_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_LAYER_INFO_H