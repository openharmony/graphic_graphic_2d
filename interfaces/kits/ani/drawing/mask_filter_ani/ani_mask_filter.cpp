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

#include "ani_mask_filter.h"

namespace OHOS::Rosen {
namespace Drawing {

const char* ANI_CLASS_MASKFILTER_NAME = "@ohos.graphics.drawing.drawing.MaskFilter";

ani_status AniMaskFilter::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_MASKFILTER_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_MASKFILTER_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "createBlurMaskFilter", nullptr, reinterpret_cast<void*>(CreateBlurMaskFilter) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_MASKFILTER_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

ani_object AniMaskFilter::CreateBlurMaskFilter(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_enum_item aniBlurType, ani_double sigma)
{
    ani_int blurType;
    ani_status ret = env->EnumItem_GetValue_Int(aniBlurType, &blurType);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniMaskFilter::CreateBlurMaskFilter failed cause by aniBlurType %{public}d", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param blurType.");
        return CreateAniUndefined(env);
    }
    AniMaskFilter* maskFilter = new AniMaskFilter(
        MaskFilter::CreateBlurMaskFilter(static_cast<BlurType>(blurType), sigma));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_MASKFILTER_NAME, maskFilter);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete maskFilter;
        ROSEN_LOGE("AniMaskFilter::CreateBlurMaskFilter failed cause aniObj is undefined");
    }
    return aniObj;
}

AniMaskFilter::~AniMaskFilter()
{
    maskFilter_ = nullptr;
}

std::shared_ptr<MaskFilter> AniMaskFilter::GetMaskFilter()
{
    return maskFilter_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
