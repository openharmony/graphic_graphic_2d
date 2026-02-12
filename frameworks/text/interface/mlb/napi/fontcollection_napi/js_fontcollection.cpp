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

namespace OHOS::Rosen {
namespace {
const std::string CLASS_NAME = "FontCollection";

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
} // namespace

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
        DECLARE_NAPI_FUNCTION("loadFontWithCheck", JsFontCollection::LoadFontAsyncWithCheck),
        DECLARE_NAPI_FUNCTION("loadFontSyncWithCheck", JsFontCollection::LoadFontSyncWithCheck),
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
    return (me != nullptr) ? me->OnLoadFont(env, info).result : nullptr;
}


napi_value JsFontCollection::LoadFontSyncWithCheck(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    if (me == nullptr) {
        return NapiThrowError(env, MLB::ERROR_INVALID_PARAM, "invalid param");
    }
    auto result = me->OnLoadFont(env, info);
    if (result.success) {
        return NapiGetUndefined(env);
    } else {
        return NapiThrowError(env, result.errorCode, result.ToString());
    }
}

napi_value JsFontCollection::LoadFontAsyncWithCheck(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    if (me == nullptr) {
        return NapiThrowError(env, MLB::ERROR_INVALID_PARAM, "invalid param");
    }
    auto result = me->OnLoadFontAsync(env, info, true);
    return result.result;
}

NapiTextResult JsFontCollection::LoadFontFromPath(const std::string path, const std::string familyName, uint32_t index)
{
    if (fontcollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return NapiTextResult::Invalid();
    }

    char tmpPath[PATH_MAX] = { 0 };
    if (realpath(path.c_str(), tmpPath) == nullptr) {
        TEXT_LOGE("Invalid path %{public}s", path.c_str());
        return NapiTextResult::Error(MLB::ERROR_FILE_NOT_EXIST);
    }

    std::ifstream ifs(tmpPath, std::ios_base::in);
    if (!ifs.is_open()) {
        return NapiTextResult::Error(MLB::ERROR_FILE_OPEN_FAILED);
    }

    ifs.seekg(0, ifs.end);
    size_t datalen = static_cast<size_t>(ifs.tellg());
    if (ifs.fail()) {
        return NapiTextResult::Error(MLB::ERROR_FILE_SIZE_FAILED);
    }
    ifs.seekg(ifs.beg);
    if (ifs.fail()) {
        return NapiTextResult::Error(MLB::ERROR_FILE_SEEK_FAILED);
    }

    if (datalen == 0) {
        return NapiTextResult::Error(MLB::ERROR_FILE_EMPTY);
    }
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(datalen);
    ifs.read(buffer.get(), datalen);
    if (ifs.fail()) {
        return NapiTextResult::Error(MLB::ERROR_FILE_READ_FAILED);
    }
    const uint8_t* data = reinterpret_cast<uint8_t*>(buffer.get());
    if (fontcollection_->LoadFont(familyName.c_str(), data, datalen, index) == nullptr) {
        return NapiTextResult::Error(MLB::ERROR_FILE_CORRUPTED);
    }
    return NapiTextResult::Success();
}

