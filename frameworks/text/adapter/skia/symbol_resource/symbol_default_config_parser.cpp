/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <functional>

#include "include/private/SkOnce.h"
#include "securec.h"
#include "src/ports/skia_ohos/HmSymbolConfig_ohos.h"
#include "symbol_default_config_parser.h"
#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace Symbol {
using PiecewiseParaKeyFunc =
    std::function<void(const char*, const Json::Value&, RSDrawing::DrawingPiecewiseParameter&)>;
using SymbolKeyFunc = std::function<void(const char*, const Json::Value&, RSDrawing::DrawingSymbolLayersGroups&)>;
using SymnolAniFunc = std::function<void(const char*, const Json::Value&, RSDrawing::DrawingAnimationPara&)>;

using SymbolKeyFuncMap = std::unordered_map<std::string, SymbolKeyFunc>;
const char SPECIAL_ANIMATIONS[] = "special_animations";
const char COMMON_ANIMATIONS[] = "common_animations";
const char SYMBOL_LAYERS_GROUPING[] = "symbol_layers_grouping";
const char ANIMATION_TYPE[] = "animation_type";
const char ANIMATION_TYPES[] = "animation_types";
const char ANIMATION_PARAMETERS[] = "animation_parameters";
const char ANIMATION_MODE[] = "animation_mode";
const char GROUP_PARAMETERS[] = "group_parameters";
const char CURVE[] = "curve";
const char CURVE_ARGS[] = "curve_args";
const char DURATION[] = "duration";
const char DELAY[] = "delay";
const char NATIVE_GLYPH_ID[] = "native_glyph_id";
const char SYMBOL_GLYPH_ID[] = "symbol_glyph_id";
const char LAYERS[] = "layers";
const char RENDER_MODES[] = "render_modes";
const char MODE[] = "mode";
const char RENDER_GROUPS[] = "render_groups";
const char GROUP_INDEXES[] = "group_indexes";
const char DEFAULT_COLOR[] = "default_color";
const char FIX_ALPHA[] = "fix_alpha";
const char LAYER_INDEXES[] = "layer_indexes";
const char MASK_INDEXES[] = "mask_indexes";
const char GROUP_SETTINGS[] = "group_settings";
const char ANIMATION_INDEX[] = "animation_index";
const char COMMON_SUB_TYPE[] = "common_sub_type";
const char ANIMATION_SETTINGS[] = "animation_settings";
const char PROPERTIES[] = "properties";
const char SLOPE[] = "slope";

const int NO_ERROR = 0;                          // no error
const int ERROR_CONFIG_NOT_FOUND = 1;            // the configuration document is not found
const int ERROR_CONFIG_FORMAT_NOT_SUPPORTED = 2; // the formation of configuration is not supported
const int ERROR_CONFIG_INVALID_VALUE_TYPE = 4;   // invalid value type in the configuration
const int ERROR_TYPE_COUNT = 11;

class SymbolAutoRegister {
public:
    SymbolAutoRegister() { RegisterSymbolConfigCallBack(); }

private:
    bool RegisterSymbolConfigCallBack()
    {
        static std::once_flag flag;
        std::call_once(flag, []() {
            std::function<int(const char*)> loadSymbolConfigFunc = [](const char* filePath) -> int {
                return SymbolDefaultConfigParser::GetInstance()->ParseConfigOfHmSymbol(filePath);
            };
            skia::text::HmSymbolConfig_OHOS::SetLoadSymbolConfig(loadSymbolConfigFunc);
        });

        return true;
    }
};

SymbolAutoRegister g_symbolAutoRegister;

static const std::map<std::string, RSDrawing::DrawingAnimationType> ANIMATIONS_TYPES = {
    {"scale", RSDrawing::DrawingAnimationType::SCALE_TYPE},
    {"appear", RSDrawing::DrawingAnimationType::APPEAR_TYPE},
    {"disappear", RSDrawing::DrawingAnimationType::DISAPPEAR_TYPE},
    {"bounce", RSDrawing::DrawingAnimationType::BOUNCE_TYPE},
    {"variable_color", RSDrawing::DrawingAnimationType::VARIABLE_COLOR_TYPE},
    {"pulse", RSDrawing::DrawingAnimationType::PULSE_TYPE},
    {"replace_appear", RSDrawing::DrawingAnimationType::REPLACE_APPEAR_TYPE},
    {"replace_disappear", RSDrawing::DrawingAnimationType::REPLACE_DISAPPEAR_TYPE},
    {"disable", RSDrawing::DrawingAnimationType::DISABLE_TYPE},
    {"quick_replace_appear", RSDrawing::DrawingAnimationType::QUICK_REPLACE_APPEAR_TYPE},
    {"quick_replace_disappear", RSDrawing::DrawingAnimationType::QUICK_REPLACE_DISAPPEAR_TYPE}
};

