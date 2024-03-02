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

#ifndef RS_UNI_UI_CAPTURE
#define RS_UNI_UI_CAPTURE

#include "pixel_map.h"
#include "rs_base_render_engine.h"

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

class RSUniUICapture {
public:
    RSUniUICapture(NodeId nodeId, float scaleX, float scaleY);
    ~RSUniUICapture() = default;

    std::shared_ptr<Media::PixelMap> TakeLocalCapture();

private:
    class RSUniUICaptureVisitor : public RSNodeVisitor {
    public:
        RSUniUICaptureVisitor(NodeId nodeId, float scaleX, float scaleY);
        ~RSUniUICaptureVisitor() noexcept override = default;
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
        void ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode& node);
        void ProcessSurfaceViewWithUni(RSSurfaceRenderNode& node);
        void ProcessSurfaceViewWithoutUni(RSSurfaceRenderNode& node);
        std::shared_ptr<RSPaintFilterCanvas> canvas_ = nullptr;

        NodeId nodeId_;
        float scaleX_ = 1.0f;
        float scaleY_ = 1.0f;

        Drawing::Matrix captureMatrix_ = Drawing::Matrix();
        bool isUniRender_ = false;
        std::shared_ptr<RSBaseRenderEngine> renderEngine_;
    };
    std::shared_ptr<Drawing::Surface> CreateSurface(const std::shared_ptr<Media::PixelMap>& pixelmap);
    void PostTaskToRSRecord(std::shared_ptr<ExtendRecordingCanvas> canvas, std::shared_ptr<RSRenderNode> node,
        std::shared_ptr<RSUniUICaptureVisitor> visitor);
    bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img,
        std::shared_ptr<Media::PixelMap> pixelmap);
    std::shared_ptr<Media::PixelMap> CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const;

    NodeId nodeId_;
    float scaleX_;
    float scaleY_;
    bool isUniRender_ = false;
    bool isUseCpuSurface_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_UNI_UI_CAPTURE
