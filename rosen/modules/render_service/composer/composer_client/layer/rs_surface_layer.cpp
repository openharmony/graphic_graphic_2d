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

#include "rs_surface_layer.h"
#include <memory>
#include "rs_composer_context.h"
#include "rs_layer_parcel.h"
#include "rs_surface_layer_parcel.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSLayer> RSSurfaceLayer::Create(const std::shared_ptr<RSComposerContext>& context,
    RSLayerId rsLayerId)
{
    if (context == nullptr) {
        RS_LOGE("RSSurfaceLayer::Create context is nullptr");
        return nullptr;
    }
    std::shared_ptr<RSLayer> layer = context->GetRSLayer(rsLayerId);
    if (layer != nullptr) {
        RS_TRACE_NAME_FMT("RSSurfaceLayer::Create use exist layer, id: %" PRIu64 ", name: %s",
            rsLayerId, layer->GetSurfaceName().c_str());
        RS_LOGD("RSSurfaceLayer::Create get cache layer by layer id: %{public}" PRIu64, rsLayerId);
        layer->SetRSLayerId(rsLayerId);
        return layer;
    }
    layer = std::make_shared<RSSurfaceLayer>(rsLayerId, context);
    context->AddRSLayer(layer);
    return layer;
}

std::shared_ptr<RSComposerContext> RSSurfaceLayer::GetComposerContext() const
{
    return rsComposerContext_.lock();
}

void RSSurfaceLayer::SetComposerContext(std::shared_ptr<RSComposerContext> rsComposerContext)
{
    rsComposerContext_ = rsComposerContext;
}

RSSurfaceLayer::RSSurfaceLayer(RSLayerId rsLayerId, std::shared_ptr<RSComposerContext> rsComposerContext)
{
    RS_TRACE_NAME_FMT("RSSurfaceLayer::RSSurfaceLayer id: %" PRIu64, rsLayerId);
    ROSEN_LOGI("Constructing RSSurfaceLayer, id: %{public}" PRIu64, rsLayerId);
    rsLayerId_ = rsLayerId;
    rsComposerContext_ = rsComposerContext;

    SetRSLayerCmd<RSLayerRSLayerIdCmd>(rsLayerId);
}

RSSurfaceLayer::~RSSurfaceLayer()
{
    RS_TRACE_NAME_FMT("RSSurfaceLayer::~RSSurfaceLayer id: %" PRIu64 ", name %s",
        rsLayerId_, surfaceName_.c_str());
    ROSEN_LOGI("Destructing RSSurfaceLayer, id: %{public}" PRIu64 ", name %{public}s",
        rsLayerId_, surfaceName_.c_str());
    auto destroyCmd = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
    auto renderLayerCmd = std::make_shared<RSRenderLayerPreMultiCmd>(destroyCmd);
    std::shared_ptr<RSLayerParcel> layerParcel =
        std::make_shared<RSDestroyRSLayerCmd>(rsLayerId_, renderLayerCmd);

    bool success = AddRSLayerParcel(layerParcel, rsLayerId_);
    if (!success) {
        ROSEN_LOGE("RSSurfaceLayer::~RSSurfaceLayer failed to send destroy command, layerId: %{public}" PRIu64, GetRSLayerId());
    } else {
        ROSEN_LOGI("RSSurfaceLayer::~RSSurfaceLayer destroy command sent successfully, layerId: %{public}" PRIu64, GetRSLayerId());
    }
    auto context = rsComposerContext_.lock();
    if (context) {
        context->RemoveRSLayer(rsLayerId_);
    }
}

void RSSurfaceLayer::SetRSLayerId(RSLayerId rsLayerId)
{
    rsLayerId_ = rsLayerId;
    SetRSLayerCmd<RSLayerRSLayerIdCmd>(rsLayerId);
}

RSLayerId RSSurfaceLayer::GetRSLayerId() const
{
    return rsLayerId_;
}

bool RSSurfaceLayer::AddRSLayerParcel(std::shared_ptr<RSLayerParcel>& layerParcel, RSLayerId rsLayerId)
{
    auto rsComposerContext = rsComposerContext_.lock();
    if (!rsComposerContext) {
        return false;
    }
    auto rsLayerTransaction = rsComposerContext->GetRSLayerTransaction();
    if (rsLayerTransaction != nullptr) {
        rsLayerTransaction->AddRSLayerParcel(layerParcel, rsLayerId);
        return true;
    }
    return false;
}

