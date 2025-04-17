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

#include "ani_font_collection.h"

#include <codecvt>
#include <cstdint>
#include <sys/stat.h>

#include "ani_text_utils.h"

#include "utils/text_log.h"

namespace OHOS::Rosen {
namespace {
const char* ANI_CLASS_FONT_COLLECTION = "L@ohos/graphics/text/text/FontCollection;";
} // namespace

AniFontCollection::AniFontCollection() {}

void AniFontCollection::Constructor(ani_env* env, ani_object object)
{
    AniFontCollection* aniFontCollection = new AniFontCollection();
    if (ANI_OK != env->Object_SetFieldByName_Long(object, "nativeObj", reinterpret_cast<ani_long>(aniFontCollection))) {
        TEXT_LOGE("Failed to create ani FontCollection obj");
        return;
    }
}

ani_object AniFontCollection::GetGlobalInstance(ani_env* env, ani_class cls)
{
    return ani_object {};
}

void AniFontCollection::LoadFontSync(ani_env* env, ani_object obj, ani_string name, ani_object path)
{
    TEXT_LOGD("[ANI] %{public}s", __func__);
}

void AniFontCollection::ClearCaches(ani_env* env, ani_object obj)
{
    TEXT_LOGD("[ANI] %{public}s", __func__);
}

ani_status AniFontCollection::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    ani_status ret;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TEXT_LOGE("[ANI] null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_FONT_COLLECTION, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_FONT_COLLECTION);
        return ANI_NOT_FOUND;
    }

    std::string globalInstance = ":" + std::string(ANI_CLASS_FONT_COLLECTION);
    std::string loadFontSync = "Lstd/core/String;Lstd/core/Object;:V";

    std::array methods = {
        ani_native_function { "<ctor>", ":V", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "getGlobalInstance", globalInstance.c_str(), reinterpret_cast<void*>(GetGlobalInstance) },
        ani_native_function { "loadFontSync", loadFontSync.c_str(), reinterpret_cast<void*>(LoadFontSync) },
        ani_native_function { "clearCaches", ":V", reinterpret_cast<void*>(ClearCaches) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_FONT_COLLECTION);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}
} // namespace OHOS::Rosen