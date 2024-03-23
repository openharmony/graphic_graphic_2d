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

#ifndef RENDER_SERVICE_BASE_SRC_PLATFORM_DARWIN_RS_SURFACE_DARWIN_H
#define RENDER_SERVICE_BASE_SRC_PLATFORM_DARWIN_RS_SURFACE_DARWIN_H

#include <memory>

#include "image/gpu_context.h"

#include "common/rs_common_def.h"
#include "platform/drawing/rs_surface.h"
#include "platform/drawing/rs_surface_frame.h"

namespace OHOS {
namespace Rosen {
class RenderContext;
using OnRenderFunc = bool (*)(const void*, const size_t, const int32_t, const int32_t, const uint64_t);
class RSSurfaceDarwin : public RSSurface {
public:
    explicit RSSurfaceDarwin(OnRenderFunc onRender);
    ~RSSurfaceDarwin() override;

    bool IsValid() const override;

    void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp = 0) override;

    std::unique_ptr<RSSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC = true) override;

    bool FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;
    RenderContext* GetRenderContext() override;
    void SetRenderContext(RenderContext* context) override;
    uint32_t GetQueueSize() const override;
    void ClearBuffer() override;
    void ClearAllBuffer() override;
    void ResetBufferAge() override;
    GraphicColorGamut GetColorSpace() const override
    {
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    }

    void SetColorSpace(GraphicColorGamut colorSpace) override
    {
    }

private:
    void YInvert(void *addr, int32_t width, int32_t height);
    bool SetupGrContext();

    RenderContext* renderContext_ = nullptr;
    std::shared_ptr<Drawing::GPUContext> grContext_ = nullptr;
    std::shared_ptr<Drawing::ColorSpace> drColorSpace_ = nullptr;
    OnRenderFunc onRender_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_SRC_PLATFORM_DARWIN_RS_SURFACE_DARWIN_H
