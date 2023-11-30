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

#ifndef RS_RENDER_SURFACE_H
#define RS_RENDER_SURFACE_H

#include <memory>

#include "common/rs_rect.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

#include "drawing_context.h"
#include "rs_render_surface_frame.h"
#include "render_context_base.h"

namespace OHOS {
namespace Rosen {
class RSRenderSurface {
public:
    explicit RSRenderSurface() = default;
    virtual ~RSRenderSurface() = default;
    virtual bool IsValid() const = 0;
    virtual uint32_t GetQueueSize() const = 0;
    virtual std::shared_ptr<RSRenderSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp = 0, bool useAFBC = true) = 0;
    virtual bool FlushFrame(uint64_t uiTimestamp = 0) = 0;
    virtual void SetUiTimeStamp(uint64_t uiTimestamp = 0) = 0;
    virtual void SetDamageRegion(const std::vector<RectI>& rects) = 0;
    virtual int32_t GetBufferAge() = 0;
    virtual GraphicColorGamut GetColorSpace() = 0;
    virtual void SetColorSpace(GraphicColorGamut colorSpace) = 0;
    virtual void ClearBuffer() = 0;
    virtual SkCanvas* GetCanvas() = 0;
    virtual sk_sp<SkSurface> GetSurface() = 0;
    void SetRSRenderSurfaceFrame(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
    {
        frame_ = frame;
    }
    void SetRenderContext(const std::shared_ptr<RenderContextBase>& renderContext)
    {
        renderContext_ = renderContext;
    }
    void SetDrawingContext(const std::shared_ptr<DrawingContext>& drawingContext)
    {
        drawingContext_ = drawingContext;
    }
protected:
    std::shared_ptr<RSRenderSurfaceFrame> frame_;
    std::shared_ptr<RenderContextBase> renderContext_;
    std::shared_ptr<DrawingContext> drawingContext_;
};
}
} // namespace Rosen
#endif