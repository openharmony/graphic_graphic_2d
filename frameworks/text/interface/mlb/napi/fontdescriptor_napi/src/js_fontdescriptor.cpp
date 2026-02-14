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

#include <functional>
#include <variant>
#include <vector>
#include "js_fontdescriptor.h"

#include "font_descriptor_mgr.h"
#include "napi_async_work.h"
#include "napi_common.h"
#include "utils/include/font_utils.h"

namespace OHOS::Rosen {
namespace {
using FontDescriptorProperty = std::variant<
    std::reference_wrapper<int>,
    std::reference_wrapper<bool>,
    std::reference_wrapper<std::string>>;
using FontDescriptorPropertyList = std::vector<std::pair<const char*, FontDescriptorProperty>>;
FontDescriptorPropertyList GenerateDescriptorPropList(FontDescSharedPtr fd)
{
    if (fd == nullptr) {
        return {};
    }
    FontDescriptorPropertyList propList = {
        {"path", std::ref(fd->path)},
        {"postScriptName", std::ref(fd->postScriptName)},
        {"fullName", std::ref(fd->fullName)},
        {"fontFamily", std::ref(fd->fontFamily)},
        {"fontSubfamily", std::ref(fd->fontSubfamily)},
        {"width", std::ref(fd->width)},
        {"italic", std::ref(fd->italic)},
        {"monoSpace", std::ref(fd->monoSpace)},
        {"symbolic", std::ref(fd->symbolic)},
        {"localPostscriptName", std::ref(fd->localPostscriptName)},
        {"localFullName", std::ref(fd->localFullName)},
        {"localFamilyName", std::ref(fd->localFamilyName)},
        {"localSubFamilyName", std::ref(fd->localSubFamilyName)},
        {"version", std::ref(fd->version)},
        {"manufacture", std::ref(fd->manufacture)},
        {"copyright", std::ref(fd->copyright)},
        {"trademark", std::ref(fd->trademark)},
        {"license", std::ref(fd->license)},
        {"index", std::ref(fd->index)},
    };
    return propList;
}
struct FontDescriptorsListContext : public FontPathResourceContext {
    std::vector<FontDescSharedPtr> fontDescriptersOutput;
};

struct FontUnicodeSetListContext : public FontPathResourceContext {
    int32_t index{0};
    std::vector<uint32_t> unicodeSetOutput;
};

bool ProcessFontPath(sptr<FontDescriptorsListContext> context, std::string& path)
{
    if (SplitAbsolutePath(path)) {
        context->fontDescriptersOutput = TextEngine::FontParser::ParserFontDescriptorsFromPath(path);
        return true;
    }
    return false;
}
}

napi_value JsFontDescriptor::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("matchFontDescriptors", JsFontDescriptor::MatchFontDescriptorsAsync),
        DECLARE_NAPI_STATIC_FUNCTION("getSystemFontFullNamesByType", JsFontDescriptor::GetSystemFontFullNamesByType),
        DECLARE_NAPI_STATIC_FUNCTION("getFontDescriptorByFullName", JsFontDescriptor::GetFontDescriptorByFullName),
        DECLARE_NAPI_STATIC_FUNCTION("getFontDescriptorsFromPath", JsFontDescriptor::GetFontDescriptorsFromPath),
        DECLARE_NAPI_STATIC_FUNCTION("getFontUnicodeSet", JsFontDescriptor::GetFontUnicodeSet),
        DECLARE_NAPI_STATIC_FUNCTION("getFontCount", JsFontDescriptor::GetFontCount),
        DECLARE_NAPI_STATIC_FUNCTION("getFontPathsByType", JsFontDescriptor::GetFontPathsByType),
        DECLARE_NAPI_STATIC_FUNCTION("isFontSupported", JsFontDescriptor::IsFontSupported),
    };
    
    NAPI_CHECK_AND_THROW_ERROR(
        napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties) == napi_ok,
        TextErrorCode::ERROR_INVALID_PARAM, "Failed to define properties");
    return exportObj;
}

