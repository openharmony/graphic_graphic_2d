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

#include "ani_shader_effect.h"
#include "matrix_ani/ani_matrix.h"
#include "pixel_map_taihe_ani.h"
#include "image/image.h"
#include "canvas_ani/ani_canvas.h"
 
#ifdef ROSEN_OHOS
#include "pixel_map.h"
#endif

namespace OHOS::Rosen {
namespace Drawing {

ani_status AniShaderEffect::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().shaderEffect;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_SHADER_EFFECT_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "createColorShader", "i:C{@ohos.graphics.drawing.drawing.ShaderEffect}",
            reinterpret_cast<void*>(CreateColorShader) },
        ani_native_function { "createLinearGradient", nullptr,
            reinterpret_cast<void*>(CreateLinearGradient) },
        ani_native_function { "createConicalGradient", nullptr, reinterpret_cast<void*>(CreateConicalGradient) },
        ani_native_function { "createSweepGradient", nullptr, reinterpret_cast<void*>(CreateSweepGradient) },
        ani_native_function { "createRadialGradient", nullptr, reinterpret_cast<void*>(CreateRadialGradient) },
        ani_native_function { "createImageShader", nullptr, reinterpret_cast<void*>(CreateImageShader) },
        ani_native_function { "createComposeShader", nullptr, reinterpret_cast<void*>(CreateComposeShader) },
    };

    ani_status ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_SHADER_EFFECT_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

bool GetColorsArray(ani_env* env, ani_array corlorsArray, std::vector<ColorQuad>& colors)
{
    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(corlorsArray, &aniLength)) {
        ROSEN_LOGE("colors are invalid");
        return false;
    }
    uint32_t colorsSize = static_cast<uint32_t>(aniLength);
    colors.reserve(colorsSize);
    for (uint32_t i = 0; i < colorsSize; i++) {
        ani_int color;
        ani_ref colorRef;
        if (ANI_OK != env->Array_Get(corlorsArray, static_cast<ani_size>(i), &colorRef) ||
            ANI_OK != env->Object_CallMethod_Int(
                static_cast<ani_object>(colorRef), AniGlobalMethod::GetInstance().intGet, &color)) {
            ROSEN_LOGE("get color ref failed.");
            return false;
        }
        colors.push_back(static_cast<Drawing::ColorQuad>(color));
    }
    return true;
}

bool GetPosArray(ani_env* env, ani_array posArray, std::vector<float>& pos)
{
    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(posArray, &aniLength)) {
        ROSEN_LOGE("pos are invalid");
        return false;
    }
    uint32_t size = static_cast<uint32_t>(aniLength);
    pos.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
        ani_double value;
        ani_ref posRef;
        if (ANI_OK != env->Array_Get(posArray, static_cast<ani_size>(i), &posRef) ||
            ANI_OK != env->Object_CallMethod_Double(
                static_cast<ani_object>(posRef), AniGlobalMethod::GetInstance().doubleGet, &value)) {
            ROSEN_LOGE("get pos ref failed.");
            return false;
        }
        pos.push_back(static_cast<float>(value));
    }
    return true;
}

bool GetTileMode(ani_env* env, ani_enum_item aniTileMode, TileMode& mode)
{
    ani_int tileMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniTileMode, &tileMode)) {
        return false;
    }

    mode = static_cast<TileMode>(tileMode);
    if (mode < TileMode::CLAMP || mode > TileMode::DECAL) {
        return false;
    }
    return true;
}

