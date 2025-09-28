/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "js_fontcollection.h"

#include <fstream>
#include <tuple>

#include "ability.h"
#include "file_ex.h"
#include "font_collection_mgr.h"
#include "napi_async_work.h"

namespace OHOS::Rosen {
namespace {
const std::string CLASS_NAME = "FontCollection";
struct FontArgumentsConcreteContext : public FontPathResourceContext {
    std::string familyName;
};

void GetFilePathResource(napi_env env, napi_value* argv, sptr<FontArgumentsConcreteContext> context)
{
    if (!ParseContextFilePath(env, argv, context, ARGC_ONE)) {
        auto* fontCollection = reinterpret_cast<JsFontCollection*>(context->native);
        NAPI_CHECK_ARGS(context, fontCollection != nullptr, napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "FontCollection is null");
        NAPI_CHECK_ARGS(context, ParseResourceType(env, argv[ARGC_ONE], context->info),
            napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return, "Parse resource error");
    }
}
}

std::mutex JsFontCollection::constructorMutex_;
thread_local napi_ref JsFontCollection::constructor_ = nullptr;

napi_value JsFontCollection::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get params");
        return nullptr;
    }

    JsFontCollection* jsFontCollection = new(std::nothrow) JsFontCollection();
    if (jsFontCollection == nullptr) {
        TEXT_LOGE("Failed to new font collection");
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsFontCollection,
        JsFontCollection::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsFontCollection;
        TEXT_LOGE("Failed to wrap font collection");
        return nullptr;
    }
    return jsThis;
}

napi_value JsFontCollection::Init(napi_env env, napi_value exportObj)
{
    if (!CreateConstructor(env)) {
        TEXT_LOGE("Failed to create constructor");
        return nullptr;
    }
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get reference, ret %{public}d", status);
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set named property");
        return nullptr;
    }
    return exportObj;
}

bool JsFontCollection::CreateConstructor(napi_env env)
{
    std::lock_guard<std::mutex> lock(constructorMutex_);
    if (constructor_) {
        return true;
    }
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getGlobalInstance", JsFontCollection::GetGlobalInstance),
        DECLARE_NAPI_STATIC_FUNCTION("getLocalInstance", JsFontCollection::GetLocalInstance),
        DECLARE_NAPI_FUNCTION("loadFontSync", JsFontCollection::LoadFontSync),
        DECLARE_NAPI_FUNCTION("clearCaches", JsFontCollection::ClearCaches),
        DECLARE_NAPI_FUNCTION("loadFont", JsFontCollection::LoadFontAsync),
        DECLARE_NAPI_FUNCTION("unloadFontSync", JsFontCollection::UnloadFontSync),
        DECLARE_NAPI_FUNCTION("unloadFont", JsFontCollection::UnloadFontAsync),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to define class, ret %{public}d", status);
        return false;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to create reference, ret %{public}d", status);
        return false;
    }
    return true;
}

void JsFontCollection::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsFontCollection* napi = reinterpret_cast<JsFontCollection*>(nativeObject);
        delete napi;
    }
}

JsFontCollection::JsFontCollection()
{
    fontcollection_ = OHOS::Rosen::FontCollection::From(nullptr);
}

napi_status JsFontCollection::CreateFontCollection(napi_env env, napi_value exportObj, napi_value* obj)
{
    return NewInstanceFromConstructor(env, exportObj, CLASS_NAME.c_str(), obj);
}

napi_status JsFontCollection::SetFontCollection(
    napi_env env, napi_value obj, std::shared_ptr<FontCollection> fontCollection)
{
    if (env == nullptr || obj == nullptr || fontCollection == nullptr) {
        TEXT_LOGE("Invalid arguments");
        return napi_invalid_arg;
    }
    JsFontCollection* jsFontCollection = nullptr;
    napi_status status = napi_unwrap(env, obj, reinterpret_cast<void**>(&jsFontCollection));
    if (status != napi_ok || jsFontCollection == nullptr) {
        TEXT_LOGE("Failed to unwrap JsFontCollection, status: %{public}d", status);
        return status;
    }
    jsFontCollection->fontcollection_ = fontCollection;
    return napi_ok;
}

