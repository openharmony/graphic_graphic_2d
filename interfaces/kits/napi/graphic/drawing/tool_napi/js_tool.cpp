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

#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
#include <regex>
#include "ability.h"
#endif

#include "js_drawing_utils.h"
#include "js_tool.h"


namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsTool::constructor_ = nullptr;
const std::string CLASS_NAME = "Tool";
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
const int32_t GLOBAL_ERROR = 10000;
// The expectLength of regex match of "rgb(255,0,0)". The elements:0 is the string, 1 is r, 2 is g, 3 is b.
constexpr uint32_t RGB_SUB_MATCH_SIZE = 4;
// The expectLength of regex match of "rgba(255,0,0,0.5)". The elements:0 is the string, 1 is r, 2 is g, 3 is b, 4 is a.
constexpr uint32_t RGBA_SUB_MATCH_SIZE = 7;
// The length of standard color, the length of str is must like "FF00FF00" which length is 8.
constexpr uint32_t COLOR_STRING_SIZE_STANDARD = 8;
constexpr uint32_t COLOR_OFFSET_GREEN = 8;
constexpr uint32_t COLOR_OFFSET_RED = 16;
// The string will be parsed in hexadecimal format during the conversion
constexpr uint32_t COLOR_STRING_FORMAT = 16;
constexpr uint32_t COLOR_OFFSET_ALPHA = 24;
constexpr uint32_t COLOR_DEFAULT_ALPHA = 0xFF000000;
const std::vector<size_t> EXPECT_MAGIC_COLOR_LENGTHS = {7, 9};
const std::vector<size_t> EXPECT_MAGIC_MINI_COLOR_LENGTHS = {4, 5};
const std::regex COLOR_WITH_RGB(R"(rgb\(([+]?[0-9]+)\,([+]?[0-9]+)\,([+]?[0-9]+)\))", std::regex::icase);
const std::regex COLOR_WITH_RGBA(R"(rgba\(([+]?[0-9]+)\,([+]?[0-9]+)\,([+]?[0-9]+)\,((0|1)(\.\d+)?)\))",
    std::regex::icase);
const std::regex HEX_PATTERN("^[0-9a-fA-F]+$");
#endif
napi_value JsTool::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("makeColorFromResourceColor", JsTool::makeColorFromResourceColor),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsTool::Init failed to define Tool class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsTool::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsTool::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsTool::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsTool::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsTool* jsTool = new JsTool();
    if (jsTool == nullptr) {
        ROSEN_LOGE("JsTool::Constructor jsTool is nullptr");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsTool, JsTool::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsTool;
        ROSEN_LOGE("JsTool::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsTool::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsTool *napi = reinterpret_cast<JsTool *>(nativeObject);
        delete napi;
    }
}

JsTool::JsTool() {}

JsTool::~JsTool() {}

napi_value JsTool::makeColorFromResourceColor(napi_env env, napi_callback_info info)
{
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_ONE);

    uint32_t colorNumber = 0;
    if (!GetResourceColor(env, argv[ARGC_ZERO], colorNumber)) {
        ROSEN_LOGE("JsTool::makeColorFromResourceColor failed to get colorNumber!");
        return nullptr;
    }

    napi_value objColor = nullptr;
    napi_create_object(env, &objColor);
    if (objColor == nullptr) {
        ROSEN_LOGE("JsTool::makeColorFromResourceColor failed to create color!");
        return nullptr;
    }

    napi_set_named_property(env, objColor, "alpha", CreateJsNumber(env, (colorNumber >> COLOR_OFFSET_ALPHA) & 0xFF));
    napi_set_named_property(env, objColor, "red", CreateJsNumber(env, (colorNumber >> COLOR_OFFSET_RED) & 0xFF));
    napi_set_named_property(env, objColor, "green", CreateJsNumber(env, (colorNumber >> COLOR_OFFSET_GREEN) & 0xFF));
    napi_set_named_property(env, objColor, "blue", CreateJsNumber(env, colorNumber & 0xFF));

    return objColor;
#else
    return nullptr;