static const std::map<std::string, RSDrawing::DrawingCurveType> CURVE_TYPES = {
    {"spring", RSDrawing::DrawingCurveType::SPRING},
    {"linear", RSDrawing::DrawingCurveType::LINEAR},
    {"friction", RSDrawing::DrawingCurveType::FRICTION},
    {"sharp", RSDrawing::DrawingCurveType::SHARP}
};

/* To get the display text of an error
 * \param err the id of an error
 * \return The text to explain the error
 */
const char* ErrInfoToString(int err)
{
    const static std::array<const char*, ERROR_TYPE_COUNT> errInfoToString{
        "successful",                                 // NO_ERROR = 0
        "config file is not found",                   // ERROR_CONFIG_NOT_FOUND
        "the format of config file is not supported", // ERROR_CONFIG_FORMAT_NOT_SUPPORTED
        "missing tag",                                // ERROR_CONFIG_MISSING_TAG
        "invalid value type",                         // ERROR_CONFIG_INVALID_VALUE_TYPE
        "font file is not exist",                     // ERROR_FONT_NOT_EXIST
        "invalid font stream",                        // ERROR_FONT_INVALID_STREAM
        "no font stream",                             // ERROR_FONT_NO_STREAM
        "family is not found",                        // ERROR_FAMILY_NOT_FOUND
        "no available family in the system",          //ERROR_NO_AVAILABLE_FAMILY
        "no such directory"                           // ERROR_DIR_NOT_FOUND
    };
    if (err >= 0 && err < ERROR_TYPE_COUNT) {
        return errInfoToString[err];
    }
    return "unknown error";
}

/* To log the error information
 * \param err the id of an error
 * \param key the key which indicates the the part with the error
 * \param expected the expected type of json node.
 * \n     It's used only for err 'ERROR_CONFIG_INVALID_VALUE_TYPE'
 * \param actual the actual type of json node.
 * \n     It's used only for err 'ERROR_CONFIG_INVALID_VALUE_TYPE'
 * \return err
 */
int LogErrInfo(
    int err, const char* key, Json::ValueType expected = Json::nullValue, Json::ValueType actual = Json::nullValue)
{
    if (err != ERROR_CONFIG_INVALID_VALUE_TYPE) {
        TEXT_LOGE("%s : %s\n", ErrInfoToString(err), key);
    } else {
        const char* types[] = {
            "null",
            "int",
            "unit",
            "real",
            "string",
            "boolean",
            "array",
            "object",
        };
        int size = sizeof(types) / sizeof(char*);
        if ((expected >= 0 && expected < size) && (actual >= 0 && actual < size)) {
            TEXT_LOGE("%s : '%s' should be '%s', but here it's '%s'\n", ErrInfoToString(err), key, types[expected],
                types[actual]);
        } else {
            TEXT_LOGE("%s : %s\n", ErrInfoToString(err), key);
        }
    }
    return err;
}

/* To get the data of font configuration file
 * \param fname the full name of the font configuration file
 * \param[out] size the size of data returned to the caller
 * \return The pointer of content of the file
 * \note The returned pointer should be freed by the caller
 */
std::unique_ptr<char[]> GetDataFromFile(const char* fname, int& size)
{
    auto file = std::make_unique<std::ifstream>(fname);
    if (file == nullptr || !file->is_open()) {
        size = 0;
        return nullptr;
    }

    file->seekg(0, std::ios::end);
    size = static_cast<int>(file->tellg()); // get the length of file
    if (size <= 0) {
        size = 0;
        return nullptr;
    }
    auto data = std::make_unique<char[]>(size);
    if (data == nullptr) {
        size = 0;
        return nullptr;
    }
    file->seekg(0, std::ios::beg);
    file->read(data.get(), size); // read data a block
    return data;
}

SymbolDefaultConfigParser* SymbolDefaultConfigParser::GetInstance()
{
    static SymbolDefaultConfigParser singleton;
    return &singleton;
}

