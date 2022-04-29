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

#ifndef RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
#define RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H

#include <surface.h>

#include "platform/drawing/rs_surface.h"
#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {
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
    virtual void SetColorSpace(ColorGamut colorSpace) override;
    virtual ColorGamut GetColorSpace() const override;

    virtual void SetSurfaceBufferUsage(int32_t usage) = 0;
protected:
    sptr<Surface> producer_;
    RenderContext* context_ = nullptr;
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    int32_t bufferUsage_ = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
