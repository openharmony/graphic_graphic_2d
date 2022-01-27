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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_RENDER_TRANSITION_EFFECT_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_RENDER_TRANSITION_EFFECT_H

#ifdef ROSEN_OHOS
#include <parcel.h>
#include <refbase.h>
#endif
#include <memory>

#include "animation/rs_animation_common.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSCanvasRenderNode;
class RSPaintFilterCanvas;
class RSProperties;

#ifdef ROSEN_OHOS
class RSRenderTransitionEffect : public Parcelable {
#else
class RSRenderTransitionEffect {
#endif
public:
    RSRenderTransitionEffect() = default;
    virtual ~RSRenderTransitionEffect() = default;
    virtual void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) = 0;

#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel& parcel) const override = 0;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif

protected:
};

class RSTransitionFade : public RSRenderTransitionEffect {
public:
    explicit RSTransitionFade(float alpha) : alpha_(alpha = 0.0f) {}
    virtual ~RSTransitionFade() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float alpha_;
};

class RSTransitionScale : public RSRenderTransitionEffect {
public:
    RSTransitionScale() = default;
    explicit RSTransitionScale(
        float scaleX = 0.0f, float scaleY = 0.0f, float scaleZ = 0.0f, float pivotX = 0.5, float pivotY = 0.5)
        : scaleX_(scaleX), scaleY_(scaleY), scaleZ_(scaleZ), pivotX_(pivotX), pivotY_(pivotY)
    {}
    virtual ~RSTransitionScale() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float scaleX_;
    float scaleY_;
    float scaleZ_;
    float pivotX_;
    float pivotY_;
};

class RSTransitionTranslate : public RSRenderTransitionEffect {
public:
    RSTransitionTranslate() = default;
    explicit RSTransitionTranslate(float translateX, float translateY, float translateZ)
        : translateX_(translateX), translateY_(translateY), translateZ_(translateZ)
    {}
    virtual ~RSTransitionTranslate() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float translateX_;
    float translateY_;
    float translateZ_;
};

class RSTransitionRotate : public RSRenderTransitionEffect {
public:
    RSTransitionRotate() = default;
    explicit RSTransitionRotate(float dx, float dy, float dz, float angle, float pivotX = 0.5, float pivotY = 0.5)
        : dx_(dx), dy_(dy), dz_(dz), angle_(angle), pivotX_(pivotX), pivotY_(pivotY)
    {}
    virtual ~RSTransitionRotate() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float dx_;
    float dy_;
    float dz_;
    float angle_;
    float pivotX_;
    float pivotY_;
};
} // namespace Rosen
} // namespace OHOS

#endif