RSDrawing::DrawingSymbolLayersGroups SymbolDefaultConfigParser::GetSymbolLayersGroups(uint16_t glyphId)
{
    RSDrawing::DrawingSymbolLayersGroups symbolLayersGroups;
    if (hmSymbolConfig_.size() == 0) {
        return symbolLayersGroups;
    }
    auto iter = hmSymbolConfig_.find(glyphId);
    if (iter != hmSymbolConfig_.end()) {
        symbolLayersGroups = iter->second;
    }
    return symbolLayersGroups;
}

std::vector<std::vector<RSDrawing::DrawingPiecewiseParameter>> SymbolDefaultConfigParser::GetGroupParameters(
    RSDrawing::DrawingAnimationType type, uint16_t groupSum, uint16_t animationMode,
    RSDrawing::DrawingCommonSubType commonSubType)
{
    std::vector<std::vector<RSDrawing::DrawingPiecewiseParameter>> groupParametersOut;
    if (animationInfos_.empty()) {
        return groupParametersOut;
    }
    std::unordered_map<RSDrawing::DrawingAnimationType, RSDrawing::DrawingAnimationInfo>::iterator iter =
        animationInfos_.find(type);
    if (iter == animationInfos_.end()) {
        return groupParametersOut;
    }
    auto key = EncodeAnimationAttribute(groupSum, animationMode, commonSubType);
    auto para = iter->second.animationParas.find(key);
    if (para != iter->second.animationParas.end()) {
        groupParametersOut = para->second.groupParameters;
    }
    return groupParametersOut;
}

/* check the system font configuration document
 * \param fname the full name of the font configuration document
 * \return NO_ERROR successful
 * \return ERROR_CONFIG_NOT_FOUND config document is not found
 * \return ERROR_CONFIG_FORMAT_NOT_SUPPORTED config document format is not supported
 */
int SymbolDefaultConfigParser::CheckConfigFile(const char* fname, Json::Value& root)
{
    int size = 0;
    auto data = GetDataFromFile(fname, size);
    if (data == nullptr) {
        return LogErrInfo(ERROR_CONFIG_NOT_FOUND, fname);
    }
    JSONCPP_STRING errs;
    Json::CharReaderBuilder charReaderBuilder;
    std::unique_ptr<Json::CharReader> jsonReader(charReaderBuilder.newCharReader());
    bool isJson = jsonReader->parse(data.get(), data.get() + size, &root, &errs);
    if (!isJson || !errs.empty()) {
        return LogErrInfo(ERROR_CONFIG_FORMAT_NOT_SUPPORTED, fname);
    }
    return NO_ERROR;
}

int SymbolDefaultConfigParser::ParseConfigOfHmSymbol(const char* filePath)
{
    std::lock_guard<std::mutex> lock(hmSymbolMut_);
    if (GetInit()) {
        return NO_ERROR;
    }
    if (filePath == nullptr || strlen(filePath) == 0) {
        return ERROR_CONFIG_NOT_FOUND;
    }
    Clear();
    Json::Value root;
    int err = CheckConfigFile(filePath, root);
    if (err != NO_ERROR) {
        return err;
    }

    const char* key = nullptr;
    std::vector<std::string> tags = {COMMON_ANIMATIONS, SPECIAL_ANIMATIONS, SYMBOL_LAYERS_GROUPING};
    for (unsigned int i = 0; i < tags.size(); i++) {
        key = tags[i].c_str();
        if (!root.isMember(key) || !root[key].isArray()) {
            continue;
        }
        if (!strcmp(key, COMMON_ANIMATIONS) || !strcmp(key, SPECIAL_ANIMATIONS)) {
            ParseSymbolAnimations(root[key], &animationInfos_);
        } else {
            ParseSymbolLayersGrouping(root[key]);
        }
    }
    root.clear();
    SetInit(true);
    return NO_ERROR;
}

void SymbolDefaultConfigParser::ParseSymbolAnimations(const Json::Value& root,
    std::unordered_map<RSDrawing::DrawingAnimationType, RSDrawing::DrawingAnimationInfo>* animationInfos)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            TEXT_LOGE("animation is not object!");
            continue;
        }

        if (!root[i].isMember(ANIMATION_TYPE) || !root[i].isMember(ANIMATION_PARAMETERS)) {
            continue;
        }
        RSDrawing::DrawingAnimationInfo animationInfo;
        if (!root[i][ANIMATION_TYPE].isString()) {
            TEXT_LOGE("animation_type is not string!");
            continue;
        }
        const std::string animationType = root[i][ANIMATION_TYPE].asString();
        ParseAnimationType(animationType, animationInfo.animationType);

        if (!root[i][ANIMATION_PARAMETERS].isArray()) {
            TEXT_LOGE("animation_parameters is not array!");
            continue;
        }
        ParseSymbolAnimationParas(root[i][ANIMATION_PARAMETERS], animationInfo.animationParas);
        animationInfos->insert({animationInfo.animationType, animationInfo});
    }
}

