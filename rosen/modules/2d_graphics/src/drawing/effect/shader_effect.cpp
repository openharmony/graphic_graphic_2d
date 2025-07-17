/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#include "utils/log.h"

#include "impl_factory.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
/* ColorShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, ColorQuad color) noexcept : ShaderEffect(t)
{
    impl_->InitWithColor(color);
}

/* ColorShader With ColorSpace */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Color4f& color, std::shared_ptr<ColorSpace> colorSpace) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithColorSpace(color, colorSpace);
}

/* BlendShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, ShaderEffect& dst, ShaderEffect& src, BlendMode mode) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithBlend(dst, src, mode);
}

/* ImageShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Image& image, TileMode tileX, TileMode tileY,
    const SamplingOptions& sampling, const Matrix& matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithImage(image, tileX, tileY, sampling, matrix);
}

/* PictureShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Picture& picture, TileMode tileX, TileMode tileY, FilterMode mode,
    const Matrix& matrix, const Rect& rect) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithPicture(picture, tileX, tileY, mode, matrix, rect);
}

/* LinearGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, const Point& endPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithLinearGradient(startPt, endPt, colors, pos, mode, matrix);
}

/* LinearGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, const Point& endPt,
    const std::vector<Color4f>& colors, std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
    TileMode mode, const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithLinearGradient(startPt, endPt, colors, colorSpace, pos, mode, matrix);
}

/* RadialGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithRadialGradient(centerPt, radius, colors, pos, mode, matrix);
}

/* RadialGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, scalar radius,
    const std::vector<Color4f>& colors, std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
    TileMode mode, const Matrix* matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithRadialGradient(centerPt, radius, colors, colorSpace, pos, mode, matrix);
}

/* TwoPointConicalGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, scalar startRadius, const Point& endPt,
    scalar endRadius, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode,
    const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithTwoPointConical(startPt, startRadius, endPt, endRadius, colors, pos, mode, matrix);
}

/* TwoPointConicalGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, scalar startRadius, const Point& endPt,
    scalar endRadius, const std::vector<Color4f>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, const Matrix* matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithTwoPointConical(startPt, startRadius, endPt, endRadius, colors, colorSpace, pos, mode, matrix);
}

/* SweepGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, const std::vector<ColorQuad>& colors,
    const std::vector<scalar>& pos, TileMode mode, scalar startAngle, scalar endAngle, const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithSweepGradient(centerPt, colors, pos, mode, startAngle, endAngle, matrix);
}

/* SweepGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, const std::vector<Color4f>& colors,
    std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos, TileMode mode, scalar startAngle,
    scalar endAngle, const Matrix* matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithSweepGradient(centerPt, colors, colorSpace, pos, mode, startAngle, endAngle, matrix);
}

/* LightUpShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, const float& lightUpDeg, ShaderEffect& imageShader) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithLightUp(lightUpDeg, imageShader);
}

/* ExtendShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, std::shared_ptr<ExtendObject> object) noexcept
    : type_(t), impl_(ImplFactory::CreateShaderEffectImpl()), object_(object) {}

/* SdfShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, const SDFShapeBase& shape) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithSdf(shape);
}

ShaderEffect::ShaderEffect() noexcept
    : type_(ShaderEffect::ShaderEffectType::NO_TYPE), impl_(ImplFactory::CreateShaderEffectImpl())
{}

ShaderEffect::ShaderEffect(ShaderEffectType t) noexcept : type_(t), impl_(ImplFactory::CreateShaderEffectImpl()) {}

ShaderEffect::ShaderEffectType ShaderEffect::GetType() const
{
    return type_;
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateColorShader(ColorQuad color)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR_SHADER, color);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateColorSpaceShader(const Color4f& color,
    std::shared_ptr<ColorSpace> colorSpace)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR_SHADER, color, colorSpace);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateBlendShader(ShaderEffect& dst, ShaderEffect& src, BlendMode mode)
{
    // Check if dst shader is Lazy type, not supported for direct CreateBlendShader
    if (dst.IsLazy()) {
        LOGE("ShaderEffect::CreateBlendShader, dst shader is Lazy type, not supported.");
        return nullptr;
    }

    // Check if src shader is Lazy type, not supported for direct CreateBlendShader
    if (src.IsLazy()) {
        LOGE("ShaderEffect::CreateBlendShader, src shader is Lazy type, not supported.");
        return nullptr;
    }

    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::BLEND, dst, src, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateImageShader(
    const Image& image, TileMode tileX, TileMode tileY, const SamplingOptions& sampling, const Matrix& matrix)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::IMAGE, image, tileX, tileY, sampling, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreatePictureShader(
    const Picture& picture, TileMode tileX, TileMode tileY, FilterMode mode, const Matrix& matrix, const Rect& rect)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::PICTURE, picture, tileX, tileY, mode, matrix, rect);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateLinearGradient(const Point& startPt, const Point& endPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, startPt, endPt, colors, pos, mode, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateLinearGradient(const Point& startPt, const Point& endPt,
    const std::vector<Color4f>& colors, std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
    TileMode mode, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, startPt, endPt, colors, colorSpace, pos, mode, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateRadialGradient(const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::RADIAL_GRADIENT, centerPt, radius, colors, pos, mode, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateRadialGradient(const Point& centerPt, scalar radius,
    const std::vector<Color4f>& colors, std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
    TileMode mode, const Matrix* matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::RADIAL_GRADIENT, centerPt, radius, colors, colorSpace, pos, mode, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateTwoPointConical(const Point& startPt, scalar startRadius,
    const Point& endPt, scalar endRadius, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos,
    TileMode mode, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, startPt, startRadius, endPt, endRadius, colors, pos, mode,
        matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateTwoPointConical(const Point& startPt, scalar startRadius,
    const Point& endPt, scalar endRadius, const std::vector<Color4f>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, startPt, startRadius, endPt, endRadius, colors, colorSpace,
        pos, mode, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateSweepGradient(const Point& centerPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, scalar startAngle,
    scalar endAngle, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::SWEEP_GRADIENT, centerPt, colors, pos, mode, startAngle, endAngle, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateSweepGradient(const Point& centerPt,
    const std::vector<Color4f>& colors, std::shared_ptr<ColorSpace> colorSpace,const std::vector<scalar>& pos,
    TileMode mode, scalar startAngle, scalar endAngle, const Matrix* matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::SWEEP_GRADIENT, centerPt, colors, colorSpace, pos, mode, startAngle,
        endAngle, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateLightUp(const float& lightUpDeg, ShaderEffect& imageShader)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::LIGHT_UP, lightUpDeg, imageShader);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateExtendShader(std::shared_ptr<ExtendObject> object)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::EXTEND_SHADER, object);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateSdfShader(const SDFShapeBase& shape)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::SDF_SHADER, shape);
}

#ifdef RS_ENABLE_GPU
void ShaderEffect::SetGPUContext(std::shared_ptr<GPUContext> gpuContext) const
{
    impl_->SetGPUContext(gpuContext);
}
#endif

std::shared_ptr<Data> ShaderEffect::Serialize() const
{
    return impl_->Serialize();
}

bool ShaderEffect::Deserialize(std::shared_ptr<Data> data)
{
    return impl_->Deserialize(data);
}

#ifdef ROSEN_OHOS
bool ShaderEffect::Marshalling(Parcel& parcel)
{
    // Write type first
    if (!parcel.WriteInt32(static_cast<int32_t>(type_))) {
        LOGE("ShaderEffect::Marshalling, failed to write type");
        return false;
    }

    // Use Serialize to convert to Data then serialize
    auto data = Serialize();

    // Write flag indicating whether data is valid
    bool hasValidData = (data != nullptr && data->GetSize() > 0);
    if (!parcel.WriteBool(hasValidData)) {
        LOGE("ShaderEffect::Marshalling, failed to write hasData flag");
        return false;
    }

    // If data is null or empty (empty shader), just write the flag and return success
    // This prevents parcel failure when underlying shader creation failed
    if (!hasValidData) {
        LOGD("ShaderEffect::Marshalling, Serialize returned null or empty data (empty shader), "
             "continuing with empty marker");
        return true;
    }

    // Use registered callback for Data marshalling
    auto callback = ObjectHelper::Instance().GetDataMarshallingCallback();
    if (!callback) {
        LOGE("ShaderEffect::Marshalling, DataMarshallingCallback is not registered");
        return false;
    }
    if (!callback(parcel, data)) {
        LOGE("ShaderEffect::Marshalling, DataMarshallingCallback failed");
        return false;
    }
    return true;
}

std::shared_ptr<ShaderEffect> ShaderEffect::Unmarshalling(Parcel& parcel, bool& isValid)
{
    // Read type first
    int32_t typeValue;
    if (!parcel.ReadInt32(typeValue)) {
        LOGE("ShaderEffect::Unmarshalling, failed to read type");
        return nullptr;
    }

    // Validate type range (should be valid shader types, excluding NO_TYPE and LAZY_SHADER)
    if (typeValue < static_cast<int32_t>(ShaderEffectType::NO_TYPE) ||
        typeValue >= static_cast<int32_t>(ShaderEffectType::LAZY_SHADER)) {
        LOGE("ShaderEffect::Unmarshalling, invalid type value: %{public}d", typeValue);
        return nullptr;
    }

    // Read hasData flag
    bool hasData;
    if (!parcel.ReadBool(hasData)) {
        LOGE("ShaderEffect::Unmarshalling, failed to read hasData flag");
        return nullptr;
    }

    // If no data (empty shader), create an empty ShaderEffect and return
    if (!hasData) {
        LOGD("ShaderEffect::Unmarshalling, empty shader marker detected, creating empty ShaderEffect");
        auto shader = std::make_shared<ShaderEffect>(static_cast<ShaderEffectType>(typeValue));
        return shader;
    }

    // Use registered callback for Data unmarshalling
    auto callback = ObjectHelper::Instance().GetDataUnmarshallingCallback();
    if (!callback) {
        LOGE("ShaderEffect::Unmarshalling, DataUnmarshallingCallback is not registered");
        return nullptr;
    }
    auto data = callback(parcel);
    if (!data) {
        LOGE("ShaderEffect::Unmarshalling, DataUnmarshallingCallback failed");
        return nullptr;
    }

    // Create ShaderEffect with correct type
    auto shader = std::make_shared<ShaderEffect>(static_cast<ShaderEffectType>(typeValue));
    if (!shader->Deserialize(data)) {
        LOGE("ShaderEffect::Unmarshalling, Deserialize failed");
        // For compatibility: mark as invalid but return object instead of nullptr
        isValid = false;
        return shader;
    }
    return shader;
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
