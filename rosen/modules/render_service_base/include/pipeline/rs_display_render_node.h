/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H

#include <memory>
#ifndef ROSEN_CROSS_PLATFORM
#include <ibuffer_consumer_listener.h>
#include <surface.h>
#include "sync_fence.h"
#endif

#include "common/rs_macros.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "platform/drawing/rs_surface.h"

namespace OHOS {
namespace Rosen {
enum class ScreenRotation : uint32_t;
class RSB_EXPORT RSDisplayRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    enum CompositeType {
        UNI_RENDER_COMPOSITE = 0,
        UNI_RENDER_MIRROR_COMPOSITE,
        HARDWARE_COMPOSITE,
        SOFTWARE_COMPOSITE
    };
    using WeakPtr = std::weak_ptr<RSDisplayRenderNode>;
    using SharedPtr = std::shared_ptr<RSDisplayRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::DISPLAY_NODE;

    explicit RSDisplayRenderNode(NodeId id, const RSDisplayNodeConfig& config, std::weak_ptr<RSContext> context = {});
    ~RSDisplayRenderNode() override;

    void SetScreenId(uint64_t screenId)
    {
        screenId_ = screenId;
    }

    uint64_t GetScreenId() const
    {
        return screenId_;
    }

    void SetDisplayOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    int32_t GetDisplayOffsetX() const
    {
        return offsetX_;
    }

    int32_t GetDisplayOffsetY() const
    {
        return offsetY_;
    }

    void CollectSurface(
        const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec,
        bool isUniRender) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::DISPLAY_NODE;
    }

    bool IsMirrorDisplay() const;

    void SetCompositeType(CompositeType type);
    CompositeType GetCompositeType() const;
    void SetForceSoftComposite(bool flag);
    bool IsForceSoftComposite() const;
    void SetMirrorSource(SharedPtr node);
    void ResetMirrorSource();
    void SetIsMirrorDisplay(bool isMirror);
    void SetSecurityDisplay(bool isSecurityDisplay);
    bool GetSecurityDisplay() const;
    bool SkipFrame(uint32_t skipFrameInterval);

    WeakPtr GetMirrorSource() const
    {
        return mirrorSource_;
    }

    bool HasDisappearingTransition(bool) const override
    {
        return false;
    }

    std::shared_ptr<RSSurface> GetRSSurface() const
    {
        return surface_;
    }

#ifndef ROSEN_CROSS_PLATFORM
    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    sptr<IBufferConsumerListener> GetConsumerListener() const
    {
        return consumerListener_;
    }
#endif

    bool IsSurfaceCreated() const
    {
        return surfaceCreated_;
    }

    ScreenRotation GetRotation() const;

    std::shared_ptr<RSDirtyRegionManager> GetDirtyManager()
    {
        return dirtyManager_;
    }
    void UpdateDisplayDirtyManager(int32_t bufferage);
    void ClearCurrentSurfacePos();
    void UpdateSurfaceNodePos(NodeId id, RectI rect)
    {
        currentFrameSurfacePos_[id] = rect;
    }

    RectI GetLastFrameSurfacePos(NodeId id)
    {
        return lastFrameSurfacePos_[id];
    }

    RectI GetCurrentFrameSurfacePos(NodeId id)
    {
        return currentFrameSurfacePos_[id];
    }

    const std::vector<RectI> GetSurfaceChangedRects() const
    {
        std::vector<RectI> rects;
        for (auto iter = lastFrameSurfacePos_.begin(); iter != lastFrameSurfacePos_.end(); iter++) {
            if (currentFrameSurfacePos_.find(iter->first) == currentFrameSurfacePos_.end()) {
                rects.emplace_back(iter->second);
            }
        }
        for (auto iter = currentFrameSurfacePos_.begin(); iter != currentFrameSurfacePos_.end(); iter++) {
            if (lastFrameSurfacePos_.find(iter->first) == lastFrameSurfacePos_.end()) {
                rects.emplace_back(iter->second);
            }
        }
        return rects;
    }

    std::vector<RSBaseRenderNode::SharedPtr>& GetCurAllSurfaces()
    {
        return curAllSurfaces_;
    }

    void UpdateRotation();
    bool IsRotationChanged() const;
private:
    CompositeType compositeType_ { HARDWARE_COMPOSITE };
    uint64_t screenId_;
    int32_t offsetX_;
    int32_t offsetY_;
    bool forceSoftComposite_ { false };
    bool isMirroredDisplay_ = false;
    bool isSecurityDisplay_ = false;
    WeakPtr mirrorSource_;
    float lastRotation_ = 0.f;

    std::shared_ptr<RSSurface> surface_;
    bool surfaceCreated_ { false };
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IBufferConsumerListener> consumerListener_;
#endif
    uint64_t frameCount_ = 0;

    std::map<NodeId, RectI> lastFrameSurfacePos_;
    std::map<NodeId, RectI> currentFrameSurfacePos_;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;

    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
