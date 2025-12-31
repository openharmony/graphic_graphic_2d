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

#include "rs_render_surface_layer.h"
#include "rs_render_layer_cmd.h"

namespace OHOS {
namespace Rosen {
using RSLayerCmdHandler = std::function<void(std::shared_ptr<RSRenderSurfaceLayer>,
    const std::shared_ptr<Rosen::RSRenderLayerPropertyBase>&)>;
#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE)                                                            \
    {                                                                                                            \
        RSLayerCmdType::CMD_TYPE,                                                                                \
        [](std::shared_ptr<RSRenderSurfaceLayer> rsRenderLayer,                                                  \
            const std::shared_ptr<RSRenderLayerPropertyBase>& property) -> void {                                \
            std::shared_ptr<RSRenderLayerCmdProperty<TYPE>> prop =                                               \
                std::static_pointer_cast<RSRenderLayerCmdProperty<TYPE>>(property);                              \
            TYPE target = prop->Get();                                                                           \
            rsRenderLayer->Set##CMD_NAME(target);                                                                \
        },                                                                                                       \
    },

static std::unordered_map<RSLayerCmdType, RSLayerCmdHandler> cmdHandlers_ = {
#include "rs_layer_cmd_def.in"
};
#undef DECLARE_RSLAYER_CMD

RSRenderSurfaceLayer::RSRenderSurfaceLayer()
{
    RS_TRACE_NAME_FMT("Constructing RSRenderSurfaceLayer");
    RS_LOGD("Constructing RSRenderSurfaceLayer");
}

RSRenderSurfaceLayer::~RSRenderSurfaceLayer()
{
    RS_TRACE_NAME_FMT("RSRenderSurfaceLayer::~RSRenderSurfaceLayer id: %" PRIu64 ", name: %s",
        rsLayerId_, surfaceName_.c_str());
    RS_LOGD("Destructing RSRenderSurfaceLayer, id: %{public}" PRIu64 ", surface name: %{public}s",
        rsLayerId_, surfaceName_.c_str());
}

RSLayerId RSRenderSurfaceLayer::GetRSLayerId() const
{
    return rsLayerId_;
}

void RSRenderSurfaceLayer::SetRSLayerId(RSLayerId rsLayerId)
{
    rsLayerId_ = rsLayerId;
}

void RSRenderSurfaceLayer::SetAlpha(const GraphicLayerAlpha& alpha)
{
    layerAlpha_ = alpha;
}

const GraphicLayerAlpha& RSRenderSurfaceLayer::GetAlpha() const
{
    return layerAlpha_;
}

void RSRenderSurfaceLayer::SetZorder(int32_t zOrder)
{
    zOrder_ = zOrder;
}

uint32_t RSRenderSurfaceLayer::GetZorder() const
{
    return zOrder_;
}

void RSRenderSurfaceLayer::SetType(const GraphicLayerType layerType)
{
    layerType_ = layerType;
}

GraphicLayerType RSRenderSurfaceLayer::GetType() const
{
    return layerType_;
}

void RSRenderSurfaceLayer::SetTransform(GraphicTransformType type)
{
    transformType_ = type;
}

GraphicTransformType RSRenderSurfaceLayer::GetTransform() const
{
    return transformType_;
}

void RSRenderSurfaceLayer::SetCompositionType(GraphicCompositionType type)
{
    compositionType_ = type;
}

GraphicCompositionType RSRenderSurfaceLayer::GetCompositionType() const
{
    return compositionType_;
}

void RSRenderSurfaceLayer::SetVisibleRegions(const std::vector<GraphicIRect>& visibleRegions)
{
    visibleRegions_ = visibleRegions;
}

const std::vector<GraphicIRect>& RSRenderSurfaceLayer::GetVisibleRegions() const
{
    return visibleRegions_;
}

void RSRenderSurfaceLayer::SetDirtyRegions(const std::vector<GraphicIRect>& dirtyRegions)
{
    dirtyRegions_ = dirtyRegions;
}

const std::vector<GraphicIRect>& RSRenderSurfaceLayer::GetDirtyRegions() const
{
    return dirtyRegions_;
}

void RSRenderSurfaceLayer::SetBlendType(GraphicBlendType type)
{
    blendType_ = type;
}

GraphicBlendType RSRenderSurfaceLayer::GetBlendType() const
{
    return blendType_;
}

void RSRenderSurfaceLayer::SetCropRect(const GraphicIRect &crop)
{
    cropRect_ = crop;
}

const GraphicIRect& RSRenderSurfaceLayer::GetCropRect() const
{
    return cropRect_;
}

void RSRenderSurfaceLayer::SetPreMulti(bool preMulti)
{
    preMulti_ = preMulti;
}

bool RSRenderSurfaceLayer::IsPreMulti() const
{
    return preMulti_;
}

void RSRenderSurfaceLayer::SetLayerSize(const GraphicIRect& layerRect)
{
    layerRect_ = layerRect;
}

const GraphicIRect& RSRenderSurfaceLayer::GetLayerSize() const
{
    return layerRect_;
}

void RSRenderSurfaceLayer::SetBoundSize(const GraphicIRect& boundRect)
{
    boundRect_ = boundRect;
}

const GraphicIRect& RSRenderSurfaceLayer::GetBoundSize() const
{
    return boundRect_;
}

void RSRenderSurfaceLayer::SetLayerColor(GraphicLayerColor layerColor)
{
    layerColor_ = layerColor;
}

const GraphicLayerColor& RSRenderSurfaceLayer::GetLayerColor() const
{
    return layerColor_;
}

void RSRenderSurfaceLayer::SetBackgroundColor(GraphicLayerColor backgroundColor)
{
    backgroundColor_ = backgroundColor;
}

const GraphicLayerColor& RSRenderSurfaceLayer::GetBackgroundColor() const
{
    return backgroundColor_;
}

void RSRenderSurfaceLayer::SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo)
{
    drmCornerRadiusInfo_ = drmCornerRadiusInfo;
}

const std::vector<float>& RSRenderSurfaceLayer::GetCornerRadiusInfoForDRM() const
{
    return drmCornerRadiusInfo_;
}

void RSRenderSurfaceLayer::SetColorTransform(const std::vector<float>& matrix)
{
    colorTransformMatrix_ = matrix;
}

const std::vector<float>& RSRenderSurfaceLayer::GetColorTransform() const
{
    return colorTransformMatrix_;
}

void RSRenderSurfaceLayer::SetColorDataSpace(GraphicColorDataSpace colorSpace)
{
    colorSpace_ = colorSpace;
}

GraphicColorDataSpace RSRenderSurfaceLayer::GetColorDataSpace() const
{
    return colorSpace_;
}

void RSRenderSurfaceLayer::SetMetaData(const std::vector<GraphicHDRMetaData>& metaData)
{
    metaData_ = metaData;
}

const std::vector<GraphicHDRMetaData>& RSRenderSurfaceLayer::GetMetaData() const
{
    return metaData_;
}

void RSRenderSurfaceLayer::SetMetaDataSet(const GraphicHDRMetaDataSet& metaDataSet)
{
    metaDataSet_ = metaDataSet;
}

const GraphicHDRMetaDataSet& RSRenderSurfaceLayer::GetMetaDataSet() const
{
    return metaDataSet_;
}

void RSRenderSurfaceLayer::SetMatrix(const GraphicMatrix& matrix)
{
    matrix_ = matrix;
}

const GraphicMatrix& RSRenderSurfaceLayer::GetMatrix() const
{
    return matrix_;
}

void RSRenderSurfaceLayer::SetGravity(int32_t gravity)
{
    gravity_ = gravity;
}

int32_t RSRenderSurfaceLayer::GetGravity() const
{
    return gravity_;
}

void RSRenderSurfaceLayer::SetUniRenderFlag(bool isUniRender)
{
    isUniRender_ = isUniRender;
}

bool RSRenderSurfaceLayer::GetUniRenderFlag() const
{
    return isUniRender_;
}

void RSRenderSurfaceLayer::SetTunnelHandleChange(bool change)
{
    tunnelHandleChange_ = change;
}

bool RSRenderSurfaceLayer::GetTunnelHandleChange() const
{
    return tunnelHandleChange_;
}

void RSRenderSurfaceLayer::SetTunnelHandle(const sptr<SurfaceTunnelHandle>& handle)
{
    tunnelHandle_ = handle;
}

sptr<SurfaceTunnelHandle> RSRenderSurfaceLayer::GetTunnelHandle() const
{
    return tunnelHandle_;
}

void RSRenderSurfaceLayer::SetTunnelLayerId(const uint64_t tunnelLayerId)
{
    tunnelLayerId_ = tunnelLayerId;
}

uint64_t RSRenderSurfaceLayer::GetTunnelLayerId() const
{
    return tunnelLayerId_;
}

void RSRenderSurfaceLayer::SetTunnelLayerProperty(uint32_t tunnelLayerProperty)
{
    tunnelLayerProperty_ = tunnelLayerProperty;
}

uint32_t RSRenderSurfaceLayer::GetTunnelLayerProperty() const
{
    return tunnelLayerProperty_;
}

void RSRenderSurfaceLayer::SetIsSupportedPresentTimestamp(bool isSupported)
{
    isSupportedPresentTimestamp_ = isSupported;
}

bool RSRenderSurfaceLayer::GetIsSupportedPresentTimestamp() const
{
    return isSupportedPresentTimestamp_;
}

void RSRenderSurfaceLayer::SetPresentTimestamp(const GraphicPresentTimestamp& timestamp)
{
    presentTimestamp_ = timestamp;
}

const GraphicPresentTimestamp& RSRenderSurfaceLayer::GetPresentTimestamp() const
{
    return presentTimestamp_;
}

void RSRenderSurfaceLayer::SetSdrNit(float sdrNit)
{
    sdrNit_ = sdrNit;
}

float RSRenderSurfaceLayer::GetSdrNit() const
{
    return sdrNit_;
}

void RSRenderSurfaceLayer::SetDisplayNit(float displayNit)
{
    displayNit_ = displayNit;
}

float RSRenderSurfaceLayer::GetDisplayNit() const
{
    return displayNit_;
}

void RSRenderSurfaceLayer::SetBrightnessRatio(float brightnessRatio)
{
    brightnessRatio_ = brightnessRatio;
}

float RSRenderSurfaceLayer::GetBrightnessRatio() const
{
    return brightnessRatio_;
}

void RSRenderSurfaceLayer::SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix)
{
    layerLinearMatrix_ = layerLinearMatrix;
}

