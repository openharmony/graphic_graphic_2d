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

#include "ani_brush.h"
#include "color_filter_ani/ani_color_filter.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/brush_napi/js_brush.h"
#include "shadow_layer_ani/ani_shadow_layer.h"
#include "shader_effect_ani/ani_shader_effect.h"
#include "path_effect_ani/ani_path_effect.h"
#include "mask_filter_ani/ani_mask_filter.h"
#include "image_filter_ani/ani_image_filter.h"

#ifdef ROSEN_OHOS
#include "ani_color_space_manager.h"
#include "utils/colorspace_convertor.h"
#endif

namespace OHOS::Rosen {
namespace Drawing {

ani_status AniBrush::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().brush;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_BRUSH_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "C{@ohos.graphics.drawing.drawing.Brush}:",
            reinterpret_cast<void*>(ConstructorWithBrush) },
        ani_native_function { "getAlpha", ":i", reinterpret_cast<void*>(GetAlpha) },
        ani_native_function { "setColor", "C{@ohos.graphics.common2D.common2D.Color}:",
            reinterpret_cast<void*>(SetColorWithObject) },
        ani_native_function { "setColor", "iiii:", reinterpret_cast<void*>(SetColorWithARGB) },
        ani_native_function { "setColor", "i:", reinterpret_cast<void*>(SetColor) },
        ani_native_function { "getColor", nullptr, reinterpret_cast<void*>(GetColor) },
        ani_native_function { "getHexColor", nullptr, reinterpret_cast<void*>(GetHexColor) },
        ani_native_function { "setAntiAlias", nullptr, reinterpret_cast<void*>(SetAntiAlias) },
        ani_native_function { "isAntiAlias", nullptr, reinterpret_cast<void*>(IsAntiAlias) },
        ani_native_function { "reset", ":", reinterpret_cast<void*>(Reset) },
        ani_native_function { "setAlpha", "i:", reinterpret_cast<void*>(SetAlpha) },
        ani_native_function { "setBlendMode", "C{@ohos.graphics.drawing.drawing.BlendMode}:",
            reinterpret_cast<void*>(SetBlendMode) },
        ani_native_function { "setColorFilter", nullptr, reinterpret_cast<void*>(SetColorFilter) },
        ani_native_function { "getColorFilter", nullptr, reinterpret_cast<void*>(GetColorFilter) },
        ani_native_function { "setImageFilter", nullptr, reinterpret_cast<void*>(SetImageFilter) },
        ani_native_function { "setMaskFilter", nullptr, reinterpret_cast<void*>(SetMaskFilter) },
        ani_native_function { "setShadowLayer", nullptr, reinterpret_cast<void*>(SetShadowLayer) },
        ani_native_function { "setShaderEffect", nullptr, reinterpret_cast<void*>(SetShaderEffect) },
        ani_native_function { "setColor4f", nullptr, reinterpret_cast<void*>(SetColor4f) },
        ani_native_function { "getColor4f", nullptr, reinterpret_cast<void*>(GetColor4f) },
    };

    ani_status ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_BRUSH_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "brushTransferStaticNative", nullptr, reinterpret_cast<void*>(BrushTransferStatic) },
        ani_native_function { "getBrushAddr", nullptr, reinterpret_cast<void*>(GetBrushAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_BRUSH_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniBrush::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Brush> brush = std::make_shared<Brush>();
    AniBrush* aniBrush = new AniBrush(brush);
    if (ANI_OK != env->Object_SetField_Long(
        obj,  AniGlobalField::GetInstance().brushNativeObj, reinterpret_cast<ani_long>(aniBrush))) {
        ROSEN_LOGE("AniBrush::Constructor failed create aniBrush");
        delete aniBrush;
        return;
    }
}

void AniBrush::ConstructorWithBrush(ani_env* env, ani_object obj, ani_object aniBrushObj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, aniBrushObj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniBrush::ConstructorWithBrush brush is nullptr.");
        return;
    }
    std::shared_ptr<Brush> other = aniBrush->GetBrush();
    std::shared_ptr<Brush> brush = other == nullptr ? std::make_shared<Brush>() : std::make_shared<Brush>(*other);
    AniBrush* newAniBrush = new AniBrush(brush);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().brushNativeObj, reinterpret_cast<ani_long>(newAniBrush))) {
        ROSEN_LOGE("AniBrush::Constructor failed create aniBrush");
        delete newAniBrush;
        return;
    }
}

