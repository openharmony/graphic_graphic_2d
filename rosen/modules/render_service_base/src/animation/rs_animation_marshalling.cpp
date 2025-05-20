/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_property_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition_effect.h"
#include "animation/rs_render_transition.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"
#include "platform/common/rs_log.h"
#include "rs_profiler.h"

#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr int MAX_KEYFRAME_SIZE_NUMBER = 100000;

enum RSTransitionEffectType : uint16_t {
    FADE = 1,
    SCALE,
    TRANSLATE,
    ROTATE,
    UNDEFINED,
};
} // namespace

bool RSCubicBezierInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::CUBIC_BEZIER)) {
        ROSEN_LOGE("Marshalling RSCubicBezierInterpolator type failed");
        return false;
    }
    if (!parcel.WriteUint64(id_)) {
        ROSEN_LOGE("Marshalling RSCubicBezierInterpolator id failed");
        return false;
    }
    if (!(parcel.WriteFloat(controlX1_) && parcel.WriteFloat(controlY1_) && parcel.WriteFloat(controlX2_) &&
            parcel.WriteFloat(controlY2_))) {
        ROSEN_LOGE("Marshalling RSCubicBezierInterpolator parameter failed");
        return false;
    }
    return true;
}

RSCubicBezierInterpolator* RSCubicBezierInterpolator::Unmarshalling(Parcel& parcel)
{
    uint64_t id{0};
    if (!parcel.ReadUint64(id)) {
        ROSEN_LOGE("Unmarshalling RSCubicBezierInterpolator id failed");
        return nullptr;
    }
    if (id == 0) {
        ROSEN_LOGE("Unmarshalling RSCubicBezierInterpolator id == 0");
        return nullptr;
    }
    float x1 = 0;
    float y1 = 0;
    float x2 = 0;
    float y2 = 0;
    if (!(parcel.ReadFloat(x1) && parcel.ReadFloat(y1) && parcel.ReadFloat(x2) && parcel.ReadFloat(y2))) {
        ROSEN_LOGE("Unmarshalling RSCubicBezierInterpolator parameter failed");
        return nullptr;
    }
    return new RSCubicBezierInterpolator(id, x1, y1, x2, y2);
}

RSInterpolator* RSInterpolator::UnmarshallingFromParcel(Parcel& parcel)
{
    uint16_t interpolatorType = 0;
    if (!parcel.ReadUint16(interpolatorType)) {
        ROSEN_LOGE("RSInterpolator::Unmarshalling read type failed");
        return nullptr;
    }
    RSInterpolator* ret = nullptr;
    switch (interpolatorType) {
        case InterpolatorType::LINEAR:
            ret = LinearInterpolator::Unmarshalling(parcel);
            break;
        case InterpolatorType::CUSTOM:
            ret = RSCustomInterpolator::Unmarshalling(parcel);
            break;
        case InterpolatorType::CUBIC_BEZIER:
            ret = RSCubicBezierInterpolator::Unmarshalling(parcel);
            break;
        case InterpolatorType::SPRING:
            ret = RSSpringInterpolator::Unmarshalling(parcel);
            break;
        case InterpolatorType::STEPS:
            ret = RSStepsInterpolator::Unmarshalling(parcel);
            break;
        default:
            break;
    }
    return ret;
}

std::shared_ptr<RSInterpolator> RSInterpolator::Unmarshalling(Parcel& parcel)
{
    RSInterpolator* rawInterpolator = UnmarshallingFromParcel(parcel);
    if (rawInterpolator == nullptr) {
        ROSEN_LOGE("RSInterpolator::Unmarshalling rawInterpolator is nullptr");
        return nullptr;
    }

    static std::mutex cachedInterpolatorsMutex_;
    static std::unordered_map<uint64_t, std::weak_ptr<RSInterpolator>> cachedInterpolators_;
    static const auto Destructor = [](RSInterpolator* ptr) {
        if (ptr == nullptr) {
            ROSEN_LOGE("RSInterpolator::Unmarshalling, sharePtr is nullptr.");
            return;
        }
        std::unique_lock<std::mutex> lock(cachedInterpolatorsMutex_);
        cachedInterpolators_.erase(ptr->id_); // Unregister interpolator from cache before destruction.
        delete ptr;
        ptr = nullptr;
    };

    {
        // All cache operations should be performed under lock to prevent race conditions
        std::unique_lock<std::mutex> lock(cachedInterpolatorsMutex_);
        // Check if there is an existing valid interpolator in the cache, return it if found.
        if (auto it = cachedInterpolators_.find(rawInterpolator->id_); it != cachedInterpolators_.end()) {
            if (auto sharedPtr = it->second.lock()) { // Check if weak pointer is valid
                delete rawInterpolator;               // Destroy the newly created object
                rawInterpolator = nullptr;
                return sharedPtr;
            } else { // If the weak pointer has expired, erase it from the cache. This should never happen.
                ROSEN_LOGE("RSInterpolator::Unmarshalling, cached weak pointer expired.");
                cachedInterpolators_.erase(it);
            }
        }
        // No existing interpolator in the cache, create a new one and register it in the cache.
        std::shared_ptr<RSInterpolator> sharedPtr(rawInterpolator, Destructor);
        cachedInterpolators_.emplace(rawInterpolator->id_, sharedPtr);
        return sharedPtr;
    }
}