const std::vector<float>& RSRenderSurfaceLayer::GetLayerLinearMatrix() const
{
    return layerLinearMatrix_;
}

void RSRenderSurfaceLayer::SetLayerSourceTuning(int32_t layerSource)
{
    layerSource_ = layerSource;
}

int32_t RSRenderSurfaceLayer::GetLayerSourceTuning() const
{
    return layerSource_;
}

void RSRenderSurfaceLayer::SetWindowsName(std::vector<std::string>& windowsName)
{
    windowsName_ = windowsName;
}

const std::vector<std::string>& RSRenderSurfaceLayer::GetWindowsName() const
{
    return windowsName_;
}

void RSRenderSurfaceLayer::SetRotationFixed(bool rotationFixed)
{
    rotationFixed_ = rotationFixed;
}

bool RSRenderSurfaceLayer::GetRotationFixed() const
{
    return rotationFixed_;
}

void RSRenderSurfaceLayer::SetLayerArsr(bool arsrTag)
{
    arsrTag_ = arsrTag;
}

bool RSRenderSurfaceLayer::GetLayerArsr() const
{
    return arsrTag_;
}

void RSRenderSurfaceLayer::SetLayerCopybit(bool copybitTag)
{
    copybitTag_ = copybitTag;
}

bool RSRenderSurfaceLayer::GetLayerCopybit() const
{
    return copybitTag_;
}

