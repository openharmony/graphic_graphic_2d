/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#include "symbol_config_parser.h"

#include <cstdlib>
#include <functional>
#include <utility>

namespace {
constexpr char SYMBOL_LAYERS_GROUPING[] = "symbol_layers_grouping";
constexpr char NATIVE_GLYPH_ID[] = "native_glyph_id";
constexpr char SYMBOL_GLYPH_ID[] = "symbol_glyph_id";
constexpr char LAYERS[] = "layers";
constexpr char COMPONENTS[] = "components";
constexpr char RENDER_MODES[] = "render_modes";
constexpr char MODE[] = "mode";
constexpr char RENDER_GROUPS[] = "render_groups";
constexpr char GROUP_INDEXES[] = "group_indexes";
constexpr char DEFAULT_COLOR[] = "default_color";
constexpr char FIX_ALPHA[] = "fix_alpha";
constexpr char LAYER_INDEXES[] = "layer_indexes";
constexpr char MASK_INDEXES[] = "mask_indexes";
constexpr char GROUP_SETTINGS[] = "group_settings";
constexpr char ANIMATION_INDEX[] = "animation_index";
constexpr char ANIMATION_SETTINGS[] = "animation_settings";
constexpr char ANIMATION_TYPES[] = "animation_types";
const char ANIMATION_TYPE[] = "animation_type";
const char ANIMATION_PARAMETERS[] = "animation_parameters";
const char ANIMATION_MODE[] = "animation_mode";
const char COMMON_SUB_TYPE[] = "common_sub_type";
const char GROUP_PARAMETERS[] = "group_parameters";
const char CURVE[] = "curve";
const char CURVE_ARGS[] = "curve_args";
const char DURATION[] = "duration";
const char DELAY[] = "delay";
const char PROPERTIES[] = "properties";

constexpr uint32_t DEFAULT_COLOR_HEX_LEN = 9;
constexpr uint32_t DEFAULT_COLOR_STR_LEN = 7;
constexpr uint32_t HEX_FLAG = 16;
constexpr uint32_t BYTE_LEN = 8;

using SymbolKeyFunc = std::function<void(const char*, const Json::Value&, RSSymbolLayersGroups&)>;
using SymbolKeyFuncMap = std::unordered_map<std::string, SymbolKeyFunc>;
using SymnolAniFunc = std::function<void(const char*, const Json::Value&, RSAnimationPara&)>;
using SymnolAniFuncMap = std::unordered_map<std::string, SymnolAniFunc>;
using PiecewiseParaKeyFunc = std::function<void(const char*, const Json::Value&, RSPiecewiseParameter&)>;
using PiecewiseFuncMap = std::unordered_map<std::string, PiecewiseParaKeyFunc>;

const std::unordered_map<std::string, RSAnimationType> ANIMATIONS_TYPES = {
    {"scale", RSAnimationType::SCALE_TYPE},
    {"appear", RSAnimationType::APPEAR_TYPE},
    {"disappear", RSAnimationType::DISAPPEAR_TYPE},
    {"bounce", RSAnimationType::BOUNCE_TYPE},
    {"variable_color", RSAnimationType::VARIABLE_COLOR_TYPE},
    {"pulse", RSAnimationType::PULSE_TYPE},
    {"replace_appear", RSAnimationType::REPLACE_APPEAR_TYPE},
    {"replace_disappear", RSAnimationType::REPLACE_DISAPPEAR_TYPE},
    {"quick_replace_appear", RSAnimationType::QUICK_REPLACE_APPEAR_TYPE},
    {"quick_replace_disappear", RSAnimationType::QUICK_REPLACE_DISAPPEAR_TYPE}
};

const std::unordered_map<std::string, RSDrawing::DrawingCurveType> CURVE_TYPES = {
    {"spring", RSDrawing::DrawingCurveType::SPRING},
    {"linear", RSDrawing::DrawingCurveType::LINEAR},
    {"friction", RSDrawing::DrawingCurveType::FRICTION},
    {"sharp", RSDrawing::DrawingCurveType::SHARP}
};

const std::unordered_map<std::string, RSSymbolRenderingStrategy> RENDER_STRATEGY = {
    {"monochrome", RSSymbolRenderingStrategy::SINGLE},
    {"multicolor", RSSymbolRenderingStrategy::MULTIPLE_COLOR},
    {"hierarchical", RSSymbolRenderingStrategy::MULTIPLE_OPACITY},
};

const std::unordered_map<std::string, RSCommonSubType> SYMBOL_ANIMATION_DIRECTION = {
    {"up", RSCommonSubType::UP},
    {"down", RSCommonSubType::DOWN},
};
};