void RSSurfaceLayer::AddRSLayerCmd(const std::shared_ptr<RSLayerCmd> layerCmd)
{
    if (layerCmd->GetRSLayerCmdType() == RSLayerCmdType::LAYER_CMD) {
        return;
    }
    std::shared_ptr<RSLayerParcel> layerParcel =
        std::make_shared<RSUpdateRSLayerCmd>(GetRSLayerId(), layerCmd->CreateRenderLayerCmd());
    AddRSLayerParcel(layerParcel, GetRSLayerId());
}

template<typename RSLayerCmdName, typename T>
void RSSurfaceLayer::SetRSLayerCmd(const T& value)
{
    auto rsLayerCmd = std::make_shared<RSLayerCmdName>(value);
    AddRSLayerCmd(rsLayerCmd);
}

void RSSurfaceLayer::SetAlpha(const GraphicLayerAlpha& alpha)
{
    bool isSame = (layerAlpha_.enGlobalAlpha == alpha.enGlobalAlpha &&
        layerAlpha_.enPixelAlpha == alpha.enPixelAlpha &&
        layerAlpha_.alpha0 == alpha.alpha0 && layerAlpha_.alpha1 == alpha.alpha1 &&
        layerAlpha_.gAlpha == alpha.gAlpha);
    if (isSame) {
        return;
    }
    layerAlpha_ = alpha;
    SetRSLayerCmd<RSLayerAlphaCmd>(alpha);
}

const GraphicLayerAlpha& RSSurfaceLayer::GetAlpha() const
{
    return layerAlpha_;
}

bool RSSurfaceLayer::GetIsNeedComposition() const
{
    return isNeedComposition_;
}

void RSSurfaceLayer::SetIsNeedComposition(bool isNeedComposition)
{
    /* Each setting needs to be re-deliverd */
    isNeedComposition_ = isNeedComposition;
    SetRSLayerCmd<RSLayerIsNeedCompositionCmd>(isNeedComposition);
}

void RSSurfaceLayer::SetZorder(int32_t zOrder)
{
    if (zOrder_ == zOrder) {
        return;
    }
    zOrder_ = zOrder;
    SetRSLayerCmd<RSLayerZorderCmd>(zOrder);
}

uint32_t RSSurfaceLayer::GetZorder() const
{
    return zOrder_;
}

void RSSurfaceLayer::SetType(const GraphicLayerType layerType)
{
    if (layerType_ == layerType) {
        return;
    }
    layerType_ = layerType;
    SetRSLayerCmd<RSLayerTypeCmd>(layerType);
}

GraphicLayerType RSSurfaceLayer::GetType() const
{
    return layerType_;
}

void RSSurfaceLayer::SetTransform(GraphicTransformType type)
{
    if (transformType_ == type) {
        return;
    }
    transformType_ = type;
    SetRSLayerCmd<RSLayerTransformCmd>(type);
}

GraphicTransformType RSSurfaceLayer::GetTransformType() const
{
    return transformType_;
}

void RSSurfaceLayer::SetCompositionType(GraphicCompositionType type)
{
    if (compositionType_ == type) {
        return;
    }
    compositionType_ = type;
    SetRSLayerCmd<RSLayerCompositionTypeCmd>(type);
}

GraphicCompositionType RSSurfaceLayer::GetCompositionType() const
{
    return compositionType_;
}

void RSSurfaceLayer::SetVisibleRegions(const std::vector<GraphicIRect>& visibleRegions)
{
    if (visibleRegions_ == visibleRegions) {
        return;
    }
    visibleRegions_ = visibleRegions;
    SetRSLayerCmd<RSLayerVisibleRegionsCmd>(visibleRegions);
}

const std::vector<GraphicIRect> &RSSurfaceLayer::GetVisibleRegions() const
{
    return visibleRegions_;
}

void RSSurfaceLayer::SetDirtyRegions(const std::vector<GraphicIRect>& dirtyRegions)
{
    if (dirtyRegions_ == dirtyRegions) {
        return;
    }
    dirtyRegions_ = dirtyRegions;
    SetRSLayerCmd<RSLayerDirtyRegionsCmd>(dirtyRegions);
}

const std::vector<GraphicIRect>& RSSurfaceLayer::GetDirtyRegions() const
{
    return dirtyRegions_;
}

