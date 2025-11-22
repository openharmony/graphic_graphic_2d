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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_LAYER_H
#define RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_LAYER_H

#include <string>
#include "surface.h"
#include "sync_fence.h"
#include "hdi_display_type.h"
#include "common/rs_anco_type.h"
#include "iconsumer_surface.h"
#include "rs_layer.h"
#include "rs_render_composer_client.h"

namespace OHOS {
namespace Rosen {
class RSLayerContext;

class RSSurfaceLayer : public RSLayer {
public:
    RSSurfaceLayer() = default;
    ~RSSurfaceLayer() = default;
    static std::shared_ptr<RSLayer> CreateLayer(const std::shared_ptr<RSRenderComposerClient>& client);

    void SetAlpha(const GraphicLayerAlpha& alpha) override;
    const GraphicLayerAlpha& GetAlpha() const override;
    void SetZorder(int32_t zOrder) override;
    uint32_t GetZorder() const override;
    void SetType(const GraphicLayerType layerType) override;
    GraphicLayerType GetType() const override;
    void SetTransform(GraphicTransformType type) override;
    GraphicTransformType GetTransformType() const override;
    void SetCompositionType(GraphicCompositionType type) override;
    GraphicCompositionType GetCompositionType() const override;
    void SetVisibleRegions(const std::vector<GraphicIRect>& visibleRegions) override;
    const std::vector<GraphicIRect>& GetVisibleRegions() const override;
    void SetDirtyRegions(const std::vector<GraphicIRect>& dirtyRegions) override;
    const std::vector<GraphicIRect>& GetDirtyRegions() const override;
    void SetBlendType(GraphicBlendType type) override;
    GraphicBlendType GetBlendType() const override;
    void SetCropRect(const GraphicIRect& crop) override;
    const GraphicIRect& GetCropRect() const override;
    void SetPreMulti(bool preMulti) override;
    bool IsPreMulti() const override;
    void SetLayerSize(const GraphicIRect& layerRect) override;
    const GraphicIRect& GetLayerSize() const override;
    void SetBoundSize(const GraphicIRect& boundRect) override;
    const GraphicIRect& GetBoundSize() const override;
    void SetLayerColor(GraphicLayerColor layerColor) override;
    const GraphicLayerColor& GetLayerColor() const override;
    void SetBackgroundColor(GraphicLayerColor backgroundColor) override;
    const GraphicLayerColor& GetBackgroundColor() const override;
    void SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo) override;
    const std::vector<float>& GetCornerRadiusInfoForDRM() const override;
    void SetColorTransform(const std::vector<float>& matrix) override;
    const std::vector<float>& GetColorTransform() const override;
    void SetColorDataSpace(GraphicColorDataSpace colorSpace) override;
    GraphicColorDataSpace GetColorDataSpace() const override;
    void SetMetaData(const std::vector<GraphicHDRMetaData>& metaData) override;
    const std::vector<GraphicHDRMetaData>& GetMetaData() const override;
    void SetMetaDataSet(const GraphicHDRMetaDataSet& metaDataSet) override;
    const GraphicHDRMetaDataSet& GetMetaDataSet() const override;
    void SetMatrix(GraphicMatrix matrix) override;
    const GraphicMatrix& GetMatrix() const override;
    void SetGravity(int32_t gravity) override;
    int32_t GetGravity() const override;
    void SetUniRenderFlag(bool isUniRender) override;
    bool GetUniRenderFlag() const override;
    void SetTunnelHandleChange(bool change) override;
    bool GetTunnelHandleChange() const override;
    void SetTunnelHandle(const sptr<SurfaceTunnelHandle>& handle) override;
    sptr<SurfaceTunnelHandle> GetTunnelHandle() const override;
    void SetTunnelLayerId(const uint64_t tunnelLayerId) override;
    uint64_t GetTunnelLayerId() const override;
    void SetTunnelLayerProperty(uint32_t tunnelLayerProperty) override;
    uint32_t GetTunnelLayerProperty() const override;
    void SetIsSupportedPresentTimestamp(bool isSupported) override;
    bool IsSupportedPresentTimestamp() const override;
    void SetPresentTimestamp(const GraphicPresentTimestamp& timestamp) override;
    const GraphicPresentTimestamp& GetPresentTimestamp() const override;
    void SetSdrNit(float sdrNit) override;
    float GetSdrNit() const override;
    void SetDisplayNit(float displayNit) override;
    float GetDisplayNit() const override;
    void SetBrightnessRatio(float brightnessRatio) override;
    float GetBrightnessRatio() const override;
    void SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix) override;
    const std::vector<float>& GetLayerLinearMatrix() const override;
    void SetLayerSourceTuning(int32_t layerSource) override;
    int32_t GetLayerSourceTuning() const override;
    void SetWindowsName(std::vector<std::string>& windowsName) override;
    const std::vector<std::string>& GetWindowsName() const override;
    void SetRotationFixed(bool rotationFixed) override;
    bool GetRotationFixed() const override;
    void SetLayerArsr(bool arsrTag) override;
    bool GetLayerArsr() const override;
    void SetLayerCopybit(bool copybitTag) override;
    bool GetLayerCopybit() const override;
    void SetNeedBilinearInterpolation(bool need) override;
    bool GetNeedBilinearInterpolation() const override;
    void SetIsMaskLayer(bool isMaskLayer) override;
    bool IsMaskLayer() const override;
    void SetNodeId(uint64_t nodeId) override;
    uint64_t GetNodeId() const override;
    void SetAncoFlags(const uint32_t ancoFlags) override;
    uint32_t GetAncoFlags() const override;
    bool IsAncoNative() const override;
    void SetLayerMaskInfo(LayerMask mask) override;
    LayerMask GetLayerMaskInfo() const override;

