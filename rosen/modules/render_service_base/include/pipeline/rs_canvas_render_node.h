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

#include "memory/rs_memory_track.h"

#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawCmdList;
}
class RSModifierContext;

class RSCanvasRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasRenderNode>;
    using SharedPtr = std::shared_ptr<RSCanvasRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::CANVAS_NODE;

    explicit RSCanvasRenderNode(NodeId id,
        const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false);
    virtual ~RSCanvasRenderNode();

    void UpdateRecording(std::shared_ptr<Drawing::DrawCmdList> drawCmds,
        RSModifierType type, bool isSingleFrameComposer = false);
    void ClearRecording();

    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas, bool includeProperty) override;
    void ProcessRenderContents(RSPaintFilterCanvas& canvas) override;
    void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas) override;

    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSB_EXPORT void ProcessShadowBatching(RSPaintFilterCanvas& canvas);

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::CANVAS_NODE;
    }
    bool GetUifirstSupportFlag() override
    {
        return isChildSupportUifirst_;
    }

    void OnTreeStateChanged() override;
private:
    void ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type);
    void InternalDrawContent(RSPaintFilterCanvas& canvas);

    void PropertyDrawableRender(RSPaintFilterCanvas& canvas, bool includeProperty);
    void DrawShadow(RSModifierContext& context, RSPaintFilterCanvas& canvas);

    RSPaintFilterCanvas::SaveStatus canvasNodeSaveCount_;

    friend class RSColorfulShadowDrawable;
    friend class RSRenderTransition;
    friend class RSPropertiesPainter;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_CANVAS_RENDER_NODE_H