NapiTextResult JsFontCollection::OnLoadFont(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = { nullptr };
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok || argc < ARGC_TWO) {
        TEXT_LOGE("Failed to get argument, argc %{public}zu", argc);
        return NapiTextResult::BusinessInvalid();
    }
    if (argv[0] == nullptr || argv[1] == nullptr) {
        TEXT_LOGE("Invalid argument");
        return NapiTextResult::BusinessInvalid();
    }
    std::string alias;
    std::string path;
    if (!ConvertFromJsValue(env, argv[0], alias)) {
        TEXT_LOGE("Failed to convert family name");
        return NapiTextResult::BusinessInvalid();
    }
    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, fontcollection_)) {
        TEXT_LOGE("Failed to check local instance, familyName %{public}s", alias.c_str());
        return NapiTextResult::Invalid();
    }
    uint32_t index = 0;
    ConvertFromJsValue(env, argv[ARGC_TWO], index);
    auto pathCB = [this, env, alias, index](std::string& path) {
        if (SplitAbsolutePath(path)) {
            auto result = this->LoadFontFromPath(path, alias, index);
            result.result = result.success ? NapiGetUndefined(env) : nullptr;
            return result;
        } else {
            return NapiTextResult::Invalid("File path must start with 'file://', e.g., file:///path/to/font.ttf");
        }
    };
    if (ConvertFromJsValue(env, argv[1], path)) {
        return pathCB(path);
    }
    ResourceInfo resourceInfo;
    if (ParseResourceType(env, argv[1], resourceInfo)) {
        auto fileCB = [this, env, alias, index](const void* data, size_t size) {
            if (fontcollection_->LoadFont(alias.c_str(), static_cast<const uint8_t*>(data), size, index) != nullptr) {
                return NapiTextResult::Success(NapiGetUndefined(env));
            } else {
                return size == 0 ? NapiTextResult::Error(MLB::ERROR_FILE_EMPTY)
                                 : NapiTextResult::Error(MLB::ERROR_FILE_CORRUPTED);
            }
        };
        return ProcessResource(resourceInfo, pathCB, fileCB);
    }

    return NapiTextResult::BusinessInvalid();
}

napi_value JsFontCollection::ClearCaches(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnClearCaches(env, info).result : nullptr;
}

NapiTextResult JsFontCollection::OnClearCaches(napi_env env, napi_callback_info info)
{
    if (fontcollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return NapiTextResult::Invalid(
            "Internal Error", NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
                "JsFontCollection::OnClearCaches fontCollection is nullptr."));
    }
    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, fontcollection_)) {
        TEXT_LOGE("Failed to check local instance");
        return NapiTextResult::Invalid();
    }
    fontcollection_->ClearCaches();
    return NapiTextResult::Success(NapiGetUndefined(env));
}

napi_value JsFontCollection::LoadFontAsync(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnLoadFontAsync(env, info).result : nullptr;
}

void JsFontCollection::OnLoadFontAsyncExecutor(sptr<FontArgumentsConcreteContext> context)
{
    TEXT_ERROR_CHECK(context != nullptr, return, "Context is null");
    auto* fontCollection = reinterpret_cast<JsFontCollection*>(context->native);
    NAPI_CHECK_ARGS_WITH_STATEMENT(context, fontCollection != nullptr, napi_generic_failure,
        TextErrorCode::ERROR_INVALID_PARAM, return, context->result = NapiTextResult::Invalid(),
        "FontCollection is null");

    NAPI_CHECK_ARGS_WITH_STATEMENT(context, fontCollection->fontcollection_ != nullptr, napi_generic_failure,
        TextErrorCode::ERROR_INVALID_PARAM, return, context->result = NapiTextResult::Invalid(),
        "Inner fontcollection is null");
    NAPI_CHECK_ARGS_WITH_STATEMENT(context,
        FontCollectionMgr::GetInstance().CheckInstanceIsValid(
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(context->env)), fontCollection->fontcollection_),
        napi_generic_failure, TextErrorCode::ERROR_INVALID_PARAM, return, context->result = NapiTextResult::Invalid(),
        "Failed to check local instance, familyName %s", context->familyName.c_str());

    if (context->filePath.has_value()) {
        NAPI_CHECK_ARGS_WITH_STATEMENT(context, SplitAbsolutePath(*context->filePath), napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return,
            context->result =
                NapiTextResult::Invalid("File path must start with 'file://', e.g., file:///path/to/font.ttf"),
            "Failed to get font file properties");
        context->result = fontCollection->LoadFontFromPath(*context->filePath, context->familyName, context->index);
        NAPI_CHECK_ARGS(context, context->result.success, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return,
            "Failed to get font file properties");
    } else {
        auto pathCB = [context, fontCollection](std::string& path) {
            if (SplitAbsolutePath(path)) {
                return fontCollection->LoadFontFromPath(path, context->familyName, context->index);
            } else {
                return NapiTextResult::Invalid("File path must start with 'file://', e.g., file:///path/to/font.ttf");
            }
        };
        auto fileCB = [context, fontCollection](const void* data, size_t size) {
            if (size == 0) {
                return NapiTextResult::Error(MLB::ERROR_FILE_EMPTY);
            }
            if (fontCollection->fontcollection_->LoadFont(
                context->familyName.c_str(), static_cast<const uint8_t*>(data), size, context->index) != nullptr) {
                return NapiTextResult::Success();
            } else {
                return NapiTextResult::Error(MLB::ERROR_FILE_CORRUPTED, "Internal error");
            }
        };
        context->result = ProcessResource(context->info, pathCB, fileCB);
        NAPI_CHECK_ARGS(context, context->result.success, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return,
            "Failed to execute function, path is invalid");
    }
}

