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
const std::string GLOBAL_INSTANCE_SIGN = ":C{" + std::string(ANI_CLASS_FONT_COLLECTION) + "}";
const std::string LOAD_FONT_SYNC_SIGN = "C{std.core.String}X{C{global.resource.Resource}C{std.core.String}}:";
const std::string UNLOAD_FONT_SYNC_SIGN = "C{std.core.String}:";

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

ani_status AniFontCollection::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ret;
    }

    ani_class cls = nullptr;
    ret = AniTextUtils::FindClassWithCache(env, ANI_CLASS_FONT_COLLECTION, cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class: %{public}s, ret %{public}d", ANI_CLASS_FONT_COLLECTION, ret);
        return ret;
    }

    std::array methods = {
        ani_native_function{"constructorNative", ":", reinterpret_cast<void*>(Constructor)},
        ani_native_function{"loadFontSync", LOAD_FONT_SYNC_SIGN.c_str(), reinterpret_cast<void*>(LoadFontSync)},
        ani_native_function{"unloadFontSync", UNLOAD_FONT_SYNC_SIGN.c_str(), reinterpret_cast<void*>(UnloadFontSync)},
        ani_native_function{"clearCaches", ":", reinterpret_cast<void*>(ClearCaches)},
    };
    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for FontCollection: %{public}s, ret %{public}d",
            ANI_CLASS_FONT_COLLECTION, ret);
        return ret;
    }

    std::array staticMethods = {
        ani_native_function{"getGlobalInstance", GLOBAL_INSTANCE_SIGN.c_str(),
            reinterpret_cast<void*>(GetGlobalInstance)},
    };
    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind static methods: %{public}s, ret %{public}d", ANI_CLASS_FONT_COLLECTION, ret);
        return ret;
    }
    return ANI_OK;
}

AniFontCollection::AniFontCollection()
{
    fontCollection_ = FontCollection::From(nullptr);
}

void AniFontCollection::Constructor(ani_env* env, ani_object object)
{
    AniFontCollection* aniFontCollection = new AniFontCollection();
    ani_status ret = env->Object_CallMethodByName_Void(
        object, BIND_NATIVE, "l:", reinterpret_cast<ani_long>(aniFontCollection));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to create ani font collection obj");
        delete aniFontCollection;
        aniFontCollection = nullptr;
        return;
    }
}

ani_object AniFontCollection::GetGlobalInstance(ani_env* env, ani_class cls)
{
    AniFontCollection* aniFontCollection = new AniFontCollection();
    aniFontCollection->fontCollection_ = FontCollection::Create();
    ani_object obj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_COLLECTION, ":");
    ani_status ret = env->Object_CallMethodByName_Void(
        obj, BIND_NATIVE, "l:", reinterpret_cast<ani_long>(aniFontCollection));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to create ani font collection obj");
        delete aniFontCollection;
        aniFontCollection = nullptr;
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

    ani_class stringClass = nullptr;
    ret = AniTextUtils::FindClassWithCache(env, "std.core.String", stringClass);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to found std.core.String, ret %{public}d", ret);
        return;
    }

    ani_boolean isString = false;
    env->Object_InstanceOf(path, stringClass, &isString);

    if (isString) {
        LoadString(env, path, aniFontCollection->fontCollection_, familyName);
        return;
    }

    ani_class resourceClass = nullptr;
    ret = AniTextUtils::FindClassWithCache(env, "global.resource.Resource", resourceClass);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to found global.resource.Resource, ret %{public}d", ret);
        return;
    }
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

void AniFontCollection::UnloadFontSync(ani_env* env, ani_object obj, ani_string name)
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

    aniFontCollection->fontCollection_->UnloadFont(familyName);
}

std::shared_ptr<FontCollection> AniFontCollection::GetFontCollection()
{
    return fontCollection_;
}
} // namespace OHOS::Text::ANI