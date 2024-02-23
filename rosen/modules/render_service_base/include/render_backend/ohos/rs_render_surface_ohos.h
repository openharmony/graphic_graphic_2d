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

#ifndef RS_RENDER_SURFACE_OHOS_H
#define RS_RENDER_SURFACE_OHOS_H

#include <memory>

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

#include "common/rs_rect.h"

#include "render_backend/rs_render_surface.h"
#include "render_backend/rs_render_surface_frame.h"

namespace OHOS {
namespace Rosen {
class RSRenderSurfaceOhos : public RSRenderSurface {
public:
    explicit RSRenderSurfaceOhos(const sptr<Surface>& surface, const std::shared_ptr<DrawingContext>& drawingContext);
    ~RSRenderSurfaceOhos();
    bool IsValid() const override;
    sptr<Surface> GetSurfaceOhos() const;
    uint32_t GetQueueSize() const override;
    std::shared_ptr<RSRenderSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp = 0, bool useAFBC = true) override;
    bool FlushFrame(uint64_t uiTimestamp = 0) override;
    void SetUiTimeStamp(uint64_t uiTimestamp = 0) override;
    void SetDamageRegion(const std::vector<RectI> &rects) override;
    int32_t GetBufferAge() override;
    void ClearBuffer() override;
    SkCanvas* GetCanvas() override;
    sk_sp<SkSurface> GetSurface() override;
    GraphicColorGamut GetColorSpace() override;
    void SetColorSpace(GraphicColorGamut colorSpace) override;
    void SetSurfaceBufferUsage(uint64_t usage);
    void SetSurfacePixelFormat(uint64_t pixelFormat);
private:
    void RenderFrame();
    void SetReleaseFence(const int32_t& fence);
    int32_t GetReleaseFence() const;
};
}
} // namespace Rosen
#endif
