/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H

#include <string>
#include <vector>

#include "animation/rs_animation_common.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_property_animation.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RS_EXPORT RSPathAnimation : public RSPropertyAnimation<T> {
public:
    RSPathAnimation(RSAnimatableProperty<T>& property, const std::shared_ptr<RSPath>& animationPath)
        : RSPropertyAnimation<T>(property), animationPath_(animationPath)
    {}

    RSPathAnimation(RSAnimatableProperty<T>& property, const std::string& path, const T& startValue,
        const T& endValue) : RSPathAnimation(property, PreProcessPath(path, startValue, endValue))
    {
        RSPropertyAnimation<T>::startValue_ = startValue;
        RSPropertyAnimation<T>::endValue_ = endValue;
        InitNeedPath(RSPropertyAnimation<T>::startValue_, RSPropertyAnimation<T>::endValue_);
    }

    virtual ~RSPathAnimation() = default;

    void SetTimingCurve(const RSAnimationTimingCurve& timingCurve)
    {
        timingCurve_ = timingCurve;
    }

    const RSAnimationTimingCurve& GetTimingCurve() const
    {
        return timingCurve_;
    }

    void SetRotationMode(const RotationMode& rotationMode)
    {
        if (RSAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to enable rotate, path animation has started!");
            return;
        }

        rotationMode_ = rotationMode;
    }

    RotationMode GetRotationMode() const
    {
        return rotationMode_;
    }

    void SetBeginFraction(float fraction)
    {
        if (RSAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to set begin fraction, path animation has started!");
            return;
        }

        if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction > endFraction_) {
            ROSEN_LOGE("Failed to set begin fraction, invalid value:%f", fraction);
            return;
        }

        beginFraction_ = fraction;
    }

    float GetBeginFraction() const
    {
        return beginFraction_;
    }

    void SetEndFraction(float fraction)
    {
        if (RSAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to set end fraction, path animation has started!");
            return;
        }

        if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction < beginFraction_) {
            ROSEN_LOGE("Failed to set end fraction, invalid value:%f", fraction);
            return;
        }

        endFraction_ = fraction;
    }

    float GetEndFraction() const
    {
        return endFraction_;
    }

    void SetPathNeedAddOrigin(bool needAddOrigin)
    {
        if (RSAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to set need Add Origin, path animation has started!");
            return;
        }

        needAddOrigin_ = needAddOrigin;
    }

    bool GetPathNeedAddOrigin() const
    {
        return needAddOrigin_;
    }

protected:
    void OnStart() override
    {}

    void InitInterpolationValue() override
    {
        if (!animationPath_) {
            ROSEN_LOGE("Failed to update interpolation value, path is null!");
            return;
        }

#ifdef ROSEN_OHOS
        if (isNeedPath_) {
            animationPath_->GetPosTan(0.0f * beginFraction_, RSPropertyAnimation<T>::startValue_, startTangent_);
            animationPath_->GetPosTan(animationPath_->GetDistance() * endFraction_, RSPropertyAnimation<T>::endValue_, endTangent_);
            if (needAddOrigin_) {
                UpdateValueAddOrigin(RSPropertyAnimation<T>::startValue_);
                UpdateValueAddOrigin(RSPropertyAnimation<T>::endValue_);
            }
        }
        RSPropertyAnimation<T>::byValue_ = RSPropertyAnimation<T>::endValue_ - RSPropertyAnimation<T>::startValue_;
#endif
    }

    void OnUpdateStagingValue(bool isFirstStart) override
    {
        auto target = RSAnimation::GetTarget().lock();
        if (target == nullptr) {
            ROSEN_LOGE("Failed to update staging value, target is null!");
            return;
        }

        RSPropertyAnimation<T>::OnUpdateStagingValue(isFirstStart);

        float startTangent = 0.0f;
        float endTangent = 0.0f;
        switch (rotationMode_) {
            case RotationMode::ROTATE_NONE:
                return;
            case RotationMode::ROTATE_AUTO:
                startTangent = startTangent_;
                endTangent = endTangent_;
                break;
            case RotationMode::ROTATE_AUTO_REVERSE:
                startTangent = startTangent_ + 180.0f;
                endTangent = endTangent_ + 180.0f;
                break;
            default:
                ROSEN_LOGE("Unknow rotation mode!");
                return;
        }
        if (!RSAnimation::GetDirection()) {
            std::swap(startTangent, endTangent);
        }

        float targetRotation = 0.0f;
        float byRotation = endTangent - startTangent;
        if (isFirstStart) {
            if (RSAnimation::GetAutoReverse() && RSAnimation::GetRepeatCount() % 2 == 0) {
                targetRotation = startTangent;
            } else {
                targetRotation = endTangent;
            }
        } else {
            float currentRotation = target->GetStagingProperties().GetRotation();
            if (RSAnimation::GetAutoReverse() && RSAnimation::GetRepeatCount() % 2 == 0) {
                targetRotation = RSAnimation::IsReversed() ? currentRotation + byRotation
                    : currentRotation - byRotation;
            } else {
                targetRotation = RSAnimation::IsReversed() ? currentRotation - byRotation
                    : currentRotation + byRotation;
            }
        }

        SetRotation(target, targetRotation);
    }

    void InitRotationId(const std::shared_ptr<RSNode>& node)
    {
        if (GetRotationPropertyId(node) == 0) {
            node->SetRotation(0.f);
        }
        rotationId_ = GetRotationPropertyId(node);
    }

    PropertyId GetRotationPropertyId(const std::shared_ptr<RSNode>& node)
    {
        auto iter = node->propertyModifiers_.find(RSModifierType::ROTATION);
        if (iter != node->propertyModifiers_.end()) {
            return iter->second->GetPropertyId();
        }

        for (const auto& [id, modifier] : node->modifiers_) {
            if (modifier->GetModifierType() == RSModifierType::ROTATION) {
                return modifier->GetPropertyId();
            }
        }
        return 0;
    }

    void SetRotation(const std::shared_ptr<RSNode>& node, const float rotation)
    {
        auto iter = node->modifiers_.find(rotationId_);
        if (iter != node->modifiers_.end()) {
            auto modifier = std::static_pointer_cast<RSAnimatableModifier<float>>(iter->second);
            if (modifier != nullptr) {
                modifier->GetProperty()->stagingValue_ = rotation;
            }
            return;
        }

        for (const auto& [type, modifier] : node->propertyModifiers_) {
            if (modifier->GetPropertyId() == rotationId_) {
                auto animatableModifier = std::static_pointer_cast<RSAnimatableModifier<float>>(modifier);
                if (animatableModifier != nullptr) {
                    animatableModifier->GetProperty()->stagingValue_ = rotation;
                }
            }
            return;
        }
    }

    void OnCallFinishCallback() override
    {
        RSPropertyAnimation<T>::property_.runningPathNum_ -= 1;
    }

    void SetPropertyOnAllAnimationFinish() override {}