void RSSurfaceLayer::SetBlendType(GraphicBlendType type)
{
    if (blendType_ == type) {
        return;
    }
    blendType_ = type;
    SetRSLayerCmd<RSLayerBlendTypeCmd>(type);
}

GraphicBlendType RSSurfaceLayer::GetBlendType() const
{
    return blendType_;
}

void RSSurfaceLayer::SetCropRect(const GraphicIRect& crop)
{
    if (cropRect_ == crop) {
        return;
    }
    cropRect_ = crop;
    SetRSLayerCmd<RSLayerCropRectCmd>(crop);
}

const GraphicIRect& RSSurfaceLayer::GetCropRect() const
{
    return cropRect_;
}

void RSSurfaceLayer::SetPreMulti(bool preMulti)
{
    if (preMulti_ == preMulti) {
        return;
    }
    preMulti_ = preMulti;
    SetRSLayerCmd<RSLayerPreMultiCmd>(preMulti);
}

bool RSSurfaceLayer::IsPreMulti() const
{
    return preMulti_;
}

void RSSurfaceLayer::SetLayerSize(const GraphicIRect& layerRect)
{
    if (layerRect_ == layerRect) {
        return;
    }
    layerRect_ = layerRect;
    SetRSLayerCmd<RSLayerLayerSizeCmd>(layerRect);
}

const GraphicIRect& RSSurfaceLayer::GetLayerSize() const
{
    return layerRect_;
}

void RSSurfaceLayer::SetBoundSize(const GraphicIRect& boundRect)
{
    if (boundRect_ == boundRect) {
        return;
    }
    boundRect_ = boundRect;
    SetRSLayerCmd<RSLayerBoundSizeCmd>(boundRect);
}

const GraphicIRect& RSSurfaceLayer::GetBoundSize() const
{
    return boundRect_;
}

void RSSurfaceLayer::SetLayerColor(GraphicLayerColor layerColor)
{
    if (layerColor_ == layerColor) {
        return;
    }
    layerColor_ = layerColor;
    SetRSLayerCmd<RSLayerLayerColorCmd>(layerColor);
}

const GraphicLayerColor& RSSurfaceLayer::GetLayerColor() const
{
    return layerColor_;
}

void RSSurfaceLayer::SetBackgroundColor(GraphicLayerColor backgroundColor)
{
    if (backgroundColor_ == backgroundColor) {
        return;
    }
    backgroundColor_ = backgroundColor;
    SetRSLayerCmd<RSLayerBackgroundColorCmd>(backgroundColor);
}

const GraphicLayerColor& RSSurfaceLayer::GetBackgroundColor() const
{
    return backgroundColor_;
}

void RSSurfaceLayer::SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo)
{
    if (drmCornerRadiusInfo_ == drmCornerRadiusInfo) {
        return;
    }
    drmCornerRadiusInfo_ = drmCornerRadiusInfo;
    SetRSLayerCmd<RSLayerCornerRadiusInfoForDRMCmd>(drmCornerRadiusInfo);
}

const std::vector<float>& RSSurfaceLayer::GetCornerRadiusInfoForDRM() const
{
    return drmCornerRadiusInfo_;
}

void RSSurfaceLayer::SetColorTransform(const std::vector<float> &matrix)
{
    if (colorTransformMatrix_ == matrix) {
        return;
    }
    colorTransformMatrix_ = matrix;
    SetRSLayerCmd<RSLayerColorTransformCmd>(matrix);
}

const std::vector<float>& RSSurfaceLayer::GetColorTransform() const
{
    return colorTransformMatrix_;
}

void RSSurfaceLayer::SetColorDataSpace(GraphicColorDataSpace colorSpace)
{
    if (colorSpace_ == colorSpace) {
        return;
    }
    colorSpace_ = colorSpace;
    SetRSLayerCmd<RSLayerColorDataSpaceCmd>(colorSpace);
}

GraphicColorDataSpace RSSurfaceLayer::GetColorDataSpace() const
{
    return colorSpace_;
}

void RSSurfaceLayer::SetMetaData(const std::vector<GraphicHDRMetaData> &metaData)
{
    if (metaData_ == metaData) {
        return;
    }
    metaData_ = metaData;
    SetRSLayerCmd<RSLayerMetaDataCmd>(metaData);
}

