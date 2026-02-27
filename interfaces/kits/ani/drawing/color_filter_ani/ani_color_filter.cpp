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

#include "ani_color_filter.h"
#include "draw/color.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/color_filter_napi/js_color_filter.h"

namespace OHOS::Rosen {
namespace Drawing {

ani_status AniColorFilter::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().colorFilter;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_COLORFILTER_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "createBlendModeColorFilter", "C{@ohos.graphics.common2D.common2D.Color}"
            "C{@ohos.graphics.drawing.drawing.BlendMode}:C{@ohos.graphics.drawing.drawing.ColorFilter}",
            reinterpret_cast<void*>(CreateBlendModeColorFilter) },
        ani_native_function { "createBlendModeColorFilter",
            "X{C{@ohos.graphics.common2D.common2D.Color}C{std.core.Int}}"
            "C{@ohos.graphics.drawing.drawing.BlendMode}:C{@ohos.graphics.drawing.drawing.ColorFilter}",
            reinterpret_cast<void*>(CreateBlendModeColorFilterWithNumber) },
        ani_native_function { "colorFilterTransferStaticNative", nullptr,
            reinterpret_cast<void*>(ColorFilterTransferStatic) },
        ani_native_function { "getColorFilterAddr", nullptr, reinterpret_cast<void*>(GetColorFilterAddr) },
        ani_native_function { "createMatrixColorFilter", nullptr, reinterpret_cast<void*>(CreateMatrixColorFilter) },
        ani_native_function { "createComposeColorFilter", nullptr, reinterpret_cast<void*>(CreateComposeColorFilter) },
        ani_native_function { "createLinearToSRGBGamma", nullptr, reinterpret_cast<void*>(CreateLinearToSRGBGamma) },
        ani_native_function { "createLumaColorFilter", nullptr, reinterpret_cast<void*>(CreateLumaColorFilter) },
        ani_native_function { "createLightingColorFilter", nullptr,
            reinterpret_cast<void*>(CreateLightingColorFilterWithColor) },
        ani_native_function { "createSRGBGammaToLinear", nullptr, reinterpret_cast<void*>(CreateSRGBGammaToLinear) },
    };

    ani_status ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s ret: %{public}d", ANI_CLASS_COLORFILTER_NAME, ret);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}


AniColorFilter::~AniColorFilter()
{
    colorFilter_ = nullptr;
}

ani_object AniColorFilter::CreateBlendModeColorFilter(
    ani_env* env, [[maybe_unused]] ani_object obj, ani_object objColor, ani_enum_item aniBlendMode)
{
    ColorQuad color;
    if (!GetColorQuadFromColorObj(env, objColor, color)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid objColor params.");
        return CreateAniUndefined(env);
    }

    ani_int blendMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniBlendMode, &blendMode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid blendMode params.");
        return CreateAniUndefined(env);
    }

    AniColorFilter* colorFilter = new AniColorFilter(
        ColorFilter::CreateBlendModeColorFilter(color, static_cast<BlendMode>(blendMode)));
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateBlendModeColorFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniColorFilter::CreateBlendModeColorFilterWithNumber(
    ani_env* env, [[maybe_unused]] ani_object obj, ani_object objColor, ani_enum_item aniBlendMode)
{
    ColorQuad color;
    if (!GetColorQuadFromParam(env, objColor, color)) {
        ROSEN_LOGE("AniColorFilter::CreateBlendModeColorFilterWithNumber failed cause by colorObj");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }

    ani_int blendMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniBlendMode, &blendMode)) {
        ROSEN_LOGE("AniColorFilter::CreateBlendModeColorFilterWithNumber failed cause by blendMode");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }

    AniColorFilter* colorFilter = new AniColorFilter(
        ColorFilter::CreateBlendModeColorFilter(color, static_cast<BlendMode>(blendMode)));
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateBlendModeColorFilterWithNumber failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniColorFilter::ColorFilterTransferStatic(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniColorFilter::ColorFilterTransferStatic failed to unwrap");
        return CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniColorFilter::ColorFilterTransferStatic unwrapResult is null");
        return CreateAniUndefined(env);
    }
    auto jsColorFilter = reinterpret_cast<JsColorFilter*>(unwrapResult);
    if (jsColorFilter->GetColorFilterPtr() == nullptr) {
        ROSEN_LOGE("AniColorFilter::ColorFilterTransferStatic jsColorFilter is null");
        return CreateAniUndefined(env);
    }

    auto aniColorFilter = new AniColorFilter(jsColorFilter->GetColorFilterPtr());
    ani_object aniColorFilterObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        aniColorFilter);
    if (IsUndefined(env, aniColorFilterObj)) {
        delete aniColorFilter;
        ROSEN_LOGE("AniColorFilter::ColorFilterTransferStatic failed cause aniObj is undefined");
    }
    return aniColorFilterObj;
}

ani_long AniColorFilter::GetColorFilterAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniColorFilter = GetNativeFromObj<AniColorFilter>(env, input,
        AniGlobalField::GetInstance().colorFilterNativeObj);
    if (aniColorFilter == nullptr || aniColorFilter->GetColorFilter() == nullptr) {
        ROSEN_LOGE("AniColorFilter::GetColorFilterAddr aniColorFilter is null");
        return 0;
    }
    
    return reinterpret_cast<ani_long>(aniColorFilter->GetColorFilterPtrAddr());
}

