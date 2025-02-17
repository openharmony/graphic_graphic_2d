/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_RS_RCD_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CORE_RS_RCD_SURFACE_RENDER_NODE_H

#include <atomic>
#include <ibuffer_consumer_listener.h>
#include <memory>
#include <surface.h>

#include "common/rs_rect.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "feature/round_corner_display/rs_round_corner_config.h"
#include "platform/drawing/rs_surface.h"
#include "render_context/render_context.h"
#include "sync_fence.h"
#include <filesystem>
#include "include/core/SkBitmap.h"

namespace OHOS {
namespace Rosen {


enum class RCDSurfaceType : uint32_t {
    BOTTOM,
    TOP,
    INVALID
};

struct RcdExtInfo {
    bool surfaceCreated = false;
    RectI srcRect_;
    RectI dstRect_;

    RectF surfaceBounds;
    RectF frameBounds;
    RectF frameViewPort;

    RCDSurfaceType surfaceType = RCDSurfaceType::INVALID;

    void Clear()
    {
        surfaceBounds.Clear();
        frameBounds.Clear();
        frameViewPort.Clear();
    }

    float GetFrameOffsetX() const
    {
        return frameViewPort.GetLeft() - surfaceBounds.GetLeft();
    }

    float GetFrameOffsetY() const
    {
        return frameViewPort.GetTop() - surfaceBounds.GetTop();
    }
};

struct RcdSourceInfo {
    uint32_t bufferWidth = 0;
    uint32_t bufferHeight = 0;
    uint32_t bufferSize = 0;
};

struct HardwareLayerInfo {
    std::filesystem::path pathBin;
    int bufferSize = 0;
    int cldWidth = 0;
    int cldHeight = 0;
};

struct CldInfo {
    uint32_t cldDataOffset = 0;
    uint32_t cldSize = 0;
    uint32_t cldWidth = 0;
    uint32_t cldHeight = 0;
    uint32_t cldStride = 0;
    uint32_t exWidth = 0;
    uint32_t exHeight = 0;
    uint32_t baseColor = 0;
};

class RSRcdSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSRcdSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSRcdSurfaceRenderNode>;

    RSRcdSurfaceRenderNode(NodeId id, RCDSurfaceType type, const std::weak_ptr<RSContext>& context = {});
    static SharedPtr Create(NodeId id, RCDSurfaceType type);
    ~RSRcdSurfaceRenderNode() override;

    const RectI& GetSrcRect() const;
    const RectI& GetDstRect() const;

    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    bool IsSurfaceCreated() const;
    std::shared_ptr<RSSurface> GetRSSurface() const;
    sptr<IBufferConsumerListener> GetConsumerListener() const;
    RcdSourceInfo rcdSourceInfo;
    void SetRcdBufferWidth(uint32_t width);
    void SetRcdBufferHeight(uint32_t height);
    void SetRcdBufferSize(uint32_t bufferSize);
    void ClearBufferCache();
    void ResetCurrFrameState();
    void Reset();
    bool SetHardwareResourceToBuffer();
    BufferRequestConfig GetHardenBufferRequestConfig() const;
    bool PrepareHardwareResourceBuffer(const std::shared_ptr<rs_rcd::RoundCornerLayer>& layerInfo);
    bool IsBottomSurface() const;
    bool IsTopSurface() const;
    bool IsInvalidSurface() const;

    float GetFrameOffsetX() const;
    float GetFrameOffsetY() const;

    const CldInfo& GetCldInfo() const;

    void SetRenderTargetId(NodeId id);

    void SetRenderDisplayRect(const RectT<uint32_t>& rect)
    {
        displayRect_ = rect;
    }

    void PrintRcdNodeInfo();
private:
    float GetSurfaceWidth() const;
    float GetSurfaceHeight() const;
    bool FillHardwareResource(HardwareLayerInfo &cldLayerInfo, int height, int width);
    HardwareLayerInfo cldLayerInfo;
    Drawing::Bitmap layerBitmap;

    uint32_t GetRcdBufferWidth() const;
    uint32_t GetRcdBufferHeight() const;
    uint32_t GetRcdBufferSize() const;

    std::shared_ptr<RSSurface> surface_ = nullptr;
    sptr<IBufferConsumerListener> consumerListener_;
    RectT<uint32_t> displayRect_;

    RcdExtInfo rcdExtInfo_;

    CldInfo cldInfo_;

    NodeId renerTargetId_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_RCD_SURFACE_RENDER_NODE_H