bool LinearInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::LINEAR)) {
        ROSEN_LOGE("Marshalling LinearInterpolator type failed");
        return false;
    }
    if (!parcel.WriteUint64(id_)) {
        ROSEN_LOGE("Marshalling LinearInterpolator id failed");
        return false;
    }
    return true;
}

LinearInterpolator* LinearInterpolator::Unmarshalling(Parcel& parcel)
{
    uint64_t id{0};
    if (!parcel.ReadUint64(id)) {
        ROSEN_LOGE("Unmarshalling RSCubicBezierInterpolator id failed");
        return nullptr;
    }
    if (id == 0) {
        ROSEN_LOGE("Unmarshalling RSCubicBezierInterpolator id == 0");
        return nullptr;
    }
    return new LinearInterpolator(id);
}

bool RSCustomInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::CUSTOM)) {
        ROSEN_LOGE("RSCustomInterpolator::Marshalling, Write type failed");
        return false;
    }
    if (!parcel.WriteUint64(id_)) {
        ROSEN_LOGE("RSCustomInterpolator::Marshalling, Write id failed");
        return false;
    }
    if (!RSMarshallingHelper::MarshallingVec(parcel, times_) || !RSMarshallingHelper::MarshallingVec(parcel, values_)) {
        ROSEN_LOGE("RSCustomInterpolator::Marshalling, Write value failed");
        return false;
    }
    return true;
}

RSCustomInterpolator* RSCustomInterpolator::Unmarshalling(Parcel& parcel)
{
    uint64_t id{0};
    if (!parcel.ReadUint64(id)) {
        ROSEN_LOGE("Unmarshalling RSCustomInterpolator id failed");
        return nullptr;
    }
    std::vector<float> times;
    std::vector<float> values;
    if (!(RSMarshallingHelper::UnmarshallingVec(parcel, times, MAX_SAMPLE_POINTS) &&
        RSMarshallingHelper::UnmarshallingVec(parcel, values, MAX_SAMPLE_POINTS))) {
        ROSEN_LOGE("Unmarshalling CustomInterpolator failed");
        return nullptr;
    }
    return new RSCustomInterpolator(id, std::move(times), std::move(values));
}

bool RSRenderAnimation::Marshalling(Parcel& parcel) const
{
    // animationId, targetId
    if (!(parcel.WriteUint64(id_))) {
        ROSEN_LOGE("RSRenderAnimation::Marshalling, write id failed");
        return false;
    }
    // RSAnimationTimingProtocol
    if (!(parcel.WriteInt32(animationFraction_.GetDuration()) &&
        parcel.WriteInt32(animationFraction_.GetStartDelay()) &&
        parcel.WriteFloat(animationFraction_.GetSpeed()) &&
        parcel.WriteInt32(animationFraction_.GetRepeatCount()) &&
        parcel.WriteBool(animationFraction_.GetAutoReverse()) &&
        parcel.WriteBool(animationFraction_.GetDirection()) &&
        parcel.WriteInt32(static_cast<std::underlying_type<FillMode>::type>(animationFraction_.GetFillMode())) &&
        parcel.WriteBool(animationFraction_.GetRepeatCallbackEnable()) &&
        parcel.WriteInt32(animationFraction_.GetFrameRateRange().min_) &&
        parcel.WriteInt32(animationFraction_.GetFrameRateRange().max_) &&
        parcel.WriteInt32(animationFraction_.GetFrameRateRange().preferred_) &&
        parcel.WriteInt32(static_cast<int32_t>(animationFraction_.GetFrameRateRange().componentScene_)))) {
        ROSEN_LOGE("RSRenderAnimation::Marshalling, write param failed");
        return false;
    }
    // token for finding ui instance when executing animation callback in client
    if (!parcel.WriteUint64(token_)) {
        ROSEN_LOGE("multi-instance, RSRenderAnimation::Marshalling, write token failed");
        return false;
    }
    return true;
}

