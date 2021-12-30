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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_H

#include <memory>
#include <unistd.h>

#include "animation/rs_animation_common.h"
#include "animation/rs_animatable_property.h"
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSPropertyNode;
class AnimationFinishCallback;

class RS_EXPORT RSAnimation : public RSAnimationTimingProtocol, public std::enable_shared_from_this<RSAnimation> {
public:
    virtual ~RSAnimation() = default;

    AnimationId GetId() const;

    void SetFinishCallback(const std::function<void()>& finishCallback);

    void Start(const std::shared_ptr<RSPropertyNode>& target);

    const std::weak_ptr<RSPropertyNode> GetTarget() const;

    void Pause();

    void Resume();

    void Finish();

    void Reverse();

    void SetFraction(float fraction);

    bool IsStarted() const;

    bool IsRunning() const;

    bool IsPaused() const;

    bool IsFinished() const;

protected:
    enum class AnimationState {
        INITIALIZED,
        RUNNING,
        PAUSED,
        FINISHED,
    };

    RSAnimation();

    virtual void OnStart() {}
    virtual void OnReverse();
    virtual void OnPause();
    virtual void OnResume();
    virtual void OnFinish();
    virtual void OnSetFraction(float fraction);
    virtual void OnUpdateStagingValue(bool isFirstStart) {};
    virtual RSAnimatableProperty GetProperty() const;

    void StartInner(const std::shared_ptr<RSPropertyNode>& target);
    bool IsReversed() const;
    void CallFinishCallback();

private:
    void SetFinishCallback(const std::shared_ptr<AnimationFinishCallback>& finishCallback);
    void UpdateStagingValue(bool isFirstStart);

    static inline pid_t pid_ = getpid();
    static inline std::atomic<uint32_t> currentId_ = 0;

    AnimationId id_;
    bool isReversed_ { false };
    AnimationState state_ { AnimationState::INITIALIZED };
    std::weak_ptr<RSPropertyNode> target_;
    std::shared_ptr<AnimationFinishCallback> finishCallback_;

    friend class RSCurveImplicitAnimParam;
    friend class RSAnimationGroup;
    friend class RSPropertyNode;
    friend class RSImplicitAnimator;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_H
