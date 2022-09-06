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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_RENDER_TRANSITION_EFFECT_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_RENDER_TRANSITION_EFFECT_H

#ifdef ROSEN_OHOS
#include <parcel.h>
#include <refbase.h>
#endif
#include <memory>

#include "animation/rs_animation_common.h"

namespace OHOS {
namespace Rosen {
class RSCanvasRenderNode;
class RSPaintFilterCanvas;
class RSProperties;
class RSRenderModifier;
template<typename T>
class RSRenderAnimatableProperty;
class Quaternion;
template<typename T>
class Vector2;

#ifdef ROSEN_OHOS
class RSRenderTransitionEffect : public Parcelable {
#else
class RSRenderTransitionEffect {
#endif
public:
    RSRenderTransitionEffect() = default;
    virtual ~RSRenderTransitionEffect() = default;
    const std::shared_ptr<RSRenderModifier>& GetModifier();
    virtual void UpdateFraction(float fraction) const = 0;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override = 0;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    std::shared_ptr<RSRenderModifier> modifier_;
    virtual const std::shared_ptr<RSRenderModifier> CreateModifier() = 0;
};

class RSTransitionFade : public RSRenderTransitionEffect {
public:
    explicit RSTransitionFade(float alpha) : alpha_(alpha) {}
    ~RSTransitionFade() override = default;
    void UpdateFraction(float fraction) const override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float alpha_;
    std::shared_ptr<RSRenderAnimatableProperty<float>> property_;
    const std::shared_ptr<RSRenderModifier> CreateModifier() override;
};

class RSTransitionScale : public RSRenderTransitionEffect {
public:
    explicit RSTransitionScale(float scaleX = 0.0f, float scaleY = 0.0f, float scaleZ = 0.0f)
        : scaleX_(scaleX), scaleY_(scaleY), scaleZ_(scaleZ)
    {}
    ~RSTransitionScale() override = default;
    void UpdateFraction(float fraction) const override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float scaleX_;
    float scaleY_;
    float scaleZ_;
    std::shared_ptr<RSRenderAnimatableProperty<Vector2<float>>> property_;
    const std::shared_ptr<RSRenderModifier> CreateModifier() override;
};

class RSTransitionTranslate : public RSRenderTransitionEffect {
public:
    explicit RSTransitionTranslate(float translateX, float translateY, float translateZ)
        : translateX_(translateX), translateY_(translateY), translateZ_(translateZ)
    {}
    ~RSTransitionTranslate() override = default;
    void UpdateFraction(float fraction) const override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float translateX_;
    float translateY_;
    float translateZ_;
    std::shared_ptr<RSRenderAnimatableProperty<Vector2<float>>> property_;
    const std::shared_ptr<RSRenderModifier> CreateModifier() override;
};

class RSTransitionRotate : public RSRenderTransitionEffect {
public:
    explicit RSTransitionRotate(float dx, float dy, float dz, float angle) : dx_(dx), dy_(dy), dz_(dz), angle_(angle) {}
    ~RSTransitionRotate() override = default;
    void UpdateFraction(float fraction) const override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float dx_;
    float dy_;
    float dz_;
    float angle_;
    std::shared_ptr<RSRenderAnimatableProperty<Quaternion>> property_;
    const std::shared_ptr<RSRenderModifier> CreateModifier() override;
};
} // namespace Rosen
} // namespace OHOS

#endif
