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

#ifndef RS_DIVIDED_UI_CAPTURE
#define RS_DIVIDED_UI_CAPTURE

#include "pixel_map.h"

#include "common/rs_macros.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSDividedUICapture {
public:
    RSDividedUICapture(NodeId nodeId, float scaleX, float scaleY)
        : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY) {}
    ~RSDividedUICapture() = default;

    std::shared_ptr<Media::PixelMap> TakeLocalCapture();

private:
    class RSDividedUICaptureVisitor : public RSNodeVisitor {
    public:
        RSDividedUICaptureVisitor(NodeId nodeId, float scaleX, float scaleY)
            : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY) {}
        ~RSDividedUICaptureVisitor() noexcept override = default;
        void PrepareChildren(RSRenderNode& node) override;
        void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
        void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {};
        void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
        void PrepareRootRenderNode(RSRootRenderNode& node) override;
        void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
        void PrepareEffectRenderNode(RSEffectRenderNode& node) override;

        void ProcessChildren(RSRenderNode& node) override;
        void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
        void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override {};
        void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
        void ProcessRootRenderNode(RSRootRenderNode& node) override;
        void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
        void ProcessEffectRenderNode(RSEffectRenderNode& node) override;

        void SetCanvas(std::shared_ptr<ExtendRecordingCanvas> canvas);

        void SetPaintFilterCanvas(std::shared_ptr<RSPaintFilterCanvas> canvas);

    private:
        std::shared_ptr<RSPaintFilterCanvas> canvas_ = nullptr;

        NodeId nodeId_;
        float scaleX_ = 1.0f;
        float scaleY_ = 1.0f;
    };
    std::shared_ptr<Drawing::Surface> CreateSurface(const std::shared_ptr<Media::PixelMap>& pixelmap) const;
    void PostTaskToRTRecord(std::shared_ptr<ExtendRecordingCanvas> canvas, std::shared_ptr<RSRenderNode> node,
        std::shared_ptr<RSDividedUICaptureVisitor> visitor);
    std::shared_ptr<Media::PixelMap> CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const;

    NodeId nodeId_;
    float scaleX_;
    float scaleY_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_DIVIDED_UI_CAPTURE
