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

#include <fstream>
#include "js_fontcollection.h"

#include "ability.h"
#include "napi_async_work.h"

namespace OHOS::Rosen {
namespace {
constexpr size_t FILE_HEAD_LENGTH = 7; // 7 is the size of "file://"
const std::string CLASS_NAME = "FontCollection";
const int32_t GLOBAL_ERROR = 10000;
struct FontArgumentsConcreteContext : public ContextBase {
    std::string familyName;
    std::string filePath;
    ResourceInfo info;
};

bool ParseContextFilePath(napi_env env, napi_value* argv, sptr<FontArgumentsConcreteContext> context)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[ARGC_ONE], &valueType);

    if (valueType == napi_object) {
        return false;
    } else if (valueType == napi_string) {
        if (!ConvertFromJsValue(env, argv[ARGC_ONE], context->filePath)) {
            std::string errMessage("Failed to convert file path:");
            errMessage += context->filePath;
            context->status = napi_invalid_arg;
            context->errMessage = errMessage;
            (context)->errCode = static_cast<int32_t>(TextErrorCode::ERROR_INVALID_PARAM);
            TEXT_LOGE("%{public}s", errMessage.c_str());
        }
    }
    return true;
}
}

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
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getGlobalInstance", JsFontCollection::GetGlobalInstance),
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
        TEXT_LOGE("Failed to define class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to create reference");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set named property");
        return nullptr;
    }
    return exportObj;
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

std::shared_ptr<FontCollection> JsFontCollection::GetFontCollection()
{
    return fontcollection_;
}

napi_value JsFontCollection::GetGlobalInstance(napi_env env, napi_callback_info info)
{
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok || !constructor) {
        TEXT_LOGE("Failed to get constructor object");
        return nullptr;
    }

    napi_value object = nullptr;
    status = napi_new_instance(env, constructor, 0, nullptr, &object);
    if (status != napi_ok || !object) {
        TEXT_LOGE("Failed to new instance");
        return nullptr;
    }

    JsFontCollection* jsFontCollection = nullptr;
    status = napi_unwrap(env, object, reinterpret_cast<void**>(&jsFontCollection));
    if (status != napi_ok || !jsFontCollection) {
        TEXT_LOGE("Failed to unwrap font collection");
        return nullptr;
    }
    jsFontCollection->fontcollection_ = OHOS::Rosen::FontCollection::Create();

    return object;
}

napi_value JsFontCollection::LoadFontSync(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnLoadFont(env, info) : nullptr;
}

bool JsFontCollection::SplitAbsoluteFontPath(std::string& absolutePath)
{
    auto iter = absolutePath.find_first_of(':');
    if (iter == std::string::npos) {
        TEXT_LOGE("Failed to find separator in path:%{public}s", absolutePath.c_str());
        return false;
    }
    std::string head = absolutePath.substr(0, iter);
    if ((head == "file" && absolutePath.size() > FILE_HEAD_LENGTH)) {
        absolutePath = absolutePath.substr(iter + 3); // 3 means skip "://"
        // the file format is like "file://system/fonts...",
        return true;
    }

    return false;
}

bool JsFontCollection::GetResourcePartData(napi_env env, ResourceInfo& info, napi_value paramsNApi,
    napi_value bundleNameNApi, napi_value moduleNameNApi)
{
    napi_valuetype valueType = napi_undefined;
    bool isArray = false;
    if (napi_is_array(env, paramsNApi, &isArray) != napi_ok) {
        TEXT_LOGE("Failed to get array type");
        return false;
    }
    if (!isArray) {
        TEXT_LOGE("Invalid array type");
        return false;
    }

    uint32_t arrayLength = 0;
    napi_get_array_length(env, paramsNApi, &arrayLength);
    for (uint32_t i = 0; i < arrayLength; i++) {
        size_t ret = 0;
        napi_value indexValue = nullptr;
        napi_get_element(env, paramsNApi, i, &indexValue);
        napi_typeof(env, indexValue, &valueType);
        if (valueType == napi_string) {
            size_t strLen = GetParamLen(env, indexValue) + 1;
            std::unique_ptr<char[]> indexStr = std::make_unique<char[]>(strLen);
            napi_get_value_string_utf8(env, indexValue, indexStr.get(), strLen, &ret);
            info.params.emplace_back(indexStr.get());
        } else if (valueType == napi_number) {
            int32_t num = 0;
            napi_get_value_int32(env, indexValue, &num);
            info.params.emplace_back(std::to_string(num));
        } else {
            TEXT_LOGE("Invalid value type %{public}d", valueType);
            return false;
        }
    }

    napi_typeof(env, bundleNameNApi, &valueType);
    if (valueType == napi_string) {
        size_t ret = 0;
        size_t strLen = GetParamLen(env, bundleNameNApi) + 1;
        std::unique_ptr<char[]> bundleNameStr = std::make_unique<char[]>(strLen);
        napi_get_value_string_utf8(env, bundleNameNApi, bundleNameStr.get(), strLen, &ret);
        info.bundleName = bundleNameStr.get();
    }

    napi_typeof(env, moduleNameNApi, &valueType);
    if (valueType == napi_string) {
        size_t ret = 0;
        size_t strLen = GetParamLen(env, moduleNameNApi) + 1;
        std::unique_ptr<char[]> moduleNameStr = std::make_unique<char[]>(strLen);
        napi_get_value_string_utf8(env, moduleNameNApi, moduleNameStr.get(), strLen, &ret);
        info.moduleName = moduleNameStr.get();
    }

    return true;
}

