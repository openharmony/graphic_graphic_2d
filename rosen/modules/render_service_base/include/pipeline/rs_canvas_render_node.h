/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_CANVAS_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_CANVAS_RENDER_NODE_H

#include <memory>
#include <unordered_set>

#include "memory/MemoryTrack.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class DrawCmdList;
class RSPaintFilterCanvas;
struct RSModifierContext;

class RSCanvasRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasRenderNode>;
    using SharedPtr = std::shared_ptr<RSCanvasRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::CANVAS_NODE;

    explicit RSCanvasRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    virtual ~RSCanvasRenderNode();

    void UpdateRecording(std::shared_ptr<DrawCmdList> drawCmds, RSModifierType type);
    void ClearRecording();

    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderContents(RSPaintFilterCanvas& canvas) override;
    void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas) override;

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSB_EXPORT void ProcessDrivenBackgroundRender(RSPaintFilterCanvas& canvas);
    RSB_EXPORT void ProcessDrivenContentRender(RSPaintFilterCanvas& canvas);
    RSB_EXPORT void ProcessDrivenContentRenderAfterChildren(RSPaintFilterCanvas& canvas);

    void OnApplyModifiers() override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::CANVAS_NODE;
    }
private:
    void ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type) const;
    void InternalDrawContent(RSPaintFilterCanvas& canvas);

    std::pair<int, int> canvasNodeSaveCount_ = { 0, 0 };

    friend class RSRenderTransition;
    friend class RSPropertiesPainter;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_CANVAS_RENDER_NODE_H
