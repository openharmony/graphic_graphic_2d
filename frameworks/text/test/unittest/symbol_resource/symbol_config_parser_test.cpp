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

#include <cJSON.h>
#include <gtest/gtest.h>

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

// Helper function to delete item from cJSON object
void DeleteItemFromObject(cJSON* parent, const char* key)
{
    if (parent == nullptr || key == nullptr || !cJSON_IsObject(parent)) {
        return;
    }
    cJSON* item = cJSON_DetachItemFromObject(parent, key);
    if (item != nullptr) {
        cJSON_Delete(item);
    }
}
}

class SymbolConfigParserTest : public testing::Test {
public:
    void SetUp() override
    {
        BuildValidLayersGroupingJson();
        BuildValidAnimationsJson();
        BuildInvalidJson();
    }

    void TearDown() override
    {
        if (rootLayers_ != nullptr) {
            cJSON_Delete(rootLayers_);
            rootLayers_ = nullptr;
        }
        if (rootAnimations_ != nullptr) {
            cJSON_Delete(rootAnimations_);
            rootAnimations_ = nullptr;
        }
        if (rootInvalid_ != nullptr) {
            cJSON_Delete(rootInvalid_);
            rootInvalid_ = nullptr;
        }
    }

    cJSON* rootLayers_ = nullptr;
    cJSON* rootAnimations_ = nullptr;
    cJSON* rootInvalid_ = nullptr;

private:
    void BuildValidLayersGroupingJson()
    {
        rootLayers_ = cJSON_Parse(ROOT_LAYERS_JSON_STR);
        EXPECT_NE(rootLayers_, nullptr);
    }

    void BuildValidAnimationsJson()
    {
        rootAnimations_ = cJSON_Parse(ROOT_ANIMATIONS_JSON_STR);
        EXPECT_NE(rootAnimations_, nullptr);
    }

    void BuildInvalidJson()
    {
        rootInvalid_ = cJSON_Parse(ROOT_INVALID_JSON_STR);
        EXPECT_NE(rootInvalid_, nullptr);
    }
};

