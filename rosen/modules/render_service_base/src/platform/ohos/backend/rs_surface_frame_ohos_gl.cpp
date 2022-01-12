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

#include "rs_surface_frame_ohos_gl.h"
#include "platform/common/rs_log.h"
#include "render_context/render_context.h"

#include <hilog/log.h>
#include "pipeline/rs_render_thread.h"

namespace OHOS {
namespace Rosen {

RSSurfaceFrameOhosGl::RSSurfaceFrameOhosGl(int32_t width, int32_t height)
    : width_(width), height_(height)
{
}

void RSSurfaceFrameOhosGl::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    renderContext_->DamageFrame(left, top, width, height);
}

SkCanvas* RSSurfaceFrameOhosGl::GetCanvas()
{
    return renderContext_->AcquireCanvas(width_, height_);
}

int32_t RSSurfaceFrameOhosGl::GetReleaseFence() const
{
    return releaseFence_;
}

void RSSurfaceFrameOhosGl::SetReleaseFence(const int32_t& fence)
{
    releaseFence_ = fence;
}
} // namespace Rosen
} // namespace OHOS
