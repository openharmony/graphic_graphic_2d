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

#include "animation/rs_render_path_animation.h"

#include "animation/rs_value_estimator.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
template<>
void RSRenderPathAnimation<Vector2f>::OnAnimate(float fraction)
{
    if (animationPath_ == nullptr) {
        ROSEN_LOGE("Failed to animate motion path, path is null!");
        return;
    }

#ifdef ROSEN_OHOS
    Vector2f position;
    float tangent = 0;
    GetPosTanValue(fraction, position, tangent);
    if (needAddOrigin_) {
        SetPathValue(position + RSRenderPropertyAnimation<Vector2f>::GetOriginValue(), tangent);
    } else {
        SetPathValue(position, tangent);
    }
#endif
}

template<>
void RSRenderPathAnimation<Vector4f>::OnAnimate(float fraction)
{
    if (animationPath_ == nullptr) {
        ROSEN_LOGE("Failed to animate motion path, path is null!");
        return;
    }

#ifdef ROSEN_OHOS
    auto interpolationValue =
        RSValueEstimator::Estimate(interpolator_->Interpolate(fraction), startValue_, endValue_);
    Vector4f animationValue = RSRenderPropertyAnimation<Vector4f>::GetAnimationValue(interpolationValue);
    Vector2f position;
    float tangent = 0;
    if (isNeedPath_) {
        GetPosTanValue(fraction, position, tangent);
        animationValue[0] = position[0];
        animationValue[1] = position[1];
        if (needAddOrigin_) {
            animationValue[0] = position[0] + RSRenderPropertyAnimation<Vector4f>::GetOriginValue()[0];
            animationValue[1] = position[1] + RSRenderPropertyAnimation<Vector4f>::GetOriginValue()[1];
        }
        SetPathValue(animationValue, tangent);
    } else {
        SetPropertyValue(animationValue);
    }
#endif
}
} // namespace Rosen
} // namespace OHOS