ani_int AniBrush::GetAlpha(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::GetAlpha brush is nullptr.");
        return -1;
    }

    return aniBrush->GetBrush()->GetAlpha();
}

void AniBrush::SetColorWithObject(ani_env* env, ani_object obj, ani_object aniColor)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniBrush::SetColorWithObject brush is nullptr.");
        return;
    }

    ColorQuad color;
    if (!GetColorQuadFromParam(env, aniColor, color)) {
        ROSEN_LOGE("AniBrush::SetColorWithObject failed cause by aniColor");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniBrush::SetColorWithObject incorrect type color.");
        return;
    }

    Drawing::Color drawingColor;
    drawingColor.SetColorQuad(color);
    aniBrush->GetBrush()->SetColor(drawingColor);
}

void AniBrush::SetColor4f(ani_env* env, ani_object obj, ani_object aniColor4f, ani_object aniColorSpace)
{
#ifdef ROSEN_OHOS
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::setColor4f brush is nullptr.");
        return;
    }

    Drawing::Color4f drawingColor;
    if (!GetColor4fFromAniColor4fObj(env, aniColor4f, drawingColor)) {
        ROSEN_LOGE("AniBrush::SetColor4f failed cause by aniColor");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniBrush::setColor4f incorrect type color.");
        return;
    }

    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
    if (IsReferenceValid(env, aniColorSpace)) {
        ColorManager::AniColorSpaceManager* aniColorSpaceManager =
        GetNativeFromObj<ColorManager::AniColorSpaceManager>(env, aniColorSpace,
                AniGlobalField::GetInstance().colorSpaceManagerNativeobj);
        if (aniColorSpaceManager != nullptr) {
            auto colorManagerColorSpace = aniColorSpaceManager->GetColorSpaceToken();
            if (colorManagerColorSpace != nullptr) {
                drawingColorSpace = Drawing::ColorSpaceConvertor::
                    ColorSpaceConvertToDrawingColorSpace(colorManagerColorSpace);
            }
        }
    }
    aniBrush->GetBrush()->SetColor(drawingColor, drawingColorSpace);
    return;
#else
    return;
#endif
}

ani_object AniBrush::GetColor4f(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::GetColor4f brush is nullptr.");
        return CreateAniUndefined(env);
    }

    const Color4f& color4f = aniBrush->GetBrush()->GetColor4f();
    ani_object aniObj = nullptr;
    CreateColor4fObj(env, color4f, aniObj);
    return aniObj;
}

void AniBrush::SetColorWithARGB(ani_env* env, ani_object obj, ani_int alpha,
    ani_int red, ani_int green, ani_int blue)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniBrush::SetColorWithARGB brush is nullptr.");
        return;
    }

    if (CheckInt32OutOfRange(alpha, 0, RGBA_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "alpha is out of range, range is [0, 255]");
        return;
    }
    if (CheckInt32OutOfRange(red, 0, RGBA_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "red is out of range, range is [0, 255]");
        return;
    }
    if (CheckInt32OutOfRange(green, 0, RGBA_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "green is out of range, range is [0, 255]");
        return;
    }
    if (CheckInt32OutOfRange(blue, 0, RGBA_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "blue is out of range, range is [0, 255]");
        return;
    }

    Drawing::Color drawingColor = Color::ColorQuadSetARGB(alpha, red, green, blue);
    aniBrush->GetBrush()->SetColor(drawingColor);
}

void AniBrush::SetColor(ani_env* env, ani_object obj, ani_int color)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetColor brush is nullptr.");
        return;
    }

    Drawing::Color drawingColor;
    drawingColor.SetColorQuad(color);
    aniBrush->GetBrush()->SetColor(drawingColor);
}

