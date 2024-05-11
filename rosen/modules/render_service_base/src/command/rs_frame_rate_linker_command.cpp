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

#include "command/rs_frame_rate_linker_command.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
void RSFrameRateLinkerCommandHelper::Destroy(RSContext& context, FrameRateLinkerId id)
{
    auto& linkerMap = context.GetMutableFrameRateLinkerMap();
    auto linker = linkerMap.GetFrameRateLinker(id);
    if (linker == nullptr) {
        return;
    }
    linkerMap.UnregisterFrameRateLinker(id);
}

void RSFrameRateLinkerCommandHelper::UpdateRange(RSContext& context, FrameRateLinkerId id,
    FrameRateRange range, bool isAnimatorStopped)
{
    ROSEN_LOGD("RSFrameRateLinkerCommandHelper::UpdateRange %{public}" PRIu64 ", {%{public}d, %{public}d, %{public}d}",
        id, range.min_, range.max_, range.preferred_);
    auto linker = context.GetMutableFrameRateLinkerMap().GetFrameRateLinker(id);
    if (linker != nullptr) {
        linker->SetExpectedRange(range);
        linker->SetAnimationIdle(isAnimatorStopped);
    }
}
} // namespace Rosen
} // namespace OHOS
