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

#ifndef RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_H
#define RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_H

#include "common/rs_anco_type.h"
#include "common/rs_macros.h"
#include "iconsumer_surface.h"
#include "surface.h"
#include "sync_fence.h"
#include "hdi_display_type.h"
#include "rs_layer_common_def.h"

#include "pipeline/rs_surface_handler.h"

namespace OHOS {
namespace Rosen {
class RSLayerContext;
class RSRenderLayerCmd;

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
    {GRAPHIC_COMPOSITION_CLIENT,             "0 <client composition>"},
    {GRAPHIC_COMPOSITION_DEVICE,             "1 <device composition>"},
    {GRAPHIC_COMPOSITION_CURSOR,             "2 <cursor composition>"},
    {GRAPHIC_COMPOSITION_VIDEO,              "3 <video composition>"},
    {GRAPHIC_COMPOSITION_DEVICE_CLEAR,       "4 <device clear composition>"},
    {GRAPHIC_COMPOSITION_CLIENT_CLEAR,       "5 <client clear composition>"},
    {GRAPHIC_COMPOSITION_TUNNEL,             "6 <tunnel composition>"},
    {GRAPHIC_COMPOSITION_SOLID_COLOR,        "7 <layercolor composition>"},
    {GRAPHIC_COMPOSITION_BUTT,               "8 <uninitialized>"},
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

class RSB_EXPORT RSLayer : public std::enable_shared_from_this<RSLayer> {
public:
    virtual ~RSLayer() = default;

    virtual void SetAlpha(const GraphicLayerAlpha& alpha) = 0;
    virtual const GraphicLayerAlpha& GetAlpha() const = 0;
    virtual void SetZorder(int32_t zOrder) = 0;
    virtual uint32_t GetZorder() const = 0;
    virtual void SetType(const GraphicLayerType layerType) = 0;
    virtual GraphicLayerType GetType() const = 0;
    virtual void SetTransform(GraphicTransformType type) = 0;
    virtual GraphicTransformType GetTransformType() const = 0;
    virtual void SetCompositionType(GraphicCompositionType type) = 0;
    virtual GraphicCompositionType GetCompositionType() const = 0;
    virtual void SetVisibleRegions(const std::vector<GraphicIRect>& visibleRegions) = 0;
    virtual const std::vector<GraphicIRect>& GetVisibleRegions() const = 0;
    virtual void SetDirtyRegions(const std::vector<GraphicIRect>& dirtyRegions) = 0;
    virtual const std::vector<GraphicIRect>& GetDirtyRegions() const = 0;
    virtual void SetBlendType(GraphicBlendType type) = 0;
    virtual GraphicBlendType GetBlendType() const = 0;
    virtual void SetCropRect(const GraphicIRect& crop) = 0;
    virtual const GraphicIRect& GetCropRect() const = 0;
    virtual void SetPreMulti(bool preMulti) = 0;
    virtual bool IsPreMulti() const = 0;
    virtual void SetLayerSize(const GraphicIRect& layerRect) = 0;
    virtual const GraphicIRect& GetLayerSize() const = 0;
    virtual void SetBoundSize(const GraphicIRect& boundRect) = 0;
    virtual const GraphicIRect& GetBoundSize() const = 0;
    virtual void SetLayerColor(GraphicLayerColor layerColor) = 0;
    virtual const GraphicLayerColor& GetLayerColor() const = 0;
    virtual void SetBackgroundColor(GraphicLayerColor backgroundColor) = 0;
    virtual const GraphicLayerColor& GetBackgroundColor() const = 0;
    virtual void SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo) = 0;
    virtual const std::vector<float>& GetCornerRadiusInfoForDRM() const = 0;
    virtual void SetColorTransform(const std::vector<float>& matrix) = 0;
    virtual const std::vector<float>& GetColorTransform() const = 0;
    virtual void SetColorDataSpace(GraphicColorDataSpace colorSpace) = 0;
    virtual GraphicColorDataSpace GetColorDataSpace() const = 0;
    virtual void SetMetaData(const std::vector<GraphicHDRMetaData>& metaData) = 0;
    virtual const std::vector<GraphicHDRMetaData>& GetMetaData() const = 0;
    virtual void SetMetaDataSet(const GraphicHDRMetaDataSet& metaDataSet) = 0;
    virtual const GraphicHDRMetaDataSet& GetMetaDataSet() const = 0;
    virtual void SetMatrix(GraphicMatrix matrix) = 0;
    virtual const GraphicMatrix& GetMatrix() const = 0;
    virtual void SetGravity(int32_t gravity) = 0;
    virtual int32_t GetGravity() const = 0;
    virtual void SetUniRenderFlag(bool isUniRender) = 0;
    virtual bool GetUniRenderFlag() const = 0;
    virtual void SetTunnelHandleChange(bool change) = 0;
    virtual bool GetTunnelHandleChange() const = 0;
    virtual void SetTunnelHandle(const sptr<SurfaceTunnelHandle>& handle) = 0;
    virtual sptr<SurfaceTunnelHandle> GetTunnelHandle() const = 0;
    virtual void SetTunnelLayerId(const uint64_t tunnelLayerId) = 0;
    virtual uint64_t GetTunnelLayerId() const = 0;
    virtual void SetTunnelLayerProperty(uint32_t tunnelLayerProperty) = 0;
    virtual uint32_t GetTunnelLayerProperty() const = 0;
    virtual void SetIsSupportedPresentTimestamp(bool isSupported) = 0;
    virtual bool IsSupportedPresentTimestamp() const = 0;
    virtual void SetPresentTimestamp(const GraphicPresentTimestamp& timestamp) = 0;
    virtual const GraphicPresentTimestamp& GetPresentTimestamp() const = 0;
    virtual void SetSdrNit(float sdrNit) = 0;
    virtual float GetSdrNit() const = 0;
    virtual void SetDisplayNit(float displayNit) = 0;
    virtual float GetDisplayNit() const = 0;
    virtual void SetBrightnessRatio(float brightnessRatio) = 0;
    virtual float GetBrightnessRatio() const = 0;
    virtual void SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix) = 0;
    virtual const std::vector<float>& GetLayerLinearMatrix() const = 0;
    virtual void SetLayerSourceTuning(int32_t layerSource) = 0;
    virtual int32_t GetLayerSourceTuning() const = 0;
    virtual void SetWindowsName(std::vector<std::string>& windowsName) = 0;
    virtual const std::vector<std::string>& GetWindowsName() const= 0;
    virtual void SetRotationFixed(bool rotationFixed) = 0;
    virtual bool GetRotationFixed() const = 0;
    virtual void SetLayerArsr(bool arsrTag) = 0;
    virtual bool GetLayerArsr() const = 0;
    virtual void SetLayerCopybit(bool copybitTag) = 0;
    virtual bool GetLayerCopybit() const = 0;
    virtual void SetNeedBilinearInterpolation(bool need) = 0;
    virtual bool GetNeedBilinearInterpolation() const = 0;
    virtual void SetIsMaskLayer(bool isMaskLayer) = 0;
    virtual bool IsMaskLayer() const = 0;
    virtual void SetNodeId(uint64_t nodeId) = 0;
    virtual uint64_t GetNodeId() const = 0;
    virtual void SetAncoFlags(const uint32_t ancoFlags) = 0;
    virtual uint32_t GetAncoFlags() const = 0;
    virtual bool IsAncoNative() const = 0;
    virtual void SetLayerMaskInfo(LayerMask mask) = 0;
    virtual LayerMask GetLayerMaskInfo() const = 0;