ani_object AniBrush::GetColor(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::GetColor brush is nullptr.");
        return CreateAniUndefined(env);
    }

    const Color& color = aniBrush->GetBrush()->GetColor();
    ani_object aniObj = nullptr;
    CreateColorObj(env, color, aniObj);
    return aniObj;
}

ani_int AniBrush::GetHexColor(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::GetHexColor brush is nullptr.");
        return -1;
    }

    const Color& color = aniBrush->GetBrush()->GetColor();
    return color.CastToColorQuad();
}

void AniBrush::SetAntiAlias(ani_env* env, ani_object obj, ani_boolean aa)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetAntiAlias brush is nullptr.");
        return;
    }
    
    bool antiAlias = static_cast<bool>(aa);
    aniBrush->GetBrush()->SetAntiAlias(antiAlias);
}

ani_boolean AniBrush::IsAntiAlias(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::IsAntiAlias brush is nullptr.");
        return ANI_FALSE;
    }
    
    bool isAntiAlias = aniBrush->GetBrush()->IsAntiAlias();
    return static_cast<ani_boolean>(isAntiAlias);
}

void AniBrush::Reset(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::Reset brush is nullptr.");
        return;
    }

    aniBrush->GetBrush()->Reset();
}

void AniBrush::SetAlpha(ani_env* env, ani_object obj, ani_int alpha)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetAlpha brush is nullptr.");
        return;
    }

    if (CheckInt32OutOfRange(alpha, 0, Color::RGB_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniBrush::SetAlpha alpha is out of range, range is [0, 255].");
        return;
    }

    aniBrush->GetBrush()->SetAlpha(alpha);
}

void AniBrush::SetBlendMode(ani_env* env, ani_object obj, ani_enum_item aniBlendMode)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetBlendMode brush is nullptr.");
        return;
    }

    ani_int blendMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniBlendMode, &blendMode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniBrush::SetBlendMode get blendmode failed.");
        return;
    }

    aniBrush->GetBrush()->SetBlendMode(static_cast<BlendMode>(blendMode));
}

std::shared_ptr<Brush> AniBrush::GetBrush()
{
    return brush_;
}

std::shared_ptr<Brush>* AniBrush::GetBrushPtrAddr()
{
    return &brush_;
}

AniBrush::~AniBrush()
{
    brush_ = nullptr;
}

void AniBrush::SetColorFilter(ani_env* env, ani_object obj, ani_object objColorFilter)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetColorFilter brush is nullptr.");
        return;
    }

    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(objColorFilter, &isNull);
    AniColorFilter* aniColorFilter = nullptr;
    if (!isNull) {
        aniColorFilter = GetNativeFromObj<AniColorFilter>(env, objColorFilter,
            AniGlobalField::GetInstance().colorFilterNativeObj);
        if (aniColorFilter == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniBrush::SetColorFilter invalid param colorFilter.");
            return;
        }
    }

    Filter filter = aniBrush->GetBrush()->GetFilter();
    filter.SetColorFilter(aniColorFilter ? aniColorFilter->GetColorFilter() : nullptr);
    aniBrush->GetBrush()->SetFilter(filter);
}

ani_object AniBrush::GetColorFilter(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::GetColorFilter brush is nullptr.");
        return CreateAniUndefined(env);
    }

    if (!(aniBrush->GetBrush()->HasFilter())) {
        return CreateAniUndefined(env);
    }

    AniColorFilter* colorFilter = new AniColorFilter(aniBrush->GetBrush()->GetFilter().GetColorFilter());
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().colorFilter,
        AniGlobalMethod::GetInstance().colorFilterCtor);
    if (ANI_OK != env->Object_SetField_Long(aniObj,
        AniGlobalField::GetInstance().colorFilterNativeObj, reinterpret_cast<ani_long>(colorFilter))) {
        ROSEN_LOGE("AniBrush::GetColorFilter failed cause by Object_SetField_Long");
        delete colorFilter;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniBrush::SetImageFilter(ani_env* env, ani_object obj, ani_object imageFilterObj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetImageFilter brush is nullptr.");
        return;
    }

    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(imageFilterObj, &isNull);
    AniImageFilter* aniImageFilter = nullptr;
    if (!isNull) {
        aniImageFilter = GetNativeFromObj<AniImageFilter>(env, imageFilterObj,
            AniGlobalField::GetInstance().imageFilterNativeObj);
        if (aniImageFilter == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniBrush::SetImageFilter invalid param imageFilter.");
            return;
        }
    }
    Filter filter = aniBrush->GetBrush()->GetFilter();
    filter.SetImageFilter(aniImageFilter ? aniImageFilter->GetImageFilter() : nullptr);
    aniBrush->GetBrush()->SetFilter(filter);
}