std::shared_ptr<FontCollection> JsFontCollection::GetFontCollection()
{
    return fontcollection_;
}

napi_value JsFontCollection::GenerateNewInstance(napi_env env)
{
    if (!CreateConstructor(env)) {
        TEXT_LOGE("Failed to create constructor");
        return nullptr;
    }
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok || constructor == nullptr) {
        TEXT_LOGE("Failed to get constructor object");
        return nullptr;
    }

    napi_value object = nullptr;
    status = napi_new_instance(env, constructor, 0, nullptr, &object);
    if (status != napi_ok) {
        return nullptr;
    }
    return object;
}

napi_value JsFontCollection::GetGlobalInstance(napi_env env, napi_callback_info info)
{
    std::ignore = info;
    napi_value object = GenerateNewInstance(env);
    if (object == nullptr) {
        TEXT_LOGE("Failed to new instance");
        return nullptr;
    }

    JsFontCollection* jsFontCollection = nullptr;
    napi_status status = napi_unwrap(env, object, reinterpret_cast<void**>(&jsFontCollection));
    if (status != napi_ok || jsFontCollection == nullptr) {
        TEXT_LOGE("Failed to unwrap font collection");
        return nullptr;
    }
    jsFontCollection->fontcollection_ = OHOS::Rosen::FontCollection::Create();

    return object;
}

napi_value JsFontCollection::GetLocalInstance(napi_env env, napi_callback_info info)
{
    std::ignore = info;
    napi_value object = GenerateNewInstance(env);
    if (object == nullptr) {
        TEXT_LOGE("Failed to new instance");
        return nullptr;
    }

    JsFontCollection* jsLocalFontCollection = nullptr;
    napi_status status = napi_unwrap(env, object, reinterpret_cast<void**>(&jsLocalFontCollection));
    if (status != napi_ok || jsLocalFontCollection == nullptr) {
        TEXT_LOGE("Failed to unwrap font collection");
        return nullptr;
    }
    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    auto localInstance = FontCollectionMgr::GetInstance().GetLocalInstance(envAddress);
    if (localInstance != nullptr) {
        jsLocalFontCollection->fontcollection_ = localInstance;
        return object;
    }
    jsLocalFontCollection->fontcollection_->EnableGlobalFontMgr();
    FontCollectionMgr::GetInstance().InsertLocalInstance(envAddress, jsLocalFontCollection->fontcollection_);
    return object;
}

napi_value JsFontCollection::LoadFontSync(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnLoadFont(env, info) : nullptr;
}

bool JsFontCollection::LoadFontFromPath(const std::string path, const std::string familyName)
{
    if (fontcollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return false;
    }

    char tmpPath[PATH_MAX] = { 0 };
    if (realpath(path.c_str(), tmpPath) == nullptr) {
        TEXT_LOGE("Invalid path %{public}s", path.c_str());
        return false;
    }

    std::ifstream ifs(tmpPath, std::ios_base::in);
    if (!ifs.is_open()) {
        return false;
    }

    ifs.seekg(0, ifs.end);
    size_t datalen = static_cast<size_t>(ifs.tellg());
    ifs.seekg(ifs.beg);
    if (ifs.fail()) {
        return false;
    }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(datalen);
    ifs.read(buffer.get(), datalen);
    const uint8_t* data = reinterpret_cast<uint8_t*>(buffer.get());
    return fontcollection_->LoadFont(familyName.c_str(), data, datalen) != nullptr;
}

