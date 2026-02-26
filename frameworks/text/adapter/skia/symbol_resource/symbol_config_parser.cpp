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

#include <cJSON.h>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <utility>

#include "utils/text_trace.h"

namespace OHOS {
namespace Rosen {
namespace Symbol {
namespace {

constexpr char SPECIAL_ANIMATIONS[] = "special_animations";
constexpr char COMMON_ANIMATIONS[] = "common_animations";
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
const char SLOPE[] = "slope";

constexpr uint32_t DEFAULT_COLOR_HEX_LEN = 9;
constexpr uint32_t DEFAULT_COLOR_STR_LEN = 7;
constexpr uint32_t HEX_FLAG = 16;
constexpr uint32_t BYTE_LEN = 8;

using SymbolKeyFunc = std::function<void(const char*, const cJSON*, RSSymbolLayersGroups&)>;
using SymbolKeyFuncMap = std::unordered_map<std::string, SymbolKeyFunc>;
using SymnolAniFunc = std::function<void(const char*, const cJSON*, RSAnimationPara&)>;
using SymnolAniFuncMap = std::unordered_map<std::string, SymnolAniFunc>;
using PiecewiseParaKeyFunc = std::function<void(const char*, const cJSON*, RSPiecewiseParameter&)>;
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
    {"disable", RSAnimationType::DISABLE_TYPE},
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

} // namespace

bool SymbolConfigParser::ParseSymbolConfig(const cJSON* root,
    std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig,
    std::unordered_map<RSAnimationType, RSAnimationInfo>& animationInfos)
{
    TEXT_TRACE_FUNC();
    if (root == nullptr || !cJSON_IsObject(root)) {
        return false;
    }

    std::vector<std::string> tags = {COMMON_ANIMATIONS, SPECIAL_ANIMATIONS, SYMBOL_LAYERS_GROUPING};
    for (const auto& tag : tags) {
        const char* key = tag.c_str();
        cJSON* item = cJSON_GetObjectItem(root, key);
        if (item == nullptr || !cJSON_IsArray(item)) {
            continue;
        }
        if (strcmp(key, COMMON_ANIMATIONS) == 0 || strcmp(key, SPECIAL_ANIMATIONS) == 0) {
            if (!ParseSymbolAnimations(item, animationInfos)) {
                return false;
            }
        } else if (strcmp(key, SYMBOL_LAYERS_GROUPING) == 0) {
            if (!ParseSymbolLayersGrouping(item, symbolConfig)) {
                return false;
            }
        }
    }
    return true;
}

bool SymbolConfigParser::ParseSymbolLayersGrouping(const cJSON* root,
    std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig)
{
    TEXT_TRACE_FUNC();
    if (root == nullptr || !cJSON_IsArray(root)) {
        return false;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            continue;
        }
        ParseOneSymbol(item, symbolConfig);
    }
    return true;
}

bool SymbolConfigParser::ParseOneSymbolNativeCase(const char* key, const cJSON* root, uint16_t& nativeGlyphId)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item == nullptr || !cJSON_IsNumber(item)) {
        return false;
    }
    nativeGlyphId = static_cast<uint16_t>(item->valueint);
    return true;
}

void SymbolConfigParser::ParseComponets(const cJSON* root, std::vector<size_t>& components)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item != nullptr && cJSON_IsNumber(item)) {
            components.push_back(static_cast<size_t>(item->valueint));
        }
    }
}

void SymbolConfigParser::SymbolGlyphCase(const char* key, const cJSON* root,
    RSSymbolLayersGroups& symbolLayersGroups)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item != nullptr && cJSON_IsNumber(item)) {
        symbolLayersGroups.symbolGlyphId = static_cast<uint16_t>(item->valueint);
    }
}

void SymbolConfigParser::ParseLayers(const cJSON* root, std::vector<std::vector<size_t>>& layers)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* layerItem = cJSON_GetArrayItem(root, i);
        if (layerItem == nullptr || !cJSON_IsObject(layerItem)) {
            continue;
        }

        cJSON* componentsItem = cJSON_GetObjectItem(layerItem, COMPONENTS);
        if (componentsItem == nullptr || !cJSON_IsArray(componentsItem)) {
            continue;
        }
        std::vector<size_t> components;
        ParseComponets(componentsItem, components);
        layers.push_back(components);
    }
}

