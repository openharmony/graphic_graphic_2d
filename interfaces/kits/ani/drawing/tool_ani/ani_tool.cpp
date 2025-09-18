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
#ifdef ROSEN_OHOS
#include <regex>
#include "ability.h"
#endif

#include "ani_tool.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_TOOL_NAME = "@ohos.graphics.drawing.drawing.Tool";
#ifdef ROSEN_OHOS
const int32_t GLOBAL_ERROR = 10000;
// The expectLength of regex match of "rgb(255,0,0)". The elements:0 is the string, 1 is r, 2 is g, 3 is b.
constexpr uint32_t RGB_SUB_MATCH_SIZE = 4;
// The expectLength of regex match of "rgba(255,0,0,0.5)". The elements:0 is the string, 1 is r, 2 is g, 3 is b, 4 is a.
constexpr uint32_t RGBA_SUB_MATCH_SIZE = 5;
// The length of standard color, the length of str is must like "FF00FF00" which length is 8.
constexpr uint32_t COLOR_STRING_SIZE_STANDARD = 8;
constexpr uint32_t COLOR_OFFSET_GREEN = 8;
constexpr uint32_t COLOR_OFFSET_RED = 16;
// The string will be parsed in hexadecimal format during the conversion
constexpr uint32_t COLOR_STRING_FORMAT = 16;
constexpr uint32_t COLOR_OFFSET_ALPHA = 24;
constexpr uint32_t COLOR_DEFAULT_ALPHA = 0xFF000000;
constexpr uint32_t COLOR_ENUM_COUNT = 12;
const std::vector<size_t> EXPECT_MAGIC_COLOR_LENGTHS = {7, 9};
const std::vector<size_t> EXPECT_MAGIC_MINI_COLOR_LENGTHS = {4, 5};
const std::regex COLOR_WITH_RGB(R"(rgb\(([+]?[0-9]+)\,([+]?[0-9]+)\,([+]?[0-9]+)\))", std::regex::icase);
const std::regex COLOR_WITH_RGBA(R"(rgba\(([+]?[0-9]+)\,([+]?[0-9]+)\,([+]?[0-9]+)\,(\d+\.?\d*)\))", std::regex::icase);
const std::regex HEX_PATTERN("^[0-9a-fA-F]+$");
std::array<uint32_t, COLOR_ENUM_COUNT> colorArray = {
    0xffffff | 0xFF000000,
    0x000000 | 0xFF000000,
    0x0000ff | 0xFF000000,
    0xa52a2a | 0xFF000000,
    0x808080 | 0xFF000000,
    0x008000 | 0xFF000000,
    0x808080 | 0xFF000000,
    0xffa500 | 0xFF000000,
    0xffc0cb | 0xFF000000,
    0xff0000 | 0xFF000000,
    0xffff00 | 0xFF000000,
    0x00000000,
};
#endif

ani_status AniTool::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_TOOL_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_TOOL_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "makeColorFromResourceColor", nullptr,
            reinterpret_cast<void*>(MakeColorFromResourceColor) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_TOOL_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

std::vector<std::string> AniTool::AniToStdVectorString(ani_env* env, ani_array array)
{
    std::vector<std::string> result;

    ani_size length;
    ani_status ret = env->Array_GetLength(array, &length);
    if (ret != ANI_OK) {
        return result;
    }
    for (ani_size i = 0; i < length; i++) {
        ani_ref aniString = nullptr;
        ret = env->Array_Get(reinterpret_cast<ani_array>(array), i, &aniString);
        if (ret != ANI_OK) {
            ROSEN_LOGE("Failed to get array element %{public}zu", i);
            continue;
        }
        result.push_back(CreateStdString(env, reinterpret_cast<ani_string>(aniString)));
    }

    return result;
}