napi_value JsFontCollection::OnLoadFont(napi_env env, napi_callback_info info)
{
    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, fontcollection_)) {
        TEXT_LOGE("Failed to check local instance");
        return nullptr;
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = { nullptr };
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok || argc < ARGC_TWO) {
        TEXT_LOGE("Failed to get argument, argc %{public}zu", argc);
        return nullptr;
    }
    if (argv[0] == nullptr || argv[1] == nullptr) {
        TEXT_LOGE("Invalid argument");
        return nullptr;
    }
    std::string familyName;
    std::string familySrc;
    if (!ConvertFromJsValue(env, argv[0], familyName)) {
        TEXT_LOGE("Failed to convert family name");
        return nullptr;
    }
    if (ConvertFromJsValue(env, argv[1], familySrc) && SplitAbsolutePath(familySrc)) {
        TEXT_ERROR_CHECK(LoadFontFromPath(familySrc, familyName), return nullptr, "Failed to load font from path");
        return NapiGetUndefined(env);
    }
    ResourceInfo resourceInfo;
    if (ParseResourceType(env, argv[1], resourceInfo)) {
        auto pathCB = [this, familyName](std::string& path) -> bool {
            return SplitAbsolutePath(path) && this->LoadFontFromPath(path, familyName);
        };
        auto fileCB = [this, familyName](const void* data, size_t size) -> bool {
            return this->fontcollection_->LoadFont(familyName.c_str(), static_cast<const uint8_t*>(data), size) !=
                   nullptr;
        };
        TEXT_ERROR_CHECK(ProcessResource(resourceInfo, pathCB, fileCB), return nullptr,
            "Failed to load font from resource");
        return NapiGetUndefined(env);
    }

    return nullptr;
}

napi_value JsFontCollection::ClearCaches(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnClearCaches(env, info) : nullptr;
}

napi_value JsFontCollection::OnClearCaches(napi_env env, napi_callback_info info)
{
    if (fontcollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "JsFontCollection::OnClearCaches fontCollection is nullptr.");
    }
    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, fontcollection_)) {
        TEXT_LOGE("Failed to check local instance");
        return nullptr;
    }
    fontcollection_->ClearCaches();
    return NapiGetUndefined(env);
}

napi_value JsFontCollection::LoadFontAsync(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnLoadFontAsync(env, info) : nullptr;
}

napi_value JsFontCollection::OnLoadFontAsync(napi_env env, napi_callback_info info)
{
    sptr<FontArgumentsConcreteContext> context = sptr<FontArgumentsConcreteContext>::MakeSptr();
    NAPI_CHECK_AND_THROW_ERROR(context != nullptr, TextErrorCode::ERROR_NO_MEMORY, "Failed to make context");
    auto inputParser = [env, context](size_t argc, napi_value* argv) {
        TEXT_ERROR_CHECK(argv != nullptr, return, "Argv is null");
        NAPI_CHECK_ARGS(context, context->status == napi_ok, napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Status error, status=%d", static_cast<int>(context->status));
        NAPI_CHECK_ARGS(context, argc >= ARGC_TWO, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM,
            return, "Argc is invalid %zu", argc);
        NAPI_CHECK_ARGS(context, ConvertFromJsValue(env, argv[0], context->familyName), napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "FamilyName is invalid %s", context->familyName.c_str());
        GetFilePathResource(env, argv, context);
    };

    context->GetCbInfo(env, info, inputParser);

    auto executor = [context, env]() {
        TEXT_ERROR_CHECK(context != nullptr, return, "Context is null");

        auto* fontCollection = reinterpret_cast<JsFontCollection*>(context->native);
        NAPI_CHECK_ARGS(context, fontCollection != nullptr, napi_generic_failure,
            TextErrorCode::ERROR_INVALID_PARAM, return, "FontCollection is null");

        NAPI_CHECK_ARGS(context, fontCollection->fontcollection_ != nullptr, napi_generic_failure,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Inner fontcollection is null");
        NAPI_CHECK_ARGS(context, FontCollectionMgr::GetInstance().CheckInstanceIsValid(
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env)), fontCollection->fontcollection_),
            napi_generic_failure, TextErrorCode::ERROR_INVALID_PARAM, return, "Failed to check local instance");

        if (!context->filePath.empty()) {
            NAPI_CHECK_ARGS(context, SplitAbsolutePath(context->filePath),
                napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return, "Failed to split absolute font path");

            NAPI_CHECK_ARGS(context, fontCollection->LoadFontFromPath(context->filePath, context->familyName),
                napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return, "Failed to get font file properties");
        } else {
            auto pathCB = [context, fontCollection](std::string& path) -> bool {
                return SplitAbsolutePath(path) && fontCollection->LoadFontFromPath(path, context->familyName);
            };
            auto fileCB = [context, fontCollection](const void* data, size_t size) -> bool {
                return fontCollection->fontcollection_->LoadFont(context->familyName.c_str(),
                    static_cast<const uint8_t*>(data), size) != nullptr;
            };
            NAPI_CHECK_ARGS(context, ProcessResource(context->info, pathCB, fileCB), napi_invalid_arg,
                TextErrorCode::ERROR_INVALID_PARAM, return, "Failed to execute function, path is invalid");
        }
    };

    auto complete = [env](napi_value& output) {
        output = NapiGetUndefined(env);
    };
    return NapiAsyncWork::Enqueue(env, context, "OnLoadFontAsync", executor, complete);
}