bool RSRenderAnimation::ParseParam(Parcel& parcel)
{
    int32_t duration = 0;
    int32_t startDelay = 0;
    int32_t repeatCount = 0;
    int32_t fillMode = 0;
    float speed = 0.0;
    bool autoReverse = false;
    bool direction = false;
    bool isRepeatCallbackEnable = false;
    int fpsMin = 0;
    int fpsMax = 0;
    int fpsPreferred = 0;
    int componentScene = 0;
    uint64_t token = 0;
    if (!(parcel.ReadUint64(id_) && parcel.ReadInt32(duration) && parcel.ReadInt32(startDelay) &&
            parcel.ReadFloat(speed) && parcel.ReadInt32(repeatCount) && parcel.ReadBool(autoReverse) &&
            parcel.ReadBool(direction) && parcel.ReadInt32(fillMode) && parcel.ReadBool(isRepeatCallbackEnable) &&
            parcel.ReadInt32(fpsMin) && parcel.ReadInt32(fpsMax) && parcel.ReadInt32(fpsPreferred) &&
            parcel.ReadInt32(componentScene) && parcel.ReadUint64(token))) {
        ROSEN_LOGE("RSRenderAnimation::ParseParam, read param failed");
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, id_);
    SetDuration(duration);
    SetStartDelay(startDelay);
    SetRepeatCount(repeatCount);
    SetAutoReverse(autoReverse);
    SetSpeed(speed);
    SetDirection(direction);
    SetFillMode(static_cast<FillMode>(fillMode));
    SetRepeatCallbackEnable(isRepeatCallbackEnable);
    SetFrameRateRange({fpsMin, fpsMax, fpsPreferred, 0, static_cast<ComponentScene>(componentScene)});
    SetToken(token);
    return true;
}

bool RSRenderCurveAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_) && interpolator_ != nullptr &&
            interpolator_->Marshalling(parcel))) {
        ROSEN_LOGE("RSRenderCurveAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }
    return true;
}

RSRenderCurveAnimation* RSRenderCurveAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderCurveAnimation* renderCurveAnimation = new RSRenderCurveAnimation();
    if (!renderCurveAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::Unmarshalling, failed");
        delete renderCurveAnimation;
        renderCurveAnimation = nullptr;
        return nullptr;
    }
    return renderCurveAnimation;
}

bool RSRenderCurveAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderCurveAnimation::ParseParam, Unmarshalling Fail");
        return false;
    }

    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    if (interpolator == nullptr) {
        ROSEN_LOGE("RSRenderCurveAnimation::ParseParam, Unmarshalling interpolator failed");
        return false;
    }
    SetInterpolator(interpolator);
    return true;
}

#ifdef ROSEN_OHOS
bool RSRenderInterpolatingSpringAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }

    if (!(RSMarshallingHelper::Marshalling(parcel, response_) &&
            RSMarshallingHelper::Marshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, normalizedInitialVelocity_) &&
            RSMarshallingHelper::Marshalling(parcel, minimumAmplitudeRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, needLogicallyFinishCallback_) &&
            RSMarshallingHelper::Marshalling(parcel, zeroThreshold_))) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::Marshalling, invalid parametter failed");
        return false;
    }

    return true;
}

RSRenderInterpolatingSpringAnimation* RSRenderInterpolatingSpringAnimation::Unmarshalling(Parcel& parcel)
{
    auto* renderInterpolatingSpringAnimation = new RSRenderInterpolatingSpringAnimation();
    if (!renderInterpolatingSpringAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RenderInterpolatingSpringAnimation::Unmarshalling, failed");
        delete renderInterpolatingSpringAnimation;
        renderInterpolatingSpringAnimation = nullptr;
        return nullptr;
    }
    return renderInterpolatingSpringAnimation;
}

bool RSRenderInterpolatingSpringAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::ParseParam, RSRenderPropertyBase Fail");
        return false;
    }

    if (!(RSMarshallingHelper::Unmarshalling(parcel, response_) &&
            RSMarshallingHelper::Unmarshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, normalizedInitialVelocity_) &&
            RSMarshallingHelper::Unmarshalling(parcel, minimumAmplitudeRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, needLogicallyFinishCallback_) &&
            RSMarshallingHelper::Unmarshalling(parcel, zeroThreshold_))) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::ParseParam, MarshallingHelper Fail");
        return false;
    }

    return true;
}
#endif

bool RSRenderKeyframeAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!parcel.WriteBool(isDurationKeyframe_)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, isDurationKeyframe_ failed");
        return false;
    }
    if (isDurationKeyframe_) {
        uint32_t size = durationKeyframes_.size();
        if (!parcel.WriteUint32(size)) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write durationkeyframe size failed");
            return false;
        }
        for (const auto& [startFraction, endFraction, property, interpolator] : durationKeyframes_) {
            if (!(parcel.WriteFloat(startFraction) && parcel.WriteFloat(endFraction) &&
                    RSRenderPropertyBase::Marshalling(parcel, property) &&
                    interpolator != nullptr && interpolator->Marshalling(parcel))) {
                ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write durationkeyframe value failed");
                return false;
            }
        }
        return true;
    }
    uint32_t size = keyframes_.size();
    if (!parcel.WriteUint32(size)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write size failed");
        return false;
    }
    for (const auto& [value, property, interpolator] : keyframes_) {
        if (!(parcel.WriteFloat(value) && RSRenderPropertyBase::Marshalling(parcel, property) &&
                interpolator != nullptr && interpolator->Marshalling(parcel))) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write value failed");
            return false;
        }
    }
    return true;
}

RSRenderKeyframeAnimation* RSRenderKeyframeAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderKeyframeAnimation* renderKeyframeAnimation = new RSRenderKeyframeAnimation();
    if (!renderKeyframeAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::Unmarshalling, ParseParam failed");
        delete renderKeyframeAnimation;
        renderKeyframeAnimation = nullptr;
        return nullptr;
    }
    return renderKeyframeAnimation;
}

bool RSRenderKeyframeAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, RenderPropertyAnimation fail");
        return false;
    }
    if (!parcel.ReadBool(isDurationKeyframe_)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Parse isDurationKeyframe fail");
        return false;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Parse Keyframes size fail");
        return false;
    }
    if (size > MAX_KEYFRAME_SIZE_NUMBER) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Keyframes size number is too large.");
        return false;
    }
    keyframes_.clear();
    durationKeyframes_.clear();
    if (isDurationKeyframe_) {
        return ParseDurationKeyframesParam(parcel, size);
    }
    float tupValue0 = 0.0;
    for (uint32_t i = 0; i < size; i++) {
        if (!(parcel.ReadFloat(tupValue0))) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Unmarshalling value failed");
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> tupValue1;
        if (!RSRenderPropertyBase::Unmarshalling(parcel, tupValue1)) {
            return false;
        }
        std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
        if (interpolator == nullptr) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Unmarshalling interpolator failed");
            return false;
        }
        keyframes_.emplace_back(std::make_tuple(tupValue0, tupValue1, interpolator));
    }
    return true;
}

bool RSRenderKeyframeAnimation::ParseDurationKeyframesParam(Parcel& parcel, int keyframeSize)
{
    float startFraction = 0.0;
    float endFraction = 0.0;
    for (int i = 0; i < keyframeSize; i++) {
        if (!(parcel.ReadFloat(startFraction)) || !(parcel.ReadFloat(endFraction))) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Unmarshalling duration value failed");
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> tupValue1;
        if (!RSRenderPropertyBase::Unmarshalling(parcel, tupValue1)) {
            return false;
        }
        std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
        if (interpolator == nullptr) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::ParseDurationParam, Unmarshalling interpolator failed");
            return false;
        }
        durationKeyframes_.emplace_back(std::make_tuple(startFraction, endFraction, tupValue1, interpolator));
    }
    return true;
}