ani_object AniTool::MakeColorFromResourceColor(ani_env* env, ani_object obj, ani_object resourceColor)
{
#ifdef ROSEN_OHOS
    uint32_t colorNumber = 0;
    if (!GetResourceColor(env, resourceColor, colorNumber)) {
        ROSEN_LOGE("AniTool::makeColorFromResourceColor failed to get colorNumber!");
        return CreateAniUndefined(env);
    }
    OHOS::Rosen::Drawing::Color color = OHOS::Rosen::Drawing::Color((colorNumber >> COLOR_OFFSET_ALPHA) & 0xFF,
        (colorNumber >> COLOR_OFFSET_GREEN) & 0xFF,
        colorNumber & 0xFF,
        (colorNumber >> COLOR_OFFSET_ALPHA) & 0xFF
    );
    ani_object aniColorObj = nullptr;
    ani_status status = CreateColorObj(env, color, aniColorObj);
    if (status != ANI_OK) {
        ROSEN_LOGE("AniTool::makeColorFromResourceColor failed to CreateColorObj!");
        return nullptr;
    }
    return aniColorObj;
#else
    return CreateAniUndefined(env);
#endif
}

#ifdef ROSEN_OHOS
std::shared_ptr<Global::Resource::ResourceManager> AniTool::GetResourceManager()
{
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (context == nullptr) {
        ROSEN_LOGE("AniTool::Failed to get application context");
        return nullptr;
    }
    auto resourceManager = context->GetResourceManager();
    if (resourceManager == nullptr) {
        ROSEN_LOGE("AniTool::Failed to get resource manager");
        return nullptr;
    }
    return resourceManager;
}

bool AniTool::IsUndefinedObject(ani_env* env, ani_ref objectRef)
{
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(objectRef, &isUndefined) != ANI_OK) {
        return false;
    }
    return (bool)isUndefined;
}

bool AniTool::GetTypeParam(ani_env* env, ani_object obj, const char* name, int32_t& result)
{
    ani_ref resultRef;
    if (env->Object_GetPropertyByName_Ref(obj, name, &resultRef) != ANI_OK) {
        return false;
    }
    if (IsUndefinedObject(env, resultRef)) {
        return false;
    }
    ani_class typeClass;
    if (env->FindClass("std.core.Int", &typeClass) != ANI_OK) {
        return false;
    }
    ani_boolean isInt = ANI_TRUE;
    ani_object typeObj = static_cast<ani_object>(resultRef);
    if (env->Object_InstanceOf(typeObj, typeClass, &isInt) != ANI_OK || !isInt) {
        return false;
    }
    ani_int resultValue;
    if (env->Object_CallMethodByName_Int(typeObj, "unboxed", nullptr, &resultValue) != ANI_OK) {
        return false;
    }
    result = static_cast<int32_t>(resultValue);
    return true;
}

bool AniTool::GetParamsArray(ani_env* env, ani_object obj, const char* name, std::vector<std::string>& result)
{
    ani_ref paramsRef;
    if (env->Object_GetPropertyByName_Ref(obj, name, &paramsRef) != ANI_OK) {
        return false;
    }
    if (IsUndefinedObject(env, paramsRef)) {
        return false;
    }
    ani_class paramsClass;
    if (env->FindClass("escompat.Array", &paramsClass) != ANI_OK) {
        return false;
    }
    ani_object paramsObj = static_cast<ani_object>(paramsRef);
    ani_boolean isArray = ANI_TRUE;
    if (env->Object_InstanceOf(paramsObj, paramsClass, &isArray) != ANI_OK || !isArray) {
        return false;
    }
    ani_size length;
    ani_array paramsArrayObj = static_cast<ani_array>(paramsRef);
    if (env->Array_GetLength(paramsArrayObj, &length) != ANI_OK) {
        return false;
    }
    std::vector<std::string> paramsArray;
    for (uint32_t i = 0; i < length; i++) {
        ani_ref itemRef;
        ani_status ret = env->Array_Get(paramsArrayObj, (ani_int)i, &itemRef);
        if (ret != ANI_OK) {
            continue;
        }
        paramsArray.push_back(CreateStdString(env, reinterpret_cast<ani_string>(itemRef)));
    }
    result = paramsArray;
    return true;
}