bool SymbolConfigParser::ParseSymbolLayersGrouping(const Json::Value& root,
    std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig)
{
    if (!root.isMember(SYMBOL_LAYERS_GROUPING) || !root[SYMBOL_LAYERS_GROUPING].isArray()) {
        return false;
    }
    for (uint32_t i = 0; i < root[SYMBOL_LAYERS_GROUPING].size(); i++) {
        if (!root[SYMBOL_LAYERS_GROUPING][i].isObject()) {
            continue;
        }
        ParseOneSymbol(root[SYMBOL_LAYERS_GROUPING][i], symbolConfig);
    }
    return true;
}

bool SymbolConfigParser::ParseOneSymbolNativeCase(const char* key, const Json::Value& root, uint16_t& nativeGlyphId)
{
    if (!root[key].isInt()) {
        return false;
    }
    nativeGlyphId = root[key].asInt();
    return true;
}

void SymbolConfigParser::ParseComponets(const Json::Value& root, std::vector<size_t>& components)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isInt()) {
            continue;
        }
        components.push_back(root[i].asInt());
    }
}

void SymbolConfigParser::SymbolGlyphCase(const char* key, const Json::Value& root,
    RSSymbolLayersGroups& symbolLayersGroups)
{
    if (!root[key].isInt()) {
        return;
    }
    symbolLayersGroups.symbolGlyphId = root[key].asInt();
}

void SymbolConfigParser::ParseLayers(const Json::Value& root, std::vector<std::vector<size_t>>& layers)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            continue;
        }

        if (!root[i].isMember(COMPONENTS)) {
            continue;
        }
        if (!root[i][COMPONENTS].isArray()) {
            continue;
        }
        std::vector<size_t> components;
        ParseComponets(root[i][COMPONENTS], components);
        layers.push_back(components);
    }
}

void SymbolConfigParser::ParseOneSymbolLayerCase(const char* key, const Json::Value& root,
    RSSymbolLayersGroups& symbolLayersGroups)
{
    if (!root[key].isArray()) {
        return;
    }
    ParseLayers(root[key], symbolLayersGroups.layers);
}

void SymbolConfigParser::ParseOneSymbolRenderCase(const char* key, const Json::Value& root,
    RSSymbolLayersGroups& symbolLayersGroups)
{
    if (!root[key].isArray()) {
        return;
    }
    ParseRenderModes(root[key], symbolLayersGroups.renderModeGroups);
}

void SymbolConfigParser::ParseRenderModes(const Json::Value& root,
    std::map<RSSymbolRenderingStrategy, std::vector<RSRenderGroup>>& renderModesGroups)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            continue;
        }

        RSSymbolRenderingStrategy renderingStrategy;
        if (root[i].isMember(MODE)) {
            if (!root[i][MODE].isString()) {
                continue;
            }
            std::string modeValue = root[i][MODE].asString();
            if (RENDER_STRATEGY.count(modeValue) == 0) {
                continue;
            }
            renderingStrategy = RENDER_STRATEGY.at(modeValue);
        }

        std::vector<RSRenderGroup> renderGroups;
        if (root[i].isMember(RENDER_GROUPS)) {
            if (!root[i][RENDER_GROUPS].isArray()) {
                continue;
            }
            ParseRenderGroups(root[i][RENDER_GROUPS], renderGroups);
        }
        renderModesGroups.emplace(renderingStrategy, renderGroups);
    }
}

void SymbolConfigParser::ParseRenderGroups(const Json::Value& root, std::vector<RSRenderGroup>& renderGroups)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            continue;
        }

        RSRenderGroup renderGroup;
        if (root[i].isMember(GROUP_INDEXES) && root[i][GROUP_INDEXES].isArray()) {
            ParseGroupIndexes(root[i][GROUP_INDEXES], renderGroup.groupInfos);
        }
        if (root[i].isMember(DEFAULT_COLOR) && root[i][DEFAULT_COLOR].isString()) {
            ParseDefaultColor(root[i][DEFAULT_COLOR].asString().c_str(), renderGroup);
        }
        if (root[i].isMember(FIX_ALPHA) && root[i][FIX_ALPHA].isDouble()) {
            renderGroup.color.a = root[i][FIX_ALPHA].asDouble();
        }
        renderGroups.push_back(renderGroup);
    }
}

