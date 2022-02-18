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
        producer_->SetQueueSize(5); // set buffer queue size to 5
    }

    sptr<Surface> GetSurface() const
    {
        return producer_;
    }

    virtual RenderContext* GetRenderContext() override;
    virtual void SetRenderContext(RenderContext* context) override;
    virtual void SetColorSpace(SurfaceColorGamut colorSpace) override;
    virtual SurfaceColorGamut GetColorSpace() override;
protected:
    sptr<Surface> producer_;
    RenderContext* context_;
    SurfaceColorGamut colorSpace_ = SurfaceColorGamut::COLOR_GAMUT_SRGB;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
