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

#include "js_fontdescriptor.h"

#include "font_descriptor_mgr.h"
#include "napi_async_work.h"
#include "napi_common.h"

namespace OHOS::Rosen {
namespace {
std::unordered_map<int, int> g_weightMap = {
    {100, static_cast<int>(FontWeight::W100)},
    {200, static_cast<int>(FontWeight::W200)},
    {300, static_cast<int>(FontWeight::W300)},
    {400, static_cast<int>(FontWeight::W400)},
    {500, static_cast<int>(FontWeight::W500)},
    {600, static_cast<int>(FontWeight::W600)},
    {700, static_cast<int>(FontWeight::W700)},
    {800, static_cast<int>(FontWeight::W800)},
    {900, static_cast<int>(FontWeight::W900)}
};
}

napi_value JsFontDescriptor::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getSystemFontFullNamesByType", JsFontDescriptor::GetSystemFontFullNamesByType),
        DECLARE_NAPI_STATIC_FUNCTION("getFontDescriptorByFullName", JsFontDescriptor::GetFontDescriptorByFullName),
    };
    
    NAPI_CHECK_AND_THROW_ERROR(
        napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties) == napi_ok,
        TextErrorCode::ERROR_INVALID_PARAM, "Failed to define properties");
    return exportObj;
}

JsFontDescriptor::JsFontDescriptor() {}


bool JsFontDescriptor::SetProperty(napi_env env, napi_value object, const char* name, napi_value value)
{
    TEXT_ERROR_CHECK(napi_set_named_property(env, object, name, value) == napi_ok, return false,
        "Failed to set %{public}s", name);
    return true;
}

bool JsFontDescriptor::ConvertFontDescWeight(napi_env env, napi_value obj, int weight)
{
    auto iter = g_weightMap.find(weight);
    if (iter == g_weightMap.end()) {
        return false;
    }
    TEXT_CHECK_RETURN_VALUE(SetProperty(env, obj, "weight", CreateJsValue(env, iter->second)), false);
    return true;
}

bool JsFontDescriptor::CreateAndSetProperties(napi_env env, napi_value fontDescriptor, FontDescSharedPtr item)
{
    std::vector<std::pair<const char*, std::variant<int, double, bool, std::string>>> properties = {
        {"path", item->path},
        {"postScriptName", item->postScriptName},
        {"fullName", item->fullName},
        {"fontFamily", item->fontFamily},
        {"fontSubfamily", item->fontSubfamily},
        {"width", item->width},
        {"italic", item->italic},
        {"monoSpace", item->monoSpace},
        {"symbolic", item->symbolic},
    };

    for (const auto& prop : properties) {
        TEXT_CHECK(std::visit([&](auto& p) -> bool {
            TEXT_CHECK(SetProperty(env, fontDescriptor, prop.first, CreateJsValue(env, p)), return false);
            return true;
        }, prop.second), return false);
    }
    return true;
}

napi_value JsFontDescriptor::CreateFontDescriptor(napi_env env, FontDescSharedPtr& result)
{
    TEXT_ERROR_CHECK(env != nullptr, return nullptr, "Env is nullptr");
    TEXT_ERROR_CHECK(result != nullptr, return nullptr, "FontDescSharedPtr is nullptr");
    napi_value fontDescriptor = nullptr;
    TEXT_ERROR_CHECK(napi_create_object(env, &fontDescriptor) == napi_ok, return nullptr,
        "Failed to create fontDescriptor");
    TEXT_CHECK(CreateAndSetProperties(env, fontDescriptor, result), return nullptr);
    TEXT_CHECK(ConvertFontDescWeight(env, fontDescriptor, result->weight), return nullptr);
    return fontDescriptor;
}

