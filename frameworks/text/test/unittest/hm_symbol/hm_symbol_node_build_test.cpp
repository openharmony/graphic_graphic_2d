/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "txt/text_style.h"
#include "symbol_engine/hm_symbol_node_build.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace SPText {
class OHHmSymbolNodeBuildTest : public testing::Test {
public:
    static bool SetSymbolAnimationOne(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    static bool SetSymbolAnimationTwo(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    std::vector<std::vector<size_t>> layers_ = {{0}, {1}}; // this two layers
    Drawing::DrawingAnimationSetting animationSettingOne_ = {
        // animationTypes
        {
            Drawing::DrawingAnimationType::SCALE_TYPE,
            Drawing::DrawingAnimationType::APPEAR_TYPE,
            Drawing::DrawingAnimationType::DISAPPEAR_TYPE,
            Drawing::DrawingAnimationType::BOUNCE_TYPE
        },
        // groupSettings
        {
            // {0, 1}: layerIndes, 0: animationIndex
            {{{{0, 1}}}, 0}
        }
    };
    Drawing::DrawingAnimationSetting animationSettingTwo_ = {
        // animationTypes
        {
            Drawing::DrawingAnimationType::SCALE_TYPE,
            Drawing::DrawingAnimationType::VARIABLE_COLOR_TYPE
        },
        // groupSettings
        {
            // {0} {1}: layerIndes, 0 1: animationIndex
            {{{{0}}}, 0}, {{{{1}}}, 1}
        }
    };
    Drawing::DrawingAnimationSetting animationSettingOneMask_ = {
        // animationTypes
        {
            Drawing::DrawingAnimationType::SCALE_TYPE,
            Drawing::DrawingAnimationType::VARIABLE_COLOR_TYPE,
            Drawing::DrawingAnimationType::BOUNCE_TYPE
        },
        // groupSettings
        {
            // {0, 2}: layerIndes, {1, 3}: maskIndexes 0: animationIndex
            {{{{0, 2}, {1, 3}}}, 0}
        }
    };
    Drawing::DrawingAnimationSetting animationSettingMaskLayer_ = {
        // animationTypes
        {
            Drawing::DrawingAnimationType::SCALE_TYPE,
            Drawing::DrawingAnimationType::VARIABLE_COLOR_TYPE,
            Drawing::DrawingAnimationType::BOUNCE_TYPE
        },
        // groupSettings
        {
            // {0} {}: layerIndes, {1, 2}: maskIndexes 0 1: animationIndex
            {{{{0}}}, 0}, {{{{}, {1, 2}}}, 1}
        }
    };

    // the {0, 1} is layerIndexes of one group
    std::vector<Drawing::DrawingRenderGroup> renderGroupsOne_ = {{{{{0, 1}}}}};
    // the {0, 2} is layerIndexes, {1, 3} is maskIndexes of one group
    std::vector<Drawing::DrawingRenderGroup> renderGroupsOneMask_ = {{{{{0, 2}, {1, 3}}}}};
    // the {0} {1} is layerIndexes of two group
    std::vector<Drawing::DrawingRenderGroup> renderGroupsTwo_ = {{{{{0}}}}, {{{{1}}}}};
    // the {0},{} is layerIndexes  {1, 2}is maskIndexes of two group
    std::vector<Drawing::DrawingRenderGroup> renderGroupsMaskLayer_ = {{{{{0}}}}, {{{{}, {1, 2}}}}};
};

bool OHHmSymbolNodeBuildTest::SetSymbolAnimationOne(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig == nullptr) {
        return false;
    }

    // check the size is 1 of nodes config
    if (symbolAnimationConfig->numNodes == 1 && symbolAnimationConfig->symbolNodes.size() == 1) {
        return true;
    }
    return false;
}

bool OHHmSymbolNodeBuildTest::SetSymbolAnimationTwo(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig == nullptr) {
        return false;
    }

    // check the size is 2 of nodes config
    if (symbolAnimationConfig->numNodes == 2 && symbolAnimationConfig->symbolNodes.size() == 2) {
        return true;
    }
    return false;
}

