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

 #ifndef RS_PROCESSOR_H
 #define RS_PROCESSOR_H

#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include <surface.h>

#include "hdi_output.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/screen_types.h"

#include "platform/drawing/rs_surface_frame.h"

#include "platform/ohos/rs_surface_ohos.h"
#ifdef RS_ENABLE_GL
#include "render_context/render_context.h"
#include "rs_egl_image_manager.h"
#endif // RS_ENABLE_GL

namespace OHOS {
namespace Rosen {

class RSProcessor {
public:
    using SpecialTask = std::function<void()>;
    RSProcessor() {}
    virtual ~RSProcessor() {}
    virtual void ProcessSurface(RSSurfaceRenderNode &node) = 0;
    virtual void Init(ScreenId id, int32_t offsetX, int32_t offsetY) = 0;
    virtual void PostProcess() = 0;

protected:
    SkCanvas* CreateCanvas(
        const std::shared_ptr<RSSurfaceOhos>& surface,
        const BufferRequestConfig& requestConfig);
    bool ConsumeAndUpdateBuffer(RSSurfaceRenderNode& node, SpecialTask& task, sptr<SurfaceBuffer>& buffer);
    void SetBufferTimeStamp();
    int32_t GetOffsetX();
    int32_t GetOffsetY();
    void DropFrameProcess(RSSurfaceRenderNode& node);
    
#ifdef RS_ENABLE_GL
    std::shared_ptr<RenderContext> renderContext_;
    std::shared_ptr<RSEglImageManager> eglImageManager_;
#endif // RS_ENABLE_GL
    std::unique_ptr<RSSurfaceFrame> currFrame_;
    std::shared_ptr<RSSurfaceOhos> rsSurface_;
private:
    sptr<SurfaceBuffer> buffer_;
    int32_t releaseFence_ = -1;
};
} // namespace Rosen
} // namespace OHOS

 #endif