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

#include "ani_image_filter.h"
#include "effect/color_filter.h"
#include "color_filter_ani/ani_color_filter.h"
#include "shader_effect_ani/ani_shader_effect.h"
#include "pixel_map_taihe_ani.h"
#include "image/image.h"

#ifdef ROSEN_OHOS
#include "pixel_map.h"
#endif

namespace OHOS::Rosen {
namespace Drawing {

ani_status AniImageFilter::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().imageFilter;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_IMAGEFILTER_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "createFromColorFilter", nullptr, reinterpret_cast<void*>(CreateFromColorFilter) },
        ani_native_function { "createBlurImageFilter", nullptr, reinterpret_cast<void*>(CreateBlurImageFilter) },
        ani_native_function { "createOffsetImageFilter", nullptr, reinterpret_cast<void*>(CreateOffsetImageFilter) },
        ani_native_function { "createComposeImageFilter", nullptr, reinterpret_cast<void*>(createComposeImageFilter) },
        ani_native_function { "createBlendImageFilter", nullptr, reinterpret_cast<void*>(createBlendImageFilter) },
        ani_native_function { "createFromShaderEffect", nullptr, reinterpret_cast<void*>(CreateFromShaderEffect) },
        ani_native_function { "createFromImage", nullptr, reinterpret_cast<void*>(CreateFromImage) }
    };

    ani_status ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail. ret %{public}d %{public}s", ret, ANI_CLASS_IMAGEFILTER_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

