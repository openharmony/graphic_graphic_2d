/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_common_def.h"
#include "modifier/rs_modifier_type.h"

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []() -> int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

namespace OHOS {
namespace Rosen {
class RSNode;
class AnimationFinishCallback;
class RSRenderAnimation;

class RSC_EXPORT RSAnimation : public RSAnimationTimingProtocol, public std::enable_shared_from_this<RSAnimation> {
public:
    virtual ~RSAnimation() = default;

    AnimationId GetId() const;

    void SetFinishCallback(const std::function<void()>& finishCallback);

    void Start(const std::shared_ptr<RSNode>& target);

    const std::weak_ptr<RSNode> GetTarget() const;

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
    virtual PropertyId GetPropertyId() const;

    void StartInner(const std::shared_ptr<RSNode>& target);
    bool IsReversed() const;
    void CallFinishCallback();
    void UpdateParamToRenderAnimation(const std::shared_ptr<RSRenderAnimation>& animation);
    virtual void OnCallFinishCallback() {}
    virtual void SetPropertyOnAllAnimationFinish() {}

private:
    static AnimationId GenerateId();
    const AnimationId id_;

    void SetFinishCallback(const std::shared_ptr<AnimationFinishCallback>& finishCallback);
    void UpdateStagingValue(bool isFirstStart);
    void UIAnimationFinish();

    bool isReversed_ { false };
    AnimationState state_ { AnimationState::INITIALIZED };
    std::weak_ptr<RSNode> target_;
    std::shared_ptr<AnimationFinishCallback> finishCallback_;

    friend class RSCurveImplicitAnimParam;
    friend class RSAnimationGroup;
    friend class RSNode;
    friend class RSImplicitAnimator;
    friend class RSUIAnimationManager;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_H