#endif
}

#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
size_t JsTool::GetParamLen(napi_env env, napi_value param)
{
    size_t buffSize = 0;
    napi_status status = napi_get_value_string_utf8(env, param, nullptr, 0, &buffSize);
    if (status != napi_ok || buffSize == 0) {
        return 0;
    }
    return buffSize;
}

std::shared_ptr<Global::Resource::ResourceManager> JsTool::GetResourceManager()
{
#ifdef ROSEN_OHOS
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
#else
    std::shared_ptr<AbilityRuntime::Platform::ApplicationContext> context =
        AbilityRuntime::Platform::ApplicationContext::GetApplicationContext();
#endif
    if (context == nullptr) {
        ROSEN_LOGE("JsTool::Failed to get application context");
        return nullptr;
    }
    auto resourceManager = context->GetResourceManager();
    if (resourceManager == nullptr) {
        ROSEN_LOGE("JsTool::Failed to get resource manager");
        return nullptr;
    }
    return resourceManager;
}

bool JsTool::GetResourceInfo(napi_env env, napi_value value, ResourceInfo& info)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_object) {
        ROSEN_LOGE("JsTool::GetResourceInfo the value is not object!");
        return false;
    }

    napi_value idNApi = nullptr;
    napi_value typeNApi = nullptr;
    napi_value paramsNApi = nullptr;
    napi_get_named_property(env, value, "id", &idNApi);
    napi_get_named_property(env, value, "type", &typeNApi);
    napi_get_named_property(env, value, "params", &paramsNApi);

    napi_typeof(env, idNApi, &valueType);
    if (valueType != napi_number) {
        ROSEN_LOGE("JsTool::GetResourceInfo id is not number!");
        return false;
    }
    napi_get_value_int32(env, idNApi, &info.resId);

    napi_typeof(env, typeNApi, &valueType);
    if (valueType != napi_number) {
        ROSEN_LOGE("JsTool::GetResourceInfo type is not number!");
        return false;
    }
    napi_get_value_int32(env, typeNApi, &info.type);

    return GetResourceInfoParams(env, info, paramsNApi);
}

bool JsTool::GetResourceInfoParams(napi_env env, ResourceInfo& info, napi_value paramsNApi)
{
    napi_valuetype valueType = napi_undefined;
    bool isArray = false;
    if (napi_is_array(env, paramsNApi, &isArray) != napi_ok) {
        ROSEN_LOGE("JsTool::Failed to get array type");
        return false;
    }
    if (!isArray) {
        ROSEN_LOGE("JsTool::Invalid array type");
        return false;
    }

    // Here we use 'for' to get all params
    uint32_t arrayLength = 0;
    napi_get_array_length(env, paramsNApi, &arrayLength);
    for (uint32_t i = 0; i < arrayLength; i++) {
        size_t ret = 0;
        napi_value indexValue = nullptr;
        napi_get_element(env, paramsNApi, i, &indexValue);
        napi_typeof(env, indexValue, &valueType);
        if (valueType == napi_string) {
            size_t strlen = GetParamLen(env, indexValue) + 1;
            std::unique_ptr<char[]> indexStr = std::make_unique<char[]>(strlen);
            napi_get_value_string_utf8(env, indexValue, indexStr.get(), strlen, &ret);
            info.params.emplace_back(indexStr.get());
        } else if (valueType == napi_number) {
            int32_t num = 0;
            napi_get_value_int32(env, indexValue, &num);
            info.params.emplace_back(std::to_string(num));
        } else {
            ROSEN_LOGE("JsTool::Invalid value type %{public}d", valueType);
            return false;
        }
    }

    return true;
}