    virtual void SetSurface(const sptr<IConsumerSurface>& surface) = 0;
    virtual sptr<IConsumerSurface> GetSurface() const = 0;
    virtual void SetBuffer(const sptr<SurfaceBuffer>& sbuffer, const sptr<SyncFence>& acquireFence) = 0;
    virtual void SetBuffer(const sptr<SurfaceBuffer>& sbuffer) = 0;
    virtual sptr<SurfaceBuffer> GetBuffer() const = 0;
    virtual void SetPreBuffer(const sptr<SurfaceBuffer>& buffer) = 0;
    virtual sptr<SurfaceBuffer> GetPreBuffer() const = 0;
    virtual void SetAcquireFence(const sptr<SyncFence>& acquireFence) = 0;
    virtual sptr<SyncFence> GetAcquireFence() const = 0;
    virtual void SetUseDeviceOffline(bool useOffline) = 0;
    virtual bool GetUseDeviceOffline() const = 0;
    virtual void SetIgnoreAlpha(bool ignoreAlpha) = 0;
    virtual bool GetIgnoreAlpha() const = 0;
    virtual void SetAncoSrcRect(const GraphicIRect& ancoSrcRect) = 0;
    virtual const GraphicIRect& GetAncoSrcRect() const = 0;

    virtual void CopyLayerInfo(const std::shared_ptr<RSLayer>& rsLayer) = 0;
    virtual void Dump(std::string& result) const = 0;
    virtual void DumpCurrentFrameLayer() const = 0;
};

using RSLayerPtr = std::shared_ptr<RSLayer>;
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_H