napi_value JsFontCollection::UnloadFontAsync(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnUnloadFontAsync(env, info) : nullptr;
}

napi_value JsFontCollection::UnloadFontSync(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnUnloadFont(env, info) : nullptr;
}


napi_value JsFontCollection::OnUnloadFontAsync(napi_env env, napi_callback_info info)
{
    sptr<FontArgumentsConcreteContext> context = sptr<FontArgumentsConcreteContext>::MakeSptr();
    NAPI_CHECK_AND_THROW_ERROR(context != nullptr, TextErrorCode::ERROR_NO_MEMORY, "Failed to make context");
    auto inputParser = [env, context](size_t argc, napi_value* argv) {
        TEXT_ERROR_CHECK(argv != nullptr, return, "Argv is null");
        NAPI_CHECK_ARGS(context, context->status == napi_ok, napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Status error, status=%d", static_cast<int>(context->status));
        NAPI_CHECK_ARGS(context, argc >= ARGC_ONE, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM,
            return, "Argc is invalid %zu", argc);
        NAPI_CHECK_ARGS(context, ConvertFromJsValue(env, argv[0], context->familyName), napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "FamilyName is invalid %s", context->familyName.c_str());
    };

    context->GetCbInfo(env, info, inputParser);

    auto executor = [context, env]() {
        TEXT_ERROR_CHECK(context != nullptr, return, "Context is null");

        auto* fontCollection = reinterpret_cast<JsFontCollection*>(context->native);
        NAPI_CHECK_ARGS(context, fontCollection != nullptr, napi_generic_failure, TextErrorCode::ERROR_INVALID_PARAM,
            return, "FontCollection is null");

        NAPI_CHECK_ARGS(context, fontCollection->fontcollection_ != nullptr, napi_generic_failure,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Inner fontcollection is null");
        NAPI_CHECK_ARGS(context, FontCollectionMgr::GetInstance().CheckInstanceIsValid(
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env)), fontCollection->fontcollection_),
            napi_generic_failure, TextErrorCode::ERROR_INVALID_PARAM, return, "Failed to check local instance");
        fontCollection->fontcollection_->UnloadFont(context->familyName);
    };

    auto complete = [env](napi_value& output) {
        output = NapiGetUndefined(env);
    };
    return NapiAsyncWork::Enqueue(env, context, "OnUnloadFontAsync", executor, complete);
}

napi_value JsFontCollection::OnUnloadFont(napi_env env, napi_callback_info info)
{
    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, fontcollection_)) {
        TEXT_LOGE("Failed to check local instance");
        return nullptr;
    }
    
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = { nullptr };
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get argument, argc %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to get argument");
    }
    if (argv[0] == nullptr) {
        TEXT_LOGE("Null argv[0]");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to get argument");
    }
    std::string familyName;
    if (!ConvertFromJsValue(env, argv[0], familyName)) {
        TEXT_LOGE("Failed to convert argv[0]");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to get argument");
    }

    fontcollection_->UnloadFont(familyName);

    return NapiGetUndefined(env);
}

} // namespace OHOS::Rosen
