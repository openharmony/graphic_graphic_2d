/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_TRANSITION_PROPERTIES_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_TRANSITION_PROPERTIES_H

#include <vector>

#include "common/rs_vector3.h"
#include "include/core/SkMatrix44.h"

namespace OHOS {
namespace Rosen {
class RSTransitionProperties final {
public:
    RSTransitionProperties() = default;
    ~RSTransitionProperties() = default;

    void DoAlphaTransition(float alpha)
    {
        alpha_ *= alpha;
    }
    void DoTranslateTransition(const Vector3f& translate)
    {
        translate_ += translate;
    }
    void DoScaleTransition(const Vector3f& scale)
    {
        scale_ *= scale;
    }
    void DoRotateTransition(const SkMatrix44& rotateMatrix)
    {
        rotate_.preConcat(rotateMatrix);
    }

    float GetAlpha() const
    {
        return alpha_;
    }
    Vector3f GetTranslate() const
    {
        return translate_;
    }
    Vector3f GetScale() const
    {
        return scale_;
    }
    SkMatrix44 GetRotate() const
    {
        return rotate_;
    }

private:
    float alpha_ = 1.0f;
    Vector3f translate_ = { 0.0f, 0.0f, 0.0f };
    Vector3f scale_ = { 1.0f, 1.0f, 1.0f };
    SkMatrix44 rotate_ = SkMatrix44::I();
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_TRANSITION_PROPERTIES_H
