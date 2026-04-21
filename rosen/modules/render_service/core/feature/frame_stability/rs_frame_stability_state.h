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

#ifndef RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_STATE_H
#define RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_STATE_H

#include <chrono>
#include <memory>

#include "common/rs_occlusion_region.h"
#include "ipc_callbacks/rs_iframe_stability_callback.h"
#include "transaction/rs_frame_stability_types.h"

namespace OHOS {
namespace Rosen {
enum class DetectionState {
    INIT,
    STABLE,
    NOTSTABLE
};

struct DetectorContext {
    FrameStabilityConfig config;
    sptr<RSIFrameStabilityCallback> callback;
    DetectionState state = DetectionState::INIT;
    Occlusion::Region accumulatedDirtyRegion;
    std::chrono::steady_clock::time_point startTime;
    float screenArea = 0;
    bool hasPendingStabilityTask = false;
    int32_t pid = 0;
    FrameStabilityTargetType targetType = FrameStabilityTargetType::SCREEN;
};

struct CollectorContext {
    FrameStabilityConfig config;
    Occlusion::Region collectionDirtyRegion;
    float screenArea = 0;
    int32_t pid = 0;
    FrameStabilityTargetType targetType = FrameStabilityTargetType::SCREEN;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_STATE_H