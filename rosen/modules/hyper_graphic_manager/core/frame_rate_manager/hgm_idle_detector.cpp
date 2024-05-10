/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "hgm_idle_detector.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint64_t SURFACE_TIME_OUT = 200000000; // 200ms
    constexpr uint64_t MAX_BUFFER_COUNT = 10;
}

void HgmIdleDetector::UpdateSurfaceTime(const std::string& name, uint64_t timestamp)
{
    if (!GetAppSupportStatus() || frameTimeMap_.size() > MAX_BUFFER_COUNT) {
        if (!frameTimeMap_.empty()) {
            frameTimeMap_.clear();
        }
        return;
    }
    if (name.empty()) {
        return;
    }
    frameTimeMap_[name] = timestamp;
}

bool HgmIdleDetector::GetSurFaceIdleState(uint64_t timestamp)
{
    bool idle = true;

    if (frameTimeMap_.empty()) {
        return idle;
    }

    for (auto it = frameTimeMap_.begin(); it != frameTimeMap_.end();) {
        if ((timestamp - it->second) > SURFACE_TIME_OUT) {
            it = frameTimeMap_.erase(it);
        } else {
            idle = false;
            it++;
        }
    }

    return idle;
}

} // namespace Rosen
} // namespace OHOS