const std::vector<GraphicHDRMetaData>& RSSurfaceLayer::GetMetaData() const
{
    return metaData_;
}

void RSSurfaceLayer::SetMetaDataSet(const GraphicHDRMetaDataSet &metaDataSet)
{
    if (metaDataSet_ == metaDataSet) {
        return;
    }
    metaDataSet_ = metaDataSet;
    SetRSLayerCmd<RSLayerMetaDataSetCmd>(metaDataSet);
}

const GraphicHDRMetaDataSet& RSSurfaceLayer::GetMetaDataSet() const
{
    return metaDataSet_;
}

void RSSurfaceLayer::SetMatrix(GraphicMatrix matrix)
{
    if (matrix_ == matrix) {
        return;
    }
    matrix_ = matrix;
    SetRSLayerCmd<RSLayerMatrixCmd>(matrix);
}

const GraphicMatrix& RSSurfaceLayer::GetMatrix() const
{
    return matrix_;
}

void RSSurfaceLayer::SetGravity(int32_t gravity)
{
    if (gravity_ == gravity) {
        return;
    }
    gravity_ = gravity;
    SetRSLayerCmd<RSLayerGravityCmd>(gravity);
}

int32_t RSSurfaceLayer::GetGravity() const
{
    return gravity_;
}

void RSSurfaceLayer::SetUniRenderFlag(bool isUniRender)
{
    if (isUniRender_ == isUniRender) {
        return;
    }
    isUniRender_ = isUniRender;
    SetRSLayerCmd<RSLayerUniRenderFlagCmd>(isUniRender);
}

bool RSSurfaceLayer::GetUniRenderFlag() const
{
    return isUniRender_;
}

void RSSurfaceLayer::SetTunnelHandleChange(bool change)
{
    if (tunnelHandleChange_ == change) {
        return;
    }
    tunnelHandleChange_ = change;
    SetRSLayerCmd<RSLayerTunnelHandleChangeCmd>(change);
}

bool RSSurfaceLayer::GetTunnelHandleChange() const
{
    return tunnelHandleChange_;
}

void RSSurfaceLayer::SetTunnelHandle(const sptr<SurfaceTunnelHandle>& handle)
{
    if (tunnelHandle_ == handle) {
        return;
    }
    tunnelHandle_ = handle;
    SetRSLayerCmd<RSLayerTunnelHandleCmd>(handle);
}

sptr<SurfaceTunnelHandle> RSSurfaceLayer::GetTunnelHandle() const
{
    return tunnelHandle_;
}

void RSSurfaceLayer::SetTunnelLayerId(const uint64_t tunnelLayerId)
{
    if (tunnelLayerId_ == tunnelLayerId) {
        return;
    }
    tunnelLayerId_ = tunnelLayerId;
    SetRSLayerCmd<RSLayerTunnelLayerIdCmd>(tunnelLayerId);
}

uint64_t RSSurfaceLayer::GetTunnelLayerId() const
{
    return tunnelLayerId_;
}

void RSSurfaceLayer::SetTunnelLayerProperty(uint32_t tunnelLayerProperty)
{
    if (tunnelLayerProperty_ == tunnelLayerProperty) {
        return;
    }
    tunnelLayerProperty_ = tunnelLayerProperty;
    SetRSLayerCmd<RSLayerTunnelLayerPropertyCmd>(tunnelLayerProperty);
}

uint32_t RSSurfaceLayer::GetTunnelLayerProperty() const
{
    return tunnelLayerProperty_;
}

void RSSurfaceLayer::SetPresentTimestamp(const GraphicPresentTimestamp& timestamp)
{
    if (presentTimestamp_ == timestamp) {
        return;
    }
    presentTimestamp_ = timestamp;
    SetRSLayerCmd<RSLayerPresentTimestampCmd>(timestamp);
}

const GraphicPresentTimestamp& RSSurfaceLayer::GetPresentTimestamp() const
{
    return presentTimestamp_;
}

void RSSurfaceLayer::SetIsSupportedPresentTimestamp(bool isSupported)
{
    if (isSupportedPresentTimestamp_ == isSupported) {
        return;
    }
    isSupportedPresentTimestamp_ = isSupported;
    SetRSLayerCmd<RSLayerIsSupportedPresentTimestampCmd>(isSupported);
}