private:
    void ReplaceSubString(std::string& sourceStr, const std::string& subStr, const std::string& newStr) const
    {
        std::string::size_type position = 0;
        while ((position = sourceStr.find(subStr)) != std::string::npos) {
            sourceStr.replace(position, subStr.length(), newStr);
        }

        ROSEN_LOGD("SVG path:%s", sourceStr.c_str());
    }

    const std::shared_ptr<RSPath> ProcessPath(const std::string& path, const float startX, const float startY,
        const float endX, const float endY) const
    {
        std::string animationPath = path;
        ReplaceSubString(animationPath, "start.x", std::to_string(startX));
        ReplaceSubString(animationPath, "start.y", std::to_string(startY));
        ReplaceSubString(animationPath, "end.x", std::to_string(endX));
        ReplaceSubString(animationPath, "end.y", std::to_string(endY));
#ifdef ROSEN_OHOS
        return RSPath::CreateRSPath(animationPath);
#else
        return nullptr;
#endif
    }

    const std::shared_ptr<RSPath> PreProcessPath(const std::string& path, const T& startValue,
        const T& endValue) const
    {
        return {};
    }

    void InitNeedPath(const T& startValue, const T& endValue)
    {
        RSPropertyAnimation<T>::SetAdditive(false);
    }

    void UpdateValueAddOrigin(T& value) {}

    template<typename P>
    void StartAnimationImpl();

    float beginFraction_ { FRACTION_MIN };
    float endFraction_ { FRACTION_MAX };
    float startTangent_ { 0.0f };
    float endTangent_ { 0.0f };
    bool isNeedPath_ { true };
    bool needAddOrigin_ { true };
    PropertyId rotationId_;
    RotationMode rotationMode_ { RotationMode::ROTATE_NONE };
    RSAnimationTimingCurve timingCurve_ { RSAnimationTimingCurve::DEFAULT };
    std::shared_ptr<RSPath> animationPath_;
};

template<>
void RSPathAnimation<Vector2f>::OnStart();

template<>
void RSPathAnimation<Vector4f>::OnStart();

template<>
const std::shared_ptr<RSPath> RSPathAnimation<Vector2f>::PreProcessPath(
    const std::string& path, const Vector2f& startValue, const Vector2f& endValue) const;

template<>
const std::shared_ptr<RSPath> RSPathAnimation<Vector4f>::PreProcessPath(
    const std::string& path, const Vector4f& startValue, const Vector4f& endValue) const;

template<>
void RSPathAnimation<Vector4f>::InitNeedPath(const Vector4f& startValue, const Vector4f& endValue);

template<>
void RSPathAnimation<Vector2f>::UpdateValueAddOrigin(Vector2f& value);

template<>
void RSPathAnimation<Vector4f>::UpdateValueAddOrigin(Vector4f& value);
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PATH_ANIMATION_H
