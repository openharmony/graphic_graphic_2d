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

#ifndef RS_SURFACE_OHOS_RASTER_H
#define RS_SURFACE_OHOS_RASTER_H

#include <surface.h>

#include "platform/drawing/rs_surface.h"
#include "platform/ohos/rs_surface_ohos.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceOhosRaster : public RSSurfaceOhos {
public:
    explicit RSSurfaceOhosRaster(const sptr<Surface>& producer);
    ~RSSurfaceOhosRaster() = default;

    bool IsValid() const override
    {
        return producer_ != nullptr;
    }

    std::unique_ptr<RSSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC = true, bool isProtected = false) override;
    bool FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;

    void SetSurfaceBufferUsage(uint64_t usage) override;
    void SetSurfacePixelFormat(int32_t pixelFormat) override;
    void ClearBuffer() override;
    void ResetBufferAge() override;
    void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
