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

#include "effect/conical_gradient_shader_obj.h"
#include "effect/shader_effect_lazy.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

ConicalGradientShaderObj::ConicalGradientShaderObj()
    : GradientShaderObjBase(static_cast<int32_t>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT)),
      startPt_({0, 0}), startRadius_(0.0f), endPt_({0, 0}), endRadius_(0.0f)
{
}

std::shared_ptr<ConicalGradientShaderObj> ConicalGradientShaderObj::CreateForUnmarshalling()
{
    return std::shared_ptr<ConicalGradientShaderObj>(new ConicalGradientShaderObj());
}

std::shared_ptr<ConicalGradientShaderObj> ConicalGradientShaderObj::Create(const Point& startPt, scalar startRadius,
    const Point& endPt, scalar endRadius, const std::vector<UIColor>& colors,
    std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
    TileMode mode, const Matrix* matrix)
{
    if (colors.empty()) {
        LOGE("ConicalGradientShaderObj::Create, colors is empty");
        return nullptr;
    }
    return std::shared_ptr<ConicalGradientShaderObj>(
        new ConicalGradientShaderObj(startPt, startRadius, endPt, endRadius, colors, colorSpace, pos, mode, matrix));
}

ConicalGradientShaderObj::ConicalGradientShaderObj(const Point& startPt, scalar startRadius,
    const Point& endPt, scalar endRadius, const std::vector<UIColor>& colors,
    std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
    TileMode mode, const Matrix* matrix)
    : GradientShaderObjBase(static_cast<int32_t>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT),
      colors, colorSpace, pos, mode, matrix), startPt_(startPt), startRadius_(startRadius),
      endPt_(endPt), endRadius_(endRadius)
{
}

std::shared_ptr<void> ConicalGradientShaderObj::GenerateBaseObject()
{
    if (colors_.empty()) {
        return nullptr;
    }

    auto gradientShader = ShaderEffect::CreateTwoPointConicalNotLazy(startPt_, startRadius_, endPt_, endRadius_,
        colors_, colorSpace_, pos_, mode_, matrix_.get());
    return std::static_pointer_cast<void>(gradientShader);
}

#ifdef ROSEN_OHOS
bool ConicalGradientShaderObj::Marshalling(Parcel& parcel)
{
    // Write start point
    if (!parcel.WriteFloat(startPt_.GetX()) || !parcel.WriteFloat(startPt_.GetY())) {
        LOGE("ConicalGradientShaderObj::Marshalling, failed to write start point");
        return false;
    }

    // Write start radius
    if (!parcel.WriteFloat(startRadius_)) {
        LOGE("ConicalGradientShaderObj::Marshalling, failed to write start radius");
        return false;
    }

    // Write end point
    if (!parcel.WriteFloat(endPt_.GetX()) || !parcel.WriteFloat(endPt_.GetY())) {
        LOGE("ConicalGradientShaderObj::Marshalling, failed to write end point");
        return false;
    }

    // Write end radius
    if (!parcel.WriteFloat(endRadius_)) {
        LOGE("ConicalGradientShaderObj::Marshalling, failed to write end radius");
        return false;
    }

    // Marshal common gradient data (colors, positions, mode, matrix)
    return MarshalCommonData(parcel);
}

bool ConicalGradientShaderObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("ConicalGradientShaderObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Read start point
    scalar startX;
    scalar startY;
    if (!parcel.ReadFloat(startX) || !parcel.ReadFloat(startY)) {
        LOGE("ConicalGradientShaderObj::Unmarshalling, failed to read start point");
        return false;
    }
    startPt_ = Point(startX, startY);

    // Read start radius
    if (!parcel.ReadFloat(startRadius_)) {
        LOGE("ConicalGradientShaderObj::Unmarshalling, failed to read start radius");
        return false;
    }

    // Read end point
    scalar endX;
    scalar endY;
    if (!parcel.ReadFloat(endX) || !parcel.ReadFloat(endY)) {
        LOGE("ConicalGradientShaderObj::Unmarshalling, failed to read end point");
        return false;
    }
    endPt_ = Point(endX, endY);

    // Read end radius
    if (!parcel.ReadFloat(endRadius_)) {
        LOGE("ConicalGradientShaderObj::Unmarshalling, failed to read end radius");
        return false;
    }

    // Unmarshal common gradient data (colors, positions, mode, matrix)
    return UnmarshalCommonData(parcel);
}

// Register ConicalGradientShaderObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(ConicalGradientShaderObj,
    static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT),
    static_cast<int32_t>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
