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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H

#include <memory>

#include "animation/rs_animation_manager.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "property/rs_properties.h"

class SkCanvas;
namespace OHOS {
namespace Rosen {
class DrawCmdList;
class RSPaintFilterCanvas;

class RSRenderNode : public RSBaseRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSRenderNode>;
    using SharedPtr = std::shared_ptr<RSRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::RS_NODE;

    virtual ~RSRenderNode();

    bool Animate(int64_t timestamp) override;
    bool Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty);

    RSProperties& GetMutableRenderProperties();
    const RSProperties& GetRenderProperties() const;

    // used for animation test
    RSAnimationManager& GetAnimationManager()
    {
        return animationManager_;
    }

    virtual void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessRenderContents(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas);

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::RS_NODE;
    }

    bool HasTransition(bool recursive) const override
    {
        return animationManager_.HasTransition() || RSBaseRenderNode::HasTransition(recursive);
    }

protected:
    explicit RSRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    void UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager);
    bool IsDirty() const override;

private:
    void FallbackAnimationsToRoot();

    RectI oldDirty_;
    RSProperties renderProperties_;
    RSAnimationManager animationManager_;

    friend class RSRenderTransition;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