JsFontDescriptor::JsFontDescriptor() {}

template <typename T>
bool JsFontDescriptor::CheckAndConvertProperty(napi_env env, napi_value obj, const std::string& fieldName, T& out)
{
    bool hasValue = false;
    TEXT_ERROR_CHECK(napi_has_named_property(env, obj, fieldName.c_str(), &hasValue) == napi_ok, return false,
        "Failed to judge the existence of field %{public}s", fieldName.c_str());
    if (hasValue) {
        napi_value napiVal = nullptr;
        TEXT_ERROR_CHECK(napi_get_named_property(env, obj, fieldName.c_str(), &napiVal) == napi_ok, return false,
            "Failed to get the value of field %{public}s", fieldName.c_str());
        return ConvertFromJsValue(env, napiVal, out);
    }
    return true;
}

bool JsFontDescriptor::ParseFontDescWeight(napi_env env, napi_value obj, int& weight)
{
    bool hasValue = false;
    TEXT_ERROR_CHECK(napi_has_named_property(env, obj, "weight", &hasValue) == napi_ok, return false,
        "Failed to judge the existence of field weight");
    if (hasValue) {
        napi_value napiVal = nullptr;
        TEXT_ERROR_CHECK(napi_get_named_property(env, obj, "weight", &napiVal) == napi_ok, return false,
            "Failed to get the value of field weight");
        int weightEnum = 0;
        if (!ConvertFromJsValue(env, napiVal, weightEnum)) {
            return false;
        }
        int result = 0;
        if (OHOS::MLB::FindFontWeightEnum(weightEnum, result)) {
            weight = result;
            return true;
        }
        return false;
    }
    return true;
}

napi_value JsFontDescriptor::MatchFontDescriptorsAsync(napi_env env, napi_callback_info info)
{
    struct MatchFontDescriptorsContext : public ContextBase {
        FontDescSharedPtr matchDesc = nullptr;
        std::set<FontDescSharedPtr> matchResult;
    };
    sptr<MatchFontDescriptorsContext> cb = sptr<MatchFontDescriptorsContext>::MakeSptr();
    NAPI_CHECK_AND_THROW_ERROR(cb != nullptr, TextErrorCode::ERROR_NO_MEMORY, "Failed to make context");
    auto inputParser = [env, cb](size_t argc, napi_value *argv) {
        cb->status = napi_invalid_arg;
        cb->errCode = static_cast<int32_t>(TextErrorCode::ERROR_INVALID_PARAM);
        cb->errMessage = "Parameter error";
        TEXT_ERROR_CHECK(argc == ARGC_ONE, return, "The number of parameters is not equal to 1");
        napi_valuetype valueType = napi_undefined;
        TEXT_ERROR_CHECK(napi_typeof(env, argv[0], &valueType) == napi_ok, return, "Failed to get type");
        TEXT_ERROR_CHECK(valueType == napi_object, return, "Parameter type is not object");

        cb->matchDesc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
        auto properties = GenerateDescriptorPropList(cb->matchDesc);
        for (auto& item : properties) {
            TEXT_CHECK(std::visit([&](auto& p) -> bool {
                TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], item.first, p.get()), return false,
                    "Failed to convert %{public}s", item.first);
                return true;
            }, item.second), return);
        }
        TEXT_ERROR_CHECK(ParseFontDescWeight(env, argv[0], cb->matchDesc->weight), return, "Failed to parse weight");
        cb->status = napi_ok;
        cb->errCode = static_cast<int32_t>(TextErrorCode::OK);
        cb->errMessage = "";
    };
    cb->GetCbInfo(env, info, inputParser);

    auto executor = [cb]() {
        FontDescriptorMgrInstance.MatchFontDescriptors(cb->matchDesc, cb->matchResult);
    };
    auto complete = [env, cb](napi_value& output) {
        output = JsFontDescriptor::CreateFontDescriptorArray(env, cb->matchResult);
    };
    return NapiAsyncWork::Enqueue(env, cb, "MatchFontDescriptors", executor, complete).result;
}

