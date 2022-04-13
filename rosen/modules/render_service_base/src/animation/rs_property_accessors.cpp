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

#include "animation/rs_property_accessors.h"

#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "platform/common/rs_log.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
const std::shared_ptr<RSBasePropertyAccessors> RSBasePropertyAccessors::GetAccessor(RSAnimatableProperty property)
{
    auto it = PROPERTY_ACCESSOR_LUT.find(property);
    if (it == PROPERTY_ACCESSOR_LUT.end()) {
        ROSEN_LOGE("Accessor for property %d could not be found", property);
        return PROPERTY_ACCESSOR_LUT.at(RSAnimatableProperty::INVALID);
    }
    return it->second;
}

const std::unordered_map<RSAnimatableProperty, std::shared_ptr<RSBasePropertyAccessors>>
    RSBasePropertyAccessors::PROPERTY_ACCESSOR_LUT = {
        { RSAnimatableProperty::INVALID,
            std::make_shared<RSPropertyAccessors<float>>(nullptr, nullptr) },

        { RSAnimatableProperty::BOUNDS_WIDTH,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetBoundsWidth, &RSProperties::GetBoundsWidth) },
        { RSAnimatableProperty::BOUNDS_HEIGHT,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetBoundsHeight, &RSProperties::GetBoundsHeight) },
        { RSAnimatableProperty::BOUNDS_SIZE,
            std::make_shared<RSPropertyAccessors<Vector2f>>(
                &RSProperties::SetBoundsSize, &RSProperties::GetBoundsSize) },
        { RSAnimatableProperty::BOUNDS_POSITION_X,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetBoundsPositionX, &RSProperties::GetBoundsPositionX) },
        { RSAnimatableProperty::BOUNDS_POSITION_Y,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetBoundsPositionY, &RSProperties::GetBoundsPositionY) },
        { RSAnimatableProperty::BOUNDS_POSITION,
            std::make_shared<RSPropertyAccessors<Vector2f>>(
                &RSProperties::SetBoundsPosition, &RSProperties::GetBoundsPosition) },
        { RSAnimatableProperty::BOUNDS,
            std::make_shared<RSPropertyAccessors<Vector4f>>(
                &RSProperties::SetBounds, &RSProperties::GetBounds) },

        { RSAnimatableProperty::FRAME_WIDTH,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetFrameWidth, &RSProperties::GetFrameWidth) },
        { RSAnimatableProperty::FRAME_HEIGHT,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetFrameHeight, &RSProperties::GetFrameHeight) },
        { RSAnimatableProperty::FRAME_SIZE,
            std::make_shared<RSPropertyAccessors<Vector2f>>(
                &RSProperties::SetFrameSize, &RSProperties::GetFrameSize) },
        { RSAnimatableProperty::FRAME_POSITION_X,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetFramePositionX, &RSProperties::GetFramePositionX) },
        { RSAnimatableProperty::FRAME_POSITION_Y,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetFramePositionY, &RSProperties::GetFramePositionY) },
        { RSAnimatableProperty::FRAME_POSITION,
            std::make_shared<RSPropertyAccessors<Vector2f>>(
                &RSProperties::SetFramePosition, &RSProperties::GetFramePosition) },
        { RSAnimatableProperty::FRAME,
            std::make_shared<RSPropertyAccessors<Vector4f>>(
                &RSProperties::SetFrame, &RSProperties::GetFrame) },

        { RSAnimatableProperty::POSITION_Z,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetPositionZ, &RSProperties::GetPositionZ) },

        { RSAnimatableProperty::PIVOT_X,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetPivotX, &RSProperties::GetPivotX) },
        { RSAnimatableProperty::PIVOT_Y,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetPivotY, &RSProperties::GetPivotY) },
        { RSAnimatableProperty::PIVOT,
            std::make_shared<RSPropertyAccessors<Vector2f>>(
                &RSProperties::SetPivot, &RSProperties::GetPivot) },

        { RSAnimatableProperty::CORNER_RADIUS,
            std::make_shared<RSPropertyAccessors<Vector4f>>(
                &RSProperties::SetCornerRadius, &RSProperties::GetCornerRadius) },

        { RSAnimatableProperty::ROTATION,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetRotation, &RSProperties::GetRotation) },
        { RSAnimatableProperty::ROTATION_X,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetRotationX, &RSProperties::GetRotationX) },
        { RSAnimatableProperty::ROTATION_Y,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetRotationY, &RSProperties::GetRotationY) },
        { RSAnimatableProperty::ROTATION_3D,
            std::make_shared<RSPropertyAccessors<Quaternion>>(
                &RSProperties::SetQuaternion, &RSProperties::GetQuaternion) },

        { RSAnimatableProperty::TRANSLATE,
            std::make_shared<RSPropertyAccessors<Vector2f>>(
                &RSProperties::SetTranslate, &RSProperties::GetTranslate) },
        { RSAnimatableProperty::TRANSLATE_X,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetTranslateX, &RSProperties::GetTranslateX) },
        { RSAnimatableProperty::TRANSLATE_Y,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetTranslateY, &RSProperties::GetTranslateY) },
        { RSAnimatableProperty::TRANSLATE_Z,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetTranslateZ, &RSProperties::GetTranslateZ) },

        { RSAnimatableProperty::SCALE_X,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetScaleX, &RSProperties::GetScaleX) },
        { RSAnimatableProperty::SCALE_Y,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetScaleY, &RSProperties::GetScaleY) },
        { RSAnimatableProperty::SCALE,
            std::make_shared<RSPropertyAccessors<Vector2f>>(
                &RSProperties::SetScale, &RSProperties::GetScale) },

        { RSAnimatableProperty::ALPHA,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetAlpha, &RSProperties::GetAlpha) },
        { RSAnimatableProperty::FOREGROUND_COLOR,
            std::make_shared<RSPropertyAccessors<Color>>(
                &RSProperties::SetForegroundColor, &RSProperties::GetForegroundColor) },
        { RSAnimatableProperty::BACKGROUND_COLOR,
            std::make_shared<RSPropertyAccessors<Color>>(
                &RSProperties::SetBackgroundColor, &RSProperties::GetBackgroundColor) },
        { RSAnimatableProperty::BGIMAGE_WIDTH,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetBgImageWidth, &RSProperties::GetBgImageWidth) },
        { RSAnimatableProperty::BGIMAGE_HEIGHT,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetBgImageHeight, &RSProperties::GetBgImageHeight) },
        { RSAnimatableProperty::BGIMAGE_POSITION_X,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetBgImagePositionX, &RSProperties::GetBgImagePositionX) },
        { RSAnimatableProperty::BGIMAGE_POSITION_Y,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetBgImagePositionY, &RSProperties::GetBgImagePositionY) },
        { RSAnimatableProperty::BORDER_COLOR,
            std::make_shared<RSPropertyAccessors<Vector4<Color>>>(
                &RSProperties::SetBorderColor, &RSProperties::GetBorderColor) },
        { RSAnimatableProperty::BORDER_WIDTH,
            std::make_shared<RSPropertyAccessors<Vector4f>>(
                &RSProperties::SetBorderWidth, &RSProperties::GetBorderWidth) },
        { RSAnimatableProperty::SUB_LAYER_TRANSFORM,
            std::make_shared<RSPropertyAccessors<Matrix3f>>(
                &RSProperties::SetSublayerTransform, &RSProperties::GetSublayerTransform) },
        { RSAnimatableProperty::SHADOW_COLOR,
            std::make_shared<RSPropertyAccessors<Color>>(
                &RSProperties::SetShadowColor, &RSProperties::GetShadowColor) },
        { RSAnimatableProperty::SHADOW_OFFSET_X,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetShadowOffsetX, &RSProperties::GetShadowOffsetX) },
        { RSAnimatableProperty::SHADOW_OFFSET_Y,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetShadowOffsetY, &RSProperties::GetShadowOffsetY) },
        { RSAnimatableProperty::SHADOW_ALPHA,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetShadowAlpha, &RSProperties::GetShadowAlpha) },
        { RSAnimatableProperty::SHADOW_ELEVATION,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetShadowElevation, &RSProperties::GetShadowElevation) },
        { RSAnimatableProperty::SHADOW_RADIUS,
            std::make_shared<RSPropertyAccessors<float>>(
                &RSProperties::SetShadowRadius, &RSProperties::GetShadowRadius) },
        { RSAnimatableProperty::FILTER,
            std::make_shared<RSPropertyAccessors<std::shared_ptr<RSFilter>>>(
                &RSProperties::SetFilter, &RSProperties::GetFilter) },
        { RSAnimatableProperty::BACKGROUND_FILTER,
            std::make_shared<RSPropertyAccessors<std::shared_ptr<RSFilter>>>(
                &RSProperties::SetBackgroundFilter, &RSProperties::GetBackgroundFilter) },
    };
} // namespace Rosen
} // namespace OHOS