bool RSRenderPathAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderPathAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(parcel.WriteFloat(originRotation_) && parcel.WriteFloat(beginFraction_) && parcel.WriteFloat(endFraction_) &&
            RSMarshallingHelper::Marshalling(parcel, animationPath_) &&
            parcel.WriteInt32(static_cast<std::underlying_type<RotationMode>::type>(rotationMode_)) &&
            parcel.WriteBool(isNeedPath_) && parcel.WriteBool(needAddOrigin_) && interpolator_ != nullptr &&
            interpolator_->Marshalling(parcel) && RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_) && parcel.WriteUint64(rotationId_))) {
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
        renderPathAnimation = nullptr;
        return nullptr;
    }
    return renderPathAnimation;
}

bool RSRenderPathAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Parse RenderProperty Fail");
        return false;
    }

    int32_t rotationMode = 0;
    bool isNeedPath = true;
    if (!(parcel.ReadFloat(originRotation_) && parcel.ReadFloat(beginFraction_) &&
            parcel.ReadFloat(endFraction_) && RSMarshallingHelper::Unmarshalling(parcel, animationPath_) &&
            parcel.ReadInt32(rotationMode) && parcel.ReadBool(isNeedPath) && parcel.ReadBool(needAddOrigin_))) {
        ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Parse PathAnimation Failed");
        return false;
    }

    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    if (interpolator == nullptr) {
        ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Unmarshalling interpolator failed");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_) && parcel.ReadUint64(rotationId_))) {
        ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Parse values failed");
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, rotationId_);
    SetInterpolator(interpolator);
    SetRotationMode(static_cast<RotationMode>(rotationMode));
    SetIsNeedPath(isNeedPath);
    return true;
}

bool RSRenderPropertyAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, RenderAnimation failed");
        return false;
    }
    if (!parcel.WriteUint64(propertyId_)) {
        ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, write PropertyId failed");
        return false;
    }
    if (!(RSMarshallingHelper::Marshalling(parcel, isAdditive_) &&
            RSRenderPropertyBase::Marshalling(parcel, originValue_))) {
        ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, write value failed");
        return false;
    }
    return true;
}

bool RSRenderPropertyAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, RenderAnimation failed");
        return false;
    }

    if (!(parcel.ReadUint64(propertyId_) && RSMarshallingHelper::Unmarshalling(parcel, isAdditive_))) {
        ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, Unmarshalling failed");
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, propertyId_);
    if (!RSRenderPropertyBase::Unmarshalling(parcel, originValue_)) {
        return false;
    }
    if (originValue_ == nullptr) {
        ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, originValue_ is nullptr!");
        return false;
    }
    lastValue_ = originValue_->Clone();

    return true;
}

#ifdef ROSEN_OHOS
bool RSRenderSpringAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, RSRenderPropertyBase failed");
        return false;
    }

    if (!(RSMarshallingHelper::Marshalling(parcel, response_) &&
            RSMarshallingHelper::Marshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, blendDuration_) &&
            RSMarshallingHelper::Marshalling(parcel, minimumAmplitudeRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, needLogicallyFinishCallback_) &&
            RSMarshallingHelper::Marshalling(parcel, zeroThreshold_))) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }

    if (initialVelocity_) {
        return RSMarshallingHelper::Marshalling(parcel, true) &&
               RSMarshallingHelper::Marshalling(parcel, initialVelocity_);
    }

    return RSMarshallingHelper::Marshalling(parcel, false);
}

RSRenderSpringAnimation* RSRenderSpringAnimation::Unmarshalling(Parcel& parcel)
{
    auto* renderSpringAnimation = new RSRenderSpringAnimation();
    if (!renderSpringAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::Unmarshalling, failed");
        delete renderSpringAnimation;
        renderSpringAnimation = nullptr;
        return nullptr;
    }
    return renderSpringAnimation;
}

bool RSRenderSpringAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderSpringAnimation::ParseParam, RSRenderPropertyBase Fail");
        return false;
    }

    auto haveInitialVelocity = false;
    if (!(RSMarshallingHelper::Unmarshalling(parcel, response_) &&
            RSMarshallingHelper::Unmarshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, blendDuration_) &&
            RSMarshallingHelper::Unmarshalling(parcel, minimumAmplitudeRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, needLogicallyFinishCallback_) &&
            RSMarshallingHelper::Unmarshalling(parcel, zeroThreshold_) &&
            RSMarshallingHelper::Unmarshalling(parcel, haveInitialVelocity))) {
        ROSEN_LOGE("RSRenderSpringAnimation::ParseParam, MarshallingHelper Fail");
        return false;
    }

    if (haveInitialVelocity) {
        return RSMarshallingHelper::Unmarshalling(parcel, initialVelocity_);
    }
    return true;
}
#endif

