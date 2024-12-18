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

#ifdef ROSEN_OHOS
#include <regex>
#include "ability.h"
#endif

#include "js_drawing_utils.h"
#include "js_tool.h"


namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsTool::constructor_ = nullptr;
const std::string CLASS_NAME = "Tool";
#ifdef ROSEN_OHOS
const int32_t GLOBAL_ERROR = 10000;
constexpr uint32_t RGB_SUB_MATCH_SIZE = 4;
constexpr uint32_t RGBA_SUB_MATCH_SIZE = 5;
constexpr uint32_t COLOR_STRING_SIZE_STANDARD = 8;
constexpr uint32_t COLOR_OFFSET_GREEN = 8;
constexpr uint32_t COLOR_OFFSET_RED = 16;
constexpr uint32_t COLOR_STRING_BASE = 16;
constexpr uint32_t COLOR_OFFSET_ALPHA = 24;
constexpr uint32_t COLOR_ALPHA_VALUE = 0xFF000000;
constexpr uint32_t COLOR_ALPHA_MASK = 0xFF000000;
const std::vector<size_t> EXPECT_MAGIC_COLOR_LENGTHS = {7, 9};
const std::vector<size_t> EXPECT_MAGIC_MINI_COLOR_LENGTHS = {4, 5};
const std::regex COLOR_WITH_RGB(R"(rgb\(([+]?[0-9]+)\,([+]?[0-9]+)\,([+]?[0-9]+)\))", std::regex::icase);
const std::regex COLOR_WITH_RGBA(R"(rgba\(([+]?[0-9]+)\,([+]?[0-9]+)\,([+]?[0-9]+)\,(\d+\.?\d*)\))", std::regex::icase);
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
#ifdef ROSEN_OHOS
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_ONE);

    Drawing::Color color;
    if (!GetResourceColor(env, argv[ARGC_ZERO], color)) {
        ROSEN_LOGE("JsTool::makeColorFromResourceColor failed to make color!");
        return nullptr;
    }
    return GetColorAndConvertToJsValue(env, color);
#else
    return nullptr;
#endif
}

#ifdef ROSEN_OHOS
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
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
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

unsigned int JsTool::GetColorNumberResult(unsigned int origin)
{
    unsigned int result = origin;
    if ((origin >> COLOR_OFFSET_ALPHA) == 0) {
        result = origin | COLOR_ALPHA_VALUE;
    }
    return result;
}

bool JsTool::FastCheckColorType(const std::string& colorStr, const std::string& expectPrefix,
    const std::vector<size_t>& expectLengths)
{
    if (colorStr.rfind(expectPrefix, 0) != 0) {
        return false;
    }
    if (expectLengths.size() == 0) {
        return true;
    }
    for (size_t expectLength : expectLengths) {
        if (expectLength == colorStr.size()) {
            return true;
        }
    }
    return false;
}

bool JsTool::IsHexNumber(std::string& colorStr)
{
    for (size_t i = 1; i < colorStr.size(); i++) {
        if (colorStr[i] >= '0' && colorStr[i] <= '9') {
            continue;
        }
        if (colorStr[i] >= 'A' && colorStr[i] <= 'F') {
            continue;
        }
        if (colorStr[i] >= 'a' && colorStr[i] <= 'f') {
            continue;
        }
        return false;
    }
    return true;
}

