/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_MANAGER_H
#define RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_MANAGER_H

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "common/rs_common_def.h"
#include "ipc_callbacks/rs_iframe_stability_callback.h"
#include "params/rs_screen_render_params.h"
#include "transaction/rs_frame_stability_types.h"

namespace OHOS {
namespace Rosen {
class RSFrameStabilityManager {
public:
    static RSFrameStabilityManager& GetInstance();

    int32_t RegisterFrameStabilityDetection(
        pid_t pid,
        const FrameStabilityTarget& target,
        const FrameStabilityConfig& config,
        sptr<RSIFrameStabilityCallback> callback
    );

    int32_t UnregisterFrameStabilityDetection(pid_t pid, const FrameStabilityTarget& target);

    int32_t StartFrameStabilityCollection(
        pid_t pid,
        const FrameStabilityTarget& target,
        const FrameStabilityConfig& config
    );

    int32_t GetFrameStabilityResult(pid_t pid, const FrameStabilityTarget& target, bool& result);
private:
    RSFrameStabilityManager() = default;
    ~RSFrameStabilityManager() = default;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_MANAGER_H