bool JsFontDescriptor::SetProperty(napi_env env, napi_value object, const char* name, napi_value value)
{
    TEXT_ERROR_CHECK(napi_set_named_property(env, object, name, value) == napi_ok, return false,
        "Failed to set %{public}s", name);
    return true;
}

bool JsFontDescriptor::ConvertFontDescWeight(napi_env env, napi_value obj, int weight)
{
    int result = -1;
    if (!OHOS::MLB::FindFontWeight(OHOS::MLB::RegularWeight(weight), result)) {
        return false;
    }
    TEXT_CHECK_RETURN_VALUE(SetProperty(env, obj, "weight", CreateJsValue(env, result)), false);
    return true;
}

bool JsFontDescriptor::CreateAndSetProperties(napi_env env, napi_value fontDescriptor, FontDescSharedPtr item)
{
    TEXT_ERROR_CHECK(item != nullptr, return false, "FontDescSharedPtr is nullptr");
    auto properties = GenerateDescriptorPropList(item);
    for (const auto& prop : properties) {
        TEXT_CHECK(std::visit([&](auto& p) -> bool {
            return SetProperty(env, fontDescriptor, prop.first, CreateJsValue(env, p.get()));
        }, prop.second), return false);
    }

    if (!item->variationAxisRecords.empty()) {
        napi_value axisArray = CreateVariationAxisArray(env, item->variationAxisRecords);
        TEXT_CHECK(axisArray != nullptr, return false);
        TEXT_CHECK(SetProperty(env, fontDescriptor, "variationAxisRecords", axisArray), return false);
    }

    if (!item->variationInstanceRecords.empty()) {
        napi_value instanceArray = CreateVariationInstanceArray(env, item->variationInstanceRecords);
        TEXT_CHECK(instanceArray != nullptr, return false);
        TEXT_CHECK(SetProperty(env, fontDescriptor, "variationInstanceRecords", instanceArray), return false);
    }

    return true;
}

napi_value JsFontDescriptor::CreateVariationAxisArray(
    napi_env env, const std::vector<TextEngine::FontParser::FontVariationAxis>& axes)
{
    napi_value axisArray = nullptr;
    TEXT_ERROR_CHECK(napi_create_array(env, &axisArray) == napi_ok, return nullptr,
        "Failed to create variation axis array");
    uint32_t index = 0;
    for (const auto& axis : axes) {
        napi_value axisObj = nullptr;
        TEXT_ERROR_CHECK(napi_create_object(env, &axisObj) == napi_ok, return nullptr,
            "Failed to create axis object");
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, axisObj, "key", CreateJsValue(env, axis.key)), nullptr);
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, axisObj, "minValue", CreateJsValue(env, axis.minValue)), nullptr);
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, axisObj, "maxValue", CreateJsValue(env, axis.maxValue)), nullptr);
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, axisObj, "defaultValue",
            CreateJsValue(env, axis.defaultValue)), nullptr);
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, axisObj, "flags", CreateJsValue(env, axis.flags)), nullptr);
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, axisObj, "name", CreateJsValue(env, axis.name)), nullptr);
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, axisObj, "localName", CreateJsValue(env, axis.localName)), nullptr);
        TEXT_ERROR_CHECK(napi_set_element(env, axisArray, index++, axisObj) == napi_ok, return nullptr,
            "Failed to set axis element");
    }
    return axisArray;
}