void RSRenderSurfaceLayer::SetNeedBilinearInterpolation(bool need)
{
    needBilinearInterpolation_ = need;
}

bool RSRenderSurfaceLayer::GetNeedBilinearInterpolation() const
{
    return needBilinearInterpolation_;
}

void RSRenderSurfaceLayer::SetIsMaskLayer(bool isMaskLayer)
{
    isMaskLayer_ = isMaskLayer;
}

bool RSRenderSurfaceLayer::GetIsMaskLayer() const
{
    return isMaskLayer_;
}

void RSRenderSurfaceLayer::SetNodeId(uint64_t nodeId)
{
    nodeId_ = nodeId;
}

inline uint64_t RSRenderSurfaceLayer::GetNodeId() const
{
    return nodeId_;
}

void RSRenderSurfaceLayer::SetAncoFlags(uint32_t ancoFlags)
{
    ancoFlags_ = ancoFlags;
}

uint32_t RSRenderSurfaceLayer::GetAncoFlags() const
{
    return ancoFlags_;
}

// to-do
bool RSRenderSurfaceLayer::IsAncoNative() const
{
    constexpr uint32_t ANCO_NATIVE_NODE_FLAG = static_cast<uint32_t>(AncoFlags::ANCO_NATIVE_NODE);
    return (ancoFlags_ & ANCO_NATIVE_NODE_FLAG) == ANCO_NATIVE_NODE_FLAG;
}

