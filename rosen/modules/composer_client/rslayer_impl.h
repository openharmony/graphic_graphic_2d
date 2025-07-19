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

#ifndef COMPOSER_CLIENT_RS_LAYER_IMPL_H
#define COMPOSER_CLIENT_RS_LAYER_IMPL_H

#include <string>
#include "iconsumer_surface.h"
#include <surface.h>
#include <sync_fence.h>
#include "graphic_error.h"
#include "hdi_display_type.h"
#include "common/rs_anco_type.h"

#include "rslayer.h"
#include "rslayer_property_cmd.h"

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

class RSLayerImpl : public RSLayer {
public:
    RSLayerImpl() = default;
    virtual ~RSLayerImpl() = default;

    enum class LayerMask {
        LAYER_MASK_NORMAL = 0,
        LAYER_MASK_HBM_SYNC = 1,   // enable fingerprint
    };

    // // /* rs create and set/get layer info begin */
    // // static std::shared_ptr<HdiLayerInfo> CreateHdiLayerInfo()
    // // {
    // //     return std::make_shared<HdiLayerInfo>();
    // // }

    // /* create or update rslayer info */
    // bool CreateRsLayer();
    // bool DestroyRsLayer();

    // void SetSurface(const sptr<IConsumerSurface> &surface);
    // void SetBuffer(const sptr<SurfaceBuffer> &sbuffer, const sptr<SyncFence> &acquireFence);
    // void SetPreBuffer(const sptr<SurfaceBuffer> &buffer);

    // void SetZorder(int32_t zOrder);
    // void SetType(const GraphicLayerType& layerType);

    // GraphicLayerType GetType() const;

    void SetAlpha(const GraphicLayerAlpha &alpha) override;

    // void SetTransform(GraphicTransformType type);

    // void SetCompositionType(GraphicCompositionType type);

    // void SetVisibleRegions(const std::vector<GraphicIRect> &visibleRegions);
    // void SetDirtyRegions(const std::vector<GraphicIRect> &dirtyRegions);

    // void SetBlendType(GraphicBlendType type);
    // void SetCropRect(const GraphicIRect &crop);
    // void SetPreMulti(bool preMulti);
    // void SetLayerSize(const GraphicIRect &layerRect);
    // void SetBoundSize(const GraphicIRect &boundRect);
    // void SetLayerColor(GraphicLayerColor layerColor);
    // void SetBackgroundColor(GraphicLayerColor backgroundColor);
    // void SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo);
    // void SetColorTransform(const std::vector<float> &matrix);
    // void SetColorDataSpace(GraphicColorDataSpace colorSpace);
    // void SetMetaData(const std::vector<GraphicHDRMetaData> &metaData);
    // void SetMetaDataSet(const GraphicHDRMetaDataSet &metaDataSet);
    // void SetMatrix(GraphicMatrix matrix);
    // void SetGravity(int32_t gravity);
    // void SetUniRenderFlag(bool isUniRender);
    // void SetTunnelHandleChange(bool change);
    // void SetTunnelHandle(const sptr<SurfaceTunnelHandle> &handle);
    // void SetTunnelLayerId(const uint64_t &tunnelLayerId);
    // uint64_t GetTunnelLayerId() const;
    // void SetTunnelLayerProperty(uint32_t tunnelLayerProperty);
    // uint32_t GetTunnelLayerProperty() const;
    // bool IsSupportedPresentTimestamp() const;
    // const GraphicPresentTimestamp &GetPresentTimestamp();

    // /* create or update rslayer info */

    // /* get rslayer info */
    // sptr<IConsumerSurface> GetSurface() const;
    // sptr<SurfaceBuffer> GetBuffer() const;
    // sptr<SurfaceBuffer> GetPreBuffer() const;
    // uint32_t GetZorder() const;
    // sptr<SyncFence> GetAcquireFence() const;
    // const GraphicLayerAlpha &GetAlpha();
    // GraphicTransformType GetTransformType() const;
    // GraphicCompositionType GetCompositionType() const;
    // const std::vector<GraphicIRect> &GetVisibleRegions();
    // const std::vector<GraphicIRect> &GetDirtyRegions();
    // GraphicBlendType GetBlendType() const;
    // const GraphicIRect &GetCropRect();
    // const GraphicIRect &GetLayerSize();
    // GraphicIRect GetBoundSize() const;
    // GraphicMatrix GetMatrix() const;
    // int32_t GetGravity() const;
    // bool GetUniRenderFlag() const;
    // bool IsPreMulti() const;
    // void SetWindowsName(std::vector<std::string>& windowsName);
    // const std::vector<std::string>& GetWindowsName();
    // const std::vector<float> &GetColorTransform();
    // GraphicColorDataSpace GetColorDataSpace() const;
    // GraphicLayerColor GetLayerColor() const;
    // GraphicLayerColor GetBackgroundColor() const;
    // const std::vector<float>& GetCornerRadiusInfoForDRM() const;
    // std::vector<GraphicHDRMetaData> &GetMetaData();
    // GraphicHDRMetaDataSet &GetMetaDataSet();
    // bool GetTunnelHandleChange() const;
    // sptr<SurfaceTunnelHandle> GetTunnelHandle() const;
    // void SetIsSupportedPresentTimestamp(bool isSupported);
    // void SetPresentTimestamp(const GraphicPresentTimestamp &timestamp);
    // float GetSdrNit() const;
    // float GetDisplayNit() const;
    // float GetBrightnessRatio() const;
    // void SetSdrNit(float sdrNit);
    // void SetDisplayNit(float displayNit);
    // void SetBrightnessRatio(float brightnessRatio);
    // const std::vector<float>& GetLayerLinearMatrix() const;
    // void SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix);
    // // source crop tuning
    // int32_t GetLayerSourceTuning() const;
    // void SetLayerSourceTuning(int32_t layerSouce);
    // void SetRotationFixed(bool rotationFixed);
    // bool GetRotationFixed() const;
    // void SetLayerArsr(bool arsrTag);
    // bool GetLayerArsr() const;
    // void SetLayerCopybit(bool copybitTag);
    // bool GetLayerCopybit() const;
    // void SetNeedBilinearInterpolation(bool need);
    // bool GetNeedBilinearInterpolation() const;
    // void SetIsMaskLayer(bool isMaskLayer);
    // bool IsMaskLayer() const;