napi_value JsFontDescriptor::CreateVariationInstanceArray(
    napi_env env, const std::vector<TextEngine::FontParser::FontVariationInstance>& instances)
{
    napi_value instanceArray = nullptr;
    TEXT_ERROR_CHECK(napi_create_array(env, &instanceArray) == napi_ok, return nullptr,
        "Failed to create variation instance array");
    uint32_t index = 0;
    for (const auto& instance : instances) {
        napi_value instanceObj = nullptr;
        TEXT_ERROR_CHECK(napi_create_object(env, &instanceObj) == napi_ok, return nullptr,
            "Failed to create instance object");
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, instanceObj, "name", CreateJsValue(env, instance.name)), nullptr);
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, instanceObj, "localName",
            CreateJsValue(env, instance.localName)), nullptr);

        napi_value coordArray = nullptr;
        TEXT_ERROR_CHECK(napi_create_array(env, &coordArray) == napi_ok, return nullptr,
            "Failed to create coordinates array");
        uint32_t coordIndex = 0;
        for (const auto& coord : instance.coordinates) {
            napi_value coordObj = nullptr;
            TEXT_ERROR_CHECK(napi_create_object(env, &coordObj) == napi_ok, return nullptr,
                "Failed to create coordinate object");
            TEXT_CHECK_RETURN_VALUE(SetProperty(env, coordObj, "axis", CreateJsValue(env, coord.axis)), nullptr);
            TEXT_CHECK_RETURN_VALUE(SetProperty(env, coordObj, "value", CreateJsValue(env, coord.value)), nullptr);
            TEXT_ERROR_CHECK(napi_set_element(env, coordArray, coordIndex++, coordObj) == napi_ok, return nullptr,
                "Failed to set coordinate element");
        }
        TEXT_CHECK_RETURN_VALUE(SetProperty(env, instanceObj, "coordinates", coordArray), nullptr);

        TEXT_ERROR_CHECK(napi_set_element(env, instanceArray, index++, instanceObj) == napi_ok, return nullptr,
            "Failed to set instance element");
    }
    return instanceArray;
}

napi_value JsFontDescriptor::CreateFontDescriptorArray(napi_env env, const std::set<FontDescSharedPtr>& result)
{
    TEXT_ERROR_CHECK(env != nullptr, return nullptr, "env is nullptr");
    napi_value descArray = nullptr;
    TEXT_ERROR_CHECK(napi_create_array(env, &descArray) == napi_ok, return nullptr, "Failed to create array");
    uint32_t index = 0;
    for (const auto& item : result) {
        napi_value fontDescriptor = nullptr;
        TEXT_ERROR_CHECK(napi_create_object(env, &fontDescriptor) == napi_ok, return nullptr,
            "Failed to create object");
        TEXT_ERROR_CHECK(CreateAndSetProperties(env, fontDescriptor, item), return nullptr,
            "Failed to set font descriptor");
        TEXT_ERROR_CHECK(ConvertFontDescWeight(env, fontDescriptor, item->weight), return nullptr,
            "Failed to convert weight");
        TEXT_ERROR_CHECK(napi_set_element(env, descArray, index++, fontDescriptor) == napi_ok, return nullptr,
            "Failed to set element");
    }
    return descArray;
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

    return NapiAsyncWork::Enqueue(env, context, "GetSystemFontFullNamesByType", executor, complete).result;
}

napi_value JsFontDescriptor::CreateFontList(napi_env env, const std::unordered_set<std::string>& fontList)
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