std::shared_ptr<ColorFilter>* AniColorFilter::GetColorFilterPtrAddr()
{
    return &colorFilter_;
}

ani_object AniColorFilter::CreateLightingColorFilterWithColor(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object aniMultiplyColorObj, ani_object aniAddColorObj)
{
    Drawing::ColorQuad multiplyColor;
    if (!GetColorQuadFromParam(env, aniMultiplyColorObj, multiplyColor)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param mutColor.");
        return CreateAniUndefined(env);
    }

    Drawing::ColorQuad addColor;
    if (!GetColorQuadFromParam(env, aniAddColorObj, addColor)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param addColor.");
        return CreateAniUndefined(env);
    }
    AniColorFilter* colorFilter = new AniColorFilter(
        ColorFilter::CreateLightingColorFilter(multiplyColor, addColor));
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateLightingColorFilterWithColor failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniColorFilter::CreateLightingColorFilter(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_double multiplyColor, ani_double addColor)
{
    AniColorFilter* colorFilter = new AniColorFilter(
        ColorFilter::CreateLightingColorFilter(multiplyColor, addColor));
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateLightingColorFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniColorFilter::CreateMatrixColorFilter(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_array aniMatrixArrayObj)
{
    ani_size length;
    ani_status ret = env->Array_GetLength(aniMatrixArrayObj, &length);
    if (ret != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param matrix.");
        return CreateAniUndefined(env);
    }

    uint32_t arraySize = static_cast<uint32_t>(length);
    if (arraySize != ColorMatrix::MATRIX_SIZE) {
        ROSEN_LOGD("AniColorFilter::CreateMatrixColorFilter aniMatrixArrayObj size is invalid %{public}u.", arraySize);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid matrix array size.");
        return CreateAniUndefined(env);
    }
    std::unique_ptr<float[]> matrixArray = std::make_unique<float[]>(arraySize);
    for (uint32_t i = 0; i < arraySize; i++) {
        ani_double value;
        ani_ref aniRef = nullptr;
        ret = env->Array_Get(aniMatrixArrayObj, i, &aniRef);
        if (ret != ANI_OK) {
            ROSEN_LOGD("AniColorFilter::CreateMatrixColorFilter get matrix array element failed %{public}d", ret);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid matrix array element.");
            return CreateAniUndefined(env);
        }
        ret = env->Object_CallMethod_Double(
            reinterpret_cast<ani_object>(aniRef), AniGlobalMethod::GetInstance().doubleGet, &value);
        if (ret != ANI_OK) {
            ROSEN_LOGD("AniColorFilter::CreateMatrixColorFilter get matrix array element failed %{public}d", ret);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid matrix array element.");
            return CreateAniUndefined(env);
        }
        matrixArray[i] = value;
    }
    Drawing::ColorMatrix matrix;
    matrix.SetArray(matrixArray.get());
    AniColorFilter* colorFilter = new AniColorFilter(ColorFilter::CreateMatrixColorFilter(matrix));
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateMatrixColorFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniColorFilter::CreateComposeColorFilter(
    ani_env* env, [[maybe_unused]] ani_object obj, ani_object aniOuterColorFilterObj, ani_object aniInnerColorFilterObj)
{
    auto aniOuterColorFilter = GetNativeFromObj<AniColorFilter>(env, aniOuterColorFilterObj,
        AniGlobalField::GetInstance().colorFilterNativeObj);
    if (aniOuterColorFilter == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param outer.");
        return CreateAniUndefined(env);
    }
    auto aniInnerColorFilter = GetNativeFromObj<AniColorFilter>(env, aniInnerColorFilterObj,
        AniGlobalField::GetInstance().colorFilterNativeObj);
    if (aniInnerColorFilter == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param inner.");
        return CreateAniUndefined(env);
    }
    AniColorFilter* colorFilter = new AniColorFilter(ColorFilter::CreateComposeColorFilter(
        *(aniOuterColorFilter->GetColorFilter()), *(aniInnerColorFilter->GetColorFilter())));
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateMatrixColorFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniColorFilter::CreateLinearToSRGBGamma(ani_env* env, [[maybe_unused]] ani_object obj)
{
    AniColorFilter* colorFilter = new AniColorFilter(ColorFilter::CreateLinearToSrgbGamma());
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateLinearToSRGBGamma failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniColorFilter::CreateLumaColorFilter(ani_env* env, [[maybe_unused]] ani_object obj)
{
    AniColorFilter* colorFilter = new AniColorFilter(ColorFilter::CreateLumaColorFilter());
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateLumaColorFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniColorFilter::CreateSRGBGammaToLinear(ani_env* env, [[maybe_unused]] ani_object obj)
{
    AniColorFilter* colorFilter = new AniColorFilter(ColorFilter::CreateSrgbGammaToLinear());
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor, AniGlobalMethod::GetInstance().colorFilterBindNative,
        colorFilter);
    if (IsUndefined(env, aniObj)) {
        delete colorFilter;
        ROSEN_LOGE("AniColorFilter::CreateSRGBGammaToLinear failed cause aniObj is undefined");
    }
    return aniObj;
}

std::shared_ptr<ColorFilter> AniColorFilter::GetColorFilter()
{
    return colorFilter_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