    // void CopyLayerInfo(const std::shared_ptr<HdiLayerInfo> &layerInfo);
    // void Dump(std::string &result) const;
    // void DumpCurrentFrameLayer() const;
    // RosenError SetLayerMaskInfo(LayerMask mask);
    // LayerMask GetLayerMaskInfo();
    // inline uint64_t GetNodeId();
    // void SetNodeId(uint64_t nodeId);
    // void SetAncoFlags(const uint32_t ancoFlags);
    // uint32_t GetAncoFlags() const;
    // bool IsAncoNative() const;
    // /* get rslayer info */

private:
    template<typename RSLayerCmdName, typename PropertyName, typename T>
    void SetProperty(RSLayerCmdType modifierType, T value);

    template<typename RSLayerCmdName, typename PropertyName, typename T>
    void SetRsLayerCmd(RSLayerCmdType RSLayerCmdType, T value);


    void AddRsLayerCmd(const std::shared_ptr<RslayerCmd> layerCmd);
    bool AddCommand(std::unique_ptr<RSCommand>& command,
        FollowType followType, LayerId layerId) const;
    std::shared_ptr<RSTransactionHandler> GetRSTransaction() const;


    std::map<PropertyId, std::shared_ptr<RSModifier>> commands_;
    std::map<uint16_t, std::shared_ptr<RSModifier>> commandsTypeMap_;

    std::map<PropertyId, std::shared_ptr<RSLayerProperty>> properties_;
    std::map<RSLayerCmdType, std::shared_ptr<RSLayerCmd>> rsLayerCmds_;

    mutable std::recursive_mutex propertyMutex_;


    RSLayerId rsLayerId_;

    // uint32_t zOrder_ = 0;
    // GraphicLayerType layerType_ = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
    // GraphicIRect layerRect_;
    // GraphicIRect boundRect_; // node's bound width and height related to this layer, used for uni render redraw
    // std::vector<GraphicIRect> visibleRegions_;
    // std::vector<GraphicIRect> dirtyRegions_;
    // GraphicIRect cropRect_;
    // GraphicMatrix matrix_; // matrix used for uni render redraw
    // int32_t gravity_; // used for uni render redraw
    // bool isUniRender_ = false; // true for uni render layer (DisplayNode)
    GraphicLayerAlpha layerAlpha_;
    // GraphicTransformType transformType_ = GraphicTransformType::GRAPHIC_ROTATE_BUTT;
    // GraphicCompositionType compositionType_;
    // GraphicBlendType blendType_;
    // std::vector<float> colorTransformMatrix_;
    // GraphicLayerColor layerColor_;
    // GraphicLayerColor backgroundColor_;
    // GraphicColorDataSpace colorSpace_ = GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    // std::vector<GraphicHDRMetaData> metaData_;
    // GraphicHDRMetaDataSet metaDataSet_;
    // sptr<SurfaceTunnelHandle> tunnelHandle_ = nullptr;
    // std::vector<std::string> windowsName_;
    // bool tunnelHandleChange_ = false;
    // bool IsSupportedPresentTimestamp_ = false;
    // GraphicPresentTimestamp presentTimestamp_ = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};

    // sptr<IConsumerSurface> cSurface_ = nullptr;
    // sptr<SyncFence> acquireFence_ = SyncFence::InvalidFence();
    // sptr<SurfaceBuffer> sbuffer_ = nullptr;
    // sptr<SurfaceBuffer> pbuffer_ = nullptr;
    // bool preMulti_ = false;
    // bool needBilinearInterpolation_ = false;
    // LayerMask layerMask_ = LayerMask::LAYER_MASK_NORMAL;
    // mutable std::mutex mutex_;
    // float sdrNit_ = 500.0f; // default sdr nit
    // float displayNit_ = 500.0f; // default luminance for sdr
    // float brightnessRatio_ = 1.0f; // default ratio for sdr
    // std::vector<float> layerLinearMatrix_
    //     = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}; // matrix for linear colorspace
    // uint64_t nodeId_ = 0;
    // uint64_t tunnelLayerId_ = 0;
    // uint32_t tunnelLayerProperty_ = 0;
    // int32_t layerSource_ = 0; // default layer source tag
    // bool rotationFixed_ = false;
    // bool arsrTag_ = true;
    // bool copybitTag_ = false;
    // std::vector<float> drmCornerRadiusInfo_;
    // bool isMaskLayer_ = false;
    // uint32_t ancoFlags_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // COMPOSER_CLIENT_RS_LAYER_IMPL_H