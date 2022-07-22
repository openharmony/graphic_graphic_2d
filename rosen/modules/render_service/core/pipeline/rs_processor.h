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

 #ifndef RS_CORE_PIPELINE_PROCESSOR_H
 #define RS_CORE_PIPELINE_PROCESSOR_H

#include <memory>

#include "rs_render_engine.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
class RSProcessor {
public:
    RSProcessor() = default;
    virtual ~RSProcessor() noexcept = default;

    RSProcessor(const RSProcessor&) = delete;
    void operator=(const RSProcessor&) = delete;

    virtual bool Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId);
    virtual void ProcessSurface(RSSurfaceRenderNode& node) = 0;
    virtual void ProcessDisplaySurface(RSDisplayRenderNode& node) = 0;
    virtual void PostProcess() = 0;
    virtual void SetBoundsGeometry(const std::shared_ptr<RSObjAbsGeometry>& frameGeo);
    virtual const std::shared_ptr<RSObjAbsGeometry>& GetBoundsGeometry() const;

protected:
    void CalculateMirrorAdaptiveCoefficient(float curWidth, float curHeight,
        float mirroredWidth, float mirroredHeight);

    ScreenInfo screenInfo_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    ScreenId mirroredId_ = INVALID_SCREEN_ID;
    float mirrorAdaptiveCoefficient_ = 1.0f;
    std::shared_ptr<RSRenderEngine> renderEngine_;
    std::shared_ptr<RSObjAbsGeometry> boundsGeo_ = std::make_shared<RSObjAbsGeometry>();;
    BufferRequestConfig renderFrameConfig_ {};
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_PROCESSOR_H