bool JsFontCollection::ParseResourceType(napi_env env, napi_value value, ResourceInfo& info)
{
    napi_value idNApi = nullptr;
    napi_value typeNApi = nullptr;
    napi_value paramsNApi = nullptr;
    napi_value bundleNameNApi = nullptr;
    napi_value moduleNameNApi = nullptr;
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType == napi_object) {
        napi_get_named_property(env, value, "id", &idNApi);
        napi_get_named_property(env, value, "type", &typeNApi);
        napi_get_named_property(env, value, "params", &paramsNApi);
        napi_get_named_property(env, value, "bundleName", &bundleNameNApi);
        napi_get_named_property(env, value, "moduleName", &moduleNameNApi);
    } else {
        return false;
    }

    napi_typeof(env, idNApi, &valueType);
    if (valueType == napi_number) {
        napi_get_value_int32(env, idNApi, &info.resId);
    }

    napi_typeof(env, typeNApi, &valueType);
    if (valueType == napi_number) {
        napi_get_value_int32(env, typeNApi, &info.type);
    }
    if (!GetResourcePartData(env, info, paramsNApi, bundleNameNApi, moduleNameNApi)) {
        return false;
    }

    return true;
}

std::shared_ptr<Global::Resource::ResourceManager> JsFontCollection::GetResourceManager() const
{
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    TEXT_ERROR_CHECK(context != nullptr, return nullptr, "Failed to get application context");
    auto resourceManager = context->GetResourceManager();
    TEXT_ERROR_CHECK(resourceManager != nullptr, return nullptr, "Failed to get resource manager");
    return resourceManager;
}

bool JsFontCollection::ParseResourcePath(const std::string familyName, ResourceInfo& info)
{
    int32_t state = 0;
    auto resourceManager = GetResourceManager();
    TEXT_ERROR_CHECK(resourceManager != nullptr, return false,
        "Failed to get resourceManager, resourceManager is nullptr");

    if (info.type == static_cast<int32_t>(ResourceType::STRING)) {
        std::string rPath;
        if (info.resId < 0 && !info.params.empty() && info.params[0].size() > 0) {
            rPath = info.params[0];
        } else {
            state = resourceManager->GetStringById(info.resId, rPath);
            if (state >= GLOBAL_ERROR || state < 0) {
                return false;
            }
            if (!SplitAbsoluteFontPath(rPath) || !GetFontFileProperties(rPath, familyName)) {
                return false;
            }
        }
    } else if (info.type == static_cast<int32_t>(ResourceType::RAWFILE)) {
        size_t dataLen = 0;
        std::unique_ptr<uint8_t[]> rawData;

        if (info.params.empty()) {
            return false;
        }

        state = resourceManager->GetRawFileFromHap(info.params[0], dataLen, rawData);
        if (state >= GLOBAL_ERROR || state < 0) {
            return false;
        }
        if (!fontcollection_->LoadFont(familyName.c_str(), rawData.get(), dataLen)) {
            return false;
        }
        return true;
    } else {
        TEXT_LOGE("Invalid resource type %{public}d", info.type);
        return false;
    }
    return true;
}

bool JsFontCollection::GetFontFileProperties(const std::string path, const std::string familyName)
{
    if (fontcollection_ == nullptr) {
        TEXT_LOGE("Null font collection");
        return false;
    }

    char tmpPath[PATH_MAX] = {0};
    if (realpath(path.c_str(), tmpPath) == nullptr) {
        TEXT_LOGE("Invalid path %{public}s", path.c_str());
        return false;
    }

    std::ifstream f(tmpPath);
    if (!f.good()) {
        TEXT_LOGE("Failed to access %{public}s, %{public}s", tmpPath, strerror(errno));
        return false;
    }

    std::ifstream ifs(tmpPath, std::ios_base::in);
    if (!ifs.is_open()) {
        return false;
    }

    ifs.seekg(0, ifs.end);
    if (!ifs.good()) {
        ifs.close();
        return false;
    }

    size_t datalen = static_cast<size_t>(ifs.tellg());
    if (ifs.fail()) {
        ifs.close();
        return false;
    }

    ifs.seekg(ifs.beg);
    if (!ifs.good()) {
        ifs.close();
        return false;
    }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(datalen);
    ifs.read(buffer.get(), datalen);
    if (!ifs.good()) {
        TEXT_LOGE("Failed to read %{public}s, data len %{public}zu, %{public}s",
            tmpPath, datalen, strerror(errno));
        ifs.close();
        return false;
    }
    ifs.close();
    const uint8_t* rawData = reinterpret_cast<uint8_t*>(buffer.get());
    if (!fontcollection_->LoadFont(familyName.c_str(), rawData, datalen)) {
        TEXT_LOGE("Failed to load font %{public}s", familyName.c_str());
        return false;
    }
    return true;
}