ani_object AniImageFilter::CreateFromColorFilter(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object aniColorFilterObj, ani_object aniImageFilterObj)
{
    auto aniColorFilter = GetNativeFromObj<AniColorFilter>(env, aniColorFilterObj,
        AniGlobalField::GetInstance().colorFilterNativeObj);
    if (aniColorFilter == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param colorFilter.");
        return CreateAniUndefined(env);
    }

    AniImageFilter* aniImageFilter = nullptr;
    if (!IsNull(env, aniImageFilterObj) && !IsUndefined(env, aniImageFilterObj)) {
        aniImageFilter = GetNativeFromObj<AniImageFilter>(env, aniImageFilterObj,
            AniGlobalField::GetInstance().imageFilterNativeObj);
        if (aniImageFilter == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param imageFilter.");
            return CreateAniUndefined(env);
        }
    }
    AniImageFilter* imageFilter = new AniImageFilter(ImageFilter::CreateColorFilterImageFilter(
        *(aniColorFilter->GetColorFilter()), aniImageFilter ? aniImageFilter->GetImageFilter() : nullptr));
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().imageFilter,
        AniGlobalMethod::GetInstance().imageFilterCtor, AniGlobalMethod::GetInstance().imageFilterBindNative,
        imageFilter);
    if (IsUndefined(env, aniObj)) {
        delete imageFilter;
        ROSEN_LOGE("AniImageFilter::CreateFromColorFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniImageFilter::CreateBlurImageFilter(ani_env* env, [[maybe_unused]]ani_object obj,
    ani_double sigmaX, ani_double sigmaY, ani_enum_item aniTileMode, ani_object aniImageFilterObj)
{
    AniImageFilter* aniImageFilter = nullptr;
    if (!IsNull(env, aniImageFilterObj) && !IsUndefined(env, aniImageFilterObj)) {
        aniImageFilter = GetNativeFromObj<AniImageFilter>(env, aniImageFilterObj,
            AniGlobalField::GetInstance().imageFilterNativeObj);
        if (aniImageFilter == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param imageFilter.");
            return CreateAniUndefined(env);
        }
    }
    ani_int tMode;
    ani_status ret = env->EnumItem_GetValue_Int(aniTileMode, &tMode);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniImageFilter::CreateBlurImageFilter failed cause by aniTileMode %{public}d", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param tileMode.");
        return CreateAniUndefined(env);
    }

    AniImageFilter* imageFilter = new AniImageFilter(ImageFilter::CreateBlurImageFilter(
        sigmaX, sigmaY, static_cast<TileMode>(tMode),
        aniImageFilter ? aniImageFilter->GetImageFilter() : nullptr, ImageBlurType::GAUSS));
    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().imageFilter,
        AniGlobalMethod::GetInstance().imageFilterCtor, AniGlobalMethod::GetInstance().imageFilterBindNative,
        imageFilter);
    if (IsUndefined(env, aniObj)) {
        delete imageFilter;
        ROSEN_LOGE("AniImageFilter::CreateBlurImageFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniImageFilter::CreateOffsetImageFilter(ani_env* env, ani_object obj, ani_double dx, ani_double dy,
    ani_object aniInputImageFilterobj)
{
    AniImageFilter* aniImageFilter = nullptr;
    if (!IsNull(env, aniInputImageFilterobj) && !IsUndefined(env, aniInputImageFilterobj)) {
        aniImageFilter = GetNativeFromObj<AniImageFilter>(env, aniInputImageFilterobj,
            AniGlobalField::GetInstance().imageFilterNativeObj);
        if (aniImageFilter == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param imageFilter.");
            return CreateAniUndefined(env);
        }
    }

    AniImageFilter* imageFilter = new AniImageFilter(ImageFilter::CreateOffsetImageFilter(
        dx, dy, aniImageFilter ? aniImageFilter->GetImageFilter() : nullptr));

    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().imageFilter,
        AniGlobalMethod::GetInstance().imageFilterCtor, AniGlobalMethod::GetInstance().imageFilterBindNative,
        imageFilter);
    
    if (IsUndefined(env, aniObj)) {
        delete imageFilter;
        ROSEN_LOGE("AniImageFilter::CreateOffsetImageFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniImageFilter::createComposeImageFilter(ani_env* env, ani_object obj, ani_object cOuterImageFilterobj,
        ani_object cInnerImageFilterobj)
{
    AniImageFilter* aniCouterImageFilter = nullptr;
    AniImageFilter* aniCinnerImageFilter = nullptr;
    if (!IsNull(env, cOuterImageFilterobj) && !IsUndefined(env, cOuterImageFilterobj) &&
        !IsNull(env, cInnerImageFilterobj) && !IsUndefined(env, cInnerImageFilterobj)) {
        aniCouterImageFilter = GetNativeFromObj<AniImageFilter>(env, cOuterImageFilterobj,
            AniGlobalField::GetInstance().imageFilterNativeObj);

        aniCinnerImageFilter = GetNativeFromObj<AniImageFilter>(env, cInnerImageFilterobj,
            AniGlobalField::GetInstance().imageFilterNativeObj);
        if (aniCouterImageFilter == nullptr || aniCinnerImageFilter == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param imageFilter.");
            return CreateAniUndefined(env);
        }
    }

    AniImageFilter* imageFilter = new AniImageFilter(ImageFilter::CreateComposeImageFilter(aniCouterImageFilter ?
        aniCouterImageFilter->GetImageFilter() : nullptr, aniCinnerImageFilter ?
        aniCinnerImageFilter->GetImageFilter() : nullptr));

    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().imageFilter,
        AniGlobalMethod::GetInstance().imageFilterCtor, AniGlobalMethod::GetInstance().imageFilterBindNative,
        imageFilter);
    
    if (IsUndefined(env, aniObj)) {
        delete imageFilter;
        ROSEN_LOGE("AniImageFilter::createComposeImageFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniImageFilter::createBlendImageFilter(ani_env* env, ani_object obj, ani_enum_item blendModeobj,
    ani_object backgroundImageFilteObj, ani_object foregroundImageFilterobj)
{
    AniImageFilter* anibackgroundImageFilter = nullptr;
    AniImageFilter* aniforegroundImageFilter = nullptr;
    if (!IsNull(env, backgroundImageFilteObj) && !IsUndefined(env, backgroundImageFilteObj) &&
        !IsNull(env, foregroundImageFilterobj) && !IsUndefined(env, foregroundImageFilterobj)) {
        anibackgroundImageFilter = GetNativeFromObj<AniImageFilter>(env, backgroundImageFilteObj,
            AniGlobalField::GetInstance().imageFilterNativeObj);

        aniforegroundImageFilter = GetNativeFromObj<AniImageFilter>(env, foregroundImageFilterobj,
            AniGlobalField::GetInstance().imageFilterNativeObj);
        if (anibackgroundImageFilter == nullptr || aniforegroundImageFilter == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param imageFilter.");
            return CreateAniUndefined(env);
        }
    }

    ani_int blendMode;
    ani_status ret = env->EnumItem_GetValue_Int(blendModeobj, &blendMode);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniImageFilter::createBlendImageFilter failed cause by blendMode %{public}d", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param blendMode.");
        return CreateAniUndefined(env);
    }

    AniImageFilter* imageFilter = new AniImageFilter(ImageFilter::CreateBlendImageFilter(
        static_cast<BlendMode>(blendMode), anibackgroundImageFilter ? anibackgroundImageFilter->GetImageFilter() :
        nullptr, aniforegroundImageFilter ? aniforegroundImageFilter->GetImageFilter() : nullptr));

    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().imageFilter,
        AniGlobalMethod::GetInstance().imageFilterCtor, AniGlobalMethod::GetInstance().imageFilterBindNative,
        imageFilter);
    
    if (IsUndefined(env, aniObj)) {
        delete imageFilter;
        ROSEN_LOGE("AniImageFilter::createBlendImageFilter failed cause aniObj is undefined");
    }
    return aniObj;
}
ani_object AniImageFilter::CreateFromShaderEffect(ani_env* env, ani_object obj, ani_object aniShaderEffectobj)
{
    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(aniShaderEffectobj, &isNull);
    AniShaderEffect* aniShaderEffect = nullptr;
    if (!isNull) {
        aniShaderEffect = GetNativeFromObj<AniShaderEffect>(env, aniShaderEffectobj,
            AniGlobalField::GetInstance().shaderEffectNativeObj);
        if (aniShaderEffect == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param shaderEffect.");
            return CreateAniUndefined(env);
        }
    }

    std::shared_ptr<ShaderEffect> shaderEffect = aniShaderEffect->GetShaderEffect();
    AniImageFilter* imageFilter = new AniImageFilter(ImageFilter::CreateShaderImageFilter(shaderEffect));

    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().imageFilter,
        AniGlobalMethod::GetInstance().imageFilterCtor, AniGlobalMethod::GetInstance().imageFilterBindNative,
        imageFilter);
    
    if (IsUndefined(env, aniObj)) {
        delete imageFilter;
        ROSEN_LOGE("AniImageFilter::CreateFromShaderEffect failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniImageFilter::CreateFromImage(ani_env* env, ani_object obj, ani_object pixelmapObj, ani_object srcRectobj,
        ani_object dstRectobj)
{
#ifdef ROSEN_OHOS
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ROSEN_LOGE("AniImageFilter::CreateFromImage get pixelMap failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniImageFilter::CreateFromImage pixelMap is nullptr.");
        return CreateAniUndefined(env);
    }
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMap);
    if (image == nullptr) {
        ROSEN_LOGE("AniImageFilter::CreateFromImage image is nullptr");
        return CreateAniUndefined(env);
    }
    Drawing::Rect srcRect = Drawing::Rect(0.0f, 0.0f, image->GetWidth(), image->GetHeight());
    Drawing::Rect dstRect = srcRect;

    if (IsReferenceValid(env, srcRectobj)) {
        if (!GetRectFromAniRectObj(env, srcRectobj, srcRect)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniImageFilter::CreateFromImage get srcRect fail.");
            return CreateAniUndefined(env);
        }
    }
    
    if (IsReferenceValid(env, dstRectobj)) {
        if (!GetRectFromAniRectObj(env, dstRectobj, dstRect)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, 
                "AniImageFilter::CreateFromImage get dstRect fail.");
            return CreateAniUndefined(env);
        }
    }

    std::shared_ptr<ImageFilter> imgFilter = ImageFilter::CreateImageImageFilter(image, srcRect, dstRect,
        SamplingOptions(FilterMode::LINEAR));

    AniImageFilter* imageFilter = new AniImageFilter(imgFilter);

    ani_object aniObj = CreateAniObjectStatic(env, AniGlobalClass::GetInstance().imageFilter,
        AniGlobalMethod::GetInstance().imageFilterCtor, AniGlobalMethod::GetInstance().imageFilterBindNative,
        imageFilter);
    
    if (IsUndefined(env, aniObj)) {
        delete imageFilter;
        ROSEN_LOGE("AniImageFilter::CreateFromImage failed cause aniObj is undefined");
    }
    return aniObj;
#else
    return nullptr;
#endif
}

AniImageFilter::~AniImageFilter()
{
    imageFilter_ = nullptr;
}

std::shared_ptr<ImageFilter> AniImageFilter::GetImageFilter()
{
    return imageFilter_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