bool AniTool::GetResourceInfo(ani_env* env, ani_object obj, ResourceInfo& result)
{
    ani_long aniId;
    if (env->Object_GetPropertyByName_Long(obj, "id", &aniId) != ANI_OK) {
        return false;
    }
    result.resId = static_cast<int32_t>(aniId);

    if (!GetTypeParam(env, obj, "type", result.type)) {
        return false;
    }
    if (!GetParamsArray(env, obj, "params", result.params)) {
        return false;
    }
    return true;
}

bool AniTool::GetResourceRawFileDataBuffer(std::unique_ptr<uint8_t[]>&& buffer, size_t& len, ResourceInfo& info)
{
    auto resourceManager = GetResourceManager();
    if (resourceManager == nullptr) {
        ROSEN_LOGE("AniTool::ResolveResource failed to get resourceManager, resourceManager is nullptr");
        return false;
    }
    if (info.type != static_cast<int32_t>(ResourceType::RAWFILE)) {
        ROSEN_LOGE("AniTool::ResolveResource invalid resource type %{public}d", info.type);
        return false;
    }

    if (info.params.empty()) {
        ROSEN_LOGE("AniTool::ResolveResource failed to get RawFile resource, RawFile is null");
        return false;
    }

    int32_t state = resourceManager->GetRawFileFromHap(info.params[0], len, buffer);
    if (state >= GLOBAL_ERROR || state < 0) {
        ROSEN_LOGE("AniTool::ResolveResource failed to get Rawfile buffer");
        return false;
    }
    return true;
}

uint32_t AniTool::GetColorNumberResult(uint32_t origin)
{
    uint32_t result = origin;
    if ((origin >> COLOR_OFFSET_ALPHA) == 0) {
        result = origin | COLOR_DEFAULT_ALPHA;
    }
    return result;
}

