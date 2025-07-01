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

#ifndef RS_SURFACE_CAPTURE_TASK
#define RS_SURFACE_CAPTURE_TASK
#define EGL_EGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include "common/rs_common_def.h"
#include "system/rs_system_parameters.h"
#include "draw/canvas.h"
#include "draw/surface.h"
#include "utils/matrix.h"
#include "ipc_callbacks/surface_capture_callback.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pixel_map.h"
#include "visitor/rs_node_visitor.h"
namespace OHOS {
namespace Rosen {
bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img, const std::unique_ptr<Media::PixelMap>& pixelmap,
    std::shared_ptr<Drawing::ColorSpace> colorSpace = nullptr);
class RSSurfaceCaptureVisitor : public RSNodeVisitor {
    public:
        RSSurfaceCaptureVisitor(const RSSurfaceCaptureConfig& captureConfig, bool isUniRender);
        ~RSSurfaceCaptureVisitor() noexcept override = default;
        void PrepareChildren(RSRenderNode& node) override {}
        void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
        void PrepareLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override {}
        void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
        void PrepareRootRenderNode(RSRootRenderNode& node) override {}
        void PrepareScreenRenderNode(RSScreenRenderNode& node) override {}
        void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
        void PrepareEffectRenderNode(RSEffectRenderNode& node) override {}

        void ProcessChildren(RSRenderNode& node) override;
        void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {}
        void ProcessLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override;
        void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
        void ProcessRootRenderNode(RSRootRenderNode& node) override {}
        void ProcessScreenRenderNode(RSScreenRenderNode& node) override;
        void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
        void ProcessEffectRenderNode(RSEffectRenderNode& node) override {}

        void SetSurface(Drawing::Surface* surface);
        void IsDisplayNode(bool isDisplayNode)
        {
            isDisplayNode_ = isDisplayNode;
        }

        bool IsUniRender() const
        {
            return isUniRender_;
        }

    private:
        void ProcessSurfaceRenderNodeWithoutUni(RSSurfaceRenderNode& node);
        void CaptureSingleSurfaceNodeWithoutUni(RSSurfaceRenderNode& node);
        void CaptureSurfaceInDisplayWithoutUni(RSSurfaceRenderNode& node);

        std::unique_ptr<RSPaintFilterCanvas> canvas_ = nullptr;
        bool isDisplayNode_ = false;
        RSSurfaceCaptureConfig captureConfig_ = {};
        bool isUniRender_ = false;
        std::shared_ptr<RSBaseRenderEngine> renderEngine_ = nullptr;
};

class RSSurfaceCaptureTask {
public:
    explicit RSSurfaceCaptureTask(NodeId nodeId,
        const RSSurfaceCaptureConfig& captureConfig)
        : nodeId_(nodeId), captureConfig_(captureConfig) {}
    ~RSSurfaceCaptureTask() = default;

    bool Run(sptr<RSISurfaceCaptureCallback> callback);

private:
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor_ = nullptr;

    std::shared_ptr<Drawing::Surface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap,
        std::shared_ptr<Drawing::ColorSpace> colorSpace = nullptr);

    std::unique_ptr<Media::PixelMap> CreatePixelMapBySurfaceNode(std::shared_ptr<RSSurfaceRenderNode> node,
        bool isUniRender = false);

    std::unique_ptr<Media::PixelMap> CreatePixelMapByDisplayNode(std::shared_ptr<RSLogicalDisplayRenderNode> node,
        bool isUniRender = false);

    NodeId nodeId_ = INVALID_NODEID;

    RSSurfaceCaptureConfig captureConfig_ = {};

    ScreenRotation screenCorrection_ = ScreenRotation::ROTATION_0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SURFACE_CAPTURE_TASK