napi_value JsFontCollection::OnLoadFont(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok ||
        argc < ARGC_TWO) {
        TEXT_LOGE("Failed to get argument, argc %{public}zu", argc);
        return nullptr;
    }
    if (argv[0] == nullptr) {
        TEXT_LOGE("Null argv[0]");
        return nullptr;
    }
    std::string familyName;
    std::string familySrc;
    if (!ConvertFromJsValue(env, argv[0], familyName)) {
        TEXT_LOGE("Failed to convert argv[0]");
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[1] == nullptr) {
        TEXT_LOGE("Null arv[1]");
        return nullptr;
    }
    napi_typeof(env, argv[1], &valueType);
    if (valueType != napi_object) {
        if (!ConvertFromJsValue(env, argv[1], familySrc)) {
            TEXT_LOGE("Failed to convert argv[1]");
            return nullptr;
        }
        if (!SplitAbsoluteFontPath(familySrc) || !GetFontFileProperties(familySrc, familyName)) {
            return nullptr;
        }
        return NapiGetUndefined(env);
    }

    ResourceInfo resourceInfo;
    if (!ParseResourceType(env, argv[1], resourceInfo) || !ParseResourcePath(familyName, resourceInfo)) {
        return nullptr;
    }

    return NapiGetUndefined(env);
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

        if (!ParseContextFilePath(env, argv, context)) {
            auto* fontCollection = reinterpret_cast<JsFontCollection*>(context->native);
            NAPI_CHECK_ARGS(context, fontCollection != nullptr, napi_invalid_arg,
                TextErrorCode::ERROR_INVALID_PARAM, return, "FontCollection is null");
            NAPI_CHECK_ARGS(context, fontCollection->ParseResourceType(env, argv[ARGC_ONE], context->info),
                napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return, "Parse resource error");
        }
    };

    context->GetCbInfo(env, info, inputParser);

    auto executor = [context]() {
        TEXT_ERROR_CHECK(context != nullptr, return, "Context is null");

        auto* fontCollection = reinterpret_cast<JsFontCollection*>(context->native);
        NAPI_CHECK_ARGS(context, fontCollection != nullptr, napi_generic_failure,
            TextErrorCode::ERROR_INVALID_PARAM, return, "FontCollection is null");

        NAPI_CHECK_ARGS(context, fontCollection->fontcollection_ != nullptr, napi_generic_failure,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Inner fontcollection is null");

        if (!context->filePath.empty()) {
            NAPI_CHECK_ARGS(context, fontCollection->SplitAbsoluteFontPath(context->filePath),
                napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return, "Failed to split absolute font path");

            NAPI_CHECK_ARGS(context, fontCollection->GetFontFileProperties(context->filePath,
                context->familyName), napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return,
                "Failed to get font file properties");
        } else {
            NAPI_CHECK_ARGS(context, fontCollection->ParseResourcePath(context->familyName, context->info),
                napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return,
                "Failed to execute function, path is invalid");
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

    auto executor = [context]() {
        TEXT_ERROR_CHECK(context != nullptr, return, "Context is null");

        auto* fontCollection = reinterpret_cast<JsFontCollection*>(context->native);
        NAPI_CHECK_ARGS(context, fontCollection != nullptr, napi_generic_failure, TextErrorCode::ERROR_INVALID_PARAM,
            return, "FontCollection is null");

        NAPI_CHECK_ARGS(context, fontCollection->fontcollection_ != nullptr, napi_generic_failure,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Inner fontcollection is null");
        fontCollection->fontcollection_->UnloadFont(context->familyName);
    };

    auto complete = [env](napi_value& output) {
        output = NapiGetUndefined(env);
    };
    return NapiAsyncWork::Enqueue(env, context, "OnUnloadFontAsync", executor, complete);
}

napi_value JsFontCollection::OnUnloadFont(napi_env env, napi_callback_info info)
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

    fontcollection_->UnloadFont(familyName);

    return NapiGetUndefined(env);
}

} // namespace OHOS::Rosen
