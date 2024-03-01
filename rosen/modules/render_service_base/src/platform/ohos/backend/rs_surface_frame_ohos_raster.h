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

#ifndef RS_SURFACE_FRAME_OHOS_RASTER_H
#define RS_SURFACE_FRAME_OHOS_RASTER_H

#include <surface.h>

#include "platform/drawing/rs_surface_frame.h"
#include "platform/ohos/rs_surface_frame_ohos.h"

namespace OHOS {
namespace Rosen {

class RSSurfaceFrameOhosRaster : public RSSurfaceFrameOhos {
public:
    RSSurfaceFrameOhosRaster(int32_t width, int32_t height);
    ~RSSurfaceFrameOhosRaster() = default;

    Drawing::Canvas* GetCanvas() override;
    std::shared_ptr<Drawing::Surface> GetSurface() override;

    sptr<SurfaceBuffer> GetBuffer() const
    {
        return buffer_;
    }
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override;
    void SetDamageRegion(const std::vector<RectI> &rects) override;
    int32_t GetBufferAge() const override;
    int32_t GetReleaseFence() const;
    void SetReleaseFence(const int32_t& fence);
    friend class RSSurfaceOhosRaster;
private:
    sptr<SurfaceBuffer> buffer_;
    int32_t releaseFence_ = -1;
    BufferRequestConfig requestConfig_ = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    BufferFlushConfig flushConfig_ = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = 0x100,
            .h = 0x100,
        },
    };

    void CreateSurface();
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_FRAME_OHOS_H