void SymbolConfigParser::ParseOneSymbolLayerCase(const char* key, const cJSON* root,
    RSSymbolLayersGroups& symbolLayersGroups)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item != nullptr && cJSON_IsArray(item)) {
        ParseLayers(item, symbolLayersGroups.layers);
    }
}

void SymbolConfigParser::ParseOneSymbolRenderCase(const char* key, const cJSON* root,
    RSSymbolLayersGroups& symbolLayersGroups)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item != nullptr && cJSON_IsArray(item)) {
        ParseRenderModes(item, symbolLayersGroups.renderModeGroups);
    }
}

void SymbolConfigParser::ParseRenderModes(const cJSON* root,
    std::map<RSSymbolRenderingStrategy, std::vector<RSRenderGroup>>& renderModesGroups)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            continue;
        }

        RSSymbolRenderingStrategy renderingStrategy;
        cJSON* modeItem = cJSON_GetObjectItem(item, MODE);
        if (modeItem != nullptr && cJSON_IsString(modeItem)) {
            std::string modeValue = modeItem->valuestring;
            if (RENDER_STRATEGY.count(modeValue) == 0) {
                continue;
            }
            renderingStrategy = RENDER_STRATEGY.at(modeValue);
        } else {
            continue;
        }

        std::vector<RSRenderGroup> renderGroups;
        cJSON* renderGroupsItem = cJSON_GetObjectItem(item, RENDER_GROUPS);
        if (renderGroupsItem != nullptr && cJSON_IsArray(renderGroupsItem)) {
            ParseRenderGroups(renderGroupsItem, renderGroups);
        }
        renderModesGroups.emplace(renderingStrategy, renderGroups);
    }
}

void SymbolConfigParser::ParseRenderGroups(const cJSON* root, std::vector<RSRenderGroup>& renderGroups)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            continue;
        }

        RSRenderGroup renderGroup;
        cJSON* groupIndexesItem = cJSON_GetObjectItem(item, GROUP_INDEXES);
        if (groupIndexesItem != nullptr && cJSON_IsArray(groupIndexesItem)) {
            ParseGroupIndexes(groupIndexesItem, renderGroup.groupInfos);
        }
        cJSON* defaultColorItem = cJSON_GetObjectItem(item, DEFAULT_COLOR);
        if (defaultColorItem != nullptr && cJSON_IsString(defaultColorItem)) {
            ParseDefaultColor(defaultColorItem->valuestring, renderGroup);
        }
        cJSON* fixAlphaItem = cJSON_GetObjectItem(item, FIX_ALPHA);
        if (fixAlphaItem != nullptr && cJSON_IsNumber(fixAlphaItem)) {
            renderGroup.color.a = fixAlphaItem->valuedouble;
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

void SymbolConfigParser::ParseGroupIndexes(const cJSON* root, std::vector<RSGroupInfo>& groupInfos)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            continue;
        }

        RSGroupInfo groupInfo;
        cJSON* layerIndexesItem = cJSON_GetObjectItem(item, LAYER_INDEXES);
        if (layerIndexesItem != nullptr && cJSON_IsArray(layerIndexesItem)) {
            ParseLayerOrMaskIndexes(layerIndexesItem, groupInfo.layerIndexes);
        }
        cJSON* maskIndexesItem = cJSON_GetObjectItem(item, MASK_INDEXES);
        if (maskIndexesItem != nullptr && cJSON_IsArray(maskIndexesItem)) {
            ParseLayerOrMaskIndexes(maskIndexesItem, groupInfo.maskIndexes);
        }
        groupInfos.push_back(groupInfo);
    }
}

void SymbolConfigParser::ParseLayerOrMaskIndexes(const cJSON* root, std::vector<size_t>& indexes)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item != nullptr && cJSON_IsNumber(item)) {
            indexes.push_back(static_cast<size_t>(item->valueint));
        }
    }
}

void SymbolConfigParser::ParseOneSymbolAnimateCase(const char* key, const cJSON* root,
    RSSymbolLayersGroups& symbolLayersGroups)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item != nullptr && cJSON_IsArray(item)) {
        ParseAnimationSettings(item, symbolLayersGroups.animationSettings);
    }
}

