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

#include <memory>
#include "animation/rs_value_estimator.h"
#include "property/rs_properties.h"
#include "animation/rs_animation_common.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSRenderNode;

class RSRenderTransitionEffect {
public:
    static std::shared_ptr<RSRenderTransitionEffect> CreateTransitionEffect(
        const RSTransitionEffect& effect);
    RSRenderTransitionEffect() = default;
    virtual ~RSRenderTransitionEffect() = default;
    virtual void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) = 0;
protected:
    const std::unique_ptr<RSValueEstimator> valueEstimator_ { std::make_unique<RSValueEstimator>() };
    float GetBoundsWidth(const RSProperties& rsProperties) const { return rsProperties.GetBoundsRect().width_; };
    float GetBoundsHeight(const RSProperties& rsProperties) const { return rsProperties.GetBoundsRect().height_; };
};

class RSTransitionFadeIn : public RSRenderTransitionEffect {
public:
    RSTransitionFadeIn() = default;
    virtual ~RSTransitionFadeIn() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;
};

class RSTransitionFadeOut : public RSRenderTransitionEffect {
public:
    RSTransitionFadeOut() = default;
    virtual ~RSTransitionFadeOut() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;
};

class RSTransitionScaleIn : public RSRenderTransitionEffect {
public:
    RSTransitionScaleIn() = default;
    explicit RSTransitionScaleIn(ScaleParams scale) : scaleParams_(scale) {};
    virtual ~RSTransitionScaleIn() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;
private:
    ScaleParams scaleParams_;
};

class RSTransitionScaleOut : public RSRenderTransitionEffect {
public:
    RSTransitionScaleOut() = default;
    explicit RSTransitionScaleOut(ScaleParams scale) : scaleParams_(scale) {};
    virtual ~RSTransitionScaleOut() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;
private:
    ScaleParams scaleParams_;
};

class RSTransitionTranslateIn : public RSRenderTransitionEffect {
public:
    RSTransitionTranslateIn() = default;
    explicit RSTransitionTranslateIn(TranslateParams translate) : translateParams_(translate) {};
    virtual ~RSTransitionTranslateIn() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;
private:
    TranslateParams translateParams_;
};

class RSTransitionTranslateOut : public RSRenderTransitionEffect {
public:
    RSTransitionTranslateOut() = default;
    explicit RSTransitionTranslateOut(TranslateParams translate) : translateParams_(translate) {};
    virtual ~RSTransitionTranslateOut() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;
private:
    TranslateParams translateParams_;
};

class RSTransitionRotateIn : public RSRenderTransitionEffect {
public:
    RSTransitionRotateIn() = default;
    explicit RSTransitionRotateIn(RotateParams rotate) : rotateParams_(rotate) {};
    virtual ~RSTransitionRotateIn() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;
private:
    RotateParams rotateParams_;
};

class RSTransitionRotateOut : public RSRenderTransitionEffect {
public:
    RSTransitionRotateOut() = default;
    explicit RSTransitionRotateOut(RotateParams rotate) : rotateParams_(rotate) {};
    virtual ~RSTransitionRotateOut() = default;
    void OnTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction) override;
private:
    RotateParams rotateParams_;
};
} // namespace Rosen
} // namespace OHOS

#endif