napi_value JsFontDescriptor::GetSystemFontFullNamesByType(napi_env env, napi_callback_info info)
{
    struct SystemFontListContext : public ContextBase {
        TextEngine::FontParser::SystemFontType systemFontType;
        std::unordered_set<std::string> fontList;
    };

    sptr<SystemFontListContext> context = sptr<SystemFontListContext>::MakeSptr();

    NAPI_CHECK_AND_THROW_ERROR(context != nullptr, TextErrorCode::ERROR_NO_MEMORY,
        "Failed to get systemFont fullName list by type, no memory");

    auto inputParser = [env, context](size_t argc, napi_value *argv) {
        TEXT_ERROR_CHECK(argv != nullptr, return, "Argv is nullptr");
        NAPI_CHECK_ARGS(context, context->status == napi_ok, napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Status error, status=%d", static_cast<int>(context->status));
        NAPI_CHECK_ARGS(context, argc == ARGC_ONE, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM,
            return, "Argc is invalid %zu", argc);
        NAPI_CHECK_ARGS(context, ConvertFromJsValue(env, argv[0], context->systemFontType), napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Parameter systemFontType is invalid");
    };

    context->GetCbInfo(env, info, inputParser);

    auto executor = [context]() {
        FontDescriptorMgrInstance.GetSystemFontFullNamesByType(context->systemFontType, context->fontList);
    };

    auto complete = [env, context](napi_value& output) {
        output = JsFontDescriptor::CreateFontList(env, context->fontList);
    };

    return NapiAsyncWork::Enqueue(env, context, "GetSystemFontFullNamesByType", executor, complete);
}

napi_value JsFontDescriptor::CreateFontList(napi_env env, std::unordered_set<std::string>& fontList)
{
    TEXT_ERROR_CHECK(env != nullptr, return nullptr, "Env is nullptr");
    napi_value fullNameArray = nullptr;
    TEXT_ERROR_CHECK(napi_create_array(env, &fullNameArray) == napi_ok, return nullptr,
        "Failed to create fontList");
    uint32_t index = 0;
    for (const auto& item : fontList) {
        napi_value fullName = nullptr;
        TEXT_ERROR_CHECK(napi_create_string_utf8(env, item.c_str(), NAPI_AUTO_LENGTH, &fullName) == napi_ok,
            return nullptr, "Failed to create utf8 to string");
        TEXT_ERROR_CHECK(napi_set_element(env, fullNameArray, index++, fullName) == napi_ok, return nullptr,
            "Failed to set element");
    }
    return fullNameArray;
}

napi_value JsFontDescriptor::GetFontDescriptorByFullName(napi_env env, napi_callback_info info)
{
    struct FontDescriptorContext : public ContextBase {
        std::string fullName;
        TextEngine::FontParser::SystemFontType systemFontType;
        FontDescSharedPtr resultDesc = nullptr;
    };

    sptr<FontDescriptorContext> context = sptr<FontDescriptorContext>::MakeSptr();

    NAPI_CHECK_AND_THROW_ERROR(context != nullptr, TextErrorCode::ERROR_NO_MEMORY,
        "Failed to get fontDescriptor by name, no memory");

    auto inputParser = [env, context](size_t argc, napi_value *argv) {
        TEXT_ERROR_CHECK(argv != nullptr, return, "Argv is nullptr");
        NAPI_CHECK_ARGS(context, context->status == napi_ok, napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Status error, status=%d", static_cast<int>(context->status));
        NAPI_CHECK_ARGS(context, argc == ARGC_TWO, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM,
            return, "Argc is invalid %zu", argc);
        NAPI_CHECK_ARGS(context, ConvertFromJsValue(env, argv[0], context->fullName), napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Parameter fullName is invalid");
        NAPI_CHECK_ARGS(context, ConvertFromJsValue(env, argv[1], context->systemFontType), napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Parameter systemFontType is invalid");
    };

    context->GetCbInfo(env, info, inputParser);

    auto executor = [context]() {
        FontDescriptorMgrInstance.GetFontDescSharedPtrByFullName(
            context->fullName, context->systemFontType, context->resultDesc);
    };

    auto complete = [env, context](napi_value& output) {
        output = JsFontDescriptor::CreateFontDescriptor(env, context->resultDesc);
    };

    return NapiAsyncWork::Enqueue(env, context, "GetFontDescriptorByFullName", executor, complete);
}
}