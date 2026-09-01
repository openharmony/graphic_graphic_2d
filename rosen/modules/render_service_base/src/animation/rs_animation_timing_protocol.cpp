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

#include "animation/rs_animation_timing_protocol.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
FillMode SafeCastFillMode(int32_t val, FillMode defaultVal)
{
    if (val < 0 || val > static_cast<int32_t>(FillMode::BOTH)) {
        ROSEN_LOGE("SafeCastFillMode: invalid value %{public}d, using default", val);
        return defaultVal;
    }
    return static_cast<FillMode>(val);
}

ComponentScene SafeCastComponentScene(int32_t val, ComponentScene defaultVal)
{
    if (val < 0 || val > static_cast<int32_t>(ComponentScene::SWIPER_FLING)) {
        ROSEN_LOGE("SafeCastComponentScene: invalid value %{public}d, using default", val);
        return defaultVal;
    }
    return static_cast<ComponentScene>(val);
}

RSAnimationTimingProtocol::~RSAnimationTimingProtocol() = default;
const RSAnimationTimingProtocol RSAnimationTimingProtocol::DEFAULT = RSAnimationTimingProtocol(300);
const RSAnimationTimingProtocol RSAnimationTimingProtocol::IMMEDIATE = RSAnimationTimingProtocol(0);
} // namespace Rosen
} // namespace OHOS
