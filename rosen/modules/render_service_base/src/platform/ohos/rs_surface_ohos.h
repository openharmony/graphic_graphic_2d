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

namespace OHOS {
namespace Rosen {

class RSSurfaceOhos : public RSSurface {
public:
    RSSurfaceOhos() = default;
    explicit RSSurfaceOhos(const sptr<Surface>& producer);
    ~RSSurfaceOhos() = default;

    bool IsValid() const override
    {
        return producer_ != nullptr;
    }

    std::unique_ptr<RSSurfaceFrame> RequestFrame(int32_t width, int32_t height) override;
    bool FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame) override;

private:
    sptr<Surface> producer_;
    friend class RSSurfaceConverter;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
