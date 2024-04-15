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

#include "pipeline/rs_render_frame_rate_linker_map.h"
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSRenderFrameRateLinkerMap::RSRenderFrameRateLinkerMap()
{
}

bool RSRenderFrameRateLinkerMap::RegisterFrameRateLinker(const std::shared_ptr<RSRenderFrameRateLinker>& linkerPtr)
{
    FrameRateLinkerId id = linkerPtr->GetId();
    if (frameRateLinkerMap_.count(id)) {
        return false;
    }
    frameRateLinkerMap_.emplace(id, linkerPtr);
    return true;
}

void RSRenderFrameRateLinkerMap::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    frameRateLinkerMap_.erase(id);
}

void RSRenderFrameRateLinkerMap::FilterFrameRateLinkerByPid(pid_t pid)
{
    ROSEN_LOGD("RSRenderFrameRateLinkerMap::FilterFrameRateLinkerByPid removing all linker belong to pid %{public}llu",
        (unsigned long long)pid);
    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    EraseIf(frameRateLinkerMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });
}

std::shared_ptr<RSRenderFrameRateLinker> RSRenderFrameRateLinkerMap::GetFrameRateLinker(FrameRateLinkerId id)
{
    return frameRateLinkerMap_.count(id) ? frameRateLinkerMap_[id] : nullptr;
}
} // namespace Rosen
} // namespace OHOS