RSRenderTransitionEffect* RSRenderTransitionEffect::Unmarshalling(Parcel& parcel)
{
    uint16_t transitionType = 0;
    if (!parcel.ReadUint16(transitionType)) {
        ROSEN_LOGE("RSRenderTransitionEffect::Unmarshalling, ParseParam Failed");
        return nullptr;
    }
    switch (transitionType) {
        case RSTransitionEffectType::FADE:
            return RSTransitionFade::Unmarshalling(parcel);
        case RSTransitionEffectType::SCALE:
            return RSTransitionScale::Unmarshalling(parcel);
        case RSTransitionEffectType::TRANSLATE:
            return RSTransitionTranslate::Unmarshalling(parcel);
        case RSTransitionEffectType::ROTATE:
            return RSTransitionRotate::Unmarshalling(parcel);
        default:
            return nullptr;
    }
}

bool RSTransitionFade::Marshalling(Parcel& parcel) const
{
    bool flag = parcel.WriteUint16(RSTransitionEffectType::FADE) && parcel.WriteFloat(alpha_);
    if (!flag) {
        ROSEN_LOGE("RSTransitionFade::Marshalling, ParseParam Failed");
    }
    return flag;
}

RSRenderTransitionEffect* RSTransitionFade::Unmarshalling(Parcel& parcel)
{
    float alpha;
    if (!RSMarshallingHelper::Unmarshalling(parcel, alpha)) {
        ROSEN_LOGE("RSTransitionFade::Unmarshalling, unmarshalling alpha failed");
        return nullptr;
    }
    return new RSTransitionFade(alpha);
}

bool RSTransitionScale::Marshalling(Parcel& parcel) const
{
    bool flag = parcel.WriteUint16(RSTransitionEffectType::SCALE) && parcel.WriteFloat(scaleX_) &&
           parcel.WriteFloat(scaleY_) && parcel.WriteFloat(scaleZ_);
    if (!flag) {
        ROSEN_LOGE("RSTransitionScale::Marshalling, ParseParam Failed");
    }
    return flag;
}