bool RSSurfaceLayer::IsSupportedPresentTimestamp() const
{
    return isSupportedPresentTimestamp_;
}

void RSSurfaceLayer::SetSdrNit(float sdrNit)
{
    if (sdrNit_ == sdrNit) {
        return;
    }
    sdrNit_ = sdrNit;
    SetRSLayerCmd<RSLayerSdrNitCmd>(sdrNit);
}

float RSSurfaceLayer::GetSdrNit() const
{
    return sdrNit_;
}

void RSSurfaceLayer::SetDisplayNit(float displayNit)
{
    if (displayNit_ == displayNit) {
        return;
    }
    displayNit_ = displayNit;
    SetRSLayerCmd<RSLayerDisplayNitCmd>(displayNit);
}

float RSSurfaceLayer::GetDisplayNit() const
{
    return displayNit_;
}

void RSSurfaceLayer::SetBrightnessRatio(float brightnessRatio)
{
    if (brightnessRatio_ == brightnessRatio) {
        return;
    }
    brightnessRatio_ = brightnessRatio;
    SetRSLayerCmd<RSLayerBrightnessRatioCmd>(brightnessRatio);
}

float RSSurfaceLayer::GetBrightnessRatio() const
{
    return brightnessRatio_;
}

void RSSurfaceLayer::SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix)
{
    if (layerLinearMatrix_ == layerLinearMatrix) {
        return;
    }
    layerLinearMatrix_ = layerLinearMatrix;
    SetRSLayerCmd<RSLayerLayerLinearMatrixCmd>(layerLinearMatrix);
}

const std::vector<float>& RSSurfaceLayer::GetLayerLinearMatrix() const
{
    return layerLinearMatrix_;
}

void RSSurfaceLayer::SetLayerSourceTuning(int32_t layerSource)
{
    if (layerSource_ == layerSource) {
        return;
    }
    layerSource_ = layerSource;
    SetRSLayerCmd<RSLayerLayerSourceTuningCmd>(layerSource);
}

int32_t RSSurfaceLayer::GetLayerSourceTuning() const
{
    return layerSource_;
}

void RSSurfaceLayer::SetWindowsName(std::vector<std::string>& windowsName)
{
    if (windowsName_ == windowsName) {
        return;
    }
    windowsName_ = windowsName;
    SetRSLayerCmd<RSLayerWindowsNameCmd>(windowsName);
}

const std::vector<std::string>& RSSurfaceLayer::GetWindowsName() const
{
    return windowsName_;
}

void RSSurfaceLayer::SetRotationFixed(bool rotationFixed)
{
    if (rotationFixed_ == rotationFixed) {
        return;
    }
    rotationFixed_ = rotationFixed;
    SetRSLayerCmd<RSLayerRotationFixedCmd>(rotationFixed);
}

bool RSSurfaceLayer::GetRotationFixed() const
{
    return rotationFixed_;
}

void RSSurfaceLayer::SetLayerArsr(bool arsrTag)
{
    if (arsrTag_ == arsrTag) {
        return;
    }
    arsrTag_ = arsrTag;
    SetRSLayerCmd<RSLayerLayerArsrCmd>(arsrTag);
}

bool RSSurfaceLayer::GetLayerArsr() const
{
    return arsrTag_;
}

void RSSurfaceLayer::SetLayerCopybit(bool copybitTag)
{
    if (copybitTag_ == copybitTag) {
        return;
    }
    copybitTag_ = copybitTag;
    SetRSLayerCmd<RSLayerLayerCopybitCmd>(copybitTag);
}

bool RSSurfaceLayer::GetLayerCopybit() const
{
    return copybitTag_;
}

void RSSurfaceLayer::SetNeedBilinearInterpolation(bool need)
{
    if (needBilinearInterpolation_ == need) {
        return;
    }
    needBilinearInterpolation_ = need;
    SetRSLayerCmd<RSLayerNeedBilinearInterpolationCmd>(need);
}

bool RSSurfaceLayer::GetNeedBilinearInterpolation() const
{
    return needBilinearInterpolation_;
}

void RSSurfaceLayer::SetIsMaskLayer(bool isMaskLayer)
{
    if (isMaskLayer_ == isMaskLayer) {
        return;
    }
    isMaskLayer_ = isMaskLayer;
    SetRSLayerCmd<RSLayerIsMaskLayerCmd>(isMaskLayer);
}