uint32_t SymbolDefaultConfigParser::EncodeAnimationAttribute(
    uint16_t groupSum, uint16_t animationMode, RSDrawing::DrawingCommonSubType commonSubType)
{
    uint32_t result = static_cast<uint32_t>(groupSum);
    result = (result << oneByteBitsLen) + static_cast<uint32_t>(animationMode);
    result = (result << oneByteBitsLen) + static_cast<uint32_t>(commonSubType);
    return result;
}

void SymbolDefaultConfigParser::ParseSymbolAnimationParas(
    const Json::Value& root, std::map<uint32_t, RSDrawing::DrawingAnimationPara>& animationParas)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            TEXT_LOGE("animation_parameter is not object!");
            continue;
        }
        RSDrawing::DrawingAnimationPara animationPara;
        ParseSymbolAnimationPara(root[i], animationPara);
        uint32_t attributeKey = EncodeAnimationAttribute(
            animationPara.groupParameters.size(), animationPara.animationMode, animationPara.commonSubType);
        animationParas.insert({attributeKey, animationPara});
    }
}

void SymbolDefaultConfigParser::ParseSymbolAnimationPara(
    const Json::Value& root, RSDrawing::DrawingAnimationPara& animationPara)
{
    const char* key = nullptr;
    std::vector<std::string> tags = {ANIMATION_MODE, COMMON_SUB_TYPE, GROUP_PARAMETERS};
    using SymnolAniFuncMap = std::unordered_map<std::string, SymnolAniFunc>;
    SymnolAniFuncMap funcMap = {
        {ANIMATION_MODE,
            [](const char* key, const Json::Value& root, RSDrawing::DrawingAnimationPara& animationPara) {
                if (!root[key].isInt()) {
                    TEXT_LOGE("animation_mode is not int!");
                    return;
                }
                animationPara.animationMode = root[key].asInt();
            }},
        {COMMON_SUB_TYPE,
            [this](const char* key, const Json::Value& root, RSDrawing::DrawingAnimationPara& animationPara) {
                if (!root[key].isString()) {
                    TEXT_LOGE("sub_type is not string!");
                    return;
                }
                const std::string subTypeStr = root[key].asString();
                ParseSymbolCommonSubType(subTypeStr, animationPara.commonSubType);
            }},
        {GROUP_PARAMETERS,
            [this](const char* key, const Json::Value& root, RSDrawing::DrawingAnimationPara& animationPara) {
                if (!root[key].isArray()) {
                    TEXT_LOGE("group_parameters is not array!");
                    return;
                }
                ParseSymbolGroupParas(root[key], animationPara.groupParameters);
            }}};
    for (unsigned int i = 0; i < tags.size(); i++) {
        key = tags[i].c_str();
        if (!root.isMember(key)) {
            continue;
        }
        if (funcMap.count(key) > 0) {
            funcMap[key](key, root, animationPara);
        }
    }
}

void SymbolDefaultConfigParser::ParseSymbolCommonSubType(
    const std::string& subTypeStr, RSDrawing::DrawingCommonSubType& commonSubType)
{
    using SymbolAniSubFuncMap = std::unordered_map<std::string, std::function<void(RSDrawing::DrawingCommonSubType&)>>;
    SymbolAniSubFuncMap funcMap = {
        {"up", [](RSDrawing::DrawingCommonSubType& commonSubType) {
             commonSubType = RSDrawing::DrawingCommonSubType::UP;
         }},
        {"down", [](RSDrawing::DrawingCommonSubType& commonSubType) {
             commonSubType = RSDrawing::DrawingCommonSubType::DOWN;
         }}
    };
    if (funcMap.count(subTypeStr) > 0) {
        funcMap[subTypeStr](commonSubType);
        return;
    }
    TEXT_LOGE("%{public}s is invalid value!", subTypeStr.c_str());
}

