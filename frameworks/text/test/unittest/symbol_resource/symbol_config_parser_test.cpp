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

// Comprehensive abnormal JSON covering all fields:
// - Layers: native_glyph_id overflow/negative, symbol_glyph_id overflow, invalid render mode,
//   mode as number, non-hex/short color, fix_alpha out of range, mixed-type components,
//   unknown animation type, invalid common_sub_type
// - Animations: animation_type as number, unknown type string, non-array parameters,
//   animation_mode overflow, invalid common_sub_type
// - Piecewise: unknown curve, non-number curve_args, negative duration, overflow delay,
//   mixed-type properties
const char* ROOT_ABNORMAL_JSON_STR = R"({
    "name": "Abnormal Test",
    "version": "1.0",
    "common_animations": [
        {
            "animation_type": 42,
            "animation_parameters": [{ "animation_mode": 1, "common_sub_type": "up" }]
        },
        {
            "animation_type": "unknown_type",
            "animation_parameters": [{
                "animation_mode": 1,
                "common_sub_type": "up",
                "group_parameters": [[{
                    "curve": "spring",
                    "duration": 200,
                    "delay": 0
                }]]
            }]
        },
        {
            "animation_type": "scale",
            "animation_parameters": { "animation_mode": 1 }
        },
        {
            "animation_type": "scale",
            "animation_parameters": [{
                "animation_mode": 100000,
                "common_sub_type": "invalid_direction",
                "group_parameters": [[{
                    "curve": "unknown_curve",
                    "curve_args": { "velocity": 0, "mass": "invalid" },
                    "duration": -1,
                    "delay": 9999999999,
                    "properties": {
                        "sx": [1, "abc"],
                        "sy": [1, 1.15]
                    }
                }]]
            }]
        }
    ],
    "special_animations": [],
    "symbol_layers_grouping": [
        {
            "native_glyph_id": 100000,
            "symbol_glyph_id": 2001,
            "layers": [{ "components": [1, 2] }],
            "render_modes": [],
            "animation_settings": []
        },
        {
            "native_glyph_id": -1,
            "symbol_glyph_id": 2002,
            "layers": [{ "components": [3, 4] }]
        },
        {
            "native_glyph_id": 1003,
            "symbol_glyph_id": 100000,
            "layers": [
                { "components": [1, "abc", 3] },
                { "components": [4, 5] }
            ],
            "render_modes": [
                { "mode": "invalid_mode", "render_groups": [] },
                { "mode": 42 },
                {
                    "mode": "monochrome",
                    "render_groups": [{
                        "group_indexes": [{ "layer_indexes": [0] }],
                        "default_color": "#XXYYZZ",
                        "fix_alpha": -0.5
                    }]
                }
            ],
            "animation_settings": [{
                "animation_types": ["unknown_anim"],
                "common_sub_type": "invalid_dir",
                "group_settings": []
            }]
        },
        {
            "native_glyph_id": 1004,
            "symbol_glyph_id": 2004,
            "layers": [],
            "render_modes": [{
                "mode": "multicolor",
                "render_groups": [{
                    "group_indexes": [{ "layer_indexes": [0] }],
                    "default_color": "SHORT",
                    "fix_alpha": 2.0
                }]
            }]
        }
    ]
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
        BuildAbnormalJson();
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
        if (rootAbnormal_ != nullptr) {
            cJSON_Delete(rootAbnormal_);
            rootAbnormal_ = nullptr;
        }
    }

    cJSON* rootLayers_ = nullptr;
    cJSON* rootAnimations_ = nullptr;
    cJSON* rootInvalid_ = nullptr;
    cJSON* rootAbnormal_ = nullptr;

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

    void BuildAbnormalJson()
    {
        rootAbnormal_ = cJSON_Parse(ROOT_ABNORMAL_JSON_STR);
        EXPECT_NE(rootAbnormal_, nullptr);
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

/*
 * @tc.name: SymbolConfigParserTest012
 * @tc.desc: test for native_glyph_id overflow and negative values rejected
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest012, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootAbnormal_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    // Entries with native_glyph_id=100000 (overflow) and -1 (negative) should be rejected
    EXPECT_EQ(symbolConfig.size(), 2);
    EXPECT_TRUE(symbolConfig.count(1003) > 0);
    EXPECT_TRUE(symbolConfig.count(1004) > 0);
}

/*
 * @tc.name: SymbolConfigParserTest013
 * @tc.desc: test for symbol_glyph_id overflow falls back to native_glyph_id
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest013, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootAbnormal_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    // symbol_glyph_id=100000 overflows, should fall back to native_glyph_id=1003
    EXPECT_EQ(symbolConfig.at(1003).symbolGlyphId, 1003);
}

/*
 * @tc.name: SymbolConfigParserTest014
 * @tc.desc: test for invalid render mode and mode as number skipped, non-hex color defaults
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest014, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootAbnormal_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    auto& group = symbolConfig.at(1003);
    // "invalid_mode" and mode=42 skipped, only "monochrome" parsed
    EXPECT_EQ(group.renderModeGroups.size(), 1);
    // "#XXYYZZ" non-hex color defaults to (0,0,0), fix_alpha=-0.5 accepted
    auto& renderGroups = group.renderModeGroups.begin()->second;
    EXPECT_EQ(renderGroups[0].color.r, 0);
    EXPECT_EQ(renderGroups[0].color.g, 0);
    EXPECT_EQ(renderGroups[0].color.b, 0);
    EXPECT_DOUBLE_EQ(renderGroups[0].color.a, -0.5);
}

/*
 * @tc.name: SymbolConfigParserTest015
 * @tc.desc: test for short color string defaults and out-of-range fix_alpha accepted
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest015, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootAbnormal_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    auto& renderGroups = symbolConfig.at(1004).renderModeGroups.begin()->second;
    // "SHORT" (length 5 != 7) defaults to (0,0,0)
    EXPECT_EQ(renderGroups[0].color.r, 0);
    EXPECT_EQ(renderGroups[0].color.g, 0);
    EXPECT_EQ(renderGroups[0].color.b, 0);
    // fix_alpha=2.0 accepted without range check
    EXPECT_DOUBLE_EQ(renderGroups[0].color.a, 2.0);
}

/*
 * @tc.name: SymbolConfigParserTest016
 * @tc.desc: test for mixed-type components only parse numbers
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest016, TestSize.Level0)
{
    cJSON* grouping = cJSON_GetObjectItem(rootAbnormal_, "symbol_layers_grouping");
    ASSERT_NE(grouping, nullptr);

    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolLayersGrouping(
        grouping, symbolConfig));
    auto& layers = symbolConfig.at(1003).layers;
    // [1, "abc", 3] → only [1, 3] parsed, [4, 5] unchanged
    EXPECT_EQ(layers.size(), 2);
    EXPECT_EQ(layers[0], std::vector<size_t>({1, 3}));
    EXPECT_EQ(layers[1], std::vector<size_t>({4, 5}));
}

/*
 * @tc.name: SymbolConfigParserTest017
 * @tc.desc: test for animation_type as number and non-array parameters skipped
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest017, TestSize.Level0)
{
    cJSON* animations = cJSON_GetObjectItem(rootAbnormal_, "common_animations");
    ASSERT_NE(animations, nullptr);

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(animations, animationInfos));
    // Entry 1 (animation_type=42, not string) and entry 3 (animation_parameters as object) skipped
    // Entry 2 ("unknown_type" with default type) and entry 4 ("scale" with invalid params) still added
    EXPECT_FALSE(animationInfos.empty());
}

/*
 * @tc.name: SymbolConfigParserTest018
 * @tc.desc: test for animation_mode overflow and invalid common_sub_type default to DOWN
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest018, TestSize.Level0)
{
    cJSON* animations = cJSON_GetObjectItem(rootAbnormal_, "common_animations");
    ASSERT_NE(animations, nullptr);

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(animations, animationInfos));
    // Entry 4: "scale" with animation_mode=100000 (overflow) → mode defaults to 0
    // common_sub_type="invalid_direction" → defaults to DOWN
    auto& scaleInfo = animationInfos.at(RSAnimationType::SCALE_TYPE);
    auto& animPara = scaleInfo.animationParas.begin()->second;
    EXPECT_EQ(animPara.animationMode, 0);
    EXPECT_EQ(animPara.commonSubType, RSCommonSubType::DOWN);
}

/*
 * @tc.name: SymbolConfigParserTest019
 * @tc.desc: test for unknown curve type skipped, non-number curve_args skipped
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest019, TestSize.Level0)
{
    cJSON* animations = cJSON_GetObjectItem(rootAbnormal_, "common_animations");
    ASSERT_NE(animations, nullptr);

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(animations, animationInfos));
    auto& scaleInfo = animationInfos.at(RSAnimationType::SCALE_TYPE);
    auto& groupParas = scaleInfo.animationParas.begin()->second.groupParameters;
    ASSERT_FALSE(groupParas.empty());
    auto& piecewisePara = groupParas[0][0];
    // "unknown_curve" not found → curveType keeps default (0)
    EXPECT_EQ(static_cast<int>(piecewisePara.curveType), 0);
    // "mass": "invalid" skipped, only "velocity": 0 parsed
    EXPECT_EQ(piecewisePara.curveArgs.size(), 1);
    EXPECT_TRUE(piecewisePara.curveArgs.count("velocity") > 0);
}

/*
 * @tc.name: SymbolConfigParserTest020
 * @tc.desc: test for negative duration and overflow delay default to 0
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest020, TestSize.Level0)
{
    cJSON* animations = cJSON_GetObjectItem(rootAbnormal_, "common_animations");
    ASSERT_NE(animations, nullptr);

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(animations, animationInfos));
    auto& scaleInfo = animationInfos.at(RSAnimationType::SCALE_TYPE);
    auto& groupParas = scaleInfo.animationParas.begin()->second.groupParameters;
    ASSERT_FALSE(groupParas.empty());
    auto& piecewisePara = groupParas[0][0];
    // duration=-1 rejected by GetUint32FromCJSON → default 0
    EXPECT_EQ(piecewisePara.duration, 0);
    // delay=9999999999 rejected by GetIntFromCJSON → default 0
    EXPECT_EQ(piecewisePara.delay, 0);
}

/*
 * @tc.name: SymbolConfigParserTest021
 * @tc.desc: test for mixed-type properties only parse numbers
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest021, TestSize.Level0)
{
    cJSON* animations = cJSON_GetObjectItem(rootAbnormal_, "common_animations");
    ASSERT_NE(animations, nullptr);

    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolAnimations(animations, animationInfos));
    auto& scaleInfo = animationInfos.at(RSAnimationType::SCALE_TYPE);
    auto& groupParas = scaleInfo.animationParas.begin()->second.groupParameters;
    ASSERT_FALSE(groupParas.empty());
    auto& piecewisePara = groupParas[0][0];
    // "sx": [1, "abc"] → only [1.0] parsed
    auto& sx = piecewisePara.properties.at("sx");
    EXPECT_EQ(sx.size(), 1);
    EXPECT_FLOAT_EQ(sx[0], 1.0f);
    // "sy": [1, 1.15] → fully parsed
    auto& sy = piecewisePara.properties.at("sy");
    EXPECT_EQ(sy.size(), 2);
    EXPECT_FLOAT_EQ(sy[0], 1.0f);
    EXPECT_FLOAT_EQ(sy[1], 1.15f);
}

/*
 * @tc.name: SymbolConfigParserTest022
 * @tc.desc: test for ParseSymbolConfig with abnormal JSON parses without crash
 * @tc.type: FUNC
 */
HWTEST_F(SymbolConfigParserTest, SymbolConfigParserTest022, TestSize.Level0)
{
    std::unordered_map<RSAnimationType, RSAnimationInfo> animationInfos;
    std::unordered_map<uint16_t, RSSymbolLayersGroups> symbolConfig;
    EXPECT_TRUE(OHOS::Rosen::Symbol::SymbolConfigParser::ParseSymbolConfig(
        rootAbnormal_, symbolConfig, animationInfos));
    EXPECT_EQ(symbolConfig.size(), 2);
    EXPECT_FALSE(animationInfos.empty());
}