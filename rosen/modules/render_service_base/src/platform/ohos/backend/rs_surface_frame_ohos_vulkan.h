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

#ifndef RS_SURFACE_FRAME_OHOS_VULKAN_H
#define RS_SURFACE_FRAME_OHOS_VULKAN_H

#include <surface.h>

#include "include/gpu/GrDirectContext.h"

#include "platform/ohos/rs_surface_frame_ohos.h"

namespace OHOS {
namespace Rosen {

class RSSurfaceFrameOhosVulkan : public RSSurfaceFrameOhos {
public:
    RSSurfaceFrameOhosVulkan(std::shared_ptr<Drawing::Surface> surface, int32_t width,
        int32_t height, int32_t bufferAge);
    ~RSSurfaceFrameOhosVulkan() override = default;

    Drawing::Canvas* GetCanvas() override;
    std::shared_ptr<Drawing::Surface> GetSurface() override;
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override;
    void SetDamageRegion(const std::vector<RectI>& rects) override;
    int32_t GetReleaseFence() const;
    void SetReleaseFence(const int32_t& fence);
    int32_t GetBufferAge() const override;

private:
    int32_t releaseFence_ = 0;
    std::shared_ptr<Drawing::Surface> surface_;
    int width_ = 0;
    int height_ = 0;
    int32_t bufferAge_ = -1;
    void CreateCanvas();
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_FRAME_OHOS_H