void RSRenderSurfaceLayer::SetLayerMaskInfo(LayerMask mask)
{
    layerMask_ = mask;
}

LayerMask RSRenderSurfaceLayer::GetLayerMaskInfo() const
{
    return layerMask_;
}

void RSRenderSurfaceLayer::SetSurface(const sptr<IConsumerSurface>& surface)
{
    // only used for separate rendering
    cSurface_ = surface;
}

sptr<IConsumerSurface> RSRenderSurfaceLayer::GetSurface() const
{
    return cSurface_;
}

void RSRenderSurfaceLayer::SetSurfaceUniqueId(uint64_t uniqueId)
{
    surfaceUniqueId_ = uniqueId;
}

uint64_t RSRenderSurfaceLayer::GetSurfaceUniqueId() const
{
    return surfaceUniqueId_;
}

void RSRenderSurfaceLayer::SetBuffer(const sptr<SurfaceBuffer>& sbuffer, const sptr<SyncFence>& acquireFence)
{
    sbuffer_ = sbuffer;
    acquireFence_ = acquireFence;
}

void RSRenderSurfaceLayer::SetBuffer(const sptr<SurfaceBuffer>& sbuffer)
{
    sbuffer_ = sbuffer;
}

sptr<SurfaceBuffer> RSRenderSurfaceLayer::GetBuffer() const
{
    return sbuffer_;
}

void RSRenderSurfaceLayer::SetPreBuffer(const sptr<SurfaceBuffer>& buffer)
{
    pbuffer_ = buffer;
}

sptr<SurfaceBuffer> RSRenderSurfaceLayer::GetPreBuffer() const
{
    return pbuffer_;
}

void RSRenderSurfaceLayer::SetAcquireFence(const sptr<SyncFence>& acquireFence)
{
    acquireFence_ = acquireFence;
}

sptr<SyncFence> RSRenderSurfaceLayer::GetAcquireFence() const
{
    return acquireFence_;
}

void RSRenderSurfaceLayer::SetCycleBuffersNum(uint32_t cycleBuffersNum)
{
    cycleBuffersNum_ = cycleBuffersNum;
}

uint32_t RSRenderSurfaceLayer::GetCycleBuffersNum() const
{
    return cycleBuffersNum_;
}

void RSRenderSurfaceLayer::SetSurfaceName(std::string surfaceName)
{
    surfaceName_ = surfaceName;
}

std::string RSRenderSurfaceLayer::GetSurfaceName() const
{
    return surfaceName_;
}

void RSRenderSurfaceLayer::SetIsNeedComposition(bool isNeedComposition)
{
    isNeedComposition_ = isNeedComposition;
}

bool RSRenderSurfaceLayer::GetIsNeedComposition() const
{
    return isNeedComposition_;
}

void RSRenderSurfaceLayer::SetSolidColorLayerProperty(GraphicSolidColorLayerProperty solidColorLayerProperty)
{
    solidColorLayerProperty_ = solidColorLayerProperty;
}

GraphicSolidColorLayerProperty RSRenderSurfaceLayer::GetSolidColorLayerProperty() const
{
    return solidColorLayerProperty_;
}

void RSRenderSurfaceLayer::SetUseDeviceOffline(bool useOffline)
{
    useDeviceOffline_ = useOffline;
}

bool RSRenderSurfaceLayer::GetUseDeviceOffline() const
{
    return useDeviceOffline_;
}

void RSRenderSurfaceLayer::SetIgnoreAlpha(bool ignoreAlpha)
{
    ignoreAlpha_ = ignoreAlpha;
}

bool RSRenderSurfaceLayer::GetIgnoreAlpha() const
{
    return ignoreAlpha_;
}

void RSRenderSurfaceLayer::SetAncoSrcRect(const GraphicIRect& ancoSrcRect)
{
    ancoSrcRect_ = ancoSrcRect;
}

const GraphicIRect& RSRenderSurfaceLayer::GetAncoSrcRect() const
{
    return ancoSrcRect_;
}

