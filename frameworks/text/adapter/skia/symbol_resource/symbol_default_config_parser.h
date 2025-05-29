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

#ifndef SYMBOL_DEFAULT_CONFIG_PARSER_H
#define SYMBOL_DEFAULT_CONFIG_PARSER_H

#include <json/json.h>
#include <mutex>
#include <unordered_map>

#include "text/hm_symbol.h"

namespace OHOS {
namespace Rosen {
namespace Symbol {
namespace RSDrawing = OHOS::Rosen::Drawing;
class SymbolDefaultConfigParser {
public:
    static SymbolDefaultConfigParser* GetInstance();

    RSDrawing::DrawingSymbolLayersGroups GetSymbolLayersGroups(uint16_t glyphId);

    std::vector<std::vector<RSDrawing::DrawingPiecewiseParameter>> GetGroupParameters(
        RSDrawing::DrawingAnimationType type, uint16_t groupSum, uint16_t animationMode = 0,
        RSDrawing::DrawingCommonSubType commonSubType = RSDrawing::DrawingCommonSubType::DOWN);

    int ParseConfigOfHmSymbol(const char* filePath);

    bool GetInit() const { return isInit_; }

    void SetInit(const bool init) { isInit_ = init; }

    void Lock() { hmSymbolMut_.lock(); }

    void Unlock() { hmSymbolMut_.unlock(); }

    void Clear()
    {
        hmSymbolConfig_.clear();
        animationInfos_.clear();
        isInit_ = false;
    }

private:
    SymbolDefaultConfigParser() {}
    SymbolDefaultConfigParser(const SymbolDefaultConfigParser& hsc) = delete;
    SymbolDefaultConfigParser& operator=(const SymbolDefaultConfigParser& hsc) = delete;
    SymbolDefaultConfigParser(const SymbolDefaultConfigParser&& hsc) = delete;
    SymbolDefaultConfigParser& operator=(const SymbolDefaultConfigParser&& hsc) = delete;

    std::unordered_map<uint16_t, RSDrawing::DrawingSymbolLayersGroups> hmSymbolConfig_;
    std::unordered_map<RSDrawing::DrawingAnimationType, RSDrawing::DrawingAnimationInfo> animationInfos_;
    std::mutex hmSymbolMut_;
    bool isInit_ = false;

    const uint32_t defaultColorHexLen = 9;
    const uint32_t defaultColorStrLen = 7;
    const uint32_t hexFlag = 16;
    const uint32_t twoBytesBitsLen = 16;
    const uint32_t oneByteBitsLen = 8;

    int CheckConfigFile(const char* fname, Json::Value& root);

    uint32_t EncodeAnimationAttribute(
        uint16_t groupSum, uint16_t animationMode, RSDrawing::DrawingCommonSubType commonSubType);

    void ParseSymbolAnimations(const Json::Value& root,
        std::unordered_map<RSDrawing::DrawingAnimationType, RSDrawing::DrawingAnimationInfo>* animationInfos);
    void ParseSymbolAnimationParas(
        const Json::Value& root, std::map<uint32_t, RSDrawing::DrawingAnimationPara>& animationParas);
    void ParseSymbolAnimationPara(const Json::Value& root, RSDrawing::DrawingAnimationPara& animationPara);
    void ParseSymbolGroupParas(
        const Json::Value& root, std::vector<std::vector<RSDrawing::DrawingPiecewiseParameter>>& groupParameters);
    void ParseSymbolPiecewisePara(const Json::Value& root, RSDrawing::DrawingPiecewiseParameter& piecewiseParameter);
    void ParseSymbolCurveArgs(const Json::Value& root, std::map<std::string, float>& curveArgs);
    void ParseSymbolProperties(const Json::Value& root, std::map<std::string, std::vector<float>>& properties);

    void ParseSymbolLayersGrouping(const Json::Value& root);
    void ParseOneSymbol(
        const Json::Value& root, std::unordered_map<uint16_t, RSDrawing::DrawingSymbolLayersGroups>* hmSymbolConfig);
    void ParseLayers(const Json::Value& root, std::vector<std::vector<size_t>>& layers);
    void ParseRenderModes(const Json::Value& root,
        std::map<RSDrawing::DrawingSymbolRenderingStrategy, std::vector<RSDrawing::DrawingRenderGroup>>&
            renderModesGroups);
    void ParseComponets(const Json::Value& root, std::vector<size_t>& components);
    void ParseRenderGroups(const Json::Value& root, std::vector<RSDrawing::DrawingRenderGroup>& renderGroups);
    void ParseGroupIndexes(const Json::Value& root, std::vector<RSDrawing::DrawingGroupInfo>& groupInfos);
    void ParseLayerOrMaskIndexes(const Json::Value& root, std::vector<size_t>& indexes);
    void ParseDefaultColor(const std::string& defaultColorStr, RSDrawing::DrawingRenderGroup& renderGroup);
    void ParseAnimationSettings(
        const Json::Value& root, std::vector<RSDrawing::DrawingAnimationSetting>& animationSettings);
    void ParseAnimationSetting(const Json::Value& root, RSDrawing::DrawingAnimationSetting& animationSetting);
    void ParseAnimationType(const std::string& animationTypeStr, RSDrawing::DrawingAnimationType& animationType);
    void ParseAnimationTypes(const Json::Value& root, std::vector<RSDrawing::DrawingAnimationType>& animationTypes);
    void ParseGroupSettings(const Json::Value& root, std::vector<RSDrawing::DrawingGroupSetting>& groupSettings);
    void ParseGroupSetting(const Json::Value& root, RSDrawing::DrawingGroupSetting& groupSetting);
    void ParseSymbolCommonSubType(const std::string& subTypeStr, RSDrawing::DrawingCommonSubType& commonSubType);

    void ParseOneSymbolNativeCase(const char* key, const Json::Value& root,
        RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups, uint16_t& nativeGlyphId);
    void ParseOneSymbolLayerCase(
        const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups);
    void ParseOneSymbolRenderCase(
        const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups);
    void ParseOneSymbolAnimateCase(
        const char* key, const Json::Value& root, RSDrawing::DrawingSymbolLayersGroups& symbolLayersGroups);
};
} // namespace Symbol
} // namespace Rosen
} // namespace OHOS
#endif