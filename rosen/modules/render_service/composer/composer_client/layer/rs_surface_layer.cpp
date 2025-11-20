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


#include <memory>
#include "rs_layer_context.h"
#include "rs_surface_layer.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSLayer> RSSurfaceLayer::CreateLayer(const std::shared_ptr<RSRenderComposerClient>& client)
{
    if (client == nullptr) {
        RS_LOGE("RSSurfaceLayer::CreateLayer client is nullptr");
        return nullptr;
    }
    std::shared_ptr<RSLayer> layer = std::make_shared<RSSurfaceLayer>();
    client->AddLayer(layer);
    return layer;
}

void RSSurfaceLayer::SetAlpha(const GraphicLayerAlpha& alpha)
{
    layerAlpha_ = alpha;
}

const GraphicLayerAlpha& RSSurfaceLayer::GetAlpha() const
{
    return layerAlpha_;
}

void RSSurfaceLayer::SetZorder(int32_t zOrder)
{
    zOrder_ = static_cast<uint32_t>(zOrder);
}

uint32_t RSSurfaceLayer::GetZorder() const
{
    return zOrder_;
}

void RSSurfaceLayer::SetType(const GraphicLayerType layerType)
{
    layerType_ = layerType;
}

GraphicLayerType RSSurfaceLayer::GetType() const
{
    return layerType_;
}

void RSSurfaceLayer::SetTransform(GraphicTransformType type)
{
    transformType_ = type;
}

GraphicTransformType RSSurfaceLayer::GetTransformType() const
{
    return transformType_;
}

void RSSurfaceLayer::SetCompositionType(GraphicCompositionType type)
{
    compositionType_ = type;
}

GraphicCompositionType RSSurfaceLayer::GetCompositionType() const
{
    return compositionType_;
}

void RSSurfaceLayer::SetVisibleRegions(const std::vector<GraphicIRect>& visibleRegions)
{
    visibleRegions_ = visibleRegions;
}

const std::vector<GraphicIRect>& RSSurfaceLayer::GetVisibleRegions() const
{
    return visibleRegions_;
}

void RSSurfaceLayer::SetDirtyRegions(const std::vector<GraphicIRect>& dirtyRegions)
{
    dirtyRegions_ = dirtyRegions;
}

const std::vector<GraphicIRect>& RSSurfaceLayer::GetDirtyRegions() const
{
    return dirtyRegions_;
}

void RSSurfaceLayer::SetBlendType(GraphicBlendType type)
{
    blendType_ = type;
}

GraphicBlendType RSSurfaceLayer::GetBlendType() const
{
    return blendType_;
}

void RSSurfaceLayer::SetCropRect(const GraphicIRect& crop)
{
    cropRect_ = crop;
}

const GraphicIRect& RSSurfaceLayer::GetCropRect() const
{
    return cropRect_;
}

void RSSurfaceLayer::SetPreMulti(bool preMulti)
{
    preMulti_ = preMulti;
}

bool RSSurfaceLayer::IsPreMulti() const
{
    return preMulti_;
}

void RSSurfaceLayer::SetLayerSize(const GraphicIRect &layerRect)
{
    layerRect_ = layerRect;
}

const GraphicIRect& RSSurfaceLayer::GetLayerSize() const
{
    return layerRect_;
}

void RSSurfaceLayer::SetBoundSize(const GraphicIRect& boundRect)
{
    boundRect_ = boundRect;
}

const GraphicIRect& RSSurfaceLayer::GetBoundSize() const
{
    return boundRect_;
}

void RSSurfaceLayer::SetLayerColor(GraphicLayerColor layerColor)
{
    layerColor_ = layerColor;
}

const GraphicLayerColor& RSSurfaceLayer::GetLayerColor() const
{
    return layerColor_;
}

void RSSurfaceLayer::SetBackgroundColor(GraphicLayerColor backgroundColor)
{
    backgroundColor_ = backgroundColor;
}

const GraphicLayerColor& RSSurfaceLayer::GetBackgroundColor() const
{
    return backgroundColor_;
}

void RSSurfaceLayer::SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo)
{
    drmCornerRadiusInfo_ = drmCornerRadiusInfo;
}

const std::vector<float>& RSSurfaceLayer::GetCornerRadiusInfoForDRM() const
{
    return drmCornerRadiusInfo_;
}

void RSSurfaceLayer::SetColorTransform(const std::vector<float>& matrix)
{
    colorTransformMatrix_ = matrix;
}

const std::vector<float> &RSSurfaceLayer::GetColorTransform() const
{
    return colorTransformMatrix_;
}