void SymbolConfigParser::ParseDefaultColor(const char* defaultColorStr, RSRenderGroup& renderGroup)
{
    char defaultColorHex[DEFAULT_COLOR_HEX_LEN];
    defaultColorHex[0] = '0';
    defaultColorHex[1] = 'X';
    if (defaultColorStr == nullptr || strlen(defaultColorStr) != DEFAULT_COLOR_STR_LEN) {
        return;
    }

    for (uint32_t i = 1; i < DEFAULT_COLOR_STR_LEN; i++) {
        defaultColorHex[i + 1] = defaultColorStr[i];
    }
    defaultColorHex[DEFAULT_COLOR_HEX_LEN - 1] = '\0';
    char* end = nullptr;
    uint32_t defaultColor = std::strtoul(defaultColorHex, &end, HEX_FLAG);
    if (end == nullptr || *end != '\0') {
        return;
    }
    renderGroup.color.r = (defaultColor >> (BYTE_LEN + BYTE_LEN)) & 0xFF;
    renderGroup.color.g = (defaultColor >> BYTE_LEN) & 0xFF;
    renderGroup.color.b = defaultColor & 0xFF;
}

void SymbolConfigParser::ParseGroupIndexes(const Json::Value& root, std::vector<RSGroupInfo>& groupInfos)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        RSGroupInfo groupInfo;
        if (root[i].isMember(LAYER_INDEXES)) {
            if (!root[i][LAYER_INDEXES].isArray()) {
                continue;
            }
            ParseLayerOrMaskIndexes(root[i][LAYER_INDEXES], groupInfo.layerIndexes);
        }
        if (root[i].isMember(MASK_INDEXES)) {
            if (!root[i][MASK_INDEXES].isArray()) {
                continue;
            }
            ParseLayerOrMaskIndexes(root[i][MASK_INDEXES], groupInfo.maskIndexes);
        }
        groupInfos.push_back(groupInfo);
    }
}

void SymbolConfigParser::ParseLayerOrMaskIndexes(const Json::Value& root, std::vector<size_t>& indexes)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isInt()) {
            continue;
        }
        indexes.push_back(root[i].asInt());
    }
}

void SymbolConfigParser::ParseOneSymbolAnimateCase(const char* key, const Json::Value& root,
    RSSymbolLayersGroups& symbolLayersGroups)
{
    if (!root[key].isArray()) {
        return;
    }
    ParseAnimationSettings(root[key], symbolLayersGroups.animationSettings);
}

void SymbolConfigParser::ParseAnimationSettings(const Json::Value& root,
    std::vector<RSAnimationSetting>& animationSettings)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            continue;
        }
        RSAnimationSetting animationSetting;
        ParseAnimationSetting(root[i], animationSetting);
        animationSettings.push_back(animationSetting);
    }
}

void SymbolConfigParser::ParseAnimationSetting(const Json::Value& root, RSAnimationSetting& animationSetting)
{
    if (root.isMember(ANIMATION_TYPES) && root[ANIMATION_TYPES].isArray()) {
        ParseAnimationTypes(root[ANIMATION_TYPES], animationSetting.animationTypes);
    }

    if (root.isMember(GROUP_SETTINGS) && root[GROUP_SETTINGS].isArray()) {
        ParseGroupSettings(root[GROUP_SETTINGS], animationSetting.groupSettings);
    }
}

void SymbolConfigParser::ParseAnimationTypes(const Json::Value& root, std::vector<RSAnimationType>& animationTypes)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isString()) {
            continue;
        }
        const std::string animationTypeStr = root[i].asString();
        RSAnimationType animationType;
        ParseAnimationType(animationTypeStr, animationType);
        animationTypes.push_back(animationType);
    }
}

void SymbolConfigParser::ParseAnimationType(const std::string& animationTypeStr, RSAnimationType& animationType)
{
    auto iter = ANIMATIONS_TYPES.find(animationTypeStr);
    if (iter != ANIMATIONS_TYPES.end()) {
        animationType = iter->second;
    }
}

void SymbolConfigParser::ParseGroupSettings(const Json::Value& root, std::vector<RSGroupSetting>& groupSettings)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            continue;
        }
        RSGroupSetting groupSetting;
        ParseGroupSetting(root[i], groupSetting);
        groupSettings.push_back(groupSetting);
    }
}

void SymbolConfigParser::ParseGroupSetting(const Json::Value& root, RSGroupSetting& groupSetting)
{
    if (root.isMember(GROUP_INDEXES) && root[GROUP_INDEXES].isArray()) {
        ParseGroupIndexes(root[GROUP_INDEXES], groupSetting.groupInfos);
    }

    if (root.isMember(ANIMATION_INDEX) && root[ANIMATION_INDEX].isInt()) {
        groupSetting.animationIndex = root[ANIMATION_INDEX].asInt();
    }
}

