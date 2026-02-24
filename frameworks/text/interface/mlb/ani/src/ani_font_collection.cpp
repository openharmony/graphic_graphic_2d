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
#include "font_collection_mgr.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

namespace {
const std::string FONT_COLLECTION_INSTANCE_SIGN = ":C{" + std::string(ANI_CLASS_FONT_COLLECTION) + "}";
const std::string LOAD_FONT_SYNC_SIGN = "C{std.core.String}X{C{global.resource.Resource}C{std.core.String}}:";
const std::string UNLOAD_FONT_SYNC_SIGN = "C{std.core.String}:";
const std::string LOAD_FONT_SYNC_WITH_CHECK_SIGN =
    "C{std.core.String}X{C{global.resource.Resource}C{std.core.String}}C{std.core.Int}:";

AniTextResult LoadString(ani_env* env, ani_object path, std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection,
    std::string familyName, uint32_t index = 0)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;

    std::string pathStr;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(path), pathStr);
    if (ret != ANI_OK) {
        return AniTextResult::BusinessInvalid();
    }
    if (!AniTextUtils::SplitAbsoluteFontPath(pathStr)) {
        TEXT_LOGE("Failed to split absolute font path");
        return AniTextResult::Invalid("File path must start with 'file://', e.g., file:///path/to/font.ttf");
    }
    auto result = AniTextUtils::ReadFile(pathStr, dataLen, data);
    if (!result.success) {
        return result;
    }
    if (fontCollection->LoadFont(familyName, data.get(), dataLen, index) == nullptr) {
        TEXT_LOGE("Failed to load font");
        return AniTextResult::Error(MLB::ERROR_FILE_CORRUPTED);
    }
    return AniTextResult::Success();
}

AniTextResult LoadResource(ani_env* env, ani_object path, std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection,
    std::string familyName, uint32_t index = 0)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;

    AniResource resource = AniResourceParser::ParseResource(env, path);
    auto result = AniResourceParser::ResolveResource(resource, dataLen, data);
    if (!result.success) {
        return result;
    }
    if (dataLen == 0) {
        return AniTextResult::Error(MLB::ERROR_FILE_EMPTY);
    }
    if (fontCollection->LoadFont(familyName, data.get(), dataLen, index) == nullptr) {
        TEXT_LOGE("Failed to load font");
        return AniTextResult::Error(MLB::ERROR_FILE_CORRUPTED);
    }
    return AniTextResult::Success();
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

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "loadFontSync", LOAD_FONT_SYNC_SIGN.c_str(), reinterpret_cast<void*>(LoadFontSync) },
        ani_native_function {
            "unloadFontSync", UNLOAD_FONT_SYNC_SIGN.c_str(), reinterpret_cast<void*>(UnloadFontSync) },
        ani_native_function { "clearCaches", ":", reinterpret_cast<void*>(ClearCaches) },
        ani_native_function { "loadFontSyncWithCheck", LOAD_FONT_SYNC_WITH_CHECK_SIGN.c_str(),
            reinterpret_cast<void*>(LoadFontSyncWithCheck) },
    };
    ret = env->Class_BindNativeMethods(AniGlobalClass::GetInstance().fontCollection, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE(
            "Failed to bind methods for FontCollection: %{public}s, ret %{public}d", ANI_CLASS_FONT_COLLECTION, ret);
        return ret;
    }

    std::array staticMethods = {
        ani_native_function{"getGlobalInstance", FONT_COLLECTION_INSTANCE_SIGN.c_str(),
            reinterpret_cast<void*>(GetGlobalInstance)},
        ani_native_function{"getLocalInstance", FONT_COLLECTION_INSTANCE_SIGN.c_str(),
            reinterpret_cast<void*>(GetLocalInstance)},
        ani_native_function{"nativeTransferStatic", "C{std.interop.ESValue}:C{std.core.Object}",
            reinterpret_cast<void*>(NativeTransferStatic)},
        ani_native_function{
            "nativeTransferDynamic", "l:C{std.interop.ESValue}", reinterpret_cast<void*>(NativeTransferDynamic)},
    };
    ret = env->Class_BindStaticNativeMethods(
        AniGlobalClass::GetInstance().fontCollection, staticMethods.data(), staticMethods.size());
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
        object, TEXT_BIND_NATIVE, "l:", reinterpret_cast<ani_long>(aniFontCollection));
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
    ani_object obj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontCollection,
        AniGlobalMethod::GetInstance().fontCollection, reinterpret_cast<ani_long>(aniFontCollection));
    if (AniTextUtils::IsUndefined(env, obj)) {
        TEXT_LOGE("Failed to create ani font collection obj");
        delete aniFontCollection;
        aniFontCollection = nullptr;
        return nullptr;
    }
    return obj;
}

ani_object AniFontCollection::GetLocalInstance(ani_env* env, ani_class cls)
{
    AniFontCollection* aniFontCollection = new AniFontCollection();

    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    auto localInstance = FontCollectionMgr::GetInstance().GetLocalInstance(envAddress);
    if (localInstance != nullptr) {
        aniFontCollection->fontCollection_ = localInstance;
    } else {
        aniFontCollection->fontCollection_->EnableGlobalFontMgr();
        FontCollectionMgr::GetInstance().InsertLocalInstance(envAddress, aniFontCollection->fontCollection_);
    }

    ani_object obj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontCollection,
        AniGlobalMethod::GetInstance().fontCollection, reinterpret_cast<ani_long>(aniFontCollection));
    if (AniTextUtils::IsUndefined(env, obj)) {
        TEXT_LOGE("Failed to create ani font collection obj");
        delete aniFontCollection;
        return nullptr;
    }
    return obj;
}