bool JsTool::GetResourceRawFileDataBuffer(std::unique_ptr<uint8_t[]>&& buffer, size_t& len, ResourceInfo& info)
{
    auto resourceManager = GetResourceManager();
    if (resourceManager == nullptr) {
        ROSEN_LOGE("JsTool::Failed to get resourceManager, resourceManager is nullptr");
        return false;
    }
    if (info.type != static_cast<int32_t>(ResourceType::RAWFILE)) {
        ROSEN_LOGE("JsTool::Invalid resource type %{public}d", info.type);
        return false;
    }

    int32_t state = 0;

    if (info.params.empty()) {
        ROSEN_LOGE("JsTool::Failed to get RawFile resource, RawFile is null");
        return false;
    }

    state = resourceManager->GetRawFileFromHap(info.params[0], len, buffer);
    if (state >= GLOBAL_ERROR || state < 0) {
        ROSEN_LOGE("JsTool::Failed to get Rawfile buffer");
        return false;
    }
    return true;
}

uint32_t JsTool::GetColorNumberResult(uint32_t origin)
{
    uint32_t result = origin;
    if ((origin >> COLOR_OFFSET_ALPHA) == 0) {
        result = origin | COLOR_DEFAULT_ALPHA;
    }
    return result;
}

bool JsTool::FastCheckColorType(const std::string& colorStr, const std::string& expectPrefix,
    const std::vector<size_t>& expectLengths)
{
    // Check whether the beginning of colorStr is expectPrefix
    // and whether the length of colorStr is in expectLengths.
    if (colorStr.rfind(expectPrefix, 0) != 0) {
        return false;
    }
    // if expectLengths is null that we do not check length.
    if (expectLengths.size() == 0) {
        return true;
    }
    return std::find(expectLengths.begin(), expectLengths.end(), colorStr.size()) != expectLengths.end();
}

uint32_t JsTool::HandleIncorrectColor(const std::string& newColorStr)
{
    errno = 0;
    char* end = nullptr;
    uint32_t value = strtoul(newColorStr.c_str(), &end, COLOR_STRING_FORMAT);
    if (errno == ERANGE) {
        ROSEN_LOGE("JsTool::HandleIncorrectColor %{public}s is out of range.", newColorStr.c_str());
    }
    if (value == 0 && end == newColorStr.c_str()) {
        ROSEN_LOGE("JsTool::HandleIncorrectColor input can not be converted to number, use default :0x0");
    }
    return value;
}

bool JsTool::HandleRGBValue(int value, int& result)
{
    if (value < 0) {
        return false;
    }
    result = value > Color::RGB_MAX ? Color::RGB_MAX : value;
    return true;
}

bool JsTool::IsOpacityValid(double value)
{
    return value >= 0 && value <= 1.0;
}

bool JsTool::MatchColorWithMagic(std::string& colorStr, uint32_t& result)
{
    // This function we match string like "#FF0000" or "#FF0000FF" to get hexnumber
    // Check whether the beginning of the string is "#" and whether the length of the string is vaild.
    if (!FastCheckColorType(colorStr, "#", EXPECT_MAGIC_COLOR_LENGTHS)) {
        ROSEN_LOGE("JsTool::MatchColorWithMagic colorString is invalid");
        return false;
    }
    // Remove "#"
    colorStr.erase(0, 1);

    // Check whether the remaining part of the string is hexnumber
    if (!std::regex_match(colorStr, HEX_PATTERN)) {
        ROSEN_LOGE("JsTool::MatchColorWithMagic colorString is invalid hexnub");
        return false;
    }

    result = HandleIncorrectColor(colorStr);
    // If string is "#FF0000" that has not Alpha, set Alpha is "FF"
    if (colorStr.length() < COLOR_STRING_SIZE_STANDARD) {
        result |= COLOR_DEFAULT_ALPHA;
    }
    return true;
}

