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

#include "ani_common.h"
#include "ani_resource_parser.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

namespace {
void LoadString(
    ani_env* env, ani_object path, std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection, std::string familyName)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;

    std::string pathStr;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(path), pathStr);
    if (ret != ANI_OK) {
        return;
    }
    if (!AniTextUtils::SplitAbsoluteFontPath(pathStr) || !AniTextUtils::ReadFile(pathStr, dataLen, data)) {
        TEXT_LOGE("Failed to split absolute font path");
        return;
    }
    fontCollection->LoadFont(familyName, data.get(), dataLen);
}

void LoadResource(
    ani_env* env, ani_object path, std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection, std::string familyName)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;

    AniResource resource = AniResourceParser::ParseResource(env, path);
    if (!AniResourceParser::ResolveResource(resource, dataLen, data)) {
        TEXT_LOGE("Failed to resolve resource");
        return;
    }
    fontCollection->LoadFont(familyName, data.get(), dataLen);
}
} // namespace

AniFontCollection::AniFontCollection()
{
    fontCollection_ = FontCollection::From(nullptr);
}

AniFontCollection::AniFontCollection(std::shared_ptr<FontCollection> fc)
{
    fontCollection_ = fc;
}

void AniFontCollection::Constructor(ani_env* env, ani_object object)
{
    AniFontCollection* aniFontCollection = new AniFontCollection();
    ani_status ret = env->Object_SetFieldByName_Long(object, NATIVE_OBJ, reinterpret_cast<ani_long>(aniFontCollection));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to create ani font collection obj");
        delete aniFontCollection;
        aniFontCollection = nullptr;
        return;
    }
}

ani_object AniFontCollection::GetGlobalInstance(ani_env* env, ani_class cls)
{
    static AniFontCollection aniFontCollection = AniFontCollection(FontCollection::Create());

    ani_object obj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_COLLECTION, ":V");
    ani_status ret = env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(&aniFontCollection));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to create ani font collection obj");
        return nullptr;
    }
    return obj;
}

void AniFontCollection::LoadFontSync(ani_env* env, ani_object obj, ani_string name, ani_object path)
{
    std::string familyName;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, name, familyName);
    if (ret != ANI_OK) {
        return;
    }
    auto aniFontCollection = AniTextUtils::GetNativeFromObj<AniFontCollection>(env, obj);
    if (aniFontCollection == nullptr || aniFontCollection->fontCollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return;
    }

    ani_class stringClass;
    env->FindClass("Lstd/core/String;", &stringClass);
    ani_boolean isString = false;
    env->Object_InstanceOf(path, stringClass, &isString);

    if (isString) {
        LoadString(env, path, aniFontCollection->fontCollection_, familyName);
        return;
    }

    ani_class resourceClass;
    env->FindClass("Lglobal/resource/Resource", &resourceClass);
    ani_boolean isResource = false;
    env->Object_InstanceOf(path, resourceClass, &isResource);
    if (isResource) {
        LoadResource(env, path, aniFontCollection->fontCollection_, familyName);
        return;
    }
}

void AniFontCollection::ClearCaches(ani_env* env, ani_object obj)
{
    auto aniFontCollection = AniTextUtils::GetNativeFromObj<AniFontCollection>(env, obj);
    if (aniFontCollection == nullptr || aniFontCollection->fontCollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return;
    }
    aniFontCollection->fontCollection_->ClearCaches();
}

ani_status AniFontCollection::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK) {
        TEXT_LOGE("null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_FONT_COLLECTION, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class: %{public}s", ANI_CLASS_FONT_COLLECTION);
        return ANI_NOT_FOUND;
    }

    std::string globalInstance = ":" + std::string(ANI_CLASS_FONT_COLLECTION);
    std::string loadFontSync = "Lstd/core/String;Lstd/core/Object;:V";

    std::array methods = {
        ani_native_function{"constructorNative", ":V", reinterpret_cast<void*>(Constructor)},
        ani_native_function{"getGlobalInstance", globalInstance.c_str(), reinterpret_cast<void*>(GetGlobalInstance)},
        ani_native_function{"loadFontSync", loadFontSync.c_str(), reinterpret_cast<void*>(LoadFontSync)},
        ani_native_function{"clearCaches", ":V", reinterpret_cast<void*>(ClearCaches)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for FontCollection: %{public}s", ANI_CLASS_FONT_COLLECTION);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

std::shared_ptr<FontCollection> AniFontCollection::GetFontCollection()
{
    return fontCollection_;
}
} // namespace OHOS::Text::ANI