AniTextResult AniFontCollection::OnLoadFontSync(
    ani_env* env, ani_object obj, ani_string name, ani_object path, int32_t index)
{
    std::string familyName;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, name, familyName);
    if (ret != ANI_OK) {
        return AniTextResult::BusinessInvalid();
    }
    auto aniFontCollection = AniTextUtils::GetNativeFromObj<AniFontCollection>(
        env, obj, AniGlobalMethod::GetInstance().fontCollectionGetNative);
    if (aniFontCollection == nullptr || aniFontCollection->fontCollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return AniTextResult::Invalid();
    }

    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, aniFontCollection->fontCollection_)) {
        TEXT_LOGE("Failed to check local instance, familyName %{public}s", familyName.c_str());
        return AniTextResult::Invalid();
    }

    ani_boolean isString = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().aniString, &isString);

    if (isString) {
        return LoadString(env, path, aniFontCollection->fontCollection_, familyName, index);
    }

    ani_boolean isResource = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().globalResource, &isResource);
    if (isResource) {
        return LoadResource(env, path, aniFontCollection->fontCollection_, familyName, index);
    }
    return AniTextResult::BusinessInvalid();
}

void AniFontCollection::LoadFontSync(ani_env* env, ani_object obj, ani_string name, ani_object path)
{
    OnLoadFontSync(env, obj, name, path);
}

void AniFontCollection::LoadFontSyncWithCheck(
    ani_env* env, ani_object obj, ani_string name, ani_object path, ani_object index)
{
    int32_t nativeIndex = 0;
    ani_boolean isUndefined = false;
    if (env->Reference_IsUndefined(index, &isUndefined) == ANI_OK && !isUndefined) {
        env->Object_CallMethod_Int(index, AniGlobalMethod::GetInstance().intGet, &nativeIndex);
    }
    auto result = OnLoadFontSync(env, obj, name, path, nativeIndex);
    if (!result.success) {
        AniTextUtils::ThrowBusinessError(env, static_cast<TextErrorCode>(result.errorCode), result.ToString().c_str());
    }
}

void AniFontCollection::ClearCaches(ani_env* env, ani_object obj)
{
    auto aniFontCollection = AniTextUtils::GetNativeFromObj<AniFontCollection>(
        env, obj, AniGlobalMethod::GetInstance().fontCollectionGetNative);
    if (aniFontCollection == nullptr || aniFontCollection->fontCollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return;
    }
    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, aniFontCollection->fontCollection_)) {
        TEXT_LOGE("Failed to check local instance");
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
    auto aniFontCollection = AniTextUtils::GetNativeFromObj<AniFontCollection>(
        env, obj, AniGlobalMethod::GetInstance().fontCollectionGetNative);
    if (aniFontCollection == nullptr || aniFontCollection->fontCollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return;
    }
    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, aniFontCollection->fontCollection_)) {
        TEXT_LOGE("Failed to check local instance, familyName %{public}s", familyName.c_str());
        return;
    }

    aniFontCollection->fontCollection_->UnloadFont(familyName);
}

std::shared_ptr<FontCollection> AniFontCollection::GetFontCollection()
{
    return fontCollection_;
}

ani_object AniFontCollection::NativeTransferStatic(ani_env* env, ani_class cls, ani_object input)
{
    return AniTransferUtils::TransferStatic(env, input, [](ani_env* env, void* unwrapResult) {
        JsFontCollection* jsFontCollection = reinterpret_cast<JsFontCollection*>(unwrapResult);
        if (jsFontCollection == nullptr) {
            TEXT_LOGE("Null jsFontCollection");
            return AniTextUtils::CreateAniUndefined(env);
        }
        AniFontCollection* aniFontCollection = new AniFontCollection();
        aniFontCollection->fontCollection_ = jsFontCollection->GetFontCollection();
        ani_object staticObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontCollection,
            AniGlobalMethod::GetInstance().fontCollection, reinterpret_cast<ani_long>(aniFontCollection));
        if (AniTextUtils::IsUndefined(env, staticObj)) {
            TEXT_LOGE("Failed to create ani font collection obj");
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
        [](napi_env napiEnv, ani_long nativeObj, napi_value objValue) -> napi_value {
            objValue = JsFontCollection::Init(napiEnv, objValue);
            napi_value dynamicObj = nullptr;
            napi_status status = JsFontCollection::CreateFontCollection(napiEnv, objValue, &dynamicObj);
            if (status != napi_ok || dynamicObj == nullptr) {
                TEXT_LOGE("Failed to create font collection, status %{public}d", status);
                return nullptr;
            }
            AniFontCollection* aniFontCollection = reinterpret_cast<AniFontCollection*>(nativeObj);
            if (aniFontCollection == nullptr || aniFontCollection->fontCollection_ == nullptr) {
                TEXT_LOGE("Null font collection");
                return nullptr;
            }
            status = JsFontCollection::SetFontCollection(napiEnv, dynamicObj, aniFontCollection->GetFontCollection());
            if (status != napi_ok) {
                TEXT_LOGE("Failed to set inner font collection, status %{public}d", status);
                return nullptr;
            }
            return dynamicObj;
        });
}
} // namespace OHOS::Text::ANI