void SymbolDefaultConfigParser::ParseSymbolGroupParas(
    const Json::Value& root, std::vector<std::vector<RSDrawing::DrawingPiecewiseParameter>>& groupParameters)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isArray()) {
            TEXT_LOGE("group_parameter is not array!");
            continue;
        }
        std::vector<RSDrawing::DrawingPiecewiseParameter> piecewiseParameters;
        for (unsigned int j = 0; j < root[i].size(); j++) {
            if (!root[i][j].isObject()) {
                TEXT_LOGE("piecewise_parameter is not object!");
                continue;
            }
            RSDrawing::DrawingPiecewiseParameter piecewiseParameter;
            ParseSymbolPiecewisePara(root[i][j], piecewiseParameter);
            piecewiseParameters.push_back(piecewiseParameter);
        }

        groupParameters.push_back(piecewiseParameters);
    }
}

static void PiecewiseParaCurveCase(
    const char* key, const Json::Value& root, RSDrawing::DrawingPiecewiseParameter& piecewiseParameter)
{
    if (!root[key].isString()) {
        TEXT_LOGE("curve is not string!");
        return;
    }
    const std::string curveTypeStr = root[key].asString();
    auto iter = CURVE_TYPES.find(curveTypeStr);
    if (iter != CURVE_TYPES.end()) {
        piecewiseParameter.curveType = iter->second;
        return;
    }
    TEXT_LOGE("curve is invalid value!");
}

static void PiecewiseParaDurationCase(
    const char* key, const Json::Value& root, RSDrawing::DrawingPiecewiseParameter& piecewiseParameter)
{
    if (!root[key].isNumeric()) {
        TEXT_LOGE("duration is not numeric!");
        return;
    }
    if (root[key].isConvertibleTo(Json::uintValue)) {
        piecewiseParameter.duration = root[key].asUInt();
    }
}

static void PiecewiseParaDelayCase(
    const char* key, const Json::Value& root, RSDrawing::DrawingPiecewiseParameter& piecewiseParameter)
{
    if (!root[key].isNumeric()) {
        TEXT_LOGE("delay is not numeric!");
        return;
    }
    if (root[key].isConvertibleTo(Json::intValue)) {
        piecewiseParameter.delay = root[key].asInt();
    }
}

void SymbolDefaultConfigParser::ParseSymbolPiecewisePara(
    const Json::Value& root, RSDrawing::DrawingPiecewiseParameter& piecewiseParameter)
{
    const char* key = nullptr;
    std::vector<std::string> tags = {CURVE, CURVE_ARGS, DURATION, DELAY, PROPERTIES};
    using PiecewiseFuncMap = std::unordered_map<std::string, PiecewiseParaKeyFunc>;
    PiecewiseFuncMap funcMap = {{CURVE, PiecewiseParaCurveCase},
        {CURVE_ARGS,
            [this](const char* key, const Json::Value& root, RSDrawing::DrawingPiecewiseParameter& piecewiseParameter) {
                if (!root[key].isObject()) {
                    TEXT_LOGE("curve_args is not object!");
                    return;
                }
                ParseSymbolCurveArgs(root[key], piecewiseParameter.curveArgs);
            }},
        {DURATION, PiecewiseParaDurationCase}, {DELAY, PiecewiseParaDelayCase},
        {PROPERTIES,
            [this](const char* key, const Json::Value& root, RSDrawing::DrawingPiecewiseParameter& piecewiseParameter) {
                if (!root[key].isObject()) {
                    TEXT_LOGE("properties is not object!");
                    return;
                }
                ParseSymbolProperties(root[key], piecewiseParameter.properties);
            }}};

    for (unsigned int i = 0; i < tags.size(); i++) {
        key = tags[i].c_str();
        if (!root.isMember(key)) {
            continue;
        }
        if (funcMap.count(key) > 0) {
            funcMap[key](key, root, piecewiseParameter);
        }
    }
}

void SymbolDefaultConfigParser::ParseSymbolCurveArgs(const Json::Value& root, std::map<std::string, float>& curveArgs)
{
    if (root.empty()) {
        return;
    }

    for (Json::Value::const_iterator iter = root.begin(); iter != root.end(); ++iter) {
        std::string name = iter.name();
        const char* memberName = name.c_str();
        if (!root[memberName].isNumeric()) {
            TEXT_LOGE("%{public}s is not numeric!", memberName);
            continue;
        }
        curveArgs.insert({std::string(memberName), root[memberName].asFloat()});
    }
}

void SymbolDefaultConfigParser::ParseSymbolProperties(
    const Json::Value& root, std::map<std::string, std::vector<float>>& properties)
{
    for (Json::Value::const_iterator iter = root.begin(); iter != root.end(); ++iter) {
        std::string name = iter.name();
        const char* memberName = name.c_str();
        if (!root[memberName].isArray()) {
            TEXT_LOGE("%{public}s is not array!", memberName);
            continue;
        }

        std::vector<float> propertyValues;
        for (unsigned int i = 0; i < root[memberName].size(); i++) {
            if (!root[memberName][i].isNumeric()) {
                TEXT_LOGE("property value is not numeric!");
                continue;
            }
            propertyValues.push_back(root[memberName][i].asFloat());
        }

        properties.insert({std::string(memberName), propertyValues});
    }
}