/*
 * @tc.name: SymbolNodeBuild001
 * @tc.desc: test SymbolNodeBuild with simulation data
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SymbolNodeBuild001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSAnimationSetting animationSetting;
    RSHMSymbolData symbol;
    RSEffectStrategy effectMode = RSEffectStrategy::SCALE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    EXPECT_EQ(symbolNode.effectStrategy_, RSEffectStrategy::SCALE);
}

/*
 * @tc.name: DecomposeSymbolAndDraw001
 * @tc.desc: test DecomposeSymbolAndDraw with animation SCALE and wholeSymbol effect
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, DecomposeSymbolAndDraw001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 40); // 100 x, 100, 40 radius
    path.AddCircle(100, 100, 30, Drawing::PathDirection::CCW_DIRECTION); // 100 x, 100, 30 radius
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = layers_;
    symbol.symbolInfo_.renderGroups = renderGroupsOne_;

    RSEffectStrategy effectMode = RSEffectStrategy::SCALE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOne_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetSymbolId(0);
    symbolNode.SetAnimationMode(1); // 1 is wholeSymbol effect
    bool result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, true);

    symbol.symbolInfo_.renderGroups = {};
    symbolNode.symbolData_ = symbol;
    result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, false);

    symbol.symbolInfo_.renderGroups = renderGroupsOneMask_;
    symbolNode.symbolData_ = symbol;
    result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, true);
}

/*
 * @tc.name: DecomposeSymbolAndDraw002
 * @tc.desc: test DecomposeSymbolAndDraw with animation SCALE and byLayer effect
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, DecomposeSymbolAndDraw002, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 50); // 100 x, 100, 50 radius
    path.AddCircle(100, 100, 30, Drawing::PathDirection::CCW_DIRECTION); // 100 x, 100, 30 radius
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = layers_;
    symbol.symbolInfo_.renderGroups = renderGroupsTwo_;

    RSEffectStrategy effectMode = RSEffectStrategy::SCALE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingTwo_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationTwo);
    symbolNode.SetSymbolId(0);
    symbolNode.SetAnimationMode(0); // 0 is byLayer effect
    int result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, true);
}

/*
 * @tc.name: DecomposeSymbolAndDraw003
 * @tc.desc: test DecomposeSymbolAndDraw with animation BOUNCE, wholeSymbol effect and maskLayer
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, DecomposeSymbolAndDraw003, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 65); // 100 x, 100, 40 radius
    path.AddCircle(100, 100, 45); // 100 x, 100, 30 radius
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = layers_;
    symbol.symbolInfo_.renderGroups = renderGroupsOneMask_;

    RSEffectStrategy effectMode = RSEffectStrategy::BOUNCE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOne_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetSymbolId(0);
    symbolNode.SetAnimationMode(1); // 1 is wholeSymbol effect
    int result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, true);
}

/*
 * @tc.name: DecomposeSymbolAndDraw004
 * @tc.desc: test DecomposeSymbolAndDraw with animation VARIABLE_COLOR and iteratuve effect
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, DecomposeSymbolAndDraw004, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 50); // 100 x, 100, 50 radius
    path.AddCircle(100, 100, 30, Drawing::PathDirection::CCW_DIRECTION); // 100 x, 100, 30 radius
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = layers_;
    symbol.symbolInfo_.renderGroups = renderGroupsTwo_;

    RSEffectStrategy effectMode = RSEffectStrategy::VARIABLE_COLOR;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingTwo_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationTwo);
    symbolNode.SetSymbolId(0);
    symbolNode.SetAnimationMode(1); // 0 is iteratuve effect
    int result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, true);
}

/*
 * @tc.name: DecomposeSymbolAndDraw005
 * @tc.desc: test DecomposeSymbolAndDraw with animation BOUNCE, byLayer effect and has maskLayers
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, DecomposeSymbolAndDraw005, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 50); // 100 x, 100, 50 radius
    path.AddCircle(100, 100, 30, Drawing::PathDirection::CCW_DIRECTION); // 100 x, 100, 30 radius
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = layers_;
    symbol.symbolInfo_.renderGroups = renderGroupsMaskLayer_;

    RSEffectStrategy effectMode = RSEffectStrategy::BOUNCE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingMaskLayer_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationTwo);
    symbolNode.SetAnimationMode(0); // 0 is byLayer effect
    int result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, true);
}

/*
 * @tc.name: DecomposeSymbolAndDraw006
 * @tc.desc: test DecomposeSymbolAndDraw with animation VARIABLE_COLOR, cumulative effect and maskIndexes
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, DecomposeSymbolAndDraw006, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 65); // 100 x, 100, 40 radius
    path.AddCircle(100, 100, 45); // 100 x, 100, 30 radius
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = layers_;
    symbol.symbolInfo_.renderGroups = renderGroupsOneMask_;

    RSEffectStrategy effectMode = RSEffectStrategy::VARIABLE_COLOR;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetAnimationMode(0); // 1 is cumulative effect
    int result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, true);

    // test animation REPLACE_DISAPPEAR
    effectMode = RSEffectStrategy::REPLACE_DISAPPEAR;
    SymbolNodeBuild symbolNode1 = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode1.SetAnimation(&SetSymbolAnimationOne);
    symbolNode1.SetAnimationMode(1); // 1 is cumulative effect
    int result1 = symbolNode1.DecomposeSymbolAndDraw();
    EXPECT_EQ(result1, true);

    // test animationFunc is nullptr
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc = nullptr;
    symbolNode.SetAnimation(animationFunc);
    int result2 = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result2, false);
}

/*
 * @tc.name: DecomposeSymbolAndDraw007
 * @tc.desc: test DecomposeSymbolAndDraw with animation VARIABLE_COLOR, two rendergroups and only one animationGroup
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, DecomposeSymbolAndDraw007, TestSize.Level0)
{
    std::pair<double, double> offset = {100, 100}; // 100, 100 is the offset
    RSPath path;
    path.AddCircle(100, 100, 50); // 100 x, 100, 50 radius
    path.AddCircle(100, 100, 30, Drawing::PathDirection::CCW_DIRECTION); // 100 x, 100, 30 radius
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = layers_;
    symbol.symbolInfo_.renderGroups = renderGroupsTwo_;

    RSEffectStrategy effectMode = RSEffectStrategy::VARIABLE_COLOR;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOne_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    int result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_EQ(result, true);
}

/*
 * @tc.name: ClearAnimation001
 * @tc.desc: test ClearAnimation with animation VARIABLE_COLOR, cumulative effect and maskIndexes
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, ClearAnimation001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.ClearAnimation();
    EXPECT_EQ(symbolNode.effectStrategy_, RSEffectStrategy::NONE);
}

/*
 * @tc.name: SetSymbolId001
 * @tc.desc: test SetAnimation function
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetSymbolId001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetSymbolId(1);
    EXPECT_EQ(symbolNode.symblSpanId_, 1);
}

/*
 * @tc.name: SetAnimationMode001
 * @tc.desc: test SetAnimationMode with hierarchical
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetAnimationMode001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetAnimationMode(0);
    EXPECT_EQ(symbolNode.animationMode_, 0);
}

/*
 * @tc.name: SetAnimationMode002
 * @tc.desc: test SetAnimationMode with whole
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetAnimationMode002, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetAnimationMode(1);
    EXPECT_EQ(symbolNode.animationMode_, 1);
}

/*
 * @tc.name: SetAnimationMode003
 * @tc.desc: test SetAnimationMode with boundary value
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetAnimationMode003, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetAnimationMode(500);
    EXPECT_EQ(symbolNode.animationMode_, 1);
}

/*
 * @tc.name: SetRepeatCount001
 * @tc.desc: test SetRepeatCount function
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetRepeatCount001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetRepeatCount(1);
    EXPECT_EQ(symbolNode.repeatCount_, 1);
}

/*
 * @tc.name: SetAnimationStart001
 * @tc.desc: test SetAnimationStart function
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetAnimationStart001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetAnimationStart(true);
    EXPECT_EQ(symbolNode.animationStart_, true);
}

/*
 * @tc.name: SetCurrentAnimationHasPlayed001
 * @tc.desc: test SetCurrentAnimationHasPlayed function
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetCurrentAnimationHasPlayed001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetCurrentAnimationHasPlayed(true);
    EXPECT_EQ(symbolNode.currentAnimationHasPlayed_, true);
}

/*
 * @tc.name: SetCommonSubType001
 * @tc.desc: test SetCommonSubType with commonSubType::DOWN
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetCommonSubType001, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetCommonSubType(Drawing::DrawingCommonSubType::DOWN);
    EXPECT_EQ(symbolNode.commonSubType_, Drawing::DrawingCommonSubType::DOWN);
}

/*
 * @tc.name: SetCommonSubType002
 * @tc.desc: test SetCommonSubType with commonSubType::UP
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetCommonSubType002, TestSize.Level0)
{
    std::pair<float, float> offset = {100, 100}; // 100, 100 is the offset
    RSHMSymbolData symbol;

    RSEffectStrategy effectMode = RSEffectStrategy::NONE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationOne);
    symbolNode.SetCommonSubType(Drawing::DrawingCommonSubType::UP);
    EXPECT_EQ(symbolNode.commonSubType_, Drawing::DrawingCommonSubType::UP);
}

/*
 * @tc.name: AddHierarchicalAnimation001
 * @tc.desc: test AddHierarchicalAnimation with animation DISABLE
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, AddHierarchicalAnimation001, TestSize.Level0)
{
    std::pair<float, float> offset = {100.0f, 100.0f}; // 100.0f, 100.0f is the offset
    RSPath path;
    path.AddCircle(100.0f, 100.0f, 50.0f); // 100.0f x, 100.0f y, 50.0f radius
    // 100.0f x, 100.0f y, 30.0f radius
    path.AddCircle(100.0f, 100.0f, 30.0f, Drawing::PathDirection::CCW_DIRECTION);
    RSHMSymbolData symbol;
    symbol.path_ = path;
    symbol.symbolInfo_.layers = layers_;
    std::vector<size_t> layer = {2}; // 2: add a new layer
    symbol.symbolInfo_.layers.push_back(layer);
    symbol.symbolInfo_.renderGroups = renderGroupsMaskLayer_;

    RSEffectStrategy effectMode = RSEffectStrategy::DISABLE;
    Drawing::DrawingGroupSetting groupSetting = {{{{2}, {}}}, 1};
    auto animationSetting = animationSettingMaskLayer_;
    animationSetting.groupSettings.push_back(groupSetting); // add a new animation layer
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    symbolNode.SetAnimation(&SetSymbolAnimationTwo);
    symbolNode.SetAnimationMode(0); // 0 is byLayer effect
    Vector4f nodeBounds = {10.0f, 10.0f, 15.0f, 15.0f}; // 10.0f 10.0f: first offset, 15.0f 15.0f: width height

    std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig = nullptr;
    symbolNode.AddHierarchicalAnimation(symbol, nodeBounds, animationSetting.groupSettings, symbolAnimationConfig);
    bool result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_FALSE(result);

    symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolNode.AddHierarchicalAnimation(symbol, nodeBounds, animationSetting.groupSettings, symbolAnimationConfig);
    result = symbolNode.DecomposeSymbolAndDraw();
    EXPECT_FALSE(result);
}

/*
 * @tc.name: SetSymbolNodeColors001
 * @tc.desc: test SetSymbolNodeColors
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, SetSymbolNodeColors001, TestSize.Level0)
{
    std::pair<float, float> offset = {100.0f, 100.0f}; // 100.0f, 100.0f is the offset
    RSHMSymbolData symbol;
    RSEffectStrategy effectMode = RSEffectStrategy::DISABLE;
    SymbolNodeBuild symbolNodeBuild = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    RSPath path;
    path.AddCircle(100.0f, 100.0f, 50.0f); // 100.0f x, 100.0f y, 50.0f radius
    TextEngine::NodeLayerInfo layerInfo;
    layerInfo.path = path;
    TextEngine::SymbolNode symbolNode;
    symbolNode.pathsInfo.push_back(layerInfo);
    TextEngine::SymbolNode symbolNode1 = symbolNode;
    auto color = std::make_shared<SymbolGradient>();
    color->SetColors({0XFFFF0000}); // 0XFFFF0000 is ARGB
    symbolNode.pathsInfo[0].color = color;
    auto color1 = std::make_shared<SymbolGradient>();
    color1->SetColors({0XFF0000FF}); // 0XFFFF0000 is ARGB
    symbolNode1.pathsInfo[0].color = color1;

    symbolNodeBuild.SetSymbolNodeColors(symbolNode, symbolNode1);
    symbolNode1.pathsInfo[0].color = nullptr;
    symbolNodeBuild.SetSymbolNodeColors(symbolNode, symbolNode1);
    EXPECT_NE(symbolNode1.pathsInfo[0].color, nullptr);

    auto slashColor = std::make_shared<SymbolGradient>();
    slashColor->SetColors({0XFF0000FF}); // 0XFF0000FF is ARGB
    symbolNodeBuild.SetDisableSlashColor(slashColor);
    symbolNode1.pathsInfo[0].color = nullptr;
    symbolNodeBuild.SetSymbolNodeColors(symbolNode, symbolNode1);
    EXPECT_NE(symbolNode1.pathsInfo[0].color, nullptr);
}

/*
 * @tc.name: UpdateGradient001
 * @tc.desc: test UpdateGradient
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, UpdateGradient001, TestSize.Level0)
{
    std::pair<float, float> offset = {100.0f, 100.0f}; // 100.0f, 100.0f is the offset
    RSHMSymbolData symbol;
    RSEffectStrategy effectMode = RSEffectStrategy::DISABLE;
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSettingOneMask_, symbol, effectMode, offset);
    RSRenderGroup group;
    group.groupInfos = animationSettingOne_.groupSettings[0].groupInfos;
    std::vector<RSRenderGroup> groups = {};
    std::vector<RSPath> pathLayers;
    RSPath path;
    path.AddCircle(100.0f, 100.0f, 50.0f); // 100.0f x, 100.0f y, 50.0f radius
    pathLayers.push_back(path);
    pathLayers.push_back(path);

    // test renderMode is SINGLE
    symbolNode.SetRenderMode(RSSymbolRenderingStrategy::SINGLE);
    std::vector<std::shared_ptr<SymbolGradient>> gradients = {nullptr};
    symbolNode.SetGradients(gradients);
    symbolNode.UpdateGradient(groups, pathLayers, path);
    EXPECT_TRUE(symbolNode.gradients_.empty());

    // test renderMode is MULTIPLE_COLOR
    groups = {group};
    symbolNode.SetRenderMode(RSSymbolRenderingStrategy::MULTIPLE_COLOR);
    symbolNode.UpdateGradient(groups, pathLayers, path);
    EXPECT_TRUE(symbolNode.gradients_.empty());

    // test gradients not is empty()
    gradients = {};
    gradients.push_back(std::make_shared<SymbolGradient>());
    symbolNode.SetGradients(gradients);
    symbolNode.UpdateGradient(groups, pathLayers, path);
    EXPECT_FALSE(symbolNode.gradients_.empty());

    // test gradient is nullptr
    gradients[0] = nullptr;
    symbolNode.SetGradients(gradients);
    symbolNode.UpdateGradient(groups, pathLayers, path);
    EXPECT_TRUE(symbolNode.gradients_.empty());
}

/*
 * @tc.name: CreateGradient001
 * @tc.desc: test CreateGradient
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolNodeBuildTest, CreateGradient001, TestSize.Level0)
{
    // test input nullptr
    auto result = SymbolNodeBuild::CreateGradient(nullptr);
    EXPECT_EQ(result, nullptr);

    // test input base color
    auto gradient1 = std::make_shared<SymbolGradient>();
    result = SymbolNodeBuild::CreateGradient(gradient1);
    EXPECT_NE(result, nullptr);

    // test input line gradient
    auto gradient2 = std::make_shared<SymbolLineGradient>(45.0f); // 45.0f is angle of lineGradient
    result = SymbolNodeBuild::CreateGradient(gradient2);
    EXPECT_NE(result, nullptr);

    // test input radial gradient
    Drawing::Point centerPt = Drawing::Point(0.5f, 0.5f); // 0.5f: x, 0.5f: y
    float radiusRatio = 0.6f; // 0.6f is radius
    auto gradient3 = std::make_shared<SymbolRadialGradient>(centerPt, radiusRatio);
    result = SymbolNodeBuild::CreateGradient(gradient3);
    EXPECT_NE(result, nullptr);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS