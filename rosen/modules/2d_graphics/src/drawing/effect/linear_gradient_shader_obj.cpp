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

#include "effect/linear_gradient_shader_obj.h"
#include "effect/shader_effect_lazy.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

LinearGradientShaderObj::LinearGradientShaderObj()
    : GradientShaderObjBase(static_cast<int32_t>(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT)),
      startPt_({0, 0}), endPt_({0, 0})
{
}

std::shared_ptr<LinearGradientShaderObj> LinearGradientShaderObj::CreateForUnmarshalling()
{
    return std::shared_ptr<LinearGradientShaderObj>(new LinearGradientShaderObj());
}

std::shared_ptr<LinearGradientShaderObj> LinearGradientShaderObj::Create(const Point& startPt, const Point& endPt,
    const std::vector<UIColor>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, const Matrix* matrix)
{
    if (colors.empty()) {
        LOGE("LinearGradientShaderObj::Create, colors is empty");
        return nullptr;
    }
    return std::shared_ptr<LinearGradientShaderObj>(
        new LinearGradientShaderObj(startPt, endPt, colors, colorSpace, pos, mode, matrix));
}

LinearGradientShaderObj::LinearGradientShaderObj(const Point& startPt, const Point& endPt,
    const std::vector<UIColor>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, const Matrix* matrix)
    : GradientShaderObjBase(static_cast<int32_t>(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT),
        colors, colorSpace, pos, mode, matrix), startPt_(startPt), endPt_(endPt)
{
}

std::shared_ptr<void> LinearGradientShaderObj::GenerateBaseObject()
{
    if (colors_.empty()) {
        return nullptr;
    }

    auto gradientShader = ShaderEffect::CreateLinearGradientNotLazy(startPt_, endPt_, colors_, colorSpace_, pos_,
        mode_, matrix_.get());
    return std::static_pointer_cast<void>(gradientShader);
}

#ifdef ROSEN_OHOS
bool LinearGradientShaderObj::Marshalling(Parcel& parcel)
{
    // Write start point
    if (!parcel.WriteFloat(startPt_.GetX()) || !parcel.WriteFloat(startPt_.GetY())) {
        LOGE("LinearGradientShaderObj::Marshalling, failed to write start point");
        return false;
    }

    // Write end point
    if (!parcel.WriteFloat(endPt_.GetX()) || !parcel.WriteFloat(endPt_.GetY())) {
        LOGE("LinearGradientShaderObj::Marshalling, failed to write end point");
        return false;
    }

    // Marshal common gradient data (colors, positions, mode, matrix)
    return MarshalCommonData(parcel);
}

bool LinearGradientShaderObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("LinearGradientShaderObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Read start point
    scalar startX;
    scalar startY;
    if (!parcel.ReadFloat(startX) || !parcel.ReadFloat(startY)) {
        LOGE("LinearGradientShaderObj::Unmarshalling, failed to read start point");
        return false;
    }
    startPt_ = Point(startX, startY);

    // Read end point
    scalar endX;
    scalar endY;
    if (!parcel.ReadFloat(endX) || !parcel.ReadFloat(endY)) {
        LOGE("LinearGradientShaderObj::Unmarshalling, failed to read end point");
        return false;
    }
    endPt_ = Point(endX, endY);

    // Unmarshal common gradient data (colors, positions, mode, matrix)
    return UnmarshalCommonData(parcel);
}

// Register LinearGradientShaderObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(LinearGradientShaderObj,
    static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT),
    static_cast<int32_t>(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