bool JsTool::MatchColorWithMagicMini(std::string& colorStr, uint32_t& result)
{
    // This function we match string like "#FF0000" or "#FF0000FF" to get hexnumber
    // Check whether the beginning of the string is "#" and whether the length of the string is vaild.
    if (!FastCheckColorType(colorStr, "#", EXPECT_MAGIC_MINI_COLOR_LENGTHS)) {
        ROSEN_LOGE("JsTool::MatchColorWithMagicMini colorString is invalid");
        return false;
    }
    // Remove "#"
    colorStr.erase(0, 1);

    // Check whether the remaining part of the string is hexnumber
    if (!std::regex_match(colorStr, HEX_PATTERN)) {
        ROSEN_LOGE("JsTool::MatchColorWithMagicMini colorString is invalid hexnub");
        return false;
    }

    // "#F00" is the abbreviation of "#FF0000", every will be used twice to make origin string
    std::string newColorStr;
    for (auto& c : colorStr) {
        newColorStr += c;
        newColorStr += c;
    }
    result = HandleIncorrectColor(newColorStr);
    // If string is "#FF0000" that has not Alpha, set Alpha is "FF"
    if (newColorStr.length() < COLOR_STRING_SIZE_STANDARD) {
        result |= COLOR_DEFAULT_ALPHA;
    }
    return true;
}

bool JsTool::MatchColorWithRGB(const std::string& colorStr, uint32_t& result)
{
    // This function we match string like "rgb(255,0,0)" to get hexnumber
    // Check whether the beginning of the string is "rgb(".
    if (!FastCheckColorType(colorStr, "rgb(", {})) {
        ROSEN_LOGE("JsTool::MatchColorWithRGB colorString is invalid");
        return false;
    }

    // Using regular expressions to obtain the value of RGB
    std::smatch matches;
    if (std::regex_match(colorStr, matches, COLOR_WITH_RGB) && matches.size() == RGB_SUB_MATCH_SIZE) {
        int redInt = 0;
        int greenInt = 0;
        int blueInt = 0;
        if (!HandleRGBValue(std::stoi(matches[ARGC_ONE]), redInt) ||
            !HandleRGBValue(std::stoi(matches[ARGC_TWO]), greenInt) ||
            !HandleRGBValue(std::stoi(matches[ARGC_THREE]), blueInt)) {
            ROSEN_LOGE("JsTool::MatchColorWithRGB colorStringNub is invalid");
            return false;
        }

        auto red = static_cast<uint8_t>(redInt);
        auto green = static_cast<uint8_t>(greenInt);
        auto blue = static_cast<uint8_t>(blueInt);
        result = COLOR_DEFAULT_ALPHA | (red << COLOR_OFFSET_RED) | (green << COLOR_OFFSET_GREEN) | blue;
        return true;
    }
    return false;
}

bool JsTool::MatchColorWithRGBA(const std::string& colorStr, uint32_t& result)
{
    // This function we match string like "rgba(255,0,0,0.5)" to get hexnumber
    // Check whether the beginning of the string is "rgba(".
    if (!FastCheckColorType(colorStr, "rgba(", {})) {
        ROSEN_LOGE("JsTool::MatchColorWithRGBA colorString is invalid");
        return false;
    }

    // Using regular expressions to obtain the value of RGBA
    std::smatch matches;
    if (std::regex_match(colorStr, matches, COLOR_WITH_RGBA) && matches.size() == RGBA_SUB_MATCH_SIZE) {
        int redInt = 0;
        int greenInt = 0;
        int blueInt = 0;
        auto opacityDouble = std::stod(matches[ARGC_FOUR]);
        if (!HandleRGBValue(std::stoi(matches[ARGC_ONE]), redInt) ||
            !HandleRGBValue(std::stoi(matches[ARGC_TWO]), greenInt) ||
            !HandleRGBValue(std::stoi(matches[ARGC_THREE]), blueInt) ||
            !IsOpacityValid(opacityDouble)) {
            ROSEN_LOGE("JsTool::MatchColorWithRGBA colorStringNub is invalid");
            return false;
        }

        auto red = static_cast<uint8_t>(redInt);
        auto green = static_cast<uint8_t>(greenInt);
        auto blue = static_cast<uint8_t>(blueInt);
        uint8_t alpha = static_cast<uint8_t>(round(static_cast<double>(opacityDouble) * 0xff));
        result = (alpha << COLOR_OFFSET_ALPHA) | (red << COLOR_OFFSET_RED) | (green << COLOR_OFFSET_GREEN) | blue;
        return true;
    }
    return false;
}