ani_object AniShaderEffect::CreateAniShaderEffect(ani_env* env, std::shared_ptr<ShaderEffect> shaderEffect)
{
    AniShaderEffect* aniShaderEffect = new AniShaderEffect(shaderEffect);
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().shaderEffect,
        AniGlobalMethod::GetInstance().shaderEffectCtor, AniGlobalMethod::GetInstance().shaderEffectBindNative,
        aniShaderEffect);
    if (IsUndefined(env, aniObj)) {
        delete aniShaderEffect;
        ROSEN_LOGE("AniShaderEffect::CreateAniShaderEffect failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniShaderEffect::CreateColorShader(ani_env* env, ani_object obj, ani_int color)
{
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(color);
    return CreateAniShaderEffect(env, shaderEffect);
}

ani_object AniShaderEffect::CreateLinearGradient(ani_env* env, ani_object obj, ani_object startPt, ani_object endPt,
    ani_array colorsArray, ani_enum_item aniTileMode, ani_object aniPos, ani_object aniMatrix)
{
    Drawing::Point startPoint;
    if (GetPointFromPointObj(env, startPt, startPoint) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateLinearGradient get startPoint failed.");
        return CreateAniUndefined(env);
    }
    Drawing::Point endPoint;
    if (GetPointFromPointObj(env, endPt, endPoint) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateLinearGradient get endPoint failed.");
        return CreateAniUndefined(env);
    }
    std::vector<ColorQuad> colors;
    if (!GetColorsArray(env, colorsArray, colors)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateLinearGradient get colorsArray failed.");
        return CreateAniUndefined(env);
    }

    TileMode mode;
    if (!GetTileMode(env, aniTileMode, mode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateLinearGradient get TileMode enum failed.");
        return CreateAniUndefined(env);
    }

    std::vector<scalar> pos;
    if (IsReferenceValid(env, aniPos)) {
        if (!GetPosArray(env, reinterpret_cast<ani_array>(aniPos), pos)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateLinearGradient get posArray failed.");
            return CreateAniUndefined(env);
        }
    }

    Drawing::Matrix* drawingMatrixPtr = nullptr;
    if (IsReferenceValid(env, aniMatrix)) {
        auto aniMatrixObj = GetNativeFromObj<AniMatrix>(env, aniMatrix, AniGlobalField::GetInstance().matrixNativeObj);
        if (aniMatrixObj == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateLinearGradient get matrix failed.");
            return CreateAniUndefined(env);
        }
        if (aniMatrixObj->GetMatrix() != nullptr) {
            drawingMatrixPtr = aniMatrixObj->GetMatrix().get();
        }
    }
    std::shared_ptr<ShaderEffect> shaderEffect =
        ShaderEffect::CreateLinearGradient(startPoint, endPoint, colors, pos, mode, drawingMatrixPtr);
    return CreateAniShaderEffect(env, shaderEffect);
}

ani_object AniShaderEffect::CreateConicalGradient(ani_env* env, ani_object obj, ani_object startPt,
    ani_double startRadius, ani_object endPt, ani_double endRadius, ani_array colorsArray,
    ani_enum_item aniTileMode, ani_object aniPos, ani_object aniMatrix)
{
    Drawing::Point startPoint;
    if (GetPointFromPointObj(env, startPt, startPoint) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateConicalGradient get startPoint failed.");
        return CreateAniUndefined(env);
    }
    Drawing::Point endPoint;
    if (GetPointFromPointObj(env, endPt, endPoint) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateConicalGradient get endPoint failed.");
        return CreateAniUndefined(env);
    }
    std::vector<ColorQuad> colors;
    if (!GetColorsArray(env, colorsArray, colors)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateConicalGradient get colorsArray failed.");
        return CreateAniUndefined(env);
    }

    TileMode mode;
    if (!GetTileMode(env, aniTileMode, mode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateConicalGradient get TileMode enum failed.");
        return CreateAniUndefined(env);
    }

    std::vector<scalar> pos;
    if (IsReferenceValid(env, aniPos)) {
        if (!GetPosArray(env, reinterpret_cast<ani_array>(aniPos), pos)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateConicalGradient get posArray failed.");
            return CreateAniUndefined(env);
        }
    }

    Drawing::Matrix* drawingMatrixPtr = nullptr;
    if (IsReferenceValid(env, aniMatrix)) {
        auto aniMatrixObj = GetNativeFromObj<AniMatrix>(env, aniMatrix, AniGlobalField::GetInstance().matrixNativeObj);
        if (aniMatrixObj == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateConicalGradient get matrix failed.");
            return CreateAniUndefined(env);
        }
        if (aniMatrixObj->GetMatrix() != nullptr) {
            drawingMatrixPtr = aniMatrixObj->GetMatrix().get();
        }
    }
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateTwoPointConical(
        startPoint, startRadius, endPoint, endRadius, colors, pos, mode, drawingMatrixPtr);
    return CreateAniShaderEffect(env, shaderEffect);
}

ani_object AniShaderEffect::CreateSweepGradient(
    ani_env* env, ani_object obj, ani_object centerPt, ani_array colorsArray, ani_enum_item aniTileMode,
    ani_double startAngle, ani_double endAngle, ani_object aniPos, ani_object aniMatrix)
{
    Drawing::Point centerPoint;
    if (GetPointFromPointObj(env, centerPt, centerPoint) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateSweepGradient get centerPoint failed.");
        return CreateAniUndefined(env);
    }

    std::vector<ColorQuad> colors;
    if (!GetColorsArray(env, colorsArray, colors)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateSweepGradient get colorsArray failed.");
        return CreateAniUndefined(env);
    }

    TileMode mode;
    if (!GetTileMode(env, aniTileMode, mode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateSweepGradient get TileMode enum failed.");
        return CreateAniUndefined(env);
    }

    std::vector<scalar> pos;
    if (IsReferenceValid(env, aniPos)) {
        if (!GetPosArray(env, reinterpret_cast<ani_array>(aniPos), pos)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateSweepGradient get posArray failed.");
            return CreateAniUndefined(env);
        }
    }

    Drawing::Matrix* drawingMatrixPtr = nullptr;
    if (IsReferenceValid(env, aniMatrix)) {
        auto aniMatrixObj = GetNativeFromObj<AniMatrix>(env, aniMatrix, AniGlobalField::GetInstance().matrixNativeObj);
        if (aniMatrixObj == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateSweepGradient get matrix failed.");
            return CreateAniUndefined(env);
        }
        if (aniMatrixObj->GetMatrix() != nullptr) {
            drawingMatrixPtr = aniMatrixObj->GetMatrix().get();
        }
    }
    std::shared_ptr<ShaderEffect> shaderEffect =
       ShaderEffect::CreateSweepGradient(centerPoint, colors, pos, mode, startAngle, endAngle, drawingMatrixPtr);
    return CreateAniShaderEffect(env, shaderEffect);
}

ani_object AniShaderEffect::CreateRadialGradient(ani_env* env, ani_object obj, ani_object centerPt, ani_double radius,
    ani_array colorsArray, ani_enum_item aniTileMode, ani_object aniPos, ani_object aniMatrix)
{
    Drawing::Point centerPoint;
    if (GetPointFromPointObj(env, centerPt, centerPoint) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateRadialGradient get centerPoint failed.");
        return CreateAniUndefined(env);
    }

    std::vector<ColorQuad> colors;
    if (!GetColorsArray(env, colorsArray, colors)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateRadialGradient get colorsArray failed.");
        return CreateAniUndefined(env);
    }

    TileMode mode;
    if (!GetTileMode(env, aniTileMode, mode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateRadialGradient get TileMode enum failed.");
        return CreateAniUndefined(env);
    }

    std::vector<scalar> pos;
    if (IsReferenceValid(env, aniPos)) {
        if (!GetPosArray(env, reinterpret_cast<ani_array>(aniPos), pos)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateRadialGradient get posArray failed.");
            return CreateAniUndefined(env);
        }
    }

    Drawing::Matrix* drawingMatrixPtr = nullptr;
    if (IsReferenceValid(env, aniMatrix)) {
        auto aniMatrixObj = GetNativeFromObj<AniMatrix>(env, aniMatrix, AniGlobalField::GetInstance().matrixNativeObj);
        if (aniMatrixObj == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateRadialGradient get matrix failed.");
            return CreateAniUndefined(env);
        }
        if (aniMatrixObj->GetMatrix() != nullptr) {
            drawingMatrixPtr = aniMatrixObj->GetMatrix().get();
        }
    }
    std::shared_ptr<ShaderEffect> shaderEffect =
       ShaderEffect::CreateRadialGradient(centerPoint, radius, colors, pos, mode, drawingMatrixPtr);
    return CreateAniShaderEffect(env, shaderEffect);
}

ani_object AniShaderEffect::CreateImageShader(ani_env* env, ani_object obj, ani_object pixelmapObj,
    ani_enum_item aniTileX, ani_enum_item aniTileY, ani_object samplingOptionsobj, ani_object aniMatrix)
{
#ifdef ROSEN_OHOS
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ROSEN_LOGE("AniShaderEffect::CreateImageShader get pixelMap failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateImageShader pixelMap is nullptr.");
        return CreateAniUndefined(env);
    }
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMap);
    if (image == nullptr) {
        ROSEN_LOGE("AniShaderEffect::CreateImageShader image is nullptr");
        return CreateAniUndefined(env);
    }

    TileMode modex;
    TileMode modey;
    if (!GetTileMode(env, aniTileX, modex) || !GetTileMode(env, aniTileY, modey)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateImageShader get TileMode enum failed.");
        return CreateAniUndefined(env);
    }

    Drawing::SamplingOptions samplingOptions;
    AniSamplingOptions* aniSamplingOptions = GetNativeFromObj<AniSamplingOptions>(env, samplingOptionsobj,
        AniGlobalField::GetInstance().samplingOptionsNativeObj);
    if (aniSamplingOptions == nullptr || aniSamplingOptions->GetSamplingOptions() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniShaderEffect::CreateImageShader samplingOptions is nullptr.");
        return CreateAniUndefined(env);
    }
    samplingOptions = *(aniSamplingOptions->GetSamplingOptions());
    
    Drawing::Matrix drawingMatrix;
    if (IsReferenceValid(env, aniMatrix)) {
        auto aniMatrixObj = GetNativeFromObj<AniMatrix>(env, aniMatrix, AniGlobalField::GetInstance().matrixNativeObj);
        if (aniMatrixObj == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniShaderEffect::CreateImageShader get matrix failed.");
            return CreateAniUndefined(env);
        }
        drawingMatrix = *aniMatrixObj->GetMatrix();
    }
    std::shared_ptr<ShaderEffect> imageShader = ShaderEffect::CreateImageShader(*image,
        modex, modey, samplingOptions, drawingMatrix);
    return CreateAniShaderEffect(env, imageShader);
#endif
    return nullptr;
}

ani_object AniShaderEffect::CreateComposeShader(ani_env* env, ani_object obj, ani_object dstShaderEffect,
    ani_object srcShaderEffect, ani_enum_item blendModeobj)
{
    AniShaderEffect* aniDstShaderEffect = nullptr;
    aniDstShaderEffect = GetNativeFromObj<AniShaderEffect>(env, dstShaderEffect,
        AniGlobalField::GetInstance().shaderEffectNativeObj);
    if (aniDstShaderEffect == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param shaderEffect.");
        return CreateAniUndefined(env);
    }

    AniShaderEffect* aniSrcShaderEffect = nullptr;
    aniSrcShaderEffect = GetNativeFromObj<AniShaderEffect>(env, srcShaderEffect,
        AniGlobalField::GetInstance().shaderEffectNativeObj);
    if (aniSrcShaderEffect == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param shaderEffect.");
        return CreateAniUndefined(env);
    }

    ani_int blendMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(blendModeobj, &blendMode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<ShaderEffect> effectShader = ShaderEffect::CreateBlendShader(*aniDstShaderEffect->GetShaderEffect(),
        *aniSrcShaderEffect->GetShaderEffect(), static_cast<BlendMode>(blendMode));
    return CreateAniShaderEffect(env, effectShader);
}

AniShaderEffect::~AniShaderEffect()
{
    shaderEffect_ = nullptr;
}

std::shared_ptr<ShaderEffect> AniShaderEffect::GetShaderEffect()
{
    return shaderEffect_;
}
} // namespace Drawing
} // namespace OHOS::Rosen