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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_FRAME_STABILITY_TYPES_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_FRAME_STABILITY_TYPES_H

#include <cstdint>

namespace OHOS {
namespace Rosen {
constexpr uint32_t MIN_STABLE_DURATION = 100;
constexpr uint32_t MAX_STABLE_DURATION = 5000;
constexpr float MIN_CHANGE_PERCENT = 0.0f;
constexpr float MAX_CHANGE_PERCENT = 1.0f;
enum class FrameStabilityErrorCode : int32_t {
    SUCCESS = 0,
    NULL_CALLBACK,
    TARGET_ID_ALREADY_REGISTERED,
    TARGET_ID_NOT_REGISTERED,
    CONNECTIONS_OCCUPIED,
    INVALID_ID,
    UNKNOWN
};

enum class FrameStabilityTargetType : uint32_t {
    SCREEN = 0,
    WINDOW = 1
};

struct FrameStabilityConfig {
    // Stable duration in milliseconds (50ms-500ms)
    uint32_t stableDuration = 200;
    // Change percentage threshold (0-1.f)
    float changePercent = 0.0f;
};

// Target for frame stability detection or collection, identified by an ID and type
// For screen type, the ID is the screenId; for window type, the ID is the surfaceNodeId
// Currently, only the main screen and virtual extension screen are supported
struct FrameStabilityTarget {
    uint64_t id = 0;
    FrameStabilityTargetType type = FrameStabilityTargetType::SCREEN;
};
} // namespace namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_FRAME_STABILITY_TYPES_H
