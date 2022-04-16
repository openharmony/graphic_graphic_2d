/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SURFACE_FRAME_OHOS_RASTER_H
#define SURFACE_FRAME_OHOS_RASTER_H

#include <display_type.h>
#include <surface.h>
#include "surface_type.h"
#include "surface_frame.h"
#include "surface_frame_ohos.h"

namespace OHOS {
namespace Rosen {
class SurfaceFrameOhosRaster : public SurfaceFrameOhos {
public:
    SurfaceFrameOhosRaster(int32_t width, int32_t height);
    ~SurfaceFrameOhosRaster() override;
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override;
    void SetColorSpace(ColorGamut colorSpace) override;
    ColorGamut GetColorSpace() const override;
    friend class SurfaceOhosRaster;
    sptr<SurfaceBuffer> GetBuffer() const
    {
        return buffer_;
    }
private:
    sptr<SurfaceBuffer> buffer_;
    BufferRequestConfig requestConfig_ = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
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
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
};
} // namespace Rosen
} // namespace OHOS

#endif