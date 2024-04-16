/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_SURFACE_CAPTURE_TASK_PARALLEL
#define RS_SURFACE_CAPTURE_TASK_PARALLEL
#define EGL_EGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include "pipeline/rs_surface_capture_task.h"
#include "system/rs_system_parameters.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceCaptureTaskParallel {
public:
    explicit RSSurfaceCaptureTaskParallel(NodeId nodeId, float scaleX, float scaleY, bool isProcOnBgThread = false)
        : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY), isProcOnBgThread_(isProcOnBgThread),
        rsSurfaceCaptureType_(RSSystemParameters::GetRsSurfaceCaptureType()),
        rsParallelType_(RSSystemParameters::GetRsParallelType()) {}
    ~RSSurfaceCaptureTaskParallel() = default;

    bool Run(sptr<RSISurfaceCaptureCallback> callback);

private:
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor_ = nullptr;

    std::shared_ptr<Drawing::Surface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap);

    std::unique_ptr<Media::PixelMap> CreatePixelMapBySurfaceNode(std::shared_ptr<RSSurfaceRenderNode> node,
        bool isUniRender = false);

    std::unique_ptr<Media::PixelMap> CreatePixelMapByDisplayNode(std::shared_ptr<RSDisplayRenderNode> node,
        bool isUniRender = false, bool hasSecurityOrSkipLayer = false);

    bool FindSecurityOrSkipLayer();

    // It is currently only used on folding screen.
    int32_t ScreenCorrection(ScreenRotation screenRotation);

    NodeId nodeId_;

    float scaleX_;

    float scaleY_;

    ScreenRotation screenCorrection_ = ScreenRotation::ROTATION_0;

    // if true, do surfaceCapture on background thread
    bool isProcOnBgThread_ = false;
    RsSurfaceCaptureType rsSurfaceCaptureType_;
    RsParallelType rsParallelType_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SURFACE_CAPTURE_TASK_PARALLEL