void SymbolConfigParser::ParseAnimationSettings(const cJSON* root,
    std::vector<RSAnimationSetting>& animationSettings)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            continue;
        }
        RSAnimationSetting animationSetting;
        ParseAnimationSetting(item, animationSetting);
        animationSettings.push_back(animationSetting);
    }
}

void SymbolConfigParser::ParseAnimationSetting(const cJSON* root, RSAnimationSetting& animationSetting)
{
    if (root == nullptr) {
        return;
    }

    cJSON* animationTypesItem = cJSON_GetObjectItem(root, ANIMATION_TYPES);
    if (animationTypesItem != nullptr && cJSON_IsArray(animationTypesItem)) {
        ParseAnimationTypes(animationTypesItem, animationSetting.animationTypes);
    }

    cJSON* groupSettingsItem = cJSON_GetObjectItem(root, GROUP_SETTINGS);
    if (groupSettingsItem != nullptr && cJSON_IsArray(groupSettingsItem)) {
        ParseGroupSettings(groupSettingsItem, animationSetting.groupSettings);
    }

    cJSON* commonSubTypeItem = cJSON_GetObjectItem(root, COMMON_SUB_TYPE);
    if (commonSubTypeItem != nullptr && cJSON_IsString(commonSubTypeItem)) {
        const std::string subTypeStr = commonSubTypeItem->valuestring;
        MatchCommonSubType(subTypeStr, animationSetting.commonSubType);
    }

    cJSON* slopeItem = cJSON_GetObjectItem(root, SLOPE);
    if (slopeItem != nullptr && cJSON_IsNumber(slopeItem)) {
        animationSetting.slope = slopeItem->valuedouble;
    }
}

void SymbolConfigParser::ParseAnimationTypes(const cJSON* root, std::vector<RSAnimationType>& animationTypes)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsString(item)) {
            continue;
        }
        const std::string animationTypeStr = item->valuestring;
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

void SymbolConfigParser::ParseGroupSettings(const cJSON* root, std::vector<RSGroupSetting>& groupSettings)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            continue;
        }
        RSGroupSetting groupSetting;
        ParseGroupSetting(item, groupSetting);
        groupSettings.push_back(groupSetting);
    }
}

void SymbolConfigParser::MatchCommonSubType(const std::string& subTypeStr, RSCommonSubType& commonSubType)
{
    if (SYMBOL_ANIMATION_DIRECTION.count(subTypeStr) == 0) {
        return;
    }
    commonSubType = SYMBOL_ANIMATION_DIRECTION.at(subTypeStr);
}

void SymbolConfigParser::ParseGroupSetting(const cJSON* root, RSGroupSetting& groupSetting)
{
    if (root == nullptr) {
        return;
    }

    cJSON* groupIndexesItem = cJSON_GetObjectItem(root, GROUP_INDEXES);
    if (groupIndexesItem != nullptr && cJSON_IsArray(groupIndexesItem)) {
        ParseGroupIndexes(groupIndexesItem, groupSetting.groupInfos);
    }

    cJSON* animationIndexItem = cJSON_GetObjectItem(root, ANIMATION_INDEX);
    if (animationIndexItem != nullptr && cJSON_IsNumber(animationIndexItem)) {
        groupSetting.animationIndex = animationIndexItem->valueint;
    }
}