napi_value JsFontDescriptor::GetFontDescriptorsFromPath(napi_env env, napi_callback_info info)
{
    sptr<FontDescriptorsListContext> context = sptr<FontDescriptorsListContext>::MakeSptr();
    NAPI_CHECK_AND_THROW_ERROR(context != nullptr, TextErrorCode::ERROR_NO_MEMORY,
        "Failed to get font descriptors, no memory");

    auto inputParser = [env, context](size_t argc, napi_value* argv) {
        TEXT_ERROR_CHECK(argv != nullptr, return, "Argv is nullptr");
        TEXT_ERROR_CHECK(context->status == napi_ok, return, "Status error, status=%{public}d",
            static_cast<int>(context->status));
        TEXT_ERROR_CHECK(argc == ARGC_ONE, return, "Argc is invalid %{public}zu", argc);
        constexpr size_t argvPathNum = 0;
        if (!ParseContextFilePath(env, argv, context, argvPathNum)) {
            TEXT_ERROR_CHECK(ParseResourceType(env, argv[argvPathNum], context->info), return, "Parse resource error");
        }
    };

    context->GetCbInfo(env, info, inputParser);

    auto executor = [context]() {
        TEXT_ERROR_CHECK(context != nullptr, return, "Context is null");
        if (context->filePath.has_value() && !context->filePath->empty()) {
            TEXT_ERROR_CHECK(SplitAbsolutePath(*context->filePath), return, "Failed to split absolute font path");
            context->fontDescriptersOutput = TextEngine::FontParser::ParserFontDescriptorsFromPath(*context->filePath);
        } else {
            auto pathCB = [context](std::string& path) -> bool { return ProcessFontPath(context, path); };
            auto fileCB = [context](const void* data, size_t size) -> bool {
                context->fontDescriptersOutput = TextEngine::FontParser::ParserFontDescriptorsFromStream(data, size);
                return true;
            };
            TEXT_ERROR_CHECK(ProcessResource(context->info, pathCB, fileCB).success, return,
                "Failed to execute function, path is invalid");
        }
    };
    auto complete = [env, context](napi_value& output) {
        std::set<FontDescSharedPtr> fontDescripters(
            context->fontDescriptersOutput.begin(), context->fontDescriptersOutput.end());
        output = JsFontDescriptor::CreateFontDescriptorArray(env, fontDescripters);
    };

    return NapiAsyncWork::Enqueue(env, context, "GetFontDescriptorsFromPath", executor, complete).result;
}

void GetFontUnicodeSetParamFromJs(napi_env env, sptr<FontUnicodeSetListContext> context,
    size_t argc, napi_value* argv)
{
    TEXT_ERROR_CHECK(context != nullptr, return, "Fail to get parser, Context is null");
    TEXT_ERROR_CHECK(argv != nullptr, return, "Argv is nullptr");
    NAPI_CHECK_ARGS(context, argc == ARGC_TWO, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return,
        "Argc is invalid %{public}zu", argc);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[ARGC_ZERO], &valueType);
    bool typeIsValid = (valueType == napi_object || valueType == napi_string);
    NAPI_CHECK_ARGS(context, typeIsValid, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM,
        return, "Fail to convert path");
    TEXT_ERROR_CHECK(context->status == napi_ok, return, "Status error, status=%{public}d",
        static_cast<int>(context->status));
    if (!ParseContextFilePath(env, argv, context, ARGC_ZERO)) {
        TEXT_ERROR_CHECK(ParseResourceType(env, argv[ARGC_ZERO], context->info), return, "Parse resource error");
    }
    NAPI_CHECK_ARGS(context, ConvertFromJsValue(env, argv[ARGC_ONE], context->index), napi_invalid_arg,
        TextErrorCode::ERROR_INVALID_PARAM, return, "Fail to convert index");
}

napi_value JsFontDescriptor::GetFontUnicodeSet(napi_env env, napi_callback_info info)
{
    sptr<FontUnicodeSetListContext> context = sptr<FontUnicodeSetListContext>::MakeSptr();
    auto inputParser = [env, context](size_t argc, napi_value* argv) {
        GetFontUnicodeSetParamFromJs(env, context, argc, argv);
    };

    context->GetCbInfo(env, info, inputParser);

    auto executor = [context]() {
        TEXT_ERROR_CHECK(context != nullptr, return, "Failed to executor, Context is null");
        if (context->filePath.has_value()) {
            TEXT_ERROR_CHECK(SplitAbsolutePath(*context->filePath), return, "Failed to split absolute font path");
            context->unicodeSetOutput =
                TextEngine::FontParser::GetFontTypefaceUnicode(*context->filePath, context->index);
        } else {
            auto pathCB = [context](std::string& path) -> bool {
                TEXT_ERROR_CHECK(SplitAbsolutePath(path), return false, "Failed to split absolute font path");
                context->unicodeSetOutput = TextEngine::FontParser::GetFontTypefaceUnicode(path, context->index);
                return true;
            };
            auto fileCB = [context](const void* data, size_t size) -> bool {
                context->unicodeSetOutput = TextEngine::FontParser::GetFontTypefaceUnicode(data, size, context->index);
                return true;
            };
            TEXT_ERROR_CHECK(ProcessResource(context->info, pathCB, fileCB).success, return,
                "Failed to execute function, path is invalid");
        }
    };
    auto complete = [env, context](napi_value& output) {
        napi_value unicodeArray = nullptr;
        TEXT_ERROR_CHECK(napi_create_array(env, &unicodeArray) == napi_ok, return, "Failed to create array");
        uint32_t index = 0;
        for (const auto& item: context->unicodeSetOutput) {
            napi_value element = CreateJsValue(env, item);
            napi_set_element(env, unicodeArray, index, element);
            index++;
        }
        output = unicodeArray;
    };

    return NapiAsyncWork::Enqueue(env, context, "GetFontUnicodeSet", executor, complete).result;
}