void SymbolDefaultConfigParser::ParseSymbolLayersGrouping(const Json::Value& root)
{
    std::unordered_map<uint16_t, RSDrawing::DrawingSymbolLayersGroups>* hmSymbolConfig = &hmSymbolConfig_;
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            TEXT_LOGE("symbol_layers_grouping[%{public}d] is not object!", i);
            continue;
        }
        ParseOneSymbol(root[i], hmSymbolConfig);
    }
}

static void SymbolGlyphCase(
    const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups)
{
    if (!root[key].isInt()) {
        TEXT_LOGE("symbol_glyph_id is not int!");
        return;
    }
    symbolLayersGroups.symbolGlyphId = root[key].asInt();
}

void SymbolDefaultConfigParser::ParseOneSymbolNativeCase(const char* key, const Json::Value& root,
    RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups, uint16_t& nativeGlyphId)
{
    if (!root[key].isInt()) {
        TEXT_LOGE("native_glyph_id is not int!");
        return;
    }
    nativeGlyphId = root[key].asInt();
}

void SymbolDefaultConfigParser::ParseOneSymbolLayerCase(
    const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups)
{
    if (!root[key].isArray()) {
        TEXT_LOGE("layers is not array!");
        return;
    }
    ParseLayers(root[key], symbolLayersGroups.layers);
}

void SymbolDefaultConfigParser::ParseOneSymbolRenderCase(
    const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups)
{
    if (!root[key].isArray()) {
        TEXT_LOGE("render_modes is not array!");
        return;
    }
    ParseRenderModes(root[key], symbolLayersGroups.renderModeGroups);
}

void SymbolDefaultConfigParser::ParseOneSymbolAnimateCase(
    const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups)
{
    if (!root[key].isArray()) {
        TEXT_LOGE("animation_settings is not array!");
        return;
    }
    ParseAnimationSettings(root[key], symbolLayersGroups.animationSettings);
}

void SymbolDefaultConfigParser::ParseOneSymbol(
    const Json::Value& root, std::unordered_map<uint16_t, RSDrawing::DrawingSymbolLayersGroups>* hmSymbolConfig)
{
    const char* key = nullptr;
    std::vector<std::string> tags = {NATIVE_GLYPH_ID, SYMBOL_GLYPH_ID, LAYERS, RENDER_MODES, ANIMATION_SETTINGS};
    uint16_t nativeGlyphId;
    RSDrawing::DrawingSymbolLayersGroups symbolLayersGroups;

    SymbolKeyFuncMap funcMap = {{NATIVE_GLYPH_ID,
                                    [this, &nativeGlyphId](const char* key, const Json::Value& root,
                                        RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups) {
                                        ParseOneSymbolNativeCase(key, root, symbolLayersGroups, nativeGlyphId);
                                    }},
        {SYMBOL_GLYPH_ID, SymbolGlyphCase},
        {LAYERS,
            [this](const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups) {
                ParseOneSymbolLayerCase(key, root, symbolLayersGroups);
            }},
        {RENDER_MODES,
            [this](const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups) {
                ParseOneSymbolRenderCase(key, root, symbolLayersGroups);
            }},
        {ANIMATION_SETTINGS,
            [this](const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups) {
                ParseOneSymbolAnimateCase(key, root, symbolLayersGroups);
            }}};
    for (unsigned int i = 0; i < tags.size(); i++) {
        key = tags[i].c_str();
        if (!root.isMember(key)) {
            continue;
        }
        if (funcMap.count(key) > 0) {
            funcMap[key](key, root, symbolLayersGroups);
        }
    }
    hmSymbolConfig->insert({nativeGlyphId, symbolLayersGroups});
}

void SymbolDefaultConfigParser::ParseLayers(const Json::Value& root, std::vector<std::vector<size_t>>& layers)
{
    const char* key = "components";
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            TEXT_LOGE("layer is not object!");
            continue;
        }

        if (!root[i].isMember(key)) {
            continue;
        }
        if (!root[i][key].isArray()) {
            TEXT_LOGE("components is not array!");
            continue;
        }
        std::vector<size_t> components;
        ParseComponets(root[i][key], components);
        layers.push_back(components);
    }
}