bool RSSurfaceLayer::IsMaskLayer() const
{
    return isMaskLayer_;
}

void RSSurfaceLayer::SetNodeId(uint64_t nodeId)
{
    if (nodeId_ == nodeId) {
        return;
    }
    nodeId_ = nodeId;
    SetRSLayerCmd<RSLayerNodeIdCmd>(nodeId);
}

uint64_t RSSurfaceLayer::GetNodeId() const
{
    return nodeId_;
}

void RSSurfaceLayer::SetAncoFlags(const uint32_t ancoFlags)
{
    if (ancoFlags_ == ancoFlags) {
        return;
    }
    ancoFlags_ = ancoFlags;
    SetRSLayerCmd<RSLayerAncoFlagsCmd>(ancoFlags);
}

uint32_t RSSurfaceLayer::GetAncoFlags() const
{
    return ancoFlags_;
}

bool RSSurfaceLayer::IsAncoNative() const
{
    return false;
}

void RSSurfaceLayer::SetLayerMaskInfo(LayerMask mask)
{
    if (mask == layerMask_) {
        return;
    }

    layerMask_ = mask;
    SetRSLayerCmd<RSLayerLayerMaskInfoCmd>(mask);
    return;
}

LayerMask RSSurfaceLayer::GetLayerMaskInfo() const
{
    return layerMask_;
}

void RSSurfaceLayer::SetSurface(const sptr<IConsumerSurface> &surface)
{
    cSurface_ = surface;
}

sptr<IConsumerSurface> RSSurfaceLayer::GetSurface() const
{
    return cSurface_;
}

uint64_t RSSurfaceLayer::GetSurfaceUniqueId() const
{
    return surfaceUniqueId_;
}

void RSSurfaceLayer::SetSurfaceUniqueId(uint64_t uniqueId)
{
    if (uniqueId == surfaceUniqueId_) {
        return;
    }
    surfaceUniqueId_ = uniqueId;
    SetRSLayerCmd<RSLayerSurfaceUniqueIdCmd>(uniqueId);
}

void RSSurfaceLayer::SetBuffer(const sptr<SurfaceBuffer> &sbuffer, const sptr<SyncFence> &acquireFence)
{
    sbuffer_ = sbuffer;
    acquireFence_ = acquireFence;
    SetRSLayerCmd<RSLayerBufferCmd>(sbuffer);
    SetRSLayerCmd<RSLayerAcquireFenceCmd>(acquireFence);
}

void RSSurfaceLayer::SetBuffer(const sptr<SurfaceBuffer>& sbuffer)
{
    sbuffer_ = sbuffer;
    SetRSLayerCmd<RSLayerBufferCmd>(sbuffer);
}

sptr<SurfaceBuffer> RSSurfaceLayer::GetBuffer() const
{
    return sbuffer_;
}

void RSSurfaceLayer::SetPreBuffer(const sptr<SurfaceBuffer> &buffer)
{
    if (buffer == pbuffer_) {
        return;
    }
    pbuffer_ = buffer;
    SetRSLayerCmd<RSLayerPreBufferCmd>(buffer);
}

sptr<SurfaceBuffer> RSSurfaceLayer::GetPreBuffer() const
{
    return pbuffer_;
}

void RSSurfaceLayer::SetAcquireFence(const sptr<SyncFence>& acquireFence)
{
    if (acquireFence == acquireFence_) {
        return;
    }
    acquireFence_ = acquireFence;
    SetRSLayerCmd<RSLayerAcquireFenceCmd>(acquireFence);
}

sptr<SyncFence> RSSurfaceLayer::GetAcquireFence() const
{
    return acquireFence_;
}

void RSSurfaceLayer::SetCycleBuffersNum(uint32_t cycleBuffersNum)
{
    if (cycleBuffersNum_ == cycleBuffersNum) {
        return;
    }
    cycleBuffersNum_ = cycleBuffersNum;
    SetRSLayerCmd<RSLayerCycleBuffersNumCmd>(cycleBuffersNum);
}

uint32_t RSSurfaceLayer::GetCycleBuffersNum() const
{
    return cycleBuffersNum_;
}

void RSSurfaceLayer::SetSurfaceName(std::string surfaceName)
{
    if (surfaceName_ == surfaceName) {
        return;
    }
    surfaceName_ = surfaceName;
    SetRSLayerCmd<RSLayerSurfaceNameCmd>(surfaceName);
}

