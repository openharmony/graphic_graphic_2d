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

#include "animation/rs_render_path_animation.h"

#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSRenderPathAnimation::RSRenderPathAnimation(AnimationId id, const RSAnimatableProperty& property,
    const Vector2f& originPosition, float originRotation, const std::shared_ptr<RSPath>& animationPath)
    : RSRenderPropertyAnimation(id, property, originPosition), originRotation_(originRotation),
      animationPath_(animationPath)
{}
#ifdef ROSEN_OHOS
bool RSRenderPathAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation<Vector2f>::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderPathAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(parcel.WriteFloat(originRotation_) && parcel.WriteFloat(beginFraction_) && parcel.WriteFloat(endFraction_) &&
            RSMarshallingHelper::Marshalling(parcel, *animationPath_) &&
            parcel.WriteInt32(static_cast<std::underlying_type<RotationMode>::type>(rotationMode_)) &&
            interpolator_->Marshalling(parcel))) {
        ROSEN_LOGE("RSRenderPathAnimation::Marshalling, write failed");
        return false;
    }
    return true;
}

RSRenderPathAnimation* RSRenderPathAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderPathAnimation* renderPathAnimation = new RSRenderPathAnimation();

    if (!renderPathAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderPathAnimation::Unmarshalling, Parse RenderProperty Fail");
        delete renderPathAnimation;
        return nullptr;
    }
    return renderPathAnimation;
}
bool RSRenderPathAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation<Vector2f>::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Parse RenderProperty Fail");
        return false;
    }

    int32_t rotationMode;
    std::shared_ptr<RSPath> path;
    if (!(parcel.ReadFloat(originRotation_) && parcel.ReadFloat(beginFraction_) && parcel.ReadFloat(endFraction_) &&
            RSMarshallingHelper::Unmarshalling(parcel, path) && parcel.ReadInt32(rotationMode))) {
        ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Parse PathAnimation Failed");
        return false;
    }

    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    if (interpolator == nullptr) {
        ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Unmarshalling interpolator failed");
        return false;
    }
    SetInterpolator(interpolator);
    SetRotationMode(static_cast<RotationMode>(rotationMode));
    return true;
}
#endif
void RSRenderPathAnimation::SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator)
{
    interpolator_ = interpolator;
}

const std::shared_ptr<RSInterpolator>& RSRenderPathAnimation::GetInterpolator() const
{
    return interpolator_;
}

void RSRenderPathAnimation::SetRotationMode(const RotationMode& rotationMode)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to enable rotate, path animation has started!");
        return;
    }

    rotationMode_ = rotationMode;
}

RotationMode RSRenderPathAnimation::GetRotationMode() const
{
    return rotationMode_;
}

void RSRenderPathAnimation::SetBeginFraction(float fraction)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set begin fraction, path animation has started!");
        return;
    }

    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction > endFraction_) {
        ROSEN_LOGE("Failed to set begin fraction, invalid value:%f", fraction);
        return;
    }

    beginFraction_ = fraction;
}

float RSRenderPathAnimation::GetBeginFraction() const
{
    return beginFraction_;
}

void RSRenderPathAnimation::SetEndFraction(float fraction)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set end fraction, path animation has started!");
        return;
    }

    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction < beginFraction_) {
        ROSEN_LOGE("Failed to set end fraction, invalid value:%f", fraction);
        return;
    }

    endFraction_ = fraction;
}

float RSRenderPathAnimation::GetEndFraction() const
{
    return endFraction_;
}

void RSRenderPathAnimation::OnAnimate(float fraction)
{
    if (animationPath_ == nullptr) {
        ROSEN_LOGE("Failed to animate motion path, path is null!");
        return;
    }

#ifdef ROSEN_OHOS
    float distance = animationPath_->GetDistance();
    float progress = GetBeginFraction() * (FRACTION_MAX - fraction) + GetEndFraction() * fraction;
    Vector2f position;
    float tangent = 0;
    animationPath_->GetPosTan(distance * progress, position, tangent);
    SetPathValue(position, tangent);
#endif
}

void RSRenderPathAnimation::OnRemoveOnCompletion()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to remove on completion, target is null!");
        return;
    }

    target->GetRenderProperties().SetRotation(originRotation_);
    RSRenderPropertyAnimation::OnRemoveOnCompletion();
}

void RSRenderPathAnimation::SetPathValue(const Vector2f& position, float tangent)
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to set path value, target is null!");
        return;
    }

    switch (GetRotationMode()) {
        case RotationMode::ROTATE_AUTO:
            target->GetRenderProperties().SetRotation(tangent);
            break;
        case RotationMode::ROTATE_AUTO_REVERSE:
            target->GetRenderProperties().SetRotation(tangent + 180.0f);
            break;
        case RotationMode::ROTATE_NONE:
            break;
        default:
            ROSEN_LOGE("Unknow rotate mode!");
            break;
    }

    SetPropertyValue(position);
}
} // namespace Rosen
} // namespace OHOS
