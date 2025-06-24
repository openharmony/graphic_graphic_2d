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

#include <gtest/gtest.h>
#include <json/json.h>
#include "symbol_resource/symbol_config_parser.h"

using namespace testing;
using namespace testing::ext;

class SymbolConfigParserTest : public testing::Test {
public:
    void SetUp() override
    {
        BuildValidLayersGroupingJson();
        BuildValidAnimationsJson();
    }

    Json::Value rootLayers_;
    Json::Value rootAnimations_;
private:
    void BuildValidLayersGroupingJson()
    {
        const char* jsonStr = R"({
            "symbol_layers_grouping": [
                {
                    "native_glyph_id": 1001,
                    "symbol_glyph_id": 2001,
                    "layers": [
                        { "components": [1, 2, 3] },
                        { "components": [4, 5] }
                    ],
                    "render_modes": [
                        {
                            "mode": "monochrome",
                            "render_groups": [
                                {
                                    "group_indexes": [ { "layer_indexes": [0] } ],
                                    "default_color": "#FF00FF",
                                    "fix_alpha": 0.5
                                }
                            ]
                        }
                    ],
                    "animation_settings": [
                        {
                            "animation_types": ["scale"],
                            "group_settings": [
                                {
                                    "group_indexes": [ { "layer_indexes": [0] } ],
                                    "animation_index": 2
                                }
                            ]
                        }
                    ]
                }
            ]
        })";
        Json::Reader reader;
        EXPECT_TRUE(reader.parse(jsonStr, rootLayers_));
    }

    void BuildValidAnimationsJson()
    {
        const char* jsonStr = R"({
            "animations": [
                {
                    "animation_type": "scale",
                    "animation_parameters": [
                        {
                            "animation_mode": 1,
                            "common_sub_type": "up",
                            "group_parameters": [
                                [
                                    { "curve": "spring", "duration": 200 }
                                ]
                            ]
                        }
                    ]
                }
            ]
        })";
        Json::Reader reader;
        EXPECT_TRUE(reader.parse(jsonStr, rootAnimations_));
    }
};

/*
 * @tc.name: SymbolConfigParserTest001
 * @tc.desc: test for lack native_glyph_id
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest001, TestSize.Level0)
{
    rootLayers_["symbol_layers_grouping"][0].removeMember("native_glyph_id");
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(SymbolConfigParser::ParseSymbolLayersGrouping(rootLayers_, symbolConfig));
    EXPECT_TRUE(symbolConfig.empty());
}

/*
 * @tc.name: SymbolConfigParserTest002
 * @tc.desc: test for error native_glyph_id
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest002, TestSize.Level0)
{
    rootLayers_["symbol_layers_grouping"][0]["native_glyph_id"] = "invalid_id";
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(SymbolConfigParser::ParseSymbolLayersGrouping(rootLayers_, symbolConfig));
    EXPECT_TRUE(symbolConfig.empty());
}

/*
 * @tc.name: SymbolConfigParserTest003
 * @tc.desc: test for invalid color
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest003, TestSize.Level0)
{
    rootLayers_["symbol_layers_grouping"][0]["render_modes"][0]["render_groups"][0]["default_color"] = "INVALID_COLOR";
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(SymbolConfigParser::ParseSymbolLayersGrouping(rootLayers_, symbolConfig));
    auto& renderGroups = symbolConfig.at(1001).renderModeGroups.begin()->second;
    // default value is 0
    EXPECT_EQ(renderGroups[0].color.r, 0);
}

/*
 * @tc.name: SymbolConfigParserTest004
 * @tc.desc: test for lack animation_type
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest004, TestSize.Level0)
{
    rootAnimations_["animations"][0].removeMember("animation_type");
    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    SymbolConfigParser::ParseSymbolAnimations(rootAnimations_["animations"], animationInfos);
    EXPECT_TRUE(animationInfos.empty());
}

/*
 * @tc.name: SymbolConfigParserTest005
 * @tc.desc: test for invalid common_sub_type
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest005, TestSize.Level0)
{
    rootAnimations_["animations"][0]["animation_parameters"][0]["common_sub_type"] = "invalid_direction";
    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    SymbolConfigParser::ParseSymbolAnimations(rootAnimations_["animations"], animationInfos);
    auto& animInfo = animationInfos.at(RSAnimationType::SCALE_TYPE);
    EXPECT_EQ(animInfo.animationParas.begin()->second.commonSubType, RSCommonSubType::DOWN);
}

/*
 * @tc.name: SymbolConfigParserTest006
 * @tc.desc: test for empty layers
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest006, TestSize.Level0)
{
    rootLayers_["symbol_layers_grouping"][0]["layers"] = Json::arrayValue;
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(SymbolConfigParser::ParseSymbolLayersGrouping(rootLayers_, symbolConfig));
    EXPECT_TRUE(symbolConfig.at(1001).layers.empty());
}

/*
 * @tc.name: SymbolConfigParserTest007
 * @tc.desc: test for valid symbol layers data
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest007, TestSize.Level0)
{
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(SymbolConfigParser::ParseSymbolLayersGrouping(rootLayers_, symbolConfig));
    auto& group = symbolConfig.at(1001);
    EXPECT_EQ(group.symbolGlyphId, 2001);
    EXPECT_EQ(group.layers.size(), 2);
    EXPECT_EQ(group.layers[0], std::vector<size_t>({1, 2, 3}));
    EXPECT_EQ(group.animationSettings.size(), 1);
    EXPECT_EQ(group.animationSettings[0].animationTypes[0], RSAnimationType::SCALE_TYPE);
}

/*
 * @tc.name: SymbolConfigParserTest008
 * @tc.desc: test for valid symbol animations data
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest008, TestSize.Level0)
{
    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    SymbolConfigParser::ParseSymbolAnimations(rootAnimations_["animations"], animationInfos);
    EXPECT_FALSE(animationInfos.empty());
    auto& animPara = animationInfos.at(RSAnimationType::SCALE_TYPE).animationParas.begin()->second;
    EXPECT_EQ(animPara.animationMode, 1);
    EXPECT_EQ(animPara.commonSubType, RSCommonSubType::UP);
}