void SymbolConfigParser::ParseOneSymbol(const Json::Value& root,
    std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig)
{
    if (!root.isMember(NATIVE_GLYPH_ID)) {
        return;
    }

    uint16_t nativeGlyphId = 0;
    if (!ParseOneSymbolNativeCase(NATIVE_GLYPH_ID, root, nativeGlyphId)) {
        return;
    }
    RSSymbolLayersGroups symbolLayersGroups;
    // The default value for symbol_glyph_id is the value for native_glyph_id
    symbolLayersGroups.symbolGlyphId = nativeGlyphId;

    std::vector<std::string> tags = {SYMBOL_GLYPH_ID, LAYERS, RENDER_MODES, ANIMATION_SETTINGS};
    static SymbolKeyFuncMap funcMap = {
        {SYMBOL_GLYPH_ID, &SymbolConfigParser::SymbolGlyphCase},
        {LAYERS, &SymbolConfigParser::ParseOneSymbolLayerCase},
        {RENDER_MODES, &SymbolConfigParser::ParseOneSymbolRenderCase},
        {ANIMATION_SETTINGS, &SymbolConfigParser::ParseOneSymbolAnimateCase}
    };
    for (uint32_t i = 0; i < tags.size(); i++) {
        const char* key = tags[i].c_str();
        if (!root.isMember(key)) {
            continue;
        }
        if (funcMap.count(key) > 0) {
            funcMap[key](key, root, symbolLayersGroups);
        }
    }
    symbolConfig.emplace(nativeGlyphId, symbolLayersGroups);
}

uint32_t SymbolConfigParser::EncodeAnimationAttribute(uint16_t groupSum,
    uint16_t animationMode, RSCommonSubType commonSubType)
{
    uint32_t result = static_cast<uint32_t>(groupSum);
    result = (result << BYTE_LEN) + static_cast<uint32_t>(animationMode);
    result = (result << BYTE_LEN) + static_cast<uint32_t>(commonSubType);
    return result;
}

void SymbolConfigParser::ParseSymbolAnimations(const Json::Value& root,
    std::unordered_map<RSAnimationType, RSAnimationInfo>& animationInfos)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            continue;
        }

        if (!root[i].isMember(ANIMATION_TYPE) || !root[i].isMember(ANIMATION_PARAMETERS)) {
            continue;
        }
        RSAnimationInfo animationInfo;
        if (!root[i][ANIMATION_TYPE].isString()) {
            continue;
        }
        const std::string animationType = root[i][ANIMATION_TYPE].asString();
        ParseAnimationType(animationType, animationInfo.animationType);

        if (!root[i][ANIMATION_PARAMETERS].isArray()) {
            continue;
        }
        ParseSymbolAnimationParas(root[i][ANIMATION_PARAMETERS], animationInfo.animationParas);
        animationInfos.emplace(animationInfo.animationType, animationInfo);
    }
}

void SymbolConfigParser::ParseSymbolAnimationParas(const Json::Value& root,
    std::map<uint32_t, RSAnimationPara>& animationParas)
{
    for (uint32_t i = 0; i < root.size(); i++) {
        if (!root[i].isObject()) {
            continue;
        }
        RSAnimationPara animationPara;
        ParseSymbolAnimationPara(root[i], animationPara);
        uint32_t attributeKey = EncodeAnimationAttribute(animationPara.groupParameters.size(),
            animationPara.animationMode, animationPara.commonSubType);
        animationParas.emplace(attributeKey, animationPara);
    }
}

void SymbolConfigParser::ParseSymbolAnimationPara(const Json::Value& root, RSAnimationPara& animationPara)
{
    std::vector<std::string> tags = {ANIMATION_MODE, COMMON_SUB_TYPE, GROUP_PARAMETERS};
    static SymnolAniFuncMap funcMap = {
        {ANIMATION_MODE, [](const char* key, const Json::Value& root, RSAnimationPara& animationPara)
            {
                if (!root[key].isInt()) {
                    return;
                }
                animationPara.animationMode = root[key].asInt();
            }
        },
        {COMMON_SUB_TYPE, &SymbolConfigParser::ParseSymbolCommonSubType},
        {GROUP_PARAMETERS, &SymbolConfigParser::ParseSymbolGroupParas}
    };
    for (uint32_t i = 0; i < tags.size(); i++) {
        const char* key = tags[i].c_str();
        if (!root.isMember(key)) {
            continue;
        }
        if (funcMap.count(key) > 0) {
            funcMap[key](key, root, animationPara);
        }
    }
}

