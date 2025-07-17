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
#include <ani.h>
#include <cinttypes>
#include <tuple>

#include "ani_common.h"
#include "ani_font_collection.h"
#include "ani_fontdescriptor.h"
#include "ani_line_typeset.h"
#include "ani_paragraph.h"
#include "ani_paragraph_builder.h"
#include "ani_run.h"
#include "ani_text_line.h"
#include "ani_text_utils.h"
#include "line_typography.h"
#include "text_line_base.h"
#include "typography.h"
#include "typography_create.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
#define STRUCT_LIST(...) using AniTypes = std::tuple<__VA_ARGS__>

// add new struct in this macro
STRUCT_LIST(
    AniFontCollection, AniParagraph, AniParagraphBuilder, AniLineTypeset, AniTextLine, AniRun, AniFontDescriptor);

template <typename T>
static ani_status InitOneStruct(ani_vm* vm, uint32_t* result)
{
    return T::AniInit(vm, result);
}

template <typename Tuple, size_t... Is>
static ani_status InitAllStruct(ani_vm* vm, uint32_t* result, std::index_sequence<Is...>)
{
    ani_status ret;
    [[maybe_unused]] bool status =
        (((ret = InitOneStruct<std::tuple_element_t<Is, Tuple>>(vm, result)) == ANI_OK) && ...);
    return ret;
}

template <typename T>
void SafeDelete(ani_long& ptr)
{
    if (ptr != 0) {
        T* pointer = reinterpret_cast<T*>(ptr);
        delete pointer;
        pointer = nullptr;
        ptr = 0;
    }
}

static void Clean(ani_env* env, ani_object object)
{
    ani_long ptr = 0;
    ani_status ret = env->Object_GetFieldByName_Long(object, "ptr", &ptr);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to clean ptr");
        return;
    }
    if (ptr == 0) {
        TEXT_LOGE("Auto clean failed, undefined ptr");
        return;
    }
    ani_ref stringRef = nullptr;
    ret = env->Object_GetFieldByName_Ref(object, "className", &stringRef);
    if (ret != ANI_OK) {
        TEXT_LOGE("Auto clean failed, ptr %{public}" PRId64, ptr);
        return;
    }

    std::string className;
    ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(stringRef), className);
    if (ret != ANI_OK) {
        return;
    }
    using DeleteFunc = void (*)(ani_long&);
    static const std::unordered_map<std::string, DeleteFunc> deleteMap = {
        {"ParagraphBuilder", SafeDelete<Rosen::TypographyCreate>}, {"Typography", SafeDelete<Rosen::Typography>},
        {"FontCollection", SafeDelete<AniFontCollection>}, {"LineTypeset", SafeDelete<Rosen::LineTypography>},
        {"TextLine", SafeDelete<Rosen::TextLineBase>}, {"Run", SafeDelete<Rosen::Run>}};

    if (deleteMap.count(className)) {
        deleteMap.at(className)(ptr);
    }
}

static ani_status AniCleanerInit(ani_vm* vm)
{
    ani_env* env{nullptr};
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_CLEANER, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function{"clean", ":V", reinterpret_cast<void*>(Clean)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("[Manager] Failed to bind methods, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

static ani_status Init(ani_vm* vm, uint32_t* result)
{
    AniCleanerInit(vm);
    return InitAllStruct<AniTypes>(vm, result, std::make_index_sequence<std::tuple_size_v<AniTypes>>());
}
} // namespace OHOS::Text::ANI

extern "C"
{
    ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
    {
        ani_status status = OHOS::Text::ANI::Init(vm, result);
        if (status == ANI_OK) {
            *result = ANI_VERSION_1;
        }
        return status;
    }
}