bool AniTool::FastCheckColorType(const std::string& colorStr, const std::string& expectPrefix,
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

uint32_t AniTool::HandleIncorrectColor(const std::string& newColorStr)
{
    errno = 0;
    char* end = nullptr;
    uint32_t value = strtoul(newColorStr.c_str(), &end, COLOR_STRING_FORMAT);
    if (errno == ERANGE) {
        ROSEN_LOGE("AniTool::HandleIncorrectColor %{public}s is out of range.", newColorStr.c_str());
    }
    if (value == 0 && end == newColorStr.c_str()) {
        ROSEN_LOGE("AniTool::HandleIncorrectColor input can not be converted to number, use default :0x0");
    }
    return value;
}

bool AniTool::MatchColorWithMagic(std::string& colorStr, uint32_t& result)
{
    // This function we match string like "#FF0000" or "#FF0000FF" to get hexnumber
    // Check whether the beginning of the string is "#" and whether the length of the string is vaild.
    if (!FastCheckColorType(colorStr, "#", EXPECT_MAGIC_COLOR_LENGTHS)) {
        ROSEN_LOGE("AniTool::MatchColorWithMagic colorString is invalid");
        return false;
    }
    // Remove "#"
    colorStr.erase(0, 1);

    // Check whether the remaining part of the string is hexnumber
    if (!std::regex_match(colorStr, HEX_PATTERN)) {
        ROSEN_LOGE("AniTool::MatchColorWithMagic colorString is invalid hexnub");
        return false;
    }

    result = HandleIncorrectColor(colorStr);
    // If string is "#FF0000" that has not Alpha, set Alpha is "FF"
    if (colorStr.length() < COLOR_STRING_SIZE_STANDARD) {
        result |= COLOR_DEFAULT_ALPHA;
    }
    return true;
}


bool AniTool::HandleRGBValue(int value, int& result)
{
    if (value < 0) {
        return false;
    }
    result = value > Color::RGB_MAX ? Color::RGB_MAX : value;
    return true;
}

bool AniTool::IsOpacityValid(double value)
{
    return value >= 0 && value <= 1.0;
}

bool AniTool::MatchColorWithMagicMini(std::string& colorStr, uint32_t& result)
{
    // This function we match string like "#FF0000" or "#FF0000FF" to get hexnumber
    // Check whether the beginning of the string is "#" and whether the length of the string is vaild.
    if (!FastCheckColorType(colorStr, "#", EXPECT_MAGIC_MINI_COLOR_LENGTHS)) {
        ROSEN_LOGE("AniTool::MatchColorWithMagicMini colorString is invalid");
        return false;
    }
    // Remove "#"
    colorStr.erase(0, 1);

    // Check whether the remaining part of the string is hexnumber
    if (!std::regex_match(colorStr, HEX_PATTERN)) {
        ROSEN_LOGE("AniTool::MatchColorWithMagicMini colorString is invalid hexnub");
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

bool AniTool::MatchColorWithRGB(const std::string& colorStr, uint32_t& result)
{
    // This function we match string like "rgb(255,0,0)" to get hexnumber
    // Check whether the beginning of the string is "rgb(".
    if (!FastCheckColorType(colorStr, "rgb(", {})) {
        ROSEN_LOGE("AniTool::MatchColorWithRGB colorString is invalid");
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

bool AniTool::MatchColorWithRGBA(const std::string& colorStr, uint32_t& result)
{
    // This function we match string like "rgba(255,0,0,0.5)" to get hexnumber
    // Check whether the beginning of the string is "rgba(".
    if (!FastCheckColorType(colorStr, "rgba(", {})) {
        ROSEN_LOGE("AniTool::MatchColorWithRGBA colorString is invalid");
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
            ROSEN_LOGE("AniTool::MatchColorWithRGBA colorStringNub is invalid");
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

bool AniTool::GetColorStringResult(std::string colorStr, uint32_t& result)
{
    if (colorStr.empty()) {
        ROSEN_LOGE("AniTool::GetColorStringResult string is empty");
        return false;
    }

    // e.g: Remove ' ' that change "rgb(255, 0, 0)" to "rgb(255,0,0)"
    colorStr.erase(std::remove(colorStr.begin(), colorStr.end(), ' '), colorStr.end());

    return (MatchColorWithMagic(colorStr, result) || MatchColorWithMagicMini(colorStr, result) ||
            MatchColorWithRGB(colorStr, result) || MatchColorWithRGBA(colorStr, result));
}

bool AniTool::GetColorObjectResult(ani_env* env, ani_object value, uint32_t& result)
{
    ResourceInfo info;
    if (!GetResourceInfo(env, value, info)) {
        ROSEN_LOGE("AniTool::GetColorObjectResult GetResourceInfo failed!");
        return false;
    }

    auto resourceManager = GetResourceManager();
    if (resourceManager == nullptr) {
        ROSEN_LOGE("AniTool::GetColorObjectResult resourceManager is nullptr!");
        return false;
    }

    if (info.type == static_cast<int32_t>(ResourceType::STRING)) {
        std::string colorStr = "";
        int32_t state = resourceManager->GetStringById(static_cast<uint32_t>(info.resId), colorStr);
        if (state >= GLOBAL_ERROR || state < 0) {
            ROSEN_LOGE("AniTool::GetColorObjectResult failed to get colorString");
            return false;
        }
        if (!GetColorStringResult(colorStr, result)) {
            ROSEN_LOGE("AniTool::GetColorObjectResult failed to GetColorStringResult");
            return false;
        }
    } else if (info.type == static_cast<int32_t>(ResourceType::INTEGER)) {
        int colorResult = 0;
        int32_t state = resourceManager->GetIntegerById(static_cast<uint32_t>(info.resId), colorResult);
        if (state >= GLOBAL_ERROR || state < 0) {
            ROSEN_LOGE("AniTool::GetColorObjectResult failed to get colorInt");
            return false;
        }
        result = GetColorNumberResult(colorResult);
    } else if (info.type == static_cast<int32_t>(ResourceType::COLOR)) {
        int32_t state = resourceManager->GetColorById(static_cast<uint32_t>(info.resId), result);
        if (state >= GLOBAL_ERROR || state < 0) {
            ROSEN_LOGE("AniTool::GetColorObjectResult failed to get colorColor");
            return false;
        }
    } else {
        ROSEN_LOGE("AniTool::GetColorObjectResult invalid Resource type");
        return false;
    }
    return true;
}

bool AniTool::IsStringObject(ani_env* env, ani_object obj)
{
    ani_class stringClass;
    if (env->FindClass("std.core.String", &stringClass) != ANI_OK) {
        return false;
    }
    ani_boolean isString;
    if (env->Object_InstanceOf(obj, stringClass, &isString) != ANI_OK) {
        return false;
    }
    return (bool)isString;
}

bool AniTool::IsColorEnum(ani_env* env, ani_object obj)
{
    ani_enum colorEnum;
    if (env->FindEnum("arkui.component.enums.Color", &colorEnum) != ANI_OK) {
        return false;
    }

    ani_boolean isEnum;
    if (env->Object_InstanceOf(obj, colorEnum, &isEnum) != ANI_OK) {
        return false;
    }

    return (bool)isEnum;
}

bool AniTool::IsNumberObject(ani_env* env, ani_object obj)
{
    ani_class doubleClass;
    if (env->FindClass("std.core.Double", &doubleClass) != ANI_OK) {
        return false;
    }
    ani_boolean isDouble;
    if (env->Object_InstanceOf(obj, doubleClass, &isDouble) != ANI_OK) {
        return false;
    }
    return (bool)isDouble;
}

bool AniTool::IsResourceObject(ani_env* env, ani_object obj)
{
    ani_class resourceClass;
    if (env->FindClass("global.resource.Resource", &resourceClass) != ANI_OK) {
        return false;
    }
    ani_boolean isResource;
    if (env->Object_InstanceOf(obj, resourceClass, &isResource) != ANI_OK) {
        return false;
    }
    return (bool)isResource;
}

bool AniTool::GetResourceColor(ani_env* env, ani_object obj, uint32_t& result)
{
    if (IsStringObject(env, obj)) {
        std::string colorStr = CreateStdString(env, reinterpret_cast<ani_string>(obj));
        if (!GetColorStringResult(colorStr, result)) {
            ROSEN_LOGE("AniTool::GetResourceColor failed to GetColorStringResult!");
            return false;
        }
        return true;
    }

    if (IsColorEnum(env, obj)) {
        ani_int colorInt;
        if (env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(obj), &colorInt) != ANI_OK) {
            ROSEN_LOGE("AniTool::GetResourceColor failed to EnumItem_GetValue_Int");
            return false;
        }
        if (static_cast<size_t>(colorInt) >= colorArray.size()) {
            return false;
        }
        result = colorArray[static_cast<size_t>(colorInt)];
        return true;
    }

    if (IsNumberObject(env, obj)) {
        ani_double aniColor;
        if (ANI_OK != env->Object_CallMethodByName_Double(obj, "unboxed", nullptr, &aniColor)) {
            ROSEN_LOGE("AniTool::GetResourceColor failed by Object_CallMethodByName_Double");
            return false;
        }
        result = GetColorNumberResult(aniColor);
        return true;
    }

    if (IsResourceObject(env, obj)) {
        if (!GetColorObjectResult(env, obj, result)) {
            ROSEN_LOGE("AniTool::GetResourceColor failed to GetColorObjectResult");
            return false;
        }
        return true;
    }
    ROSEN_LOGE("AniTool::GetResourceColor invalid ResourceColor type");
    return false;
}
#endif
} // namespace Drawing
} // namespace OHOS::Rosen