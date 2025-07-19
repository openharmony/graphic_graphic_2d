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

#ifndef COMPOSER_CLIENT_RS_LAYER_H
#define COMPOSER_CLIENT_RS_LAYER_H

#include
#include "iconsumer_surface.h"
#include <surface.h>
#include <sync_fence.h>
#include "graphic_error.h"
#include "hdi_display_type.h"
#include "common/rs_anco_type.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSLayer : public std::enable_shared_from_this {
public:
    RSLayer() = default;
    virtual ~RSLayer() = default;

    // /* rs create and set/get layer info begin */
    // static std::shared_ptr<HdiLayerInfo> CreateHdiLayerInfo()
    // {
    //     return std::make_shared<HdiLayerInfo>();
    // }

    /* create or update rslayer info */
    virtual bool CreateRsLayer();
    // bool DestroyRsLayer();

    // void SetSurface(const sptr<IConsumerSurface> &surface);
    // void SetBuffer(const sptr<SurfaceBuffer> &sbuffer, const sptr<SyncFence> &acquireFence);
    // void SetPreBuffer(const sptr<SurfaceBuffer> &buffer);

    // void SetZorder(int32_t zOrder);
    // void SetType(const GraphicLayerType& layerType);

    // GraphicLayerType GetType() const;

    virtual void SetAlpha(const GraphicLayerAlpha &alpha);
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
    // sptr GetSurface() const;
    // sptr GetBuffer() const;
    // sptr GetPreBuffer() const;
    // uint32_t GetZorder() const;
    // sptr GetAcquireFence() const;
    // const GraphicLayerAlpha &GetAlpha();
    // GraphicTransformType GetTransformType() const;
    // GraphicCompositionType GetCompositionType() const;
    // const std::vector &GetVisibleRegions();
    // const std::vector &GetDirtyRegions();
    // GraphicBlendType GetBlendType() const;
    // const GraphicIRect &GetCropRect();
    // const GraphicIRect &GetLayerSize();
    // GraphicIRect GetBoundSize() const;
    // GraphicMatrix GetMatrix() const;
    // int32_t GetGravity() const;
    // bool GetUniRenderFlag() const;
    // bool IsPreMulti() const;
    // void SetWindowsName(std::vectorstd::string& windowsName);
    // const std::vectorstd::string& GetWindowsName();
    // const std::vector &GetColorTransform();
    // GraphicColorDataSpace GetColorDataSpace() const;
    // GraphicLayerColor GetLayerColor() const;
    // GraphicLayerColor GetBackgroundColor() const;
    // const std::vector& GetCornerRadiusInfoForDRM() const;
    // std::vector &GetMetaData();
    // GraphicHDRMetaDataSet &GetMetaDataSet();
    // bool GetTunnelHandleChange() const;
    // sptr GetTunnelHandle() const;
    // void SetIsSupportedPresentTimestamp(bool isSupported);
    // void SetPresentTimestamp(const GraphicPresentTimestamp &timestamp);
    // float GetSdrNit() const;
    // float GetDisplayNit() const;
    // float GetBrightnessRatio() const;
    // void SetSdrNit(float sdrNit);
    // void SetDisplayNit(float displayNit);
    // void SetBrightnessRatio(float brightnessRatio);
    // const std::vector& GetLayerLinearMatrix() const;
    // void SetLayerLinearMatrix(const std::vector& layerLinearMatrix);
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
};
} // namespace Rosen
} // namespace OHOS

#endif // COMPOSER_CLIENT_RS_LAYER_H