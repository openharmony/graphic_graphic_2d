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

#include "ani_typeface_arguments.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_TYPEFACE_ARGUMENTS_NAME = "@ohos.graphics.drawing.drawing.TypefaceArguments";
constexpr uint32_t AXIS_OFFSET_ZERO = 24;
constexpr uint32_t AXIS_OFFSET_ONE = 16;
constexpr uint32_t AXIS_OFFSET_TWO = 8;

ani_status AniTypefaceArguments::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_TYPEFACE_ARGUMENTS_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s ret: %{public}d", ANI_CLASS_TYPEFACE_ARGUMENTS_NAME, ret);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "addVariation", nullptr, reinterpret_cast<void*>(AddVariation) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s ret: %{public}d", ANI_CLASS_TYPEFACE_ARGUMENTS_NAME, ret);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniTypefaceArguments::Constructor(ani_env* env, ani_object obj)
{
    AniTypefaceArguments* aniTypefaceArguments = new AniTypefaceArguments();
    ani_status ret = env->Object_SetFieldByName_Long(
        obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniTypefaceArguments));
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniTypefaceArguments::Constructor failed create AniTypefaceArguments. ret: %{public}d", ret);
        delete aniTypefaceArguments;
        return;
    }
}

uint32_t AniTypefaceArguments::ConvertAxisToNumber(const std::string& axis)
{
    return ((static_cast<uint32_t>(axis[ARGC_ZERO]) << AXIS_OFFSET_ZERO) |
        (static_cast<uint32_t>(axis[ARGC_ONE]) << AXIS_OFFSET_ONE) |
        (static_cast<uint32_t>(axis[ARGC_TWO]) << AXIS_OFFSET_TWO) | (static_cast<uint32_t>(axis[ARGC_THREE])));
}

void AniTypefaceArguments::AddVariation(ani_env* env, ani_object obj, ani_string aniAxis, ani_double value)
{
    std::string axis = CreateStdString(env, aniAxis);
    auto aniTypefaceArguments = GetNativeFromObj<AniTypefaceArguments>(env, obj);
    if (aniTypefaceArguments == nullptr) {
        ROSEN_LOGE("AniTypefaceArguments::AddVariation aniTypefaceArguments invalid.");
        return;
    }
    aniTypefaceArguments->GetTypefaceArgumentsHelper().coordinate.push_back({ConvertAxisToNumber(axis), value});
}

TypefaceArgumentsHelper& AniTypefaceArguments::GetTypefaceArgumentsHelper()
{
    return typefaceArgumentsHelper_;
}

void AniTypefaceArguments::ConvertToFontArguments(const TypefaceArgumentsHelper& typefaceArgumentsHelper,
    FontArguments fontArguments)
{
    fontArguments.SetCollectionIndex(typefaceArgumentsHelper.fontCollectionIndex);
    fontArguments.SetVariationDesignPosition({reinterpret_cast<const FontArguments::VariationPosition::Coordinate*>(
        typefaceArgumentsHelper.coordinate.data()), typefaceArgumentsHelper.coordinate.size()});
}
} // namespace Drawing
} // namespace OHOS::Rosen