unsigned int JsTool::HandleIncorrectColor(const std::string& newColorStr)
{
    errno = 0;
    char* end = nullptr;
    unsigned int value = strtoul(newColorStr.c_str(), &end, COLOR_STRING_BASE);
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

bool JsTool::MatchColorWithMagic(std::string& colorStr, uint32_t maskAlpha, unsigned int& result)
{
    if (!FastCheckColorType(colorStr, "#", EXPECT_MAGIC_COLOR_LENGTHS)) {
        ROSEN_LOGE("JsTool::MatchColorWithMagic colorString is invalid");
        return false;
    }
    if (!IsHexNumber(colorStr)) {
        ROSEN_LOGE("JsTool::MatchColorWithMagic colorString is invalid hexnub");
        return false;
    }

    colorStr.erase(0, 1);
    result = HandleIncorrectColor(colorStr);
    if (colorStr.length() < COLOR_STRING_SIZE_STANDARD) {
        result |= maskAlpha;
    }
    return true;
}

bool JsTool::MatchColorWithMagicMini(std::string& colorStr, uint32_t maskAlpha, unsigned int& result)
{
    if (!FastCheckColorType(colorStr, "#", EXPECT_MAGIC_MINI_COLOR_LENGTHS)) {
        ROSEN_LOGE("JsTool::MatchColorWithMagicMini colorString is invalid");
        return false;
    }
    if (!IsHexNumber(colorStr)) {
        ROSEN_LOGE("JsTool::MatchColorWithMagicMini colorString is invalid hexnub");
        return false;
    }

    colorStr.erase(0, 1);
    std::string newColorStr;
    for (auto& c : colorStr) {
        newColorStr += c;
        newColorStr += c;
    }
    result = HandleIncorrectColor(newColorStr);
    if (newColorStr.length() < COLOR_STRING_SIZE_STANDARD) {
        result |= maskAlpha;
    }
    return true;
}

bool JsTool::MatchColorWithRGB(const std::string& colorStr, unsigned int& result)
{
    if (!FastCheckColorType(colorStr, "rgb(", {})) {
        ROSEN_LOGE("JsTool::MatchColorWithRGB colorString is invalid");
        return false;
    }

    std::smatch matches;
    if (std::regex_match(colorStr, matches, COLOR_WITH_RGB)) {
        if (matches.size() == RGB_SUB_MATCH_SIZE) {
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
            result = COLOR_ALPHA_VALUE | (red << COLOR_OFFSET_RED) | (green << COLOR_OFFSET_GREEN) | blue;
            return true;
        }
    }
    return false;
}

bool JsTool::MatchColorWithRGBA(const std::string& colorStr, unsigned int& result)
{
    if (!FastCheckColorType(colorStr, "rgba(", {})) {
        ROSEN_LOGE("JsTool::MatchColorWithRGBA colorString is invalid");
        return false;
    }

    std::smatch matches;
    if (std::regex_match(colorStr, matches, COLOR_WITH_RGBA)) {
        if (matches.size() == RGBA_SUB_MATCH_SIZE) {
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
            auto alpha = static_cast<uint8_t>(round(static_cast<double>(opacityDouble) * 0xff)) & 0xff;
            result = (alpha << COLOR_OFFSET_ALPHA) | (red << COLOR_OFFSET_RED) | (green << COLOR_OFFSET_GREEN) | blue;
            return true;
        }
    }
    return false;
}

bool JsTool::GetColorStringResult(std::string colorStr, unsigned int& result)
{
    if (colorStr.empty()) {
        ROSEN_LOGE("JsTool::GetColorStringResult string is empty");
        return false;
    }

    colorStr.erase(std::remove(colorStr.begin(), colorStr.end(), ' '), colorStr.end());

    return (MatchColorWithMagic(colorStr, COLOR_ALPHA_MASK, result) ||
            MatchColorWithMagicMini(colorStr, COLOR_ALPHA_MASK, result) ||
            MatchColorWithRGB(colorStr, result) || MatchColorWithRGBA(colorStr, result));
}

bool JsTool::GetColorObjectResult(napi_env env, napi_value value, unsigned int& result)
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

bool JsTool::GetResourceColor(napi_env env, napi_value res, Color& result)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, res, &valueType);
    if (valueType == napi_string) {
        size_t len = 0;
        napi_get_value_string_utf8(env, res, nullptr, 0, &len);
        char str[len + 1];
        napi_get_value_string_utf8(env, res, str, len + 1, &len);
        std::string colorStr(str, len);
        unsigned int colorResult = 0;
        if (!GetColorStringResult(colorStr, colorResult)) {
            ROSEN_LOGE("JsTool::GetResourceColor failed to GetColorStringResult!");
            return false;
        }
        result = Color(colorResult);
    } else if (valueType == napi_number) {
        unsigned int colorNumber = 0;
        napi_get_value_uint32(env, res, &colorNumber);
        result = Color(GetColorNumberResult(colorNumber));
    } else if (valueType == napi_object) {
        unsigned int colorResult = 0;
        if (!GetColorObjectResult(env, res, colorResult)) {
            ROSEN_LOGE("JsTool::GetResourceColor failed to GetColorObjectResult!");
            return false;
        }
        result = Color(colorResult);
    } else {
        ROSEN_LOGE("JsTool::GetResourceColor invalid ResourceColor type!");
        return false;
    }
    return true;
}
#endif
} // namespace Drawing
} // namespace OHOS::Rosen