napi_value JsFontDescriptor::GetFontCount(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get argument, argc %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::string familySrc;
    if (ConvertFromJsValue(env, argv[ARGC_ZERO], familySrc) && SplitAbsolutePath(familySrc)) {
        return CreateJsNumber(env, TextEngine::FontParser::GetFontCount(familySrc));
    }
    ResourceInfo resourceInfo;
    int32_t fileCount = 0;
    if (ParseResourceType(env, argv[ARGC_ZERO], resourceInfo)) {
        auto pathCB = [&fileCount](std::string& path) -> bool {
            fileCount = TextEngine::FontParser::GetFontCount(path);
            return true;
        };
        auto fileCB = [&fileCount](const void* data, size_t size) -> bool {
            const uint8_t* byteData = static_cast<const uint8_t*>(data);
            std::vector<uint8_t> fontData(byteData, byteData + size);
            fileCount = TextEngine::FontParser::GetFontCount(fontData);
            return true;
        };
        ProcessResource(resourceInfo, pathCB, fileCB);
        return CreateJsNumber(env, fileCount);
    }
    return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid argument");
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

    return NapiAsyncWork::Enqueue(env, context, "GetFontDescriptorByFullName", executor, complete).result;
}

napi_value JsFontDescriptor::GetFontPathsByType(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = { nullptr };
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok || argc != ARGC_ONE ||
        argv[0] == nullptr) {
        TEXT_LOGE("Failed to get argument, argc %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid argument");
    }
    TextEngine::FontParser::SystemFontType fontType;

    if (!ConvertFromJsValue(env, argv[0], fontType)) {
        TEXT_LOGE("Failed to convert argument to SystemFontType");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid argument");
    }

    std::unordered_set<std::string> fontPaths;
    FontDescriptorMgrInstance.GetFontPathsByType(fontType, fontPaths);

    return CreateFontList(env, fontPaths);
}

napi_value JsFontDescriptor::IsFontSupported(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = { nullptr };
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok || argc != ARGC_ONE ||
        argv[0] == nullptr) {
        TEXT_LOGE("Failed to get argument, argc %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid argument");
    }
    std::string fontPath;

    auto checkFilePath = [](std::string& path) {
        return SplitAbsolutePath(path) && Drawing::Typeface::MakeFromFile(path.c_str()) != nullptr;
    };
    if (ConvertFromJsValue(env, argv[0], fontPath)) {
        return CreateJsValue(env, checkFilePath(fontPath));
    }

    ResourceInfo resourceInfo;
    if (ParseResourceType(env, argv[0], resourceInfo)) {
        auto checkFileStream = [](const void* data, size_t size) {
            auto stream = std::make_unique<Drawing::MemoryStream>(data, size);
            return Drawing::Typeface::MakeFromStream(std::move(stream)) != nullptr;
        };

        bool ok = ProcessResource(resourceInfo, checkFilePath, checkFileStream).success;
        return CreateJsValue(env, ok);
    }

    return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid argument");
}
}