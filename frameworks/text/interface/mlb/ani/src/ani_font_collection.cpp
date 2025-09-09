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
#include "ani_transfer_util.h"
#include "fontcollection_napi/js_fontcollection.h"
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

void AniFontCollection::Constructor(ani_env* env, ani_object object)
{
    AniFontCollection* aniFontCollection = new AniFontCollection();
    ani_status ret = env->Object_CallMethodByName_Void(
        object, REGISTER_NATIVE, "J:V", reinterpret_cast<ani_long>(aniFontCollection));
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
    ani_object obj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_COLLECTION, ":V");
    ani_status ret = env->Object_CallMethodByName_Void(
        obj, REGISTER_NATIVE, "J:V", reinterpret_cast<ani_long>(aniFontCollection));
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
    ret = AniTextUtils::FindClassWithCache(env, "Lstd/core/String;", stringClass);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to found Lstd/core/String;, ret %{public}d", ret);
        return;
    }

    ani_boolean isString = false;
    env->Object_InstanceOf(path, stringClass, &isString);

    if (isString) {
        LoadString(env, path, aniFontCollection->fontCollection_, familyName);
        return;
    }

    ani_class resourceClass = nullptr;
    ret = AniTextUtils::FindClassWithCache(env, "Lglobal/resource/Resource;", resourceClass);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to found Lglobal/resource/Resource;, ret %{public}d", ret);
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

ani_status AniFontCollection::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = AniTextUtils::FindClassWithCache(env, ANI_CLASS_FONT_COLLECTION, cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class: %{public}s, ret %{public}d", ANI_CLASS_FONT_COLLECTION, ret);
        return ANI_NOT_FOUND;
    }

    std::string globalInstance = ":" + std::string(ANI_CLASS_FONT_COLLECTION);
    std::string loadFontSync = "Lstd/core/String;Lstd/core/Object;:V";

    std::array methods = {
        ani_native_function{"constructorNative", ":V", reinterpret_cast<void*>(Constructor)},
        ani_native_function{"getGlobalInstance", globalInstance.c_str(), reinterpret_cast<void*>(GetGlobalInstance)},
        ani_native_function{"loadFontSync", loadFontSync.c_str(), reinterpret_cast<void*>(LoadFontSync)},
        ani_native_function{"clearCaches", ":V", reinterpret_cast<void*>(ClearCaches)},
        ani_native_function{"nativeTransferStatic", "Lstd/interop/ESValue;:Lstd/core/Object;",
            reinterpret_cast<void*>(NativeTransferStatic)},
        ani_native_function{
            "nativeTransferDynamic", "J:Lstd/interop/ESValue;", reinterpret_cast<void*>(NativeTransferDynamic)},
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

ani_object AniFontCollection::NativeTransferStatic(ani_env* env, ani_class cls, ani_object input)
{
    return AniTransferUtils::TransferStatic(env, input, [](ani_env* env, void* unwrapResult) {
        JsFontCollection* jsFontcollection = reinterpret_cast<JsFontCollection*>(unwrapResult);
        if (jsFontcollection == nullptr) {
            TEXT_LOGE("Null jsFontcollection");
            return AniTextUtils::CreateAniUndefined(env);
        }
        ani_object staticObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_COLLECTION, ":V");
        AniFontCollection* aniFontCollection = new AniFontCollection();
        aniFontCollection->fontCollection_ = jsFontcollection->GetFontCollection();
        ani_status ret = env->Object_CallMethodByName_Void(
            staticObj, REGISTER_NATIVE, "J:V", reinterpret_cast<ani_long>(aniFontCollection));
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to create ani font collection obj, ret %{public}d", ret);
            delete aniFontCollection;
            aniFontCollection = nullptr;
            return AniTextUtils::CreateAniUndefined(env);
        }
        return staticObj;
    });
}

ani_object AniFontCollection::NativeTransferDynamic(ani_env* aniEnv, ani_class cls, ani_long nativeObj)
{
    return AniTransferUtils::TransferDynamic(aniEnv, nativeObj,
        [](napi_env napiEnv, ani_long nativeObj, napi_value objValue) {
            objValue = JsFontCollection::Init(napiEnv, objValue);
            napi_value dynamicObj = nullptr;
            napi_status status = JsFontCollection::CreateFontCollection(napiEnv, objValue, &dynamicObj);
            if (status != napi_ok || dynamicObj == nullptr) {
                TEXT_LOGE("Failed to create font collection, status %{public}d", status);
                return dynamicObj = nullptr;
            }
            AniFontCollection* aniFontCollection = reinterpret_cast<AniFontCollection*>(nativeObj);
            if (aniFontCollection == nullptr || aniFontCollection->fontCollection_ == nullptr) {
                TEXT_LOGE("Null font collection");
                return dynamicObj = nullptr;
            }
            status = JsFontCollection::SetFontCollection(napiEnv, dynamicObj, aniFontCollection->GetFontCollection());
            if (status != napi_ok) {
                TEXT_LOGE("Failed to set inner font collection, status %{public}d", status);
                return dynamicObj = nullptr;
            }
            return dynamicObj;
        });
}
} // namespace OHOS::Text::ANI