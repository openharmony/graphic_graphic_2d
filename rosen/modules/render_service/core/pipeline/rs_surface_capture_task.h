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

#ifndef RS_SURFACE_CAPTURE_TASK
#define RS_SURFACE_CAPTURE_TASK

#include "common/rs_common_def.h"
#include "include/core/SkCanvas.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pixel_map.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceCaptureTask {
public:
    explicit RSSurfaceCaptureTask(NodeId nodeId, float scaleX, float scaleY)
        : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY) {}
    ~RSSurfaceCaptureTask() = default;

    std::unique_ptr<Media::PixelMap> Run();

private:
    class RSSurfaceCaptureVisitor : public RSNodeVisitor {
    public:
        RSSurfaceCaptureVisitor() {}
        ~RSSurfaceCaptureVisitor() override {}
        virtual void PrepareBaseRenderNode(RSBaseRenderNode &node) override {}
        virtual void PrepareDisplayRenderNode(RSDisplayRenderNode &node) override {}
        virtual void PrepareSurfaceRenderNode(RSSurfaceRenderNode &node) override {}
        virtual void PrepareCanvasRenderNode(RSCanvasRenderNode &node) override {}
        virtual void PrepareRootRenderNode(RSRootRenderNode& node) override {}
        virtual void ProcessBaseRenderNode(RSBaseRenderNode &node) override {}
        virtual void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {}
        virtual void ProcessRootRenderNode(RSRootRenderNode& node) override {}

        virtual void ProcessDisplayRenderNode(RSDisplayRenderNode &node) override;
        virtual void ProcessSurfaceRenderNode(RSSurfaceRenderNode &node) override;
        void SetCanvas(std::unique_ptr<SkCanvas> canvas);
        void IsDisplayNode(bool isDisplayNode)
        {
            isDisplayNode_ = isDisplayNode;
        }
        void SetScale(float scaleX, float scaleY)
        {
            scaleX_ = scaleX;
            scaleY_ = scaleY;
        }

    private:
        void DrawSurface(RSSurfaceRenderNode &node);
        std::unique_ptr<SkCanvas> canvas_ = nullptr;
        bool isDisplayNode_ = false;
        float scaleX_ = 1.0f;
        float scaleY_ = 1.0f;
    };

    std::unique_ptr<SkCanvas> CreateCanvas(const std::unique_ptr<Media::PixelMap>& pixelmap);

    std::unique_ptr<Media::PixelMap> CreatePixelMapBySurfaceNode(std::shared_ptr<RSSurfaceRenderNode> node);

    std::unique_ptr<Media::PixelMap> CreatePixelMapByDisplayNode(std::shared_ptr<RSDisplayRenderNode> node);

    NodeId nodeId_;

    float scaleX_;

    float scaleY_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SURFACE_CAPTURE_TASK