void SymbolConfigParser::ParseSymbolCommonSubType(const char* key, const Json::Value& root,
    RSAnimationPara& animationPara)
{
    if (!root[key].isString()) {
        return;
    }

    std::string subTypeStr = root[key].asString();
    if (SYMBOL_ANIMATION_DIRECTION.count(subTypeStr) == 0) {
        return;
    }
    animationPara.commonSubType = SYMBOL_ANIMATION_DIRECTION.at(subTypeStr);
}

void SymbolConfigParser::ParseSymbolGroupParas(const char* key, const Json::Value& root,
    RSAnimationPara& animationPara)
{
    if (!root[key].isArray()) {
        return;
    }

    for (uint32_t i = 0; i < root[key].size(); i++) {
        if (!root[key][i].isArray()) {
            continue;
        }
        std::vector<RSPiecewiseParameter> piecewiseParameters;
        for (uint32_t j = 0; j < root[key][i].size(); j++) {
            if (!root[key][i][j].isObject()) {
                continue;
            }
            RSPiecewiseParameter piecewiseParameter;
            ParseSymbolPiecewisePara(root[key][i][j], piecewiseParameter);
            piecewiseParameters.push_back(piecewiseParameter);
        }

        animationPara.groupParameters.push_back(piecewiseParameters);
    }
}

void SymbolConfigParser::ParseSymbolPiecewisePara(const Json::Value& root, RSPiecewiseParameter& piecewiseParameter)
{
    std::vector<std::string> tags = {CURVE, CURVE_ARGS, DURATION, DELAY, PROPERTIES};
    static PiecewiseFuncMap funcMap = {
        {CURVE, &SymbolConfigParser::PiecewiseParaCurveCase},
        {CURVE_ARGS, &SymbolConfigParser::ParseSymbolCurveArgs},
        {DURATION, &SymbolConfigParser::PiecewiseParaDurationCase},
        {DELAY, &SymbolConfigParser::PiecewiseParaDelayCase},
        {PROPERTIES, &SymbolConfigParser::ParseSymbolProperties}
    };

    for (uint32_t i = 0; i < tags.size(); i++) {
        const char* key = tags[i].c_str();
        if (!root.isMember(key)) {
            continue;
        }
        if (funcMap.count(key) > 0) {
            funcMap[key](key, root, piecewiseParameter);
        }
    }
}

void SymbolConfigParser::PiecewiseParaCurveCase(const char* key, const Json::Value& root,
    RSPiecewiseParameter& piecewiseParameter)
{
    if (!root[key].isString()) {
        return;
    }
    const std::string curveTypeStr = root[key].asString();
    if (CURVE_TYPES.count(curveTypeStr) == 0) {
        return;
    }
    piecewiseParameter.curveType = CURVE_TYPES.at(curveTypeStr);
}

void SymbolConfigParser::PiecewiseParaDurationCase(const char* key, const Json::Value& root,
    RSPiecewiseParameter& piecewiseParameter)
{
    if (!root[key].isDouble()) {
        return;
    }
    piecewiseParameter.duration = static_cast<uint32_t>(root[key].asDouble());
}

void SymbolConfigParser::PiecewiseParaDelayCase(const char* key, const Json::Value& root,
    RSPiecewiseParameter& piecewiseParameter)
{
    if (!root[key].isDouble()) {
        return;
    }
    piecewiseParameter.delay = static_cast<int>(root[key].asDouble());
}

void SymbolConfigParser::ParseSymbolCurveArgs(const char* key, const Json::Value& root,
    RSPiecewiseParameter& piecewiseParameter)
{
    if (!root[key].isObject() || root[key].empty()) {
        return;
    }

    for (Json::Value::const_iterator iter = root[key].begin(); iter != root[key].end(); ++iter) {
        std::string name = iter.name();
        const char* memberName = name.c_str();
        if (!root[key][memberName].isDouble()) {
            continue;
        }
        piecewiseParameter.curveArgs.emplace(std::string(memberName), root[key][memberName].asDouble());
    }
}

void SymbolConfigParser::ParseSymbolProperties(const char* key, const Json::Value& root,
    RSPiecewiseParameter& piecewiseParameter)
{
    if (!root[key].isObject()) {
        return;
    }
    for (Json::Value::const_iterator iter = root[key].begin(); iter != root[key].end(); ++iter) {
        std::string name = iter.name();
        const char* memberName = name.c_str();
        if (!root[key][memberName].isArray()) {
            continue;
        }

        std::vector<float> propertyValues;
        for (uint32_t i = 0; i < root[key][memberName].size(); i++) {
            if (!root[key][memberName][i].isNumeric()) {
                continue;
            }
            propertyValues.push_back(root[key][memberName][i].asFloat());
        }
        piecewiseParameter.properties.emplace(std::string(memberName), propertyValues);
    }
}