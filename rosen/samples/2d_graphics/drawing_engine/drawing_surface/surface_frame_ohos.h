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

#ifndef SURFACE_FRAME_OHOS_H
#define SURFACE_FRAME_OHOS_H

#include <display_type.h>
#include <surface.h>
#include "surface_frame.h"

namespace OHOS {
namespace Rosen {
class SurfaceFrameOhos : public SurfaceFrame {
public:
    SurfaceFrameOhos() = default;
    SurfaceFrameOhos(int32_t width, int32_t height)
        : width_(width), height_(height), releaseFence_(-1)
    {
    }

    ~SurfaceFrameOhos() = default;

    int32_t GetReleaseFence() const
    {
        return releaseFence_;
    }

    void SetReleaseFence(const int32_t& fence)
    {
        releaseFence_ = fence;
    }

    int32_t GetWidth() const
    {
        return width_;
    }

    int32_t GetHeight() const
    {
        return height_;
    }
protected:
    int32_t width_ = -1;
    int32_t height_ = -1;
    int32_t releaseFence_;
};
} // namespace Rosen
} // namespace OHOS
#endif