void AniBrush::SetMaskFilter(ani_env* env, ani_object obj, ani_object maskFilterObj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetMaskFilter brush is nullptr.");
        return;
    }

    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(maskFilterObj, &isNull);
    AniMaskFilter* aniMaskFilter = nullptr;
    if (!isNull) {
        aniMaskFilter = GetNativeFromObj<AniMaskFilter>(env, maskFilterObj,
            AniGlobalField::GetInstance().maskFilterNativeObj);
        if (aniMaskFilter == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniBrush::SetMaskFilter invalid param maskFilter.");
            return;
        }
    }
    Filter filter = aniBrush->GetBrush()->GetFilter();
    filter.SetMaskFilter(aniMaskFilter ? aniMaskFilter->GetMaskFilter() : nullptr);
    aniBrush->GetBrush()->SetFilter(filter);
}

void AniBrush::SetShadowLayer(ani_env* env, ani_object obj, ani_object shadowLayerObj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetShadowLayer brush is nullptr.");
        return;
    }

    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(shadowLayerObj, &isNull);
    AniShadowLayer* aniShadowLayer = nullptr;
    if (!isNull) {
        aniShadowLayer = GetNativeFromObj<AniShadowLayer>(env, shadowLayerObj,
            AniGlobalField::GetInstance().shadowLayerNativeObj);
        if (aniShadowLayer == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniBrush::SetShadowLayer invalid param shadowLayer.");
            return;
        }
    }
    aniBrush->GetBrush()->SetLooper(aniShadowLayer ? aniShadowLayer->GetBlurDrawLooper() : nullptr);
}

void AniBrush::SetShaderEffect(ani_env* env, ani_object obj, ani_object shaderEffectObj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniBrush::SetShaderEffect brush is nullptr.");
        return;
    }

    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(shaderEffectObj, &isNull);
    AniShaderEffect* aniShaderEffect = nullptr;
    if (!isNull) {
        aniShaderEffect = GetNativeFromObj<AniShaderEffect>(env, shaderEffectObj,
            AniGlobalField::GetInstance().shaderEffectNativeObj);
        if (aniShaderEffect == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniBrush::SetShaderEffect invalid param shaderEffect.");
            return;
        }
    }
    aniBrush->GetBrush()->SetShaderEffect(aniShaderEffect ? aniShaderEffect->GetShaderEffect() : nullptr);
}

ani_object AniBrush::BrushTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniBrush::BrushTransferStatic failed to unwrap");
        return CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniBrush::BrushTransferStatic unwrapResult is null");
        return CreateAniUndefined(env);
    }
    auto jsBrush = reinterpret_cast<JsBrush*>(unwrapResult);
    if (jsBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("AniBrush::BrushTransferStatic jsBrush is null");
        return CreateAniUndefined(env);
    }
    auto aniBrush = new AniBrush(jsBrush->GetBrush());
    if (ANI_OK != env->Object_SetField_Long(
        output, AniGlobalField::GetInstance().brushNativeObj, reinterpret_cast<ani_long>(aniBrush))) {
        ROSEN_LOGE("AniBrush::BrushTransferStatic failed create aniBrush");
        delete aniBrush;
        return CreateAniUndefined(env);
    }
    return output;
}

ani_long AniBrush::GetBrushAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, input, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("AniBrush::GetBrushAddr aniBrush is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniBrush->GetBrushPtrAddr());
}
} // namespace Drawing
} // namespace OHOS::Rosen