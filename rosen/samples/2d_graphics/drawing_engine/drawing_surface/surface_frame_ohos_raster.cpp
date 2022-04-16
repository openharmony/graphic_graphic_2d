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

#include "surface_frame_ohos_raster.h"

#include <cstdint>

namespace OHOS {
namespace Rosen {
SurfaceFrameOhosRaster::SurfaceFrameOhosRaster(int32_t width, int32_t height)
    : buffer_(nullptr)
{
    requestConfig_.width = width;
    requestConfig_.height = height;
    flushConfig_.damage.w = width;
    flushConfig_.damage.h = height;
}

SurfaceFrameOhosRaster::~SurfaceFrameOhosRaster()
{
}

void SurfaceFrameOhosRaster::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    flushConfig_.damage.x = left;
    flushConfig_.damage.y = top;
    flushConfig_.damage.w = width;
    flushConfig_.damage.h = height;
}

void SurfaceFrameOhosRaster::SetColorSpace(ColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

ColorGamut SurfaceFrameOhosRaster::GetColorSpace() const
{
    return colorSpace_;
}
} // namespace Rosen
} // namespace OHOS