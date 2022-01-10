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

#include "animation/rs_render_transition_effect.h"

#include "animation/rs_animation_common.h"
#include "include/core/SkMatrix44.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSRenderTransitionEffect> RSRenderTransitionEffect::CreateTransitionEffect(
    const RSTransitionEffect& effect)
{
    std::shared_ptr<RSRenderTransitionEffect> renderEffect = nullptr;
    switch (effect.GetType()) {
        case RSTransitionEffectType::FADE_IN: {
            renderEffect = std::make_shared<RSTransitionFadeIn>();
            break;
        }
        case RSTransitionEffectType::FADE_OUT: {
            renderEffect = std::make_shared<RSTransitionFadeOut>();
            break;
        }
        case RSTransitionEffectType::SCALE_IN: {
            renderEffect = std::make_shared<RSTransitionScaleIn>(effect.GetScaleParams());
            break;
        }
        case RSTransitionEffectType::SCALE_OUT: {
            renderEffect = std::make_shared<RSTransitionScaleOut>(effect.GetScaleParams());
            break;
        }
        case RSTransitionEffectType::TRANSLATE_IN: {
            renderEffect = std::make_shared<RSTransitionTranslateIn>(effect.GetTranslateParams());
            break;
        }
        case RSTransitionEffectType::TRANSLATE_OUT: {
            renderEffect = std::make_shared<RSTransitionTranslateOut>(effect.GetTranslateParams());
            break;
        }
        case RSTransitionEffectType::ROTATE_IN: {
            renderEffect = std::make_shared<RSTransitionRotateIn>(effect.GetRotateParams());
            break;
        }
        case RSTransitionEffectType::ROTATE_OUT: {
            renderEffect = std::make_shared<RSTransitionRotateOut>(effect.GetRotateParams());
            break;
        }
        default:
            break;
    }
    return renderEffect;
}

void RSTransitionFadeIn::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    canvas.MultiplyAlpha(fraction);
#endif
}

void RSTransitionFadeOut::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    canvas.MultiplyAlpha(1.0f - fraction);
#endif
}

void RSTransitionScaleIn::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    Vector2f startValue(scaleParams_.scaleX, scaleParams_.scaleY);
    Vector2f endValue(1.0f, 1.0f);
    auto value = valueEstimator_->Estimate(fraction, startValue, endValue);
    SkMatrix matrix;
    matrix.setScale(value.x_, value.y_,
                    GetBoundsWidth(renderProperties) * scaleParams_.pivotX,
                    GetBoundsHeight(renderProperties) * scaleParams_.pivotY);
    canvas.concat(matrix);
#endif
}

void RSTransitionScaleOut::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    Vector2f startValue(1.0f, 1.0f);
    Vector2f endValue(scaleParams_.scaleX, scaleParams_.scaleY);
    auto value = valueEstimator_->Estimate(fraction, startValue, endValue);
    SkMatrix matrix;
    matrix.setScale(value.x_, value.y_,
                    GetBoundsWidth(renderProperties) * scaleParams_.pivotX,
                    GetBoundsHeight(renderProperties) * scaleParams_.pivotY);
    canvas.concat(matrix);
#endif
}

void RSTransitionTranslateIn::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    Vector2f startValue(translateParams_.dx, translateParams_.dy);
    Vector2f endValue(0.0f, 0.0f);
    auto value = valueEstimator_->Estimate(fraction, startValue, endValue);
    canvas.translate(value.x_, value.y_);
#endif
}

void RSTransitionTranslateOut::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    Vector2f startValue(0.0f, 0.0f);
    Vector2f endValue(translateParams_.dx, translateParams_.dy);
    auto value = valueEstimator_->Estimate(fraction, startValue, endValue);
    canvas.translate(value.x_, value.y_);
#endif
}

void RSTransitionRotateIn::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    float startValue = rotateParams_.angle;
    float endValue = 0.0f;
    auto value = valueEstimator_->Estimate(fraction, startValue, endValue);
    SkMatrix44 matrix;
    canvas.translate(GetBoundsWidth(renderProperties) * rotateParams_.pivotX,
                     GetBoundsHeight(renderProperties) * rotateParams_.pivotY);
    matrix.setRotateDegreesAbout(rotateParams_.dx, rotateParams_.dy, rotateParams_.dz, value);
    canvas.concat(SkMatrix(matrix));
    canvas.translate(-GetBoundsWidth(renderProperties) * rotateParams_.pivotX,
                     -GetBoundsHeight(renderProperties) * rotateParams_.pivotY);
#endif
}

void RSTransitionRotateOut::OnTransition(
    RSPaintFilterCanvas& canvas, const RSProperties& renderProperties, float fraction)
{
#ifdef ROSEN_OHOS
    float startValue = 0.0f;
    float endValue = rotateParams_.angle;
    auto value = valueEstimator_->Estimate(fraction, startValue, endValue);
    SkMatrix44 matrix;
    canvas.translate(GetBoundsWidth(renderProperties) * rotateParams_.pivotX,
                     GetBoundsHeight(renderProperties) * rotateParams_.pivotY);
    matrix.setRotateDegreesAbout(rotateParams_.dx, rotateParams_.dy, rotateParams_.dz, value);
    canvas.concat(SkMatrix(matrix));
    canvas.translate(-GetBoundsWidth(renderProperties) * rotateParams_.pivotX,
                     -GetBoundsHeight(renderProperties) * rotateParams_.pivotY);
#endif
}
} // namespace Rosen
} // namespace OHOS
