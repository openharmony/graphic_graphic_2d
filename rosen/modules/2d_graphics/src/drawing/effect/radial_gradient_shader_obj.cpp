/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "effect/radial_gradient_shader_obj.h"
#include "effect/shader_effect_lazy.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

RadialGradientShaderObj::RadialGradientShaderObj()
    : GradientShaderObjBase(static_cast<int32_t>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT)),
      centerPt_({0, 0}), radius_(0.0f)
{
}

std::shared_ptr<RadialGradientShaderObj> RadialGradientShaderObj::CreateForUnmarshalling()
{
    return std::shared_ptr<RadialGradientShaderObj>(new RadialGradientShaderObj());
}

std::shared_ptr<RadialGradientShaderObj> RadialGradientShaderObj::Create(const Point& centerPt, scalar radius,
    const std::vector<UIColor>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, const Matrix* matrix)
{
    if (colors.empty()) {
        LOGE("RadialGradientShaderObj::Create, colors is empty");
        return nullptr;
    }
    return std::shared_ptr<RadialGradientShaderObj>(
        new RadialGradientShaderObj(centerPt, radius, colors, colorSpace, pos, mode, matrix));
}

RadialGradientShaderObj::RadialGradientShaderObj(const Point& centerPt, scalar radius,
    const std::vector<UIColor>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, const Matrix* matrix)
    : GradientShaderObjBase(static_cast<int32_t>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT),
      colors, colorSpace, pos, mode, matrix), centerPt_(centerPt), radius_(radius)
{
}

std::shared_ptr<void> RadialGradientShaderObj::GenerateBaseObject()
{
    if (colors_.empty()) {
        return nullptr;
    }

    auto gradientShader = ShaderEffect::CreateRadialGradientNotLazy(centerPt_, radius_, colors_, colorSpace_, pos_,
        mode_, matrix_.get());
    return std::static_pointer_cast<void>(gradientShader);
}

#ifdef ROSEN_OHOS
bool RadialGradientShaderObj::Marshalling(Parcel& parcel)
{
    // Write center point
    if (!parcel.WriteFloat(centerPt_.GetX()) || !parcel.WriteFloat(centerPt_.GetY())) {
        LOGE("RadialGradientShaderObj::Marshalling, failed to write center point");
        return false;
    }

    // Write radius
    if (!parcel.WriteFloat(radius_)) {
        LOGE("RadialGradientShaderObj::Marshalling, failed to write radius");
        return false;
    }

    // Marshal common gradient data (colors, positions, mode, matrix)
    return MarshalCommonData(parcel);
}

bool RadialGradientShaderObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("RadialGradientShaderObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Read center point
    scalar centerX;
    scalar centerY;
    if (!parcel.ReadFloat(centerX) || !parcel.ReadFloat(centerY)) {
        LOGE("RadialGradientShaderObj::Unmarshalling, failed to read center point");
        return false;
    }
    centerPt_ = Point(centerX, centerY);

    // Read radius
    if (!parcel.ReadFloat(radius_)) {
        LOGE("RadialGradientShaderObj::Unmarshalling, failed to read radius");
        return false;
    }

    // Unmarshal common gradient data (colors, positions, mode, matrix)
    return UnmarshalCommonData(parcel);
}

// Register RadialGradientShaderObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(RadialGradientShaderObj,
    static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT),
    static_cast<int32_t>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