void RSSurfaceLayer::SetColorDataSpace(GraphicColorDataSpace colorSpace)
{
    colorSpace_ = colorSpace;
}

GraphicColorDataSpace RSSurfaceLayer::GetColorDataSpace() const
{
    return colorSpace_;
}

void RSSurfaceLayer::SetMetaData(const std::vector<GraphicHDRMetaData>& metaData)
{
    metaData_ = metaData;
}

const std::vector<GraphicHDRMetaData>& RSSurfaceLayer::GetMetaData() const
{
    return metaData_;
}

void RSSurfaceLayer::SetMetaDataSet(const GraphicHDRMetaDataSet& metaDataSet)
{
    metaDataSet_ = metaDataSet;
}

const GraphicHDRMetaDataSet& RSSurfaceLayer::GetMetaDataSet() const
{
    return metaDataSet_;
}

void RSSurfaceLayer::SetMatrix(GraphicMatrix matrix)
{
    matrix_ = matrix;
}

const GraphicMatrix& RSSurfaceLayer::GetMatrix() const
{
    return matrix_;
}

void RSSurfaceLayer::SetGravity(int32_t gravity)
{
    gravity_ = gravity;
}

int32_t RSSurfaceLayer::GetGravity() const
{
    return gravity_;
}

void RSSurfaceLayer::SetUniRenderFlag(bool isUniRender)
{
    isUniRender_ = isUniRender;
}

bool RSSurfaceLayer::GetUniRenderFlag() const
{
    return isUniRender_;
}

void RSSurfaceLayer::SetTunnelHandleChange(bool change)
{
    tunnelHandleChange_ = change;
}

bool RSSurfaceLayer::GetTunnelHandleChange() const
{
    return tunnelHandleChange_;
}

void RSSurfaceLayer::SetTunnelHandle(const sptr<SurfaceTunnelHandle>& handle)
{
    tunnelHandle_ = handle;
}

sptr<SurfaceTunnelHandle> RSSurfaceLayer::GetTunnelHandle() const
{
    return tunnelHandle_;
}

void RSSurfaceLayer::SetTunnelLayerId(const uint64_t tunnelLayerId)
{
    tunnelLayerId_ = tunnelLayerId;
}

uint64_t RSSurfaceLayer::GetTunnelLayerId() const
{
    return tunnelLayerId_;
}

void RSSurfaceLayer::SetTunnelLayerProperty(uint32_t tunnelLayerProperty)
{
    tunnelLayerProperty_ = tunnelLayerProperty;
}

uint32_t RSSurfaceLayer::GetTunnelLayerProperty() const
{
    return tunnelLayerProperty_;
}

void RSSurfaceLayer::SetPresentTimestamp(const GraphicPresentTimestamp& timestamp)
{
    presentTimestamp_ = timestamp;
}

const GraphicPresentTimestamp& RSSurfaceLayer::GetPresentTimestamp() const
{
    return presentTimestamp_;
}

void RSSurfaceLayer::SetIsSupportedPresentTimestamp(bool isSupported)
{
    isSupportedPresentTimestamp_ = isSupported;
}

bool RSSurfaceLayer::IsSupportedPresentTimestamp() const
{
    return isSupportedPresentTimestamp_;
}

void RSSurfaceLayer::SetSdrNit(float sdrNit)
{
    sdrNit_ = sdrNit;
}

float RSSurfaceLayer::GetSdrNit() const
{
    return sdrNit_;
}

void RSSurfaceLayer::SetDisplayNit(float displayNit)
{
    displayNit_ = displayNit;
}

float RSSurfaceLayer::GetDisplayNit() const
{
    return displayNit_;
}

void RSSurfaceLayer::SetBrightnessRatio(float brightnessRatio)
{
    brightnessRatio_ = brightnessRatio;
}

float RSSurfaceLayer::GetBrightnessRatio() const
{
    return brightnessRatio_;
}

void RSSurfaceLayer::SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix)
{
    layerLinearMatrix_ = layerLinearMatrix;
}

const std::vector<float>& RSSurfaceLayer::GetLayerLinearMatrix() const
{
    return layerLinearMatrix_;
}

void RSSurfaceLayer::SetLayerSourceTuning(int32_t layerSource)
{
    layerSource_ = layerSource;
}

int32_t RSSurfaceLayer::GetLayerSourceTuning() const
{
    return layerSource_;
}

void RSSurfaceLayer::SetWindowsName(std::vector<std::string>& windowsName)
{
    windowsName_ = windowsName;
}

const std::vector<std::string>& RSSurfaceLayer::GetWindowsName() const
{
    return windowsName_;
}

void RSSurfaceLayer::SetRotationFixed(bool rotationFixed)
{
    rotationFixed_ = rotationFixed;
}

