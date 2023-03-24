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

#ifndef RENDER_SERVICE_CORE_RS_DRIVEN_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CORE_RS_DRIVEN_SURFACE_RENDER_NODE_H

#include <ibuffer_consumer_listener.h>
#include <memory>
#include <surface.h>

#include "common/rs_rect.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "platform/drawing/rs_surface.h"
#include "render_context/render_context.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
////////////////////////////////////////////////////////////////////////////
// the definitions must be kept in sync with libdriven_render
struct DrivenFrameState {
    RectF surfaceBounds;
    RectF contentBounds;
    RectF frameBounds;
    RectF frameViewPort;
    RectI frameClipRect;
    std::vector<std::pair<int32_t, Vector4f>> framePaintItems;

    DrivenFrameState& operator=(const DrivenFrameState& other)
    {
        surfaceBounds = other.surfaceBounds;
        contentBounds = other.contentBounds;
        frameBounds = other.frameBounds;
        frameViewPort = other.frameViewPort;
        frameClipRect = other.frameClipRect;
        framePaintItems.assign(other.framePaintItems.begin(), other.framePaintItems.end());
        return *this;
    }

    void Clear()
    {
        surfaceBounds.Clear();
        contentBounds.Clear();
        frameBounds.Clear();
        frameViewPort.Clear();
        frameClipRect.Clear();
        framePaintItems.clear();
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

enum class DrivenSurfaceRenderMode : uint32_t {
    EXPANDED,
    REUSABLE,
    DISABLED
};

enum class DrivenSurfaceType : uint32_t {
    BACKGROUND,
    CONTENT,
    INVALID
};

struct DrivenExtInfo {
    bool surfaceCreated_ = false;
    RectI srcRect_;
    RectI dstRect_;

    DrivenFrameState activateState_;
    DrivenFrameState currentState_;
    DrivenFrameState lastFrameState_;
    float yOffset_ = 0.0f;
    DrivenSurfaceRenderMode renderMode_ = DrivenSurfaceRenderMode::DISABLED;
    DrivenSurfaceType surfaceType_ = DrivenSurfaceType::INVALID;
};
////////////////////////////////////////////////////////////////////////////

class RSDrivenSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSDrivenSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSDrivenSurfaceRenderNode>;

    RSDrivenSurfaceRenderNode(NodeId id, DrivenSurfaceType type, std::weak_ptr<RSContext> context = {});
    ~RSDrivenSurfaceRenderNode() override;

    const RectI& GetSrcRect() const;
    const RectI& GetDstRect() const;

    void SetDrivenCanvasNode(RSBaseRenderNode::SharedPtr node);
    RSBaseRenderNode::SharedPtr GetDrivenCanvasNode() const;

    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    bool IsSurfaceCreated() const;
    std::shared_ptr<RSSurface> GetRSSurface() const;
    sptr<IBufferConsumerListener> GetConsumerListener() const;
    BufferRequestConfig GetBufferRequestConfig() const;
    void ClearBufferCache();

    void ResetCurrFrameState();
    void Reset();
    void PushFramePaintItem(Vector4f paintItem, int32_t itemIndex);
    void DisabledRenderMode();
    void SetCurrFrameBounds(const RectF& bounds, const RectF& viewPort, const RectI& contentAbsRect);
    void UpdateActivateFrameState(const RectI& dstRect, bool backgroundDirty, bool contentDirty, bool nonContentDirty);

    bool IsExpandedMode() const;
    bool IsReusableMode() const;
    bool IsDisabledMode() const;
    bool IsBackgroundSurface() const;
    bool IsContentSurface() const;
    bool IsInvalidSurface() const;

    float GetFrameOffsetX() const;
    float GetFrameOffsetY() const;
    const RectI& GetFrameClipRect() const;

    const DrivenSurfaceRenderMode& GetDrivenSurfaceRenderMode() const
    {
        return drivenExtInfo_.renderMode_;
    }

private:
    float GetSurfaceWidth() const;
    float GetSurfaceHeight() const;

    std::shared_ptr<RSSurface> surface_;
    sptr<IBufferConsumerListener> consumerListener_;
    RSBaseRenderNode::SharedPtr drivenCanvasNode_ = nullptr;

    DrivenExtInfo drivenExtInfo_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_DRIVEN_SURFACE_RENDER_NODE_H
