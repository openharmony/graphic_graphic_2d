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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_PROTOCOL_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_PROTOCOL_H

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
enum class RS_EXPORT FillMode { FORWARDS = 0, BACKWARDS, BOTH, NONE };

class RS_EXPORT RSAnimationTimingProtocol {
public:
    RSAnimationTimingProtocol() = default;
    virtual ~RSAnimationTimingProtocol() = default;

    void SetDuration(int duration)
    {
        duration_ = duration;
    }

    void SetStartDelay(int startDelay)
    {
        startDelay_ = startDelay;
    }

    void SetSpeed(float speed)
    {
        speed_ = speed;
    }

    void SetRepeatCount(int repeatCount)
    {
        repeatCount_ = repeatCount;
    }

    void SetAutoReverse(bool autoReverse)
    {
        autoReverse_ = autoReverse;
    }

    void SetFillMode(const FillMode& fillMode)
    {
        fillMode_ = fillMode;
    }

    void SetDirection(bool isForward)
    {
        isForward_ = isForward;
    }

    int GetDuration() const
    {
        return duration_;
    }

    int GetStartDelay() const
    {
        return startDelay_;
    }

    float GetSpeed() const
    {
        return speed_;
    }

    int GetRepeatCount() const
    {
        return repeatCount_;
    }

    bool GetAutoReverse() const
    {
        return autoReverse_;
    }

    const FillMode& GetFillMode() const
    {
        return fillMode_;
    }

    bool GetDirection() const
    {
        return isForward_;
    }

protected:
    int duration_ { 300 };
    int startDelay_ { 0 };
    float speed_ { 1.0f };
    int repeatCount_ { 1 };
    bool autoReverse_ { false };
    FillMode fillMode_ { FillMode::FORWARDS };
    bool isForward_ { true };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_PROTOCOL_H