RSRenderTransitionEffect* RSTransitionScale::Unmarshalling(Parcel& parcel)
{
    float scaleX = 0.0;
    float scaleY = 0.0;
    float scaleZ = 0.0;
    if (!parcel.ReadFloat(scaleX) || !parcel.ReadFloat(scaleY) || !parcel.ReadFloat(scaleZ)) {
        ROSEN_LOGE("RSTransitionScale::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionScale(scaleX, scaleY, scaleZ);
}

bool RSTransitionTranslate::Marshalling(Parcel& parcel) const
{
    bool flag = parcel.WriteUint16(RSTransitionEffectType::TRANSLATE) && parcel.WriteFloat(translateX_) &&
           parcel.WriteFloat(translateY_) && parcel.WriteFloat(translateZ_);
    if (!flag) {
        ROSEN_LOGE("RSTransitionTranslate::Marshalling, ParseParam failed");
    }
    return flag;
}

RSRenderTransitionEffect* RSTransitionTranslate::Unmarshalling(Parcel& parcel)
{
    float translateX = 0.0;
    float translateY = 0.0;
    float translateZ = 0.0;
    if (!parcel.ReadFloat(translateX) || !parcel.ReadFloat(translateY) || !parcel.ReadFloat(translateZ)) {
        ROSEN_LOGE("RSTransitionTranslate::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionTranslate(translateX, translateY, translateZ);
}

bool RSTransitionRotate::Marshalling(Parcel& parcel) const
{
    bool flag = parcel.WriteUint16(RSTransitionEffectType::ROTATE) && parcel.WriteFloat(dx_) &&
                parcel.WriteFloat(dy_) && parcel.WriteFloat(dz_) && parcel.WriteFloat(radian_);
    if (!flag) {
        ROSEN_LOGE("RSTransitionRotate::Marshalling, ParseParam failed");
    }
    return flag;
}

RSRenderTransitionEffect* RSTransitionRotate::Unmarshalling(Parcel& parcel)
{
    Quaternion quaternion;
    float dx = 0.0;
    float dy = 0.0;
    float dz = 0.0;
    float radian = 0.0;
    if (!parcel.ReadFloat(dx) || !parcel.ReadFloat(dy) || !parcel.ReadFloat(dz) || !parcel.ReadFloat(radian)) {
        ROSEN_LOGE("RSTransitionRotate::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionRotate(dx, dy, dz, radian);
}

bool RSRenderTransition::Marshalling(Parcel& parcel) const
{
    if (!RSRenderAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderTransition::Marshalling, step1 failed");
        return false;
    }
    if (!RSMarshallingHelper::Marshalling(parcel, effects_) ||
        !RSMarshallingHelper::Marshalling(parcel, isTransitionIn_) || interpolator_ == nullptr ||
        !interpolator_->Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderTransition::Marshalling, step2 failed");
        return false;
    }
    return true;
}

RSRenderTransition* RSRenderTransition::Unmarshalling(Parcel& parcel)
{
    RSRenderTransition* renderTransition = new RSRenderTransition();
    if (!renderTransition->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderTransition::Unmarshalling, ParseParam Failed");
        delete renderTransition;
        renderTransition = nullptr;
        return nullptr;
    }
    return renderTransition;
}

bool RSRenderTransition::ParseParam(Parcel& parcel)
{
    if (!RSRenderAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderTransition::ParseParam, RenderAnimation failed");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, effects_)) {
        ROSEN_LOGE("RSRenderTransition::ParseParam, effect failed");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, isTransitionIn_)) {
        ROSEN_LOGE("RSRenderTransition::ParseParam, transition direction failed");
        return false;
    }
    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    if (interpolator == nullptr) {
        ROSEN_LOGE("RSRenderTransition::ParseParam, interpolator is nullptr");
        return false;
    }
    SetInterpolator(interpolator);
    return true;
}

bool RSSpringInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::SPRING)) {
        ROSEN_LOGE("RSSpringInterpolator::Marshalling, Write type failed");
        return false;
    }
    if (!parcel.WriteUint64(id_)) {
        ROSEN_LOGE("RSSpringInterpolator::Marshalling, Write id failed");
        return false;
    }
    if (!(parcel.WriteFloat(response_) && parcel.WriteFloat(dampingRatio_) && parcel.WriteFloat(initialVelocity_))) {
        ROSEN_LOGE("RSSpringInterpolator::Marshalling, Write value failed");
        return false;
    }
    return true;
}

RSSpringInterpolator* RSSpringInterpolator::Unmarshalling(Parcel& parcel)
{
    uint64_t id{0};
    if (!parcel.ReadUint64(id)) {
        ROSEN_LOGE("RSSpringInterpolator::Unmarshalling Read id failed");
        return nullptr;
    }
    float response = 0.0;
    float dampingRatio = 0.0;
    float initialVelocity = 0.0;
    if (!(parcel.ReadFloat(response) && parcel.ReadFloat(dampingRatio) && parcel.ReadFloat(initialVelocity))) {
        ROSEN_LOGE("RSSpringInterpolator::Unmarshalling, SpringInterpolator failed");
        return nullptr;
    }
    auto ret = new RSSpringInterpolator(id, response, dampingRatio, initialVelocity);
    return ret;
}

bool RSStepsInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::STEPS)) {
        ROSEN_LOGE("StepsInterpolator marshalling write type failed.");
        return false;
    }
    if (!parcel.WriteUint64(id_)) {
        ROSEN_LOGE("StepsInterpolator marshalling write id failed.");
        return false;
    }
    if (!(parcel.WriteInt32(steps_) && parcel.WriteInt32(static_cast<int32_t>(position_)))) {
        ROSEN_LOGE("StepsInterpolator marshalling write value failed.");
        return false;
    }
    return true;
}

RSStepsInterpolator* RSStepsInterpolator::Unmarshalling(Parcel& parcel)
{
    uint64_t id{0};
    if (!parcel.ReadUint64(id)) {
        ROSEN_LOGE("RSStepsInterpolator::Unmarshalling Read id failed");
        return nullptr;
    }
    int32_t steps = 0;
    int32_t position = 0;
    if (!(parcel.ReadInt32(steps) && parcel.ReadInt32(position))) {
        ROSEN_LOGE("StepsInterpolator unmarshalling failed.");
        return nullptr;
    }
    return new RSStepsInterpolator(id, steps, static_cast<StepsCurvePosition>(position));
}
} // namespace Rosen
} // namespace OHOS