bool JsTool::GetColorStringResult(std::string colorStr, uint32_t& result)
{
    if (colorStr.empty()) {
        ROSEN_LOGE("JsTool::GetColorStringResult string is empty");
        return false;
    }

    // e.g: Remove ' ' that change "rgb(255, 0, 0)" to "rgb(255,0,0)"
    colorStr.erase(std::remove(colorStr.begin(), colorStr.end(), ' '), colorStr.end());

    return (MatchColorWithMagic(colorStr, result) || MatchColorWithMagicMini(colorStr, result) ||
            MatchColorWithRGB(colorStr, result) || MatchColorWithRGBA(colorStr, result));
}

bool JsTool::GetColorObjectResult(napi_env env, napi_value value, uint32_t& result)
{
    ResourceInfo info;
    if (!GetResourceInfo(env, value, info)) {
        ROSEN_LOGE("JsTool::GetColorObjectResult GetResourceInfo failed!");
        return false;
    }

    auto resourceManager = GetResourceManager();
    if (resourceManager == nullptr) {
        ROSEN_LOGE("JsTool::GetColorObjectResult resourceManager is nullptr!");
        return false;
    }

    if (info.type == static_cast<int32_t>(ResourceType::STRING)) {
        std::string colorStr = "";
        int32_t state = resourceManager->GetStringById(static_cast<uint32_t>(info.resId), colorStr);
        if (state >= GLOBAL_ERROR || state < 0) {
            ROSEN_LOGE("JsTool::GetColorObjectResult failed to get colorString!");
            return false;
        }
        if (!GetColorStringResult(colorStr, result)) {
            ROSEN_LOGE("JsTool::GetColorObjectResult failed to GetColorStringResult!");
            return false;
        }
    } else if (info.type == static_cast<int32_t>(ResourceType::INTEGER)) {
        int colorResult = 0;
        int32_t state = resourceManager->GetIntegerById(static_cast<uint32_t>(info.resId), colorResult);
        if (state >= GLOBAL_ERROR || state < 0) {
            ROSEN_LOGE("JsTool::GetColorObjectResult failed to get colorInt!");
            return false;
        }
        result = GetColorNumberResult(colorResult);
    } else if (info.type == static_cast<int32_t>(ResourceType::COLOR)) {
        int32_t state = resourceManager->GetColorById(static_cast<uint32_t>(info.resId), result);
        if (state >= GLOBAL_ERROR || state < 0) {
            ROSEN_LOGE("JsTool::GetColorObjectResult failed to get colorColor!");
            return false;
        }
    } else {
        ROSEN_LOGE("JsTool::GetColorObjectResult invalid Resource type!");
        return false;
    }
    return true;
}

bool JsTool::GetResourceColor(napi_env env, napi_value res, uint32_t& result)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, res, &valueType);
    if (valueType == napi_string) {
        size_t len = 0;
        napi_get_value_string_utf8(env, res, nullptr, 0, &len);
        char* str = new(std::nothrow) char[len + 1];
        if (!str) {
            ROSEN_LOGE("JsTool::GetResourceColor memory is insufficient and failed to apply");
            return false;
        }
        napi_get_value_string_utf8(env, res, str, len + 1, &len);
        std::string colorStr(str, len);
        delete[] str;
        if (!GetColorStringResult(colorStr, result)) {
            ROSEN_LOGE("JsTool::GetResourceColor failed to GetColorStringResult!");
            return false;
        }
    } else if (valueType == napi_number) {
        uint32_t colorNumber = 0;
        napi_get_value_uint32(env, res, &colorNumber);
        result = GetColorNumberResult(colorNumber);
    } else if (valueType == napi_object) {
        if (!GetColorObjectResult(env, res, result)) {
            ROSEN_LOGE("JsTool::GetResourceColor failed to GetColorObjectResult!");
            return false;
        }
    } else {
        ROSEN_LOGE("JsTool::GetResourceColor invalid ResourceColor type!");
        return false;
    }
    return true;
}
#endif
} // namespace Drawing
} // namespace OHOS::Rosen