std::string RSSurfaceLayer::GetSurfaceName() const
{
    return surfaceName_;
}

void RSSurfaceLayer::SetSolidColorLayerProperty(GraphicSolidColorLayerProperty solidColorLayerProperty)
{
    /* Each setting needs to be re-deliverd */
    solidColorLayerProperty_ = solidColorLayerProperty;
    SetRSLayerCmd<RSLayerSolidColorLayerPropertyCmd>(solidColorLayerProperty);
}

GraphicSolidColorLayerProperty RSSurfaceLayer::GetSolidColorLayerProperty() const
{
    return solidColorLayerProperty_;
}

void RSSurfaceLayer::SetUseDeviceOffline(bool useOffline)
{
    if (useDeviceOffline_ == useOffline) {
        return;
    }
    useDeviceOffline_ = useOffline;
    SetRSLayerCmd<RSLayerUseDeviceOfflineCmd>(useOffline);
}

bool RSSurfaceLayer::GetUseDeviceOffline() const
{
    return useDeviceOffline_;
}

void RSSurfaceLayer::SetIgnoreAlpha(bool ignoreAlpha)
{
    if (ignoreAlpha_ == ignoreAlpha) {
        return;
    }
    ignoreAlpha_ = ignoreAlpha;
    SetRSLayerCmd<RSLayerIgnoreAlphaCmd>(ignoreAlpha);
}

bool RSSurfaceLayer::GetIgnoreAlpha() const
{
    return ignoreAlpha_;
}

void RSSurfaceLayer::SetAncoSrcRect(const GraphicIRect& ancoSrcRect)
{
    if (ancoSrcRect_ == ancoSrcRect) {
        return;
    }
    ancoSrcRect_ = ancoSrcRect;
    SetRSLayerCmd<RSLayerAncoSrcRectCmd>(ancoSrcRect);
}

const GraphicIRect& RSSurfaceLayer::GetAncoSrcRect() const
{
    return ancoSrcRect_;
}

void RSSurfaceLayer::Dump(std::string& result) const
{
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
    result += " cycleBuffersNum = " + std::to_string(cycleBuffersNum_) + ", ";
    result += "surfaceName = " + surfaceName_ + ", ";
    result += "solidColorLayer Composition type = " + std::to_string(solidColorLayerProperty_.compositionType) +
        " Zorder = " + std::to_string(solidColorLayerProperty_.zOrder) + ", ";
    result += "useDeviceOffline" + std::to_string(useDeviceOffline_) + ", ";
    result += "ignoreAlpha" + std::to_string(ignoreAlpha_) + ", ";
    result += "ancoSrcRect = [" + std::to_string(ancoSrcRect_.x) + ", " + std::to_string(ancoSrcRect_.y) + ", " +
        std::to_string(ancoSrcRect_.w) + ", " + std::to_string(ancoSrcRect_.h) + "]" + ";";
}

void RSSurfaceLayer::DumpCurrentFrameLayer() const
{
    if (cSurface_ != nullptr) {
        cSurface_->DumpCurrentFrameLayer();
    }
}
void RSSurfaceLayer::SetBufferOwnerCount(std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount)
{
    if (bufferOwnerCount == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lockGuard(ownerCountMutex_);
    if (bufferOwnerCounts_.find(bufferOwnerCount->seqNum_) == bufferOwnerCounts_.end()) {
        bufferOwnerCount->AddRef();
    }
    bufferOwnerCounts_[bufferOwnerCount->seqNum_] = bufferOwnerCount;
    bufferOwnerCount_ = bufferOwnerCount;
}

std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> RSSurfaceLayer::GetSeqNumFromBufferOwnerCounts(uint64_t seqNum)
{
    std::lock_guard<std::mutex> lockGuard(ownerCountMutex_);
    auto iter = bufferOwnerCounts_.find(seqNum);
    if (iter != bufferOwnerCounts_.end()) {
        auto bufferOwnerCount = iter->second;
        bufferOwnerCounts_.erase(iter);
        return bufferOwnerCount;
    }
    return nullptr;
}

std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> RSSurfaceLayer::GetBufferOwnerCount() const
{
    std::lock_guard<std::mutex> lockGuard(ownerCountMutex_);
    return bufferOwnerCount_;
}

} // namespace Rosen
} // namespace OHOS
