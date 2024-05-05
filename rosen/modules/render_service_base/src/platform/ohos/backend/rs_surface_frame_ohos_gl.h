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

#ifndef RS_SURFACE_FRAME_OHOS_GL_H
#define RS_SURFACE_FRAME_OHOS_GL_H

#include <surface.h>

#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif

#include "platform/drawing/rs_surface_frame.h"
#include "platform/ohos/rs_surface_frame_ohos.h"

namespace OHOS {
namespace Rosen {

class RSSurfaceFrameOhosGl : public RSSurfaceFrameOhos {
public:
    RSSurfaceFrameOhosGl(int32_t width, int32_t height);
    ~RSSurfaceFrameOhosGl() = default;

    Drawing::Canvas* GetCanvas() override;
    std::shared_ptr<Drawing::Surface> GetSurface() override;
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override;
    void SetDamageRegion(const std::vector<RectI> &rects) override;
    int32_t GetBufferAge() const override;
    int32_t GetReleaseFence() const;
    void SetReleaseFence(const int32_t& fence);

private:
    int32_t releaseFence_ = 0;
    int width_ = 0;
    int height_ = 0;
    void CreateSurface();
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_FRAME_OHOS_H
