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

#include "effect/sweep_gradient_shader_obj.h"
#include "effect/shader_effect_lazy.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

SweepGradientShaderObj::SweepGradientShaderObj()
    : GradientShaderObjBase(static_cast<int32_t>(ShaderEffect::ShaderEffectType::SWEEP_GRADIENT)),
      centerPt_({0, 0}), startAngle_(0.0f), endAngle_(0.0f)
{
}

std::shared_ptr<SweepGradientShaderObj> SweepGradientShaderObj::CreateForUnmarshalling()
{
    return std::shared_ptr<SweepGradientShaderObj>(new SweepGradientShaderObj());
}

std::shared_ptr<SweepGradientShaderObj> SweepGradientShaderObj::Create(const Point& centerPt,
    const std::vector<UIColor>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, scalar startAngle,
    scalar endAngle, const Matrix* matrix)
{
    if (colors.empty()) {
        LOGE("SweepGradientShaderObj::Create, colors is empty");
        return nullptr;
    }
    return std::shared_ptr<SweepGradientShaderObj>(
        new SweepGradientShaderObj(centerPt, colors, colorSpace, pos, mode, startAngle, endAngle, matrix));
}

SweepGradientShaderObj::SweepGradientShaderObj(const Point& centerPt,
    const std::vector<UIColor>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, scalar startAngle,
    scalar endAngle, const Matrix* matrix)
    : GradientShaderObjBase(static_cast<int32_t>(ShaderEffect::ShaderEffectType::SWEEP_GRADIENT),
      colors, colorSpace, pos, mode, matrix), centerPt_(centerPt), startAngle_(startAngle), endAngle_(endAngle)
{
}

std::shared_ptr<void> SweepGradientShaderObj::GenerateBaseObject()
{
    if (colors_.empty()) {
        return nullptr;
    }

    auto gradientShader = ShaderEffect::CreateSweepGradientNotLazy(centerPt_, colors_, colorSpace_, pos_, mode_,
        startAngle_, endAngle_, matrix_.get());
    return std::static_pointer_cast<void>(gradientShader);
}

#ifdef ROSEN_OHOS
bool SweepGradientShaderObj::Marshalling(Parcel& parcel)
{
    // Write center point
    if (!parcel.WriteFloat(centerPt_.GetX()) || !parcel.WriteFloat(centerPt_.GetY())) {
        LOGE("SweepGradientShaderObj::Marshalling, failed to write center point");
        return false;
    }

    // Write start angle
    if (!parcel.WriteFloat(startAngle_)) {
        LOGE("SweepGradientShaderObj::Marshalling, failed to write start angle");
        return false;
    }

    // Write end angle
    if (!parcel.WriteFloat(endAngle_)) {
        LOGE("SweepGradientShaderObj::Marshalling, failed to write end angle");
        return false;
    }

    // Marshal common gradient data (colors, positions, mode, matrix)
    return MarshalCommonData(parcel);
}

bool SweepGradientShaderObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("SweepGradientShaderObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Read center point
    scalar centerX;
    scalar centerY;
    if (!parcel.ReadFloat(centerX) || !parcel.ReadFloat(centerY)) {
        LOGE("SweepGradientShaderObj::Unmarshalling, failed to read center point");
        return false;
    }
    centerPt_ = Point(centerX, centerY);

    // Read start angle
    if (!parcel.ReadFloat(startAngle_)) {
        LOGE("SweepGradientShaderObj::Unmarshalling, failed to read start angle");
        return false;
    }

    // Read end angle
    if (!parcel.ReadFloat(endAngle_)) {
        LOGE("SweepGradientShaderObj::Unmarshalling, failed to read end angle");
        return false;
    }

    // Unmarshal common gradient data (colors, positions, mode, matrix)
    return UnmarshalCommonData(parcel);
}

// Register SweepGradientShaderObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(SweepGradientShaderObj,
    static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT),
    static_cast<int32_t>(ShaderEffect::ShaderEffectType::SWEEP_GRADIENT));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