void RSRenderSurfaceLayer::CopyLayerInfo(const std::shared_ptr<RSLayer>& rsLayer)
{
    rsLayerId_ = rsLayer->GetRSLayerId();
    zOrder_ = rsLayer->GetZorder();
    layerType_ = rsLayer->GetType();
    layerRect_ = rsLayer->GetLayerSize();
    boundRect_ = rsLayer->GetBoundSize();
    visibleRegions_ = rsLayer->GetVisibleRegions();
    dirtyRegions_ = rsLayer->GetDirtyRegions();
    cropRect_ = rsLayer->GetCropRect();
    matrix_ = rsLayer->GetMatrix();
    gravity_ = rsLayer->GetGravity();
    layerAlpha_ = rsLayer->GetAlpha();
    transformType_ = rsLayer->GetTransform();
    compositionType_ = rsLayer->GetCompositionType();
    blendType_ = rsLayer->GetBlendType();
    colorTransformMatrix_ = rsLayer->GetColorTransform();
    colorSpace_ = rsLayer->GetColorDataSpace();
    layerColor_ = rsLayer->GetLayerColor();
    backgroundColor_ = rsLayer->GetBackgroundColor();
    drmCornerRadiusInfo_ = rsLayer->GetCornerRadiusInfoForDRM();
    isUniRender_ = rsLayer->GetUniRenderFlag();
    metaData_ = rsLayer->GetMetaData();
    metaDataSet_ = rsLayer->GetMetaDataSet();
    tunnelHandle_ = rsLayer->GetTunnelHandle();
    tunnelHandleChange_ = rsLayer->GetTunnelHandleChange();
    surfaceUniqueId_ = rsLayer->GetSurfaceUniqueId();
    sbuffer_ = rsLayer->GetBuffer();
    pbuffer_= rsLayer->GetPreBuffer();
    acquireFence_ = rsLayer->GetAcquireFence();
    preMulti_ = rsLayer->IsPreMulti();
    sdrNit_ = rsLayer->GetSdrNit();
    displayNit_ = rsLayer->GetDisplayNit();
    brightnessRatio_ = rsLayer->GetBrightnessRatio();
    layerLinearMatrix_ = rsLayer->GetLayerLinearMatrix();
    layerSource_ = rsLayer->GetLayerSourceTuning();
    rotationFixed_ = rsLayer->GetRotationFixed();
    arsrTag_ = rsLayer->GetLayerArsr();
    copybitTag_ = rsLayer->GetLayerCopybit();
    needBilinearInterpolation_ = rsLayer->GetNeedBilinearInterpolation();
    tunnelLayerId_ = rsLayer->GetTunnelLayerId();
    tunnelLayerProperty_ = rsLayer->GetTunnelLayerProperty();
    isSupportedPresentTimestamp_ = rsLayer->GetIsSupportedPresentTimestamp();
    presentTimestamp_ = rsLayer->GetPresentTimestamp();
    windowsName_ = rsLayer->GetWindowsName();
    ancoFlags_ = rsLayer->GetAncoFlags();
    isMaskLayer_ = rsLayer->GetIsMaskLayer();
    nodeId_ = rsLayer->GetNodeId();
    cSurface_ = rsLayer->GetSurface();
    layerMask_ = rsLayer->GetLayerMaskInfo();
    cycleBuffersNum_ = rsLayer->GetCycleBuffersNum();
    surfaceName_ = rsLayer->GetSurfaceName();
    solidColorLayerProperty_ = rsLayer->GetSolidColorLayerProperty();
    isNeedComposition_ = rsLayer->GetIsNeedComposition();
    useDeviceOffline_ = rsLayer->GetUseDeviceOffline();
    ignoreAlpha_ = rsLayer->GetIgnoreAlpha();
    ancoSrcRect_ = rsLayer->GetAncoSrcRect();
}

void RSRenderSurfaceLayer::UpdateRSLayerCmd(const std::shared_ptr<RSRenderLayerCmd>& command)
{
    auto type = command->GetRSRenderLayerCmdType();
    auto property = command->GetRSRenderLayerProperty();
    std::shared_ptr<RSRenderSurfaceLayer> rsRenderLayer =
        std::static_pointer_cast<RSRenderSurfaceLayer>(shared_from_this());
    if (rsRenderLayer == nullptr) {
        return;
    }
    auto it = cmdHandlers_.find(type);
    if (it != cmdHandlers_.end()) {
        it->second(rsRenderLayer, property);
    } else {
        ROSEN_LOGE("RSRenderSurfaceLayer::UpdateRSLayerCmd type err:%{public}d", static_cast<int32_t>(type));
    }
    ROSEN_LOGD("RSRenderSurfaceLayer::UpdateRSLayerCmd type:%{public}d", static_cast<int32_t>(type));
}
} // namespace Rosen
} // namespace OHOS