bool RSSurfaceLayer::GetRotationFixed() const
{
    return rotationFixed_;
}

void RSSurfaceLayer::SetLayerArsr(bool arsrTag)
{
    arsrTag_ = arsrTag;
}

bool RSSurfaceLayer::GetLayerArsr() const
{
    return arsrTag_;
}

void RSSurfaceLayer::SetLayerCopybit(bool copybitTag)
{
    copybitTag_ = copybitTag;
}

bool RSSurfaceLayer::GetLayerCopybit() const
{
    return copybitTag_;
}

void RSSurfaceLayer::SetNeedBilinearInterpolation(bool need)
{
    needBilinearInterpolation_ = need;
}

bool RSSurfaceLayer::GetNeedBilinearInterpolation() const
{
    return needBilinearInterpolation_;
}

void RSSurfaceLayer::SetIsMaskLayer(bool isMaskLayer)
{
    isMaskLayer_ = isMaskLayer;
}

bool RSSurfaceLayer::IsMaskLayer() const
{
    return isMaskLayer_;
}

void RSSurfaceLayer::SetNodeId(uint64_t nodeId)
{
    nodeId_ = nodeId;
}

uint64_t RSSurfaceLayer::GetNodeId() const
{
    return nodeId_;
}

void RSSurfaceLayer::SetAncoFlags(const uint32_t ancoFlags)
{
    ancoFlags_ = ancoFlags;
}

uint32_t RSSurfaceLayer::GetAncoFlags() const
{
    return ancoFlags_;
}

bool RSSurfaceLayer::IsAncoNative() const
{
    static constexpr uint32_t ANCO_NATIVE_NODE_FLAG = static_cast<uint32_t>(AncoFlags::ANCO_NATIVE_NODE);
    return (ancoFlags_ & ANCO_NATIVE_NODE_FLAG) == ANCO_NATIVE_NODE_FLAG;
}

void RSSurfaceLayer::SetLayerMaskInfo(LayerMask mask)
{
    layerMask_ = mask;
}

LayerMask RSSurfaceLayer::GetLayerMaskInfo() const
{
    return layerMask_;
}

void RSSurfaceLayer::SetSurface(const sptr<IConsumerSurface>& surface)
{
    cSurface_ = surface;
}

sptr<IConsumerSurface> RSSurfaceLayer::GetSurface() const
{
    return cSurface_;
}

void RSSurfaceLayer::SetBuffer(const sptr<SurfaceBuffer>& sbuffer, const sptr<SyncFence>& acquireFence)
{
    sbuffer_ = sbuffer;
    acquireFence_ = acquireFence;
}

void RSSurfaceLayer::SetBuffer(const sptr<SurfaceBuffer>& sbuffer)
{
    sbuffer_ = sbuffer;
}

sptr<SurfaceBuffer> RSSurfaceLayer::GetBuffer() const
{
    return sbuffer_;
}

void RSSurfaceLayer::SetPreBuffer(const sptr<SurfaceBuffer>& buffer)
{
    pbuffer_ = buffer;
}

sptr<SurfaceBuffer> RSSurfaceLayer::GetPreBuffer() const
{
    return pbuffer_;
}

void RSSurfaceLayer::SetAcquireFence(const sptr<SyncFence>& acquireFence)
{
    acquireFence_ = acquireFence;
}

sptr<SyncFence> RSSurfaceLayer::GetAcquireFence() const
{
    return acquireFence_;
}

void RSSurfaceLayer::SetUseDeviceOffline(bool useOffline)
{
    useDeviceOffline_ = useOffline;
}

bool RSSurfaceLayer::GetUseDeviceOffline() const
{
    return useDeviceOffline_;
}

void RSSurfaceLayer::SetIgnoreAlpha(bool ignoreAlpha)
{
    ignoreAlpha_ = ignoreAlpha;
}

bool RSSurfaceLayer::GetIgnoreAlpha() const
{
    return ignoreAlpha_;
}

void RSSurfaceLayer::SetAncoSrcRect(const GraphicIRect& ancoSrcRect)
{
    ancoSrcRect_ = ancoSrcRect;
}

const GraphicIRect& RSSurfaceLayer::GetAncoSrcRect() const
{
    return ancoSrcRect_;
}

