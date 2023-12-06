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

#ifndef LAYER_CONTEXT_H
#define LAYER_CONTEXT_H

#include <surface.h>
#include "hdi_layer_info.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
enum LayerType : uint32_t {
    LAYER_STATUS,
    LAYER_LAUNCHER,
    LAYER_NAVIGATION,
    LAYER_EXTRA
};

using YUVPixel = struct {
    uint8_t y;
    uint8_t u;
    uint8_t v;
};

class LayerContext : public IBufferConsumerListenerClazz {
public:
    LayerContext(GraphicIRect dst, GraphicIRect src, uint32_t zorder, LayerType layerType);
    virtual ~LayerContext();
    static constexpr uint32_t PIXEL_LINE_ALIGNMENT = 32;
    static constexpr uint32_t RBGA_R_MOVEBITS = 24;
    static constexpr uint32_t RBGA_B_MOVEBITS = 16;
    static constexpr uint32_t RBGA_G_MOVEBITS = 8;
    static constexpr uint32_t PIXEL_YCBCR420_UV_NUM = 2;
    static constexpr float PIXEL_YCBCR420_BYTE = 1.5;

    void OnBufferAvailable() override;
    SurfaceError DrawBufferColor();
    void FillHDILayer();
    SurfaceError FillHDIBuffer();
    const std::shared_ptr<HdiLayerInfo> GetHdiLayer();
    void SetTestClientStatus(bool status);
    void SetTestRotateStatus(bool status);
    void SetTestYUVStatus(bool status);
    void SetTestLayerColor(bool status);
    LayerType GetLayerType() const;
    sptr<SurfaceBuffer> GetPreBuffer() const
    {
        return prevBuffer_;
    }
    sptr<SyncFence> GetPreFence() const
    {
        return prevFence_;
    }

private:
    const std::vector<uint32_t> colors_ = {0xff0000ff, 0xffff00ff, 0xaa00ff00, 0xff00ffaa, 0xff0f0f00};
    GraphicIRect dst_;
    GraphicIRect src_;
    uint32_t colorIndex_ = 0;
    uint32_t frameCounter_ = 0;
    uint32_t color_ = 0xffff1111;
    uint32_t zorder_ = 0;
    sptr<SyncFence> prevFence_;
    OHOS::sptr<Surface> pSurface_;
    OHOS::sptr<IConsumerSurface> cSurface_;
    OHOS::sptr<SurfaceBuffer> prevBuffer_;
    std::shared_ptr<HdiLayerInfo> hdiLayer_;
    LayerType layerType_ = LayerType::LAYER_EXTRA;
    bool testClient_ = false;
    bool testRotate_ = false;
    bool testYUV_ = false;
    bool testLayerColor_ = false;

    void DrawColor(void *image, int width, int height);
    void DrawExtraColor(void *image, uint32_t width, uint32_t height);
    void DrawBaseColor(void *image, uint32_t width, uint32_t height);
    void DrawYUVColor(void *image, uint32_t width, uint32_t height, YUVPixel pixelValueYUV);
    void ConvertRBGA2YUV(uint32_t pixelValueRBGA, YUVPixel *pixelValueYUV);
    void SetLayerTransformType();
    void SetLayerCompositionType();
};
} // namespace Rosen
} // namespace OHOS

#endif // LAYER_CONTEXT_H