/*
 * @tc.name: SymbolConfigParserTest001
 * @tc.desc: test for lack native_glyph_id
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest001, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootLayers_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);
    ASSERT_TRUE(cJSON_IsArray(grouping));
    cJSON* firstItem = cJSON_GetArrayItem(grouping, 0);
    ASSERT_NE(firstItem, nullptr);
    DeleteItemFromObject(firstItem, "native_glyph_id");

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    EXPECT_TRUE(symbolConfig.empty());
}

/*
 * @tc.name: SymbolConfigParserTest002
 * @tc.desc: test for error native_glyph_id
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest002, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootLayers_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);
    ASSERT_TRUE(cJSON_IsArray(grouping));
    cJSON* firstItem = cJSON_GetArrayItem(grouping, 0);
    ASSERT_NE(firstItem, nullptr);
    EXPECT_TRUE(cJSON_ReplaceItemInObject(firstItem, "native_glyph_id", cJSON_CreateString("invalid_id")));

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    EXPECT_TRUE(symbolConfig.empty());
}

/*
 * @tc.name: SymbolConfigParserTest003
 * @tc.desc: test for invalid color
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest003, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootLayers_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);
    ASSERT_TRUE(cJSON_IsArray(grouping));
    cJSON* firstItem = cJSON_GetArrayItem(grouping, 0);
    ASSERT_NE(firstItem, nullptr);
    cJSON* renderModes = cJSON_GetObjectItem(firstItem, "render_modes");
    ASSERT_NE(renderModes, nullptr);
    ASSERT_TRUE(cJSON_IsArray(renderModes));
    cJSON* firstRenderMode = cJSON_GetArrayItem(renderModes, 0);
    ASSERT_NE(firstRenderMode, nullptr);
    cJSON* renderGroups = cJSON_GetObjectItem(firstRenderMode, "render_groups");
    ASSERT_NE(renderGroups, nullptr);
    ASSERT_TRUE(cJSON_IsArray(renderGroups));
    cJSON* firstRenderGroup = cJSON_GetArrayItem(renderGroups, 0);
    ASSERT_NE(firstRenderGroup, nullptr);
    EXPECT_TRUE(cJSON_SetValuestring(cJSON_GetObjectItem(firstRenderGroup, "default_color"), "INVALID_COLOR"));

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    auto& renderGroupsResult = symbolConfig.at(1001).renderModeGroups.begin()->second;
    // default value is 0
    EXPECT_EQ(renderGroupsResult[0].color.r, 0);
}

/*
 * @tc.name: SymbolConfigParserTest004
 * @tc.desc: test for lack animation_type
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest004, TestSize.Level0)
{
    cJSON* animations = cJSON_GetObjectItem(rootAnimations_, "animations");
    ASSERT_NE(animations, nullptr);
    ASSERT_TRUE(cJSON_IsArray(animations));
    cJSON* firstItem = cJSON_GetArrayItem(animations, 0);
    ASSERT_NE(firstItem, nullptr);
    cJSON* secondItem = cJSON_GetArrayItem(animations, 1);
    ASSERT_NE(secondItem, nullptr);
    DeleteItemFromObject(firstItem, "animation_type");
    DeleteItemFromObject(secondItem, "animation_type");

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(animations, animationInfos);
    EXPECT_TRUE(animationInfos.empty());
}

/*
 * @tc.name: SymbolConfigParserTest005
 * @tc.desc: test for invalid common_sub_type
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest005, TestSize.Level0)
{
    cJSON* animations = cJSON_GetObjectItem(rootAnimations_, "animations");
    ASSERT_NE(animations, nullptr);
    ASSERT_TRUE(cJSON_IsArray(animations));
    cJSON* firstItem = cJSON_GetArrayItem(animations, 0);
    ASSERT_NE(firstItem, nullptr);
    cJSON* animParams = cJSON_GetObjectItem(firstItem, "animation_parameters");
    ASSERT_NE(animParams, nullptr);
    ASSERT_TRUE(cJSON_IsArray(animParams));
    cJSON* firstParam = cJSON_GetArrayItem(animParams, 0);
    ASSERT_NE(firstParam, nullptr);
    EXPECT_TRUE(cJSON_SetValuestring(cJSON_GetObjectItem(firstParam, "common_sub_type"), "invalid_direction"));

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(animations, animationInfos);
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
    cJSON* grouping = cJSON_GetObjectItem(rootLayers_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);
    ASSERT_TRUE(cJSON_IsArray(grouping));
    cJSON* firstItem = cJSON_GetArrayItem(grouping, 0);
    ASSERT_NE(firstItem, nullptr);

    cJSON* emptyArray = cJSON_CreateArray();
    ASSERT_NE(emptyArray, nullptr);
    EXPECT_TRUE(cJSON_ReplaceItemInObject(firstItem, "layers", emptyArray));

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    EXPECT_TRUE(symbolConfig.at(1001).layers.empty());
}

/*
 * @tc.name: SymbolConfigParserTest007
 * @tc.desc: test for valid symbol layers data
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest007, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootLayers_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
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
    cJSON* animations = cJSON_GetObjectItem(rootAnimations_, "animations");
    ASSERT_NE(animations, nullptr);

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(animations, animationInfos);
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
    cJSON* emptyObj = cJSON_CreateObject();
    ASSERT_NE(emptyObj, nullptr);

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_FALSE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        emptyObj, symbolConfig));
    cJSON_Delete(emptyObj);
}

/*
 * @tc.name: SymbolConfigParserTest010
 * @tc.desc: test for not array animation
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest010, TestSize.Level0)
{
    cJSON* emptyObj = cJSON_CreateObject();
    ASSERT_NE(emptyObj, nullptr);

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    EXPECT_FALSE(
        OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(emptyObj, animationInfos));
    cJSON_Delete(emptyObj);
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