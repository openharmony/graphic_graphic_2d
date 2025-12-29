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