void RSSurfaceLayer::CopyLayerInfo(const std::shared_ptr<RSLayer>& rsLayer)
{
    zOrder_ = rsLayer->GetZorder();
    layerType_ = rsLayer->GetType();
    layerRect_ = rsLayer->GetLayerSize();
    boundRect_ = rsLayer->GetBoundSize();
    visibleRegions_ = rsLayer->GetVisibleRegions();
    dirtyRegions_ = rsLayer->GetDirtyRegions();
    cropRect_ = rsLayer->GetCropRect();
    matrix_ = rsLayer->GetMatrix();
    gravity_ = rsLayer->GetGravity();
    isUniRender_ =rsLayer->GetUniRenderFlag();
    layerAlpha_ = rsLayer->GetAlpha();
    transformType_ = rsLayer->GetTransformType();
    compositionType_ = rsLayer->GetCompositionType();
    blendType_ = rsLayer->GetBlendType();
    colorTransformMatrix_ = rsLayer->GetColorTransform();
    colorSpace_ = rsLayer->GetColorDataSpace();
    backgroundColor_ = rsLayer->GetBackgroundColor();
    layerColor_ = rsLayer->GetLayerColor();
    metaData_ = rsLayer->GetMetaData();
    metaDataSet_ = rsLayer->GetMetaDataSet();
    tunnelHandle_ = rsLayer->GetTunnelHandle();
    windowsName_ = rsLayer->GetWindowsName();
    tunnelHandleChange_ = rsLayer->GetTunnelHandleChange();
    isSupportedPresentTimestamp_ = rsLayer->IsSupportedPresentTimestamp();
    presentTimestamp_ = rsLayer->GetPresentTimestamp();
    cSurface_ = rsLayer->GetSurface();
    sbuffer_ = rsLayer->GetBuffer();
    pbuffer_= rsLayer->GetPreBuffer();
    acquireFence_ = rsLayer->GetAcquireFence();
    preMulti_ = rsLayer->IsPreMulti();
    layerMask_ = rsLayer->GetLayerMaskInfo();
    sdrNit_ = rsLayer->GetSdrNit();
    displayNit_ = rsLayer->GetDisplayNit();
    brightnessRatio_ = rsLayer->GetBrightnessRatio();
    layerLinearMatrix_ = rsLayer->GetLayerLinearMatrix();
    nodeId_ = rsLayer->GetNodeId();
    layerSource_ = rsLayer->GetLayerSourceTuning();
    rotationFixed_ = rsLayer->GetRotationFixed();
    arsrTag_ = rsLayer->GetLayerArsr();
    copybitTag_ = rsLayer->GetLayerCopybit();
    drmCornerRadiusInfo_ = rsLayer->GetCornerRadiusInfoForDRM();
    needBilinearInterpolation_ = rsLayer->GetNeedBilinearInterpolation();
    isMaskLayer_ = rsLayer->IsMaskLayer();
    tunnelLayerId_ = rsLayer->GetTunnelLayerId();
    tunnelLayerProperty_ = rsLayer->GetTunnelLayerProperty();
    ancoFlags_ = rsLayer->GetAncoFlags();
    useDeviceOffline_ = rsLayer->GetUseDeviceOffline();
    ignoreAlpha_ = rsLayer->GetIgnoreAlpha();
    ancoSrcRect_ = rsLayer->GetAncoSrcRect();
}

void RSSurfaceLayer::Dump(std::string& result) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto transformTypeStrsIter = TransformTypeStrs.find(transformType_);
    auto compositionTypeStrsIter = CompositionTypeStrs.find(compositionType_);
    auto blendTypeStrsIter = BlendTypeStrs.find(blendType_);
    if (transformTypeStrsIter != TransformTypeStrs.end() &&
        compositionTypeStrsIter != CompositionTypeStrs.end() &&
        blendTypeStrsIter != BlendTypeStrs.end()) {
        result += " zOrder = " + std::to_string(zOrder_) +
            ", visibleNum = " + std::to_string(visibleRegions_.size()) +
            ", transformType = " + transformTypeStrsIter->second +
            ", compositionType = " + compositionTypeStrsIter->second +
            ", blendType = " + blendTypeStrsIter->second +
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
    result += "layerColor = [R:" + std::to_string(layerColor_.r) + ", G:" +
        std::to_string(layerColor_.g) + ", B:" +
        std::to_string(layerColor_.b) + ", A:" +
        std::to_string(layerColor_.a) + "],";
    if (cSurface_ != nullptr) {
        cSurface_->Dump(result);
    }
    result += " displayNit = " + std::to_string(displayNit_) +
        ", brightnessRatio = " + std::to_string(brightnessRatio_) + ", ";
    result += " ancoFlags = " + std::to_string(ancoFlags_) + ", ";
}

void RSSurfaceLayer::DumpCurrentFrameLayer() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (cSurface_ != nullptr) {
        cSurface_->DumpCurrentFrameLayer();
    }
}

} // namespace Rosen
} // namespace OHOS
