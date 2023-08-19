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

#ifndef ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_H
#define ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_H

#include <functional>
#include <memory>

#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {
struct RSModifierContext;
#ifndef USE_ROSEN_DRAWING
using ThreadInfo = std::pair<uint64_t, std::function<void(sk_sp<SkSurface>)>>;
#else
using ThreadInfo = std::pair<uint64_t, std::function<void(std::shared_ptr<Drawing::Surface>)>>;
#endif

class RSB_EXPORT RSCanvasDrawingRenderNode : public RSCanvasRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasDrawingRenderNode>;
    using SharedPtr = std::shared_ptr<RSCanvasDrawingRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::CANVAS_DRAWING_NODE;

    explicit RSCanvasDrawingRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    virtual ~RSCanvasDrawingRenderNode();

    void ProcessRenderContents(RSPaintFilterCanvas& canvas) override;

    RSRenderNodeType GetType() const override
    {
        return Type;
    }

#ifndef USE_ROSEN_DRAWING
    SkBitmap GetBitmap();
#else
    Drawing::Bitmap GetBitmap();
#endif

    void SetSurfaceClearFunc(ThreadInfo threadInfo)
    {
        curThreadInfo_ = threadInfo;
    }

private:
    void ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type) const override;
    bool ResetSurface(int width, int height, RSPaintFilterCanvas& canvas);
    bool GetSizeFromDrawCmdModifiers(int& width, int& height);
    bool IsNeedResetSurface(const int& width, const int& height) const;

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> skSurface_;
#else
    std::shared_ptr<Drawing::Bitmap> bitmap_;
    std::shared_ptr<Drawing::Surface> surface_;
#endif
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
    ThreadInfo curThreadInfo_ = {};
    ThreadInfo preThreadInfo_ = {};
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_H