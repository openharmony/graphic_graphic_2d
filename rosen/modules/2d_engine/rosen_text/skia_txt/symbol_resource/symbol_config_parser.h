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
#include <unordered_map>

#include "symbol_engine/drawing.h"
#include "text/hm_symbol.h"

class SymbolConfigParser {
public:
    bool ParseSymbolLayersGrouping(const Json::Value& root,
        std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig);

private:
    bool CheckSymbolLayersIsVaild(const Json::Value& root);
    void ParseOneSymbolNativeCase(const char* key, const Json::Value& root, uint16_t& nativeGlyphId);
    void ParseComponets(const Json::Value& root, std::vector<size_t>& components);
    void SymbolGlyphCase(const char* key, const Json::Value& root, RSSymbolLayersGroups& symbolLayersGroups);
    void ParseLayers(const Json::Value& root, std::vector<std::vector<size_t>>& layers);
    void ParseOneSymbolLayerCase(const char* key, const Json::Value& root, RSSymbolLayersGroups& symbolLayersGroups);
    void ParseOneSymbolRenderCase(const char* key, const Json::Value& root, RSSymbolLayersGroups& symbolLayersGroups);
    void ParseRenderModes(const Json::Value& root, std::map<RSSymbolRenderingStrategy,
        std::vector<RSRenderGroup>>& renderModesGroups);
    void ParseRenderGroups(const Json::Value& root, std::vector<RSRenderGroup>& renderGroups);
    void ParseDefaultColor(const char* defaultColor, RSRenderGroup& renderGroup);
    void ParseGroupIndexes(const Json::Value& root, std::vector<RSGroupInfo>& groupInfos);
    void ParseLayerOrMaskIndexes(const Json::Value& root, std::vector<size_t>& indexes);
    void ParseOneSymbolAnimateCase(const char* key, const Json::Value& root, RSSymbolLayersGroups& symbolLayersGroups);
    void ParseAnimationSettings(const Json::Value& root, std::vector<RSAnimationSetting>& animationSettings);
    void ParseAnimationSetting(const Json::Value& root, RSAnimationSetting& animationSetting);
    void ParseAnimationTypes(const Json::Value& root, std::vector<RSAnimationType>& animationTypes);
    void ParseAnimationType(const std::string& animationTypeStr, RSAnimationType& animationType);
    void ParseGroupSettings(const Json::Value& root, std::vector<RSGroupSetting>& groupSettings);
    void ParseGroupSetting(const Json::Value& root, RSGroupSetting& groupSetting);
    void ParseOneSymbol(const Json::Value& root, std::unordered_map<uint16_t, RSSymbolLayersGroups>& symbolConfig);
};

#endif // SYMBOL_CONFIG_PARSER_H