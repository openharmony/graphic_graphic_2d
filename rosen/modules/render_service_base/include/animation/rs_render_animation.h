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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_ANIMATION_H

#include <parcel.h>
#include <refbase.h>
#include "animation/rs_animation_common.h"
#include "animation/rs_animation_fraction.h"
#include "common/rs_macros.h"
#include "modifier/rs_render_property.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;

enum class AnimationState {
    INITIALIZED,
    RUNNING,
    PAUSED,
    FINISHED,
};

class RSB_EXPORT RSRenderAnimation : public Parcelable {
public:
    virtual ~RSRenderAnimation() = default;
    AnimationId GetAnimationId() const;
    void Start();
    void Finish();
    void Pause();
    void Resume();
    void SetFraction(float fraction);
    void SetReversed(bool isReversed);
    virtual bool Marshalling(Parcel& parcel) const override;
    bool Animate(int64_t time);

    bool IsStarted() const;
    bool IsRunning() const;
    bool IsPaused() const;
    bool IsFinished() const;

    void SetDuration(int value)
    {
        animationFraction_.SetDuration(value);
    }

    int GetDuration() const
    {
        return animationFraction_.GetDuration();
    }

    void SetStartDelay(int value)
    {
        animationFraction_.SetStartDelay(value);
    }

    int GetStartDelay() const
    {
        return animationFraction_.GetStartDelay();
    }

    void SetRepeatCount(int value)
    {
        animationFraction_.SetRepeatCount(value);
    }

    int GetRepeatCount() const
    {
        return animationFraction_.GetRepeatCount();
    }

    void SetSpeed(float value)
    {
        animationFraction_.SetSpeed(value);
    }

    float GetSpeed() const
    {
        return animationFraction_.GetSpeed();
    }

    void SetAutoReverse(bool value)
    {
        animationFraction_.SetAutoReverse(value);
    }

    bool GetAutoReverse() const
    {
        return animationFraction_.GetAutoReverse();
    }

    void SetFillMode(const FillMode& value)
    {
        animationFraction_.SetFillMode(value);
    }

    const FillMode& GetFillMode() const
    {
        return animationFraction_.GetFillMode();
    }

    void SetDirection(bool isForward)
    {
        animationFraction_.SetDirection(isForward);
    }

    bool GetDirection() const
    {
        return animationFraction_.GetDirection();
    }

    void Attach(RSRenderNode* renderNode);
    void Detach();
    RSRenderNode* GetTarget() const;

    NodeId GetTargetId() const;

    virtual PropertyId GetPropertyId() const;

    virtual void AttachRenderProperty(const std::shared_ptr<RSRenderPropertyBase>& property) {};

    void SetStartTime(int64_t);

protected:
    explicit RSRenderAnimation(AnimationId id);
    RSRenderAnimation() = default;
    virtual bool ParseParam(Parcel& parcel);
    void SetFractionInner(float fraction);

    virtual void OnSetFraction(float fraction);

    virtual void OnAttach() {}

    virtual void OnDetach() {}

    virtual void OnInitialize() {}

    virtual void OnAnimate(float fraction) {}

    virtual void OnRemoveOnCompletion() {}

    void FinishOnCurrentPosition();

    RSAnimationFraction animationFraction_;
private:
    void ProcessFillModeOnStart(float startFraction);

    void ProcessFillModeOnFinish(float endFraction);

    AnimationId id_ = 0;
    NodeId targetId_ = 0;
    AnimationState state_ { AnimationState::INITIALIZED };
    bool needUpdateStartTime_ { true };
    bool needInitialize_ { true};
    RSRenderNode* target_ { nullptr };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_ANIMATION_H
