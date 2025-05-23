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

#ifndef RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
#define RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H

#include <surface.h>

#include "platform/drawing/rs_surface.h"

namespace OHOS {
namespace Rosen {
class RenderContext;
class RSSurfaceOhos : public RSSurface {
public:
    RSSurfaceOhos(const sptr<Surface>& producer) : producer_(producer)
    {
    }

    sptr<Surface> GetSurface() const
    {
        return producer_;
    }

    virtual RenderContext* GetRenderContext() override;
    virtual void SetRenderContext(RenderContext* context) override;
    virtual void SetColorSpace(GraphicColorGamut colorSpace) override;
    virtual GraphicColorGamut GetColorSpace() const override;
    virtual uint32_t GetQueueSize() const override;

    virtual void SetSurfaceBufferUsage(uint64_t usage) = 0;
    virtual void SetSurfacePixelFormat(int32_t pixelFormat) = 0;
    virtual sptr<SurfaceBuffer> GetCurrentBuffer() = 0;
    void ClearAllBuffer() override;
    virtual void SetTimeOut(int32_t timeOut);
protected:
    sptr<Surface> producer_;
    RenderContext* context_ = nullptr;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    int32_t pixelFormat_ = GRAPHIC_PIXEL_FMT_RGBA_8888;
    uint64_t bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
    int32_t timeOut_ = 6000; // ms
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