void SymbolDefaultConfigParser::ParseComponets(const Json::Value& root, std::vector<size_t>& components)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isInt()) {
            TEXT_LOGE("component is not int!");
            continue;
        }
        components.push_back(root[i].asInt());
    }
}

void SymbolDefaultConfigParser::ParseRenderModes(const Json::Value& root,
    std::map<RSDrawing::DrawingSymbolRenderingStrategy, std::vector<RSDrawing::DrawingRenderGroup>>& renderModesGroups)
{
    std::unordered_map<std::string, RSDrawing::DrawingSymbolRenderingStrategy> strategeMap = {
        {"monochrome", RSDrawing::DrawingSymbolRenderingStrategy::SINGLE},
        {"multicolor", RSDrawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR},
        {"hierarchical", RSDrawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY},
    };
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            TEXT_LOGE("render_mode is not object!");
            continue;
        }

        RSDrawing::DrawingSymbolRenderingStrategy renderingStrategy;
        std::vector<RSDrawing::DrawingRenderGroup> renderGroups;
        if (root[i].isMember(MODE)) {
            if (!root[i][MODE].isString()) {
                TEXT_LOGE("mode is not string!");
                continue;
            }
            std::string modeValue = root[i][MODE].asString();
            if (strategeMap.count(modeValue) > 0) {
                renderingStrategy = strategeMap[modeValue];
            } else {
                TEXT_LOGE("%{public}s is invalid value!", modeValue.c_str());
                continue;
            }
        }
        if (root[i].isMember(RENDER_GROUPS)) {
            if (!root[i][RENDER_GROUPS].isArray()) {
                TEXT_LOGE("render_groups is not array!");
                continue;
            }
            ParseRenderGroups(root[i][RENDER_GROUPS], renderGroups);
        }
        renderModesGroups.insert({renderingStrategy, renderGroups});
    }
}

void SymbolDefaultConfigParser::ParseRenderGroups(
    const Json::Value& root, std::vector<RSDrawing::DrawingRenderGroup>& renderGroups)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            TEXT_LOGE("render_group is not object!");
            continue;
        }

        RSDrawing::DrawingRenderGroup renderGroup;
        if (root[i].isMember(GROUP_INDEXES) && root[i][GROUP_INDEXES].isArray()) {
            ParseGroupIndexes(root[i][GROUP_INDEXES], renderGroup.groupInfos);
        }
        if (root[i].isMember(DEFAULT_COLOR) && root[i][DEFAULT_COLOR].isString()) {
            const std::string defaultColor = root[i][DEFAULT_COLOR].asString();
            ParseDefaultColor(defaultColor, renderGroup);
        }
        if (root[i].isMember(FIX_ALPHA) && root[i][FIX_ALPHA].isDouble()) {
            renderGroup.color.a = static_cast<float>(root[i][FIX_ALPHA].asDouble());
        }
        renderGroups.push_back(renderGroup);
    }
}

void SymbolDefaultConfigParser::ParseGroupIndexes(
    const Json::Value& root, std::vector<RSDrawing::DrawingGroupInfo>& groupInfos)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        RSDrawing::DrawingGroupInfo groupInfo;
        if (root[i].isMember(LAYER_INDEXES)) {
            if (!root[i][LAYER_INDEXES].isArray()) {
                TEXT_LOGE("layer_indexes is not array!");
                continue;
            }
            ParseLayerOrMaskIndexes(root[i][LAYER_INDEXES], groupInfo.layerIndexes);
        }
        if (root[i].isMember(MASK_INDEXES)) {
            if (!root[i][MASK_INDEXES].isArray()) {
                TEXT_LOGE("mask_indexes is not array!");
                continue;
            }
            ParseLayerOrMaskIndexes(root[i][MASK_INDEXES], groupInfo.maskIndexes);
        }
        groupInfos.push_back(groupInfo);
    }
}

void SymbolDefaultConfigParser::ParseLayerOrMaskIndexes(const Json::Value& root, std::vector<size_t>& indexes)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isInt()) {
            TEXT_LOGE("index is not int!");
            continue;
        }
        indexes.push_back(root[i].asInt());
    }
}

