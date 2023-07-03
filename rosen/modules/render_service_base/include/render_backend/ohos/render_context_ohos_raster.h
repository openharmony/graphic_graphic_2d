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

#ifndef RENDER_SERVICE_RENDER_CONTEXT_OHOS_RASTER_H
#define RENDER_SERVICE_RENDER_CONTEXT_OHOS_RASTER_H

#include <memory>
#include <mutex>

#include "render_context_base.h"

namespace OHOS {
namespace Rosen {
class RenderContextOhosRaster : public RenderContextBase {
public:
    explicit RenderContextOhosRaster() noexcept = default;
    ~RenderContextOhosRaster();
    void Init() override;
    bool CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    void DamageFrame(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    void SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
};
}
}
#endif