void SymbolConfigParser::ParseOneSymbol(const cJSON* root,
    std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig)
{
    if (root == nullptr) {
        return;
    }

    cJSON* nativeGlyphIdItem = cJSON_GetObjectItem(root, NATIVE_GLYPH_ID);
    if (nativeGlyphIdItem == nullptr) {
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
    for (const auto& tag : tags) {
        const char* key = tag.c_str();
        cJSON* item = cJSON_GetObjectItem(root, key);
        if (item == nullptr) {
            continue;
        }

        if (strcmp(key, SYMBOL_GLYPH_ID) == 0) {
            SymbolGlyphCase(key, root, symbolLayersGroups);
        } else if (strcmp(key, LAYERS) == 0) {
            ParseOneSymbolLayerCase(key, root, symbolLayersGroups);
        } else if (strcmp(key, RENDER_MODES) == 0) {
            ParseOneSymbolRenderCase(key, root, symbolLayersGroups);
        } else if (strcmp(key, ANIMATION_SETTINGS) == 0) {
            ParseOneSymbolAnimateCase(key, root, symbolLayersGroups);
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

bool SymbolConfigParser::ParseSymbolAnimations(const cJSON* root,
    std::unordered_map<RSAnimationType, RSAnimationInfo>& animationInfos)
{
    TEXT_TRACE_FUNC();
    if (root == nullptr || !cJSON_IsArray(root)) {
        return false;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            continue;
        }

        cJSON* animationTypeItem = cJSON_GetObjectItem(item, ANIMATION_TYPE);
        cJSON* animationParametersItem = cJSON_GetObjectItem(item, ANIMATION_PARAMETERS);
        if (animationTypeItem == nullptr || animationParametersItem == nullptr) {
            continue;
        }

        RSAnimationInfo animationInfo;
        if (!cJSON_IsString(animationTypeItem)) {
            continue;
        }
        const std::string animationType = animationTypeItem->valuestring;
        ParseAnimationType(animationType, animationInfo.animationType);

        if (!cJSON_IsArray(animationParametersItem)) {
            continue;
        }
        ParseSymbolAnimationParas(animationParametersItem, animationInfo.animationParas);
        animationInfos.emplace(animationInfo.animationType, animationInfo);
    }
    return true;
}

void SymbolConfigParser::ParseSymbolAnimationParas(const cJSON* root,
    std::map<uint32_t, RSAnimationPara>& animationParas)
{
    if (root == nullptr || !cJSON_IsArray(root)) {
        return;
    }
    int size = cJSON_GetArraySize(root);
    for (int i = 0; i < size; i++) {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            continue;
        }
        RSAnimationPara animationPara;
        ParseSymbolAnimationPara(item, animationPara);
        uint32_t attributeKey = EncodeAnimationAttribute(animationPara.groupParameters.size(),
            animationPara.animationMode, animationPara.commonSubType);
        animationParas.emplace(attributeKey, animationPara);
    }
}

void SymbolConfigParser::ParseSymbolAnimationPara(const cJSON* root, RSAnimationPara& animationPara)
{
    if (root == nullptr) {
        return;
    }

    std::vector<std::string> tags = {ANIMATION_MODE, COMMON_SUB_TYPE, GROUP_PARAMETERS};
    for (const auto& tag : tags) {
        const char* key = tag.c_str();
        cJSON* item = cJSON_GetObjectItem(root, key);
        if (item == nullptr) {
            continue;
        }

        if (strcmp(key, ANIMATION_MODE) == 0) {
            if (cJSON_IsNumber(item)) {
                animationPara.animationMode = static_cast<uint16_t>(item->valueint);
            }
        } else if (strcmp(key, COMMON_SUB_TYPE) == 0) {
            ParseSymbolCommonSubType(key, root, animationPara);
        } else if (strcmp(key, GROUP_PARAMETERS) == 0) {
            ParseSymbolGroupParas(key, root, animationPara);
        }
    }
}

void SymbolConfigParser::ParseSymbolCommonSubType(const char* key, const cJSON* root,
    RSAnimationPara& animationPara)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item == nullptr || !cJSON_IsString(item)) {
        return;
    }

    std::string subTypeStr = item->valuestring;
    MatchCommonSubType(subTypeStr, animationPara.commonSubType);
}

void SymbolConfigParser::ParseSymbolGroupParas(const char* key, const cJSON* root,
    RSAnimationPara& animationPara)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item == nullptr || !cJSON_IsArray(item)) {
        return;
    }

    int size = cJSON_GetArraySize(item);
    for (int i = 0; i < size; i++) {
        cJSON* groupItem = cJSON_GetArrayItem(item, i);
        if (groupItem == nullptr || !cJSON_IsArray(groupItem)) {
            continue;
        }

        std::vector<RSPiecewiseParameter> piecewiseParameters;
        int innerSize = cJSON_GetArraySize(groupItem);
        for (int j = 0; j < innerSize; j++) {
            cJSON* paraItem = cJSON_GetArrayItem(groupItem, j);
            if (paraItem == nullptr || !cJSON_IsObject(paraItem)) {
                continue;
            }
            RSPiecewiseParameter piecewiseParameter;
            ParseSymbolPiecewisePara(paraItem, piecewiseParameter);
            piecewiseParameters.push_back(piecewiseParameter);
        }

        animationPara.groupParameters.push_back(piecewiseParameters);
    }
}

void SymbolConfigParser::ParseSymbolPiecewisePara(const cJSON* root, RSPiecewiseParameter& piecewiseParameter)
{
    if (root == nullptr || !cJSON_IsObject(root)) {
        return;
    }

    std::vector<std::string> tags = {CURVE, CURVE_ARGS, DURATION, DELAY, PROPERTIES};
    for (const auto& tag : tags) {
        const char* key = tag.c_str();
        cJSON* item = cJSON_GetObjectItem(root, key);
        if (item == nullptr) {
            continue;
        }

        if (strcmp(key, CURVE) == 0) {
            PiecewiseParaCurveCase(key, root, piecewiseParameter);
        } else if (strcmp(key, CURVE_ARGS) == 0) {
            ParseSymbolCurveArgs(key, root, piecewiseParameter);
        } else if (strcmp(key, DURATION) == 0) {
            PiecewiseParaDurationCase(key, root, piecewiseParameter);
        } else if (strcmp(key, DELAY) == 0) {
            PiecewiseParaDelayCase(key, root, piecewiseParameter);
        } else if (strcmp(key, PROPERTIES) == 0) {
            ParseSymbolProperties(key, root, piecewiseParameter);
        }
    }
}

void SymbolConfigParser::PiecewiseParaCurveCase(const char* key, const cJSON* root,
    RSPiecewiseParameter& piecewiseParameter)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item == nullptr || !cJSON_IsString(item)) {
        return;
    }
    const std::string curveTypeStr = item->valuestring;
    if (CURVE_TYPES.count(curveTypeStr) == 0) {
        return;
    }
    piecewiseParameter.curveType = CURVE_TYPES.at(curveTypeStr);
}

void SymbolConfigParser::PiecewiseParaDurationCase(const char* key, const cJSON* root,
    RSPiecewiseParameter& piecewiseParameter)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item == nullptr || !cJSON_IsNumber(item)) {
        return;
    }
    piecewiseParameter.duration = static_cast<uint32_t>(item->valuedouble);
}

void SymbolConfigParser::PiecewiseParaDelayCase(const char* key, const cJSON* root,
    RSPiecewiseParameter& piecewiseParameter)
{
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (item == nullptr || !cJSON_IsNumber(item)) {
        return;
    }
    piecewiseParameter.delay = static_cast<int>(item->valuedouble);
}

void SymbolConfigParser::ParseSymbolCurveArgs(const char* key, const cJSON* root,
    RSPiecewiseParameter& piecewiseParameter)
{
    cJSON* obj = cJSON_GetObjectItem(root, key);
    cJSON* item = nullptr;
    cJSON_ArrayForEach(item, obj) {
        const char* memberName = item->string;
        if (memberName != nullptr && cJSON_IsNumber(item)) {
            piecewiseParameter.curveArgs.emplace(std::string(memberName), item->valuedouble);
        }
    }
}

void SymbolConfigParser::ParseSymbolProperties(const char* key, const cJSON* root,
    RSPiecewiseParameter& piecewiseParameter)
{
    cJSON* obj = cJSON_GetObjectItem(root, key);
    cJSON* item = nullptr;
    cJSON_ArrayForEach(item, obj) {
        const char* memberName = item->string;
        if (memberName == nullptr || !cJSON_IsArray(item)) {
            continue;
        }
        std::vector<float> propertyValues;
        int size = cJSON_GetArraySize(item);
        for (int i = 0; i < size; i++) {
            cJSON* valueItem = cJSON_GetArrayItem(item, i);
            if (valueItem != nullptr && cJSON_IsNumber(valueItem)) {
                propertyValues.push_back(static_cast<float>(valueItem->valuedouble));
            }
        }
        piecewiseParameter.properties.emplace(std::string(memberName), propertyValues);
    }
}

} // namespace Symbol
} // namespace Rosen
} // namespace OHOS
