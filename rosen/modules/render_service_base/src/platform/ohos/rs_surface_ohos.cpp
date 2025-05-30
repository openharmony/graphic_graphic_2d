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

#include "rs_surface_frame_ohos.h"
#include "rs_surface_ohos.h"
namespace OHOS {
namespace Rosen {
RenderContext* RSSurfaceOhos::GetRenderContext()
{
    return context_;
}

void RSSurfaceOhos::SetRenderContext(RenderContext* context)
{
    context_ = context;
}

void RSSurfaceOhos::SetColorSpace(GraphicColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

GraphicColorGamut RSSurfaceOhos::GetColorSpace() const
{
    return colorSpace_;
}

uint32_t RSSurfaceOhos::GetQueueSize() const
{
    if (producer_ == nullptr) {
        return 0;
    }
    return producer_->GetQueueSize();
}

void RSSurfaceOhos::ClearAllBuffer()
{
    if (producer_ != nullptr) {
        producer_->Disconnect();
    }
}

void RSSurfaceOhos::SetTimeOut(int32_t timeOut)
{
    timeOut_ = timeOut;
}
} // namespace Rosen
} // namespace OHOS
