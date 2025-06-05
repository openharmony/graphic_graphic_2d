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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_COMMON_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_COMMON_H

#include <cstdint>

namespace OHOS {
namespace Rosen {
static constexpr float FRACTION_MIN = 0.0f;
static constexpr float FRACTION_MAX = 1.0f;
static constexpr float EPSILON = 1e-6;
static constexpr int64_t MS_TO_NS = 1000000;
static constexpr int64_t SECOND_TO_MS = 1000;
static constexpr int32_t MAX_FRAME_TIME_FRACTION = 120;

enum class RotationMode {
    ROTATE_NONE,
    ROTATE_AUTO,
    ROTATE_AUTO_REVERSE
};

enum class RSInteractiveAnimationPosition : uint8_t {
    START,
    CURRENT,
    END,
};

enum class ImplicitAnimationParamType {
    INVALID,
    CURVE,
    KEYFRAME,
    PATH,
    SPRING,
    INTERPOLATING_SPRING,
    TRANSITION,
    CANCEL
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_COMMON_H