NapiTextResult JsFontCollection::OnLoadFontAsync(napi_env env, napi_callback_info info, bool withCheck)
{
    sptr<FontArgumentsConcreteContext> context = sptr<FontArgumentsConcreteContext>::MakeSptr();
    NAPI_CHECK_AND_THROW_ERROR(context != nullptr, TextErrorCode::ERROR_NO_MEMORY, "Failed to make context");
    auto inputParser = [env, context](size_t argc, napi_value* argv) {
        TEXT_ERROR_CHECK(argv != nullptr, return, "Argv is null");
        NAPI_CHECK_ARGS_WITH_STATEMENT(context, context->status == napi_ok, napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, context->result = NapiTextResult::Invalid(),
            "Status error, status=%d", context->status);
        NAPI_CHECK_ARGS_WITH_STATEMENT(context, argc >= ARGC_TWO, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM,
            return, context->result = NapiTextResult::BusinessInvalid(), "Argc is invalid: %zu", argc);
        NAPI_CHECK_ARGS_WITH_STATEMENT(context, ConvertFromJsValue(env, argv[0], context->familyName), napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, context->result = NapiTextResult::BusinessInvalid(),
            "FamilyName is invalid: %s", context->familyName.c_str());
        ConvertFromJsValue(env, argv[ARGC_TWO], context->index);
        GetFilePathResource(env, argv, context);
    };
    context->GetCbInfo(env, info, inputParser);
    auto executor = [this, context]() { OnLoadFontAsyncExecutor(context); };
    auto complete = [env, context, withCheck](napi_value& output) {
        if (withCheck && !context->result.success) {
            context->errCode = context->result.errorCode;
            context->errMessage = context->result.ToString();
        } else {
            output = NapiGetUndefined(env);
        }
    };
    return NapiAsyncWork::Enqueue(env, context, "OnLoadFontAsync", executor, complete);
}

napi_value JsFontCollection::UnloadFontAsync(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnUnloadFontAsync(env, info).result : nullptr;
}

napi_value JsFontCollection::UnloadFontSync(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnUnloadFont(env, info).result : nullptr;
}

NapiTextResult JsFontCollection::OnUnloadFontAsync(napi_env env, napi_callback_info info)
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
            napi_generic_failure, TextErrorCode::ERROR_INVALID_PARAM, return,
            "Failed to check local instance, familyName %s", context->familyName.c_str());
        fontCollection->fontcollection_->UnloadFont(context->familyName);
    };

    auto complete = [env](napi_value& output) { output = NapiGetUndefined(env); };
    return NapiAsyncWork::Enqueue(env, context, "OnUnloadFontAsync", executor, complete);
}

NapiTextResult JsFontCollection::OnUnloadFont(napi_env env, napi_callback_info info)
{
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

    uint64_t envAddress = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(env));
    if (!FontCollectionMgr::GetInstance().CheckInstanceIsValid(envAddress, fontcollection_)) {
        TEXT_LOGE("Failed to check local instance, familyName %{public}s", familyName.c_str());
        return NapiTextResult::Invalid();
    }

    fontcollection_->UnloadFont(familyName);

    return NapiTextResult::Success(NapiGetUndefined(env));
}
} // namespace OHOS::Rosen
