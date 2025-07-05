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
#include <json/value.h>

#include "symbol_resource/symbol_config_parser.h"

using namespace testing;
using namespace testing::ext;

namespace {
const char* ROOT_LAYERS_JSON_STR = R"({
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
                },
                {
                    "animation_types": ["disable"],
                    "common_sub_type": "up",
                    "slope": -1.0000001192092896,
                    "group_settings": [{
                        "animation_index": -1,
                        "group_indexes": [{
                            "layer_indexes": [0]
                        }]
                    }, {
                        "animation_index": 0,
                        "group_indexes": [{
                            "mask_indexes": [1]
                        }]
                    }, {
                        "animation_index": 0,
                        "group_indexes": [{
                            "layer_indexes": [2]
                        }]
                    }]
                }
            ]
        }
    ]
})";

const char* ROOT_ANIMATIONS_JSON_STR = R"({
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
        },
        {
            "animation_type": "disable",
            "animation_parameters": [
                {
                    "animation_mode": 0,
                    "common_sub_type": "up",
                    "group_parameters": [
                        [
                            {
                                "curve": "friction",
                                "curve_args": {
                                    "ctrlX1": 0.2,
                                    "ctrlY1": 0,
                                    "ctrlX2": 0.2,
                                    "ctrlY2": 1
                                },
                                "duration": 200,
                                "delay": 0,
                                "properties": {
                                    "sx": [
                                        1,
                                        0.9
                                    ],
                                    "sy": [
                                        1,
                                        0.9
                                    ]
                                }
                            },
                            {
                                "curve": "friction",
                                "curve_args": {
                                    "ctrlX1": 0.2,
                                    "ctrlY1": 0,
                                    "ctrlX2": 0.2,
                                    "ctrlY2": 1
                                },
                                "duration": 150,
                                "delay": 200,
                                "properties": {
                                    "sx": [
                                        0.9,
                                        1.07
                                    ],
                                    "sy": [
                                        0.9,
                                        1.07
                                    ]
                                }
                            }
                        ]
                    ]
                },
                {
                    "animation_mode": 0,
                    "common_sub_type": "down",
                    "group_parameters": []
                }
            ]
        }
    ]
})";

const char* ROOT_INVALID_JSON_STR = R"({
    "test": [],
    "common_animations": {},
    "special_animations": {},
    "symbol_layers_grouping": {}
})";
}

class SymbolConfigParserTest : public testing::Test {
public:
    void SetUp() override
    {
        BuildValidLayersGroupingJson();
        BuildValidAnimationsJson();
        BuildInvalidJson();
    }

    Json::Value rootLayers_;
    Json::Value rootAnimations_;
    Json::Value rootInvalid_;

private:
    void BuildValidLayersGroupingJson()
    {
        Json::Reader reader;
        EXPECT_TRUE(reader.parse(ROOT_LAYERS_JSON_STR, rootLayers_));
    }

    void BuildValidAnimationsJson()
    {
        Json::Reader reader;
        EXPECT_TRUE(reader.parse(ROOT_ANIMATIONS_JSON_STR, rootAnimations_));
    }

    void BuildInvalidJson()
    {
        Json::Reader reader;
        EXPECT_TRUE(reader.parse(ROOT_INVALID_JSON_STR, rootInvalid_));
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
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        rootLayers_["symbol_layers_grouping"], symbolConfig));
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
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        rootLayers_["symbol_layers_grouping"], symbolConfig));
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
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        rootLayers_["symbol_layers_grouping"], symbolConfig));
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
    rootAnimations_["animations"][1].removeMember("animation_type");
    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(rootAnimations_["animations"], animationInfos);
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
    OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(rootAnimations_["animations"], animationInfos);
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
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        rootLayers_["symbol_layers_grouping"], symbolConfig));
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
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        rootLayers_["symbol_layers_grouping"], symbolConfig));
    auto& group = symbolConfig.at(1001);
    EXPECT_EQ(group.symbolGlyphId, 2001);
    EXPECT_EQ(group.layers.size(), 2);
    EXPECT_EQ(group.layers[0], std::vector<size_t>({1, 2, 3}));
    EXPECT_EQ(group.animationSettings.size(), 2);
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
    OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(rootAnimations_["animations"], animationInfos);
    EXPECT_FALSE(animationInfos.empty());
    auto& animPara = animationInfos.at(RSAnimationType::SCALE_TYPE).animationParas.begin()->second;
    EXPECT_EQ(animPara.animationMode, 1);
    EXPECT_EQ(animPara.commonSubType, RSCommonSubType::UP);
}

/*
 * @tc.name: SymbolConfigParserTest009
 * @tc.desc: test for not array layers
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest009, TestSize.Level0)
{
    rootLayers_["symbol_layers_grouping"] = Json::objectValue;
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_FALSE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        rootLayers_["symbol_layers_grouping"], symbolConfig));
}

/*
 * @tc.name: SymbolConfigParserTest010
 * @tc.desc: test for not array animation
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest010, TestSize.Level0)
{
    rootAnimations_["animations"] = Json::objectValue;
    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    EXPECT_FALSE(
        OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(rootAnimations_["animations"], animationInfos));
}

/*
 * @tc.name: SymbolConfigParserTest011
 * @tc.desc: test for invalid json
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest011, TestSize.Level0)
{
    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolConfig(rootInvalid_, symbolConfig, animationInfos));
}
