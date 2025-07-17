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

#ifndef SYMBOL_CONFIG_PARSER_H
#define SYMBOL_CONFIG_PARSER_H

#include <cstddef>
#include <cstdint>
#include <json/json.h>
#include <mutex>
#include <unordered_map>

#include "symbol_engine/drawing.h"
#include "text/hm_symbol.h"

namespace OHOS {
namespace Rosen {
namespace Symbol {

class SymbolConfigParser {
public:
    static bool ParseSymbolConfig(const Json::Value& root,
        std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig,
        std::unordered_map<RSAnimationType, RSAnimationInfo>& animationInfos);
    static bool ParseSymbolLayersGrouping(
        const Json::Value& root, std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig);
    static bool ParseSymbolAnimations(
        const Json::Value& root, std::unordered_map<RSAnimationType, RSAnimationInfo>& animationInfos);

private:
    static bool ParseOneSymbolNativeCase(const char* key, const Json::Value& root, uint16_t& nativeGlyphId);
    static void ParseComponets(const Json::Value& root, std::vector<size_t>& components);
    static void SymbolGlyphCase(const char* key, const Json::Value& root, RSSymbolLayersGroups& symbolLayersGroups);
    static void ParseLayers(const Json::Value& root, std::vector<std::vector<size_t>>& layers);
    static void ParseOneSymbolLayerCase(const char* key, const Json::Value& root,
        RSSymbolLayersGroups& symbolLayersGroups);
    static void ParseOneSymbolRenderCase(const char* key, const Json::Value& root,
        RSSymbolLayersGroups& symbolLayersGroups);
    static void ParseRenderModes(const Json::Value& root, std::map<RSSymbolRenderingStrategy,
        std::vector<RSRenderGroup>>& renderModesGroups);
    static void ParseRenderGroups(const Json::Value& root, std::vector<RSRenderGroup>& renderGroups);
    static void ParseDefaultColor(const char* defaultColor, RSRenderGroup& renderGroup);
    static void ParseGroupIndexes(const Json::Value& root, std::vector<RSGroupInfo>& groupInfos);
    static void ParseLayerOrMaskIndexes(const Json::Value& root, std::vector<size_t>& indexes);
    static void ParseOneSymbolAnimateCase(const char* key, const Json::Value& root,
        RSSymbolLayersGroups& symbolLayersGroups);
    static void ParseAnimationSettings(const Json::Value& root, std::vector<RSAnimationSetting>& animationSettings);
    static void ParseAnimationSetting(const Json::Value& root, RSAnimationSetting& animationSetting);
    static void ParseAnimationTypes(const Json::Value& root, std::vector<RSAnimationType>& animationTypes);
    static void ParseAnimationType(const std::string& animationTypeStr, RSAnimationType& animationType);
    static void ParseGroupSettings(const Json::Value& root, std::vector<RSGroupSetting>& groupSettings);
    static void ParseGroupSetting(const Json::Value& root, RSGroupSetting& groupSetting);
    static void ParseOneSymbol(const Json::Value& root,
        std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig);
    static uint32_t EncodeAnimationAttribute(uint16_t groupSum, uint16_t animationMode, RSCommonSubType commonSubType);
    static void ParseSymbolAnimationParas(const Json::Value& root,
        std::map<uint32_t, RSAnimationPara>& animationParas);
    static void ParseSymbolAnimationPara(const Json::Value& root, RSAnimationPara& animationPara);
    static void ParseSymbolCommonSubType(const char* key, const Json::Value& root, RSAnimationPara& animationPara);
    static void MatchCommonSubType(const std::string& subTypeStr, RSCommonSubType& commonSubType);
    static void ParseSymbolGroupParas(const char* key, const Json::Value& root, RSAnimationPara& animationPara);
    static void ParseSymbolPiecewisePara(const Json::Value& root, RSPiecewiseParameter& piecewiseParameter);
    static void PiecewiseParaCurveCase(const char* key, const Json::Value& root,
        RSPiecewiseParameter& piecewiseParameter);
    static void PiecewiseParaDurationCase(const char* key, const Json::Value& root,
        RSPiecewiseParameter& piecewiseParameter);
    static void PiecewiseParaDelayCase(const char* key, const Json::Value& root,
        RSPiecewiseParameter& piecewiseParameter);
    static void ParseSymbolCurveArgs(const char* key, const Json::Value& root,
        RSPiecewiseParameter& piecewiseParameter);
    static void ParseSymbolProperties(const char* key, const Json::Value& root,
        RSPiecewiseParameter& piecewiseParameter);
};
} // namespace Symbol
} // namespace Rosen
} // namespace OHOS
#endif // SYMBOL_CONFIG_PARSER_H