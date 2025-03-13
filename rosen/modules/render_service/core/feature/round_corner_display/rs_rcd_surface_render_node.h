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

class RSRcdSurfaceRenderNode;
class RSProcessor;

struct RcdPrepareInfo {
    std::shared_ptr<RSRcdSurfaceRenderNode> bottomNode = nullptr;
    std::shared_ptr<RSRcdSurfaceRenderNode> topNode = nullptr;
    bool hasInvalidScene = false;
};

struct RcdProcessInfo {
    std::shared_ptr<RSProcessor> uniProcessor = nullptr;
    std::shared_ptr<rs_rcd::RoundCornerLayer> topLayer = nullptr;
    std::shared_ptr<rs_rcd::RoundCornerLayer> bottomLayer = nullptr;
    bool resourceChanged = false;
};

struct RoundCornerDisplayInfo {
    RcdPrepareInfo prepareInfo;
    RcdProcessInfo processInfo;
};

enum class RCDNodeId : NodeId {
    TOP_RCD_NODE_ID = 1,
    BACKGROUND_RCD_NODE_ID = 2
};

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
    std::string pathBin;
    int bufferSize = 0;
    int cldWidth = 0;
    int cldHeight = 0;
};

struct CldInfo;

// use normal hardware composer node instead if it supports rcd feature in OH 6.0 rcd refactoring
class RSRcdSurfaceRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSRcdSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSRcdSurfaceRenderNode>;

    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::ROUND_CORNER_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    RSRcdSurfaceRenderNode(RCDSurfaceType type, const std::weak_ptr<RSContext>& context);
    RSRcdSurfaceRenderNode(NodeId id, RCDSurfaceType type, const std::weak_ptr<RSContext>& context = {});
    static SharedPtr Create(NodeId id, RCDSurfaceType type, const std::weak_ptr<RSContext>& context = {});
    ~RSRcdSurfaceRenderNode() override;

    void InitRenderParams() override;

    const std::shared_ptr<RSSurfaceHandler> GetRSSurfaceHandler() const
    {
        return surfaceHandler_;
    }

    std::shared_ptr<RSSurfaceHandler> GetMutableRSSurfaceHandler()
    {
        return surfaceHandler_;
    }

    const RectI& GetSrcRect() const;
    const RectI& GetDstRect() const;

    RcdSourceInfo rcdSourceInfo;
    void SetRcdBufferWidth(uint32_t width);
    void SetRcdBufferHeight(uint32_t height);
    void SetRcdBufferSize(uint32_t bufferSize);
    void ResetCurrFrameState();
    void Reset();
    bool PrepareHardwareResourceBuffer(const std::shared_ptr<rs_rcd::RoundCornerLayer>& layerInfo);
    bool IsTopSurface() const;
    bool IsBottomSurface() const;
    bool IsInvalidSurface() const;

    const CldInfo GetCldInfo() const;

    void SetRenderDisplayRect(const RectT<uint32_t>& rect)
    {
        displayRect_ = rect;
    }

    void PrepareHardwareResource(std::shared_ptr<rs_rcd::RoundCornerLayer>& layerInfo);

    void UpdateRcdRenderParams(bool resourceChanged, const std::shared_ptr<Drawing::Bitmap> curBitmap);

    void DoProcessRenderMainThreadTask(bool resourceChanged, std::shared_ptr<RSProcessor> processor);

    void PrintRcdNodeInfo();
    
protected:
    void OnSync() override;

private:
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;

    HardwareLayerInfo cldLayerInfo_;
    Drawing::Bitmap layerBitmap_;

    uint32_t GetRcdBufferWidth() const;
    uint32_t GetRcdBufferHeight() const;
    uint32_t GetRcdBufferSize() const;

    RectT<uint32_t> displayRect_;
    RcdExtInfo rcdExtInfo_;

    NodeId renderTargetId_ = 0;

    void OnRegister(const std::weak_ptr<RSContext>& context) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_RCD_SURFACE_RENDER_NODE_H