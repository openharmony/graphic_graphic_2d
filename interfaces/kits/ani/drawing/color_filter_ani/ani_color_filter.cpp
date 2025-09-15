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

const char* ANI_CLASS_COLORFILTER_NAME = "@ohos.graphics.drawing.drawing.ColorFilter";

ani_status AniColorFilter::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_COLORFILTER_NAME, &cls);
    if (ret != ANI_OK) {
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
    };

    ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
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
        ROSEN_LOGE("AniColorFilter::CreateBlendModeColorFilter failed cause by colorObj");
        // message length must be a multiple of 4, for example 16, 20, etc
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid objColor params.");
        return CreateAniUndefined(env);
    }

    ani_int blendMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniBlendMode, &blendMode)) {
        ROSEN_LOGE("AniColorFilter::CreateBlendModeColorFilter failed cause by blendMode");
        // message length must be a multiple of 4, for example 16, 20, etc
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid blendMode params.");
        return CreateAniUndefined(env);
    }

    AniColorFilter* colorFilter = new AniColorFilter(
        ColorFilter::CreateBlendModeColorFilter(color, static_cast<BlendMode>(blendMode)));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_COLORFILTER_NAME, colorFilter);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
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
        // message length must be a multiple of 4, for example 16, 20, etc
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }

    ani_int blendMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniBlendMode, &blendMode)) {
        ROSEN_LOGE("AniColorFilter::CreateBlendModeColorFilterWithNumber failed cause by blendMode");
        // message length must be a multiple of 4, for example 16, 20, etc
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }

    AniColorFilter* colorFilter = new AniColorFilter(
        ColorFilter::CreateBlendModeColorFilter(color, static_cast<BlendMode>(blendMode)));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_COLORFILTER_NAME, colorFilter);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
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
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniColorFilter::ColorFilterTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsColorFilter = reinterpret_cast<JsColorFilter*>(unwrapResult);
    if (jsColorFilter->GetColorFilterPtr() == nullptr) {
        ROSEN_LOGE("AniColorFilter::ColorFilterTransferStatic jsColorFilter is null");
        return nullptr;
    }

    auto aniColorFilter = new AniColorFilter(jsColorFilter->GetColorFilterPtr());
    ani_object aniColorFilterObj = CreateAniObject(env, ANI_CLASS_COLORFILTER_NAME, nullptr);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniColorFilterObj,
        NATIVE_OBJ, reinterpret_cast<ani_long>(aniColorFilter))) {
        ROSEN_LOGE("AniColorFilter::ColorFilterTransferStatic failed create aniColorFilter");
        delete aniColorFilter;
        return nullptr;
    }
    return aniColorFilterObj;
}

ani_long AniColorFilter::GetColorFilterAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniColorFilter = GetNativeFromObj<AniColorFilter>(env, input);
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

std::shared_ptr<ColorFilter> AniColorFilter::GetColorFilter()
{
    return colorFilter_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