    void SetSurface(const sptr<IConsumerSurface>& surface) override;
    sptr<IConsumerSurface> GetSurface() const override;
    void SetBuffer(const sptr<SurfaceBuffer>& sbuffer, const sptr<SyncFence>& acquireFence) override;
    void SetBuffer(const sptr<SurfaceBuffer>& sbuffer) override;
    sptr<SurfaceBuffer> GetBuffer() const override;
    void SetPreBuffer(const sptr<SurfaceBuffer>& buffer) override;
    sptr<SurfaceBuffer> GetPreBuffer() const override;
    void SetAcquireFence(const sptr<SyncFence>& acquireFence) override;
    sptr<SyncFence> GetAcquireFence() const override;
    void SetUseDeviceOffline(bool useOffline) override;
    bool GetUseDeviceOffline() const override;
    void SetIgnoreAlpha(bool ignoreAlpha) override;
    bool GetIgnoreAlpha() const override;
    void SetAncoSrcRect(const GraphicIRect& ancoSrcRect) override;
    const GraphicIRect& GetAncoSrcRect() const override;

    void CopyLayerInfo(const std::shared_ptr<RSLayer>& rsLayer) override;
    void Dump(std::string& result) const override;
    void DumpCurrentFrameLayer() const override;

private:
    // rs layer pipeline info
    std::weak_ptr<RSLayerContext> rsLayerContext_;
    mutable std::mutex mutex_;

    // rs layer property info
    uint32_t zOrder_ = 0;
    GraphicLayerType layerType_ = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
    GraphicIRect layerRect_;
    GraphicIRect boundRect_; // node's bound width and height related to this layer, used for uni render redraw
    std::vector<GraphicIRect> visibleRegions_;
    std::vector<GraphicIRect> dirtyRegions_;
    GraphicIRect cropRect_;
    GraphicMatrix matrix_; // matrix used for uni render redraw
    int32_t gravity_ = 0; // used for uni render redraw
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
    float sdrNit_ = 500.0f; // default sdr nit
    float displayNit_ = 500.0f; // default luminance for sdr
    float brightnessRatio_ = 1.0f; // default ratio for sdr
    /* matrix for linear colorspace */
    std::vector<float> layerLinearMatrix_ = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
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
    bool useDeviceOffline_ = false;
    bool ignoreAlpha_ = false;
    GraphicIRect ancoSrcRect_ {-1, -1, -1, -1};
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_LAYER_H