void SymbolDefaultConfigParser::ParseDefaultColor(
    const std::string& defaultColorStr, RSDrawing::DrawingRenderGroup& renderGroup)
{
    char defaultColorHex[defaultColorHexLen];
    defaultColorHex[0] = '0';
    defaultColorHex[1] = 'X';
    if (defaultColorStr.length() == defaultColorStrLen) {
        for (unsigned int i = 1; i < defaultColorStrLen; i++) {
            defaultColorHex[i + 1] = defaultColorStr[i];
        }
        defaultColorHex[defaultColorHexLen - 1] = '\0';
    } else {
        TEXT_LOGE("%{public}s is invalid value!", defaultColorStr.c_str());
        return;
    }

    char* endPtr = nullptr;
    uint32_t defaultColor = std::strtoul(defaultColorHex, &endPtr, hexFlag);
    if (endPtr == nullptr || *endPtr != '\0') {
        return;
    }
    renderGroup.color.r = (defaultColor >> twoBytesBitsLen) & 0xFF;
    renderGroup.color.g = (defaultColor >> oneByteBitsLen) & 0xFF;
    renderGroup.color.b = defaultColor & 0xFF;
}

void SymbolDefaultConfigParser::ParseAnimationSettings(
    const Json::Value& root, std::vector<RSDrawing::DrawingAnimationSetting>& animationSettings)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            TEXT_LOGE("animation_setting is not object!");
            continue;
        }
        RSDrawing::DrawingAnimationSetting animationSetting;
        ParseAnimationSetting(root[i], animationSetting);
        animationSettings.push_back(animationSetting);
    }
}

void SymbolDefaultConfigParser::ParseAnimationSetting(
    const Json::Value& root, RSDrawing::DrawingAnimationSetting& animationSetting)
{
    if (root.isMember(ANIMATION_TYPES) && root[ANIMATION_TYPES].isArray()) {
        ParseAnimationTypes(root[ANIMATION_TYPES], animationSetting.animationTypes);
    }

    if (root.isMember(GROUP_SETTINGS) && root[GROUP_SETTINGS].isArray()) {
        ParseGroupSettings(root[GROUP_SETTINGS], animationSetting.groupSettings);
    }

    if (root.isMember(COMMON_SUB_TYPE) && root[COMMON_SUB_TYPE].isString()) {
        const std::string subTypeStr = root[COMMON_SUB_TYPE].asString();
        ParseSymbolCommonSubType(subTypeStr, animationSetting.commonSubType);
    }

    if (root.isMember(SLOPE) && root[SLOPE].isDouble()) {
        animationSetting.slope = root[SLOPE].asDouble();
    }
}

void SymbolDefaultConfigParser::ParseAnimationTypes(
    const Json::Value& root, std::vector<RSDrawing::DrawingAnimationType>& animationTypes)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isString()) {
            TEXT_LOGE("animation_types is not string!");
            continue;
        }
        const std::string animationTypeStr = root[i].asString();
        RSDrawing::DrawingAnimationType animationType;
        ParseAnimationType(animationTypeStr, animationType);
        animationTypes.push_back(animationType);
    }
}

void SymbolDefaultConfigParser::ParseAnimationType(
    const std::string& animationTypeStr, RSDrawing::DrawingAnimationType& animationType)
{
    auto iter = ANIMATIONS_TYPES.find(animationTypeStr);
    if (iter != ANIMATIONS_TYPES.end()) {
        animationType = iter->second;
    } else {
        TEXT_LOGE("%{public}s is invalid value!", animationTypeStr.c_str());
    }
}

void SymbolDefaultConfigParser::ParseGroupSettings(
    const Json::Value& root, std::vector<RSDrawing::DrawingGroupSetting>& groupSettings)
{
    for (unsigned int i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            TEXT_LOGE("group_setting is not object!");
            continue;
        }
        RSDrawing::DrawingGroupSetting groupSetting;
        ParseGroupSetting(root[i], groupSetting);
        groupSettings.push_back(groupSetting);
    }
}

void SymbolDefaultConfigParser::ParseGroupSetting(const Json::Value& root, RSDrawing::DrawingGroupSetting& groupSetting)
{
    if (root.isMember(GROUP_INDEXES)) {
        if (!root[GROUP_INDEXES].isArray()) {
            TEXT_LOGE("group_indexes is not array!");
        } else {
            ParseGroupIndexes(root[GROUP_INDEXES], groupSetting.groupInfos);
        }
    }

    if (root.isMember(ANIMATION_INDEX)) {
        if (!root[ANIMATION_INDEX].isInt()) {
            TEXT_LOGE("animation_index is not int!");
        } else {
            groupSetting.animationIndex = root[ANIMATION_INDEX].asInt();
        }
    }
}
} // namespace Symbol
} // namespace Rosen
} // namespace OHOS