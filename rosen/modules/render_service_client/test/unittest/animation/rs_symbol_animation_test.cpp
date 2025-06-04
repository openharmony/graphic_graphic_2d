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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_symbol_animation.h"
#include "animation/rs_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSSymbolAnimationTest : public RSAnimationBaseTest {
public:
    std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig_;
    void InitSymbolConfigData();
};

void RSSymbolAnimationTest::InitSymbolConfigData()
{
    symbolAnimationConfig_ = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig_->symbolNodes = {};
    symbolAnimationConfig_->effectStrategy = Drawing::DrawingEffectStrategy::DISABLE;
    symbolAnimationConfig_->slope = -1; // -1: the Angle of the slash
    Drawing::Path path;
    path.AddCircle(100.0f, 100.0f, 40.0f); // 100.0f x, 100.0f y, 40.0f radius
    TextEngine::NodeLayerInfo info;
    info.path = path;
    Vector4f boundary = {0.0f, 0.0f, 40.0f, 40.0f}; // 0.0f 0.0f: offset, 40.0f width 40.0f height
    // symbolNodes size is 3
    for (uint32_t i = 0; i < 3; i++) {
        TextEngine::SymbolNode symbolNode;
        // -1: no effect, 0: first layer effect
        symbolNode.animationIndex = i == 0 ? -1 : 0;
        symbolNode.isMask = i == 1 ? true : false;
        symbolNode.pathsInfo.push_back(info);
        symbolNode.nodeBoundary = boundary;
        symbolAnimationConfig_->symbolNodes.push_back(symbolNode);
    }
}

/**
 * @tc.name: SetSymbolAnimation001
 * @tc.desc: SetSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    /**
     * @tc.steps: step2.1 rsNode_ is nullptr, symbolAnimationConfig is nullptr
     */
    bool flag1 = symbolAnimation.SetSymbolAnimation(nullptr);
    EXPECT_TRUE(flag1 == false);
    /**
     * @tc.steps: step2.2 rsNode_ is nullptr, symbolAnimationConfig is not nullptr
     */
    bool flag2 = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2 == false);
    /**
     * @tc.steps: step2.3 rsNode_ is not nullptr, symbolAnimationConfig is nullptr
     */
    symbolAnimation.SetNode(rootNode);
    bool flag3 = symbolAnimation.SetSymbolAnimation(nullptr);
    EXPECT_TRUE(flag3 == false);
    /**
     * @tc.steps: step2.4 effectStrategy is none type
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::NONE;
    symbolAnimationConfig->symbolSpanId = 1996; // the 1996 is the unique ID of a symbol
    bool flag4 = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag4 == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation001 end";
}

/**
 * @tc.name: SetSymbolAnimation002
 * @tc.desc: SetSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 1996; // the 1996 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::VARIABLE_COLOR;
    auto newCanvasNode = RSCanvasNode::Create();
    rootNode->canvasNodesListMap_[symbolAnimationConfig->symbolSpanId] = {{newCanvasNode->GetId(), newCanvasNode}};
    /**
     * @tc.steps: step2.1 test variable_color animation
     */
    bool flag1 = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag1 == false);
    /**
     * @tc.steps: step2.2 test replace animation
     */
    symbolAnimationConfig->symbolSpanId++;
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::REPLACE_APPEAR;
    bool flag2 = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2 == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation002 end";
}

/**
 * @tc.name: SetSymbolAnimation003
 * @tc.desc: SetSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolAnimation003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 1996; // the 1996 is the unique ID of a symbol
    /**
     * @tc.steps: step2.1 test TEXT FLip animation
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::TEXT_FLIP;
    bool flag1 = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_FALSE(flag1);
}

/**
 * @tc.name: SetPublicAnimation001
 * @tc.desc: SetPublicAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetPublicAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 19; // the 19 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::SCALE;
    symbolAnimationConfig->animationMode = 1; // 1 is wholesymbol
    symbolAnimationConfig->animationStart = true;
    // init symbolNode
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100 y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    TextEngine::SymbolNode symbolNode1;
    symbolNode1.symbolData = symbol;
    symbolNode1.nodeBoundary = {100, 100, 50, 50}; // 100 x, 100 y, 50 width, 50 height
    symbolAnimationConfig->symbolNodes.push_back(symbolNode1);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2.1 test SCALE animation with wholesymbol
     */
    bool flag1 = symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag1 == true);
    /**
     * @tc.steps: step2.2 test SCALE animation with two symbolNode and maskLayer
     */
    symbolAnimationConfig->animationMode = 0; // 0 is bylayer
    TextEngine::SymbolNode symbolNode2;
    symbolNode2.animationIndex = 1;
    symbolNode2.isMask = true;
    symbolAnimationConfig->symbolNodes.push_back(symbolNode2);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    bool flag2 = symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2 == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation001 end";
}

/**
 * @tc.name: SetPublicAnimation002
 * @tc.desc: SetPublicAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetPublicAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 20; // the 20 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::SCALE;
    symbolAnimationConfig->animationStart = true;
    symbolAnimationConfig->animationMode = 0; // 0 is bylayer
    // init symbolNode
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100 y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    TextEngine::SymbolNode symbolNode1;
    symbolNode1.symbolData = symbol;
    symbolNode1.nodeBoundary = {100, 100, 50, 50}; // 100 x, 100 y, 50 width, 50 height
    symbolAnimationConfig->symbolNodes.push_back(symbolNode1);
    TextEngine::SymbolNode symbolNode2;
    symbolNode2.animationIndex = 1;
    symbolAnimationConfig->symbolNodes.push_back(symbolNode2);
    TextEngine::SymbolNode symbolNode3;
    symbolNode3.animationIndex = -1; // -1 is the symbolNode without effect
    symbolAnimationConfig->symbolNodes.push_back(symbolNode3);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2. start test SCALE animation with three symbolnodes
     */
    bool flag = symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation002 end";
}

/**
 * @tc.name: SetPublicAnimation003
 * @tc.desc: SetPublicAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetPublicAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation003 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 21; // the 20 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::SCALE;
    symbolAnimationConfig->animationStart = true;
    // init symbolNode
    TextEngine::SymbolNode symbolNode;
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolNode.animationIndex = 1; // 1 is index of animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolNode.animationIndex = 2; // 2 is index of animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolNode.animationIndex = 3; // 3 is index of animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolNode.animationIndex = 4; // 4 is index of animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2.1 start test SCALE animation with five symbolnodes
     */
    bool flag1 = symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag1 == true);
    /**
     * @tc.steps: step2.2 start test VARIABLE_COLOR animation with five symbolnodes
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::VARIABLE_COLOR;
    bool flag2 = symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2 == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation003 end";
}

/**
 * @tc.name: SetDisappearConfig001
 * @tc.desc: SetDisappearConfig of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetDisappearConfig001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetDisappearConfig001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    auto disappearConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    std::shared_ptr<TextEngine::SymbolAnimationConfig> nullptrConfig = nullptr;
    /**
     * @tc.steps: step2 start test SetDisappearConfig
     */
    bool flag1 = symbolAnimation.SetDisappearConfig(nullptrConfig, nullptrConfig);
    EXPECT_TRUE(flag1 == false);

    bool flag2 = symbolAnimation.SetDisappearConfig(symbolAnimationConfig, nullptrConfig);
    EXPECT_TRUE(flag2 == false);

    bool flag3 = symbolAnimation.SetDisappearConfig(nullptrConfig, disappearConfig);
    EXPECT_TRUE(flag3 == false);

    bool flag4 = symbolAnimation.SetDisappearConfig(symbolAnimationConfig, disappearConfig);
    EXPECT_TRUE(flag4 == true);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetDisappearConfig001 end";
}

/**
 * @tc.name: SetDisappearConfig002
 * @tc.desc: SetDisappearConfig of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetDisappearConfig002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetDisappearConfig001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    auto disappearConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    // init replaceNodesSwapArr
    OHOS::Rosen::AnimationNodeConfig animationNodeConfig;
    animationNodeConfig.nodeId = 1999;
    rootNode->replaceNodesSwapArr_[APPEAR_STATUS] = {animationNodeConfig};
    /**
     * @tc.steps: step2. start test SetDisappearConfig
     */
    bool flag1 = symbolAnimation.SetDisappearConfig(symbolAnimationConfig, disappearConfig);
    EXPECT_TRUE(flag1 == true);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetDisappearConfig001 end";
}

/**
 * @tc.name: SetReplaceAnimation001
 * @tc.desc: SetReplaceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetReplaceAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetReplaceAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 96; // 96 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::REPLACE_APPEAR;
    symbolAnimationConfig->animationMode = 1; // 1 is wholesymbol
    symbolAnimationConfig->animationStart = true;
    // init symbolNode
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100 y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    TextEngine::SymbolNode symbolNode1;
    symbolNode1.symbolData = symbol;
    symbolNode1.nodeBoundary = {100, 100, 50, 50}; // 100 x, 100 y, 50 width, 50 height
    symbolAnimationConfig->symbolNodes.push_back(symbolNode1);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2.1 start test
     */
    bool flag1 = symbolAnimation.SetReplaceAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag1 == true);
    /**
     * @tc.steps: step2.2 start test replace two node
     */
    bool flag2 = symbolAnimation.SetReplaceAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2 == true);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetReplaceAnimation001 end";
}

/**
 * @tc.name: SetReplaceAnimation002
 * @tc.desc: SetReplaceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetReplaceAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetReplaceAnimation002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 96; // 96 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::REPLACE_APPEAR;
    symbolAnimationConfig->animationMode = 0; // 1 is byLayer
    symbolAnimationConfig->animationStart = true;
    // init symbolNodes
    Drawing::Path path1;
    path1.AddCircle(100, 100, 50); // 100 x, 100 y, 50 radius
    Drawing::DrawingSColor color = {1, 255, 255, 0}; // the color 1 A, 255 R, 255 G, 0 B
    TextEngine::SymbolNode symbolNode;
    symbolNode.pathsInfo = {{path1, color}};
    symbolNode.nodeBoundary = {100, 100, 50, 50}; // 100 x, 100 y, 50 width, 50 height
    symbolNode.animationIndex = -1; // the layer is no animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode); // the first node

    symbolNode.animationIndex = 0; // the layer is a animation whith animationIndex 0
    Drawing::Path path2;
    path2.AddCircle(150, 150, 50); // 150 x, 150 y, 50 radius
    symbolNode.pathsInfo = {{path2, color}};
    symbolAnimationConfig->symbolNodes.push_back(symbolNode); // the second node

    symbolNode.animationIndex = 1; // the layer is not animation whith animationIndex 0
    Drawing::Path path3;
    path3.AddCircle(200, 200, 50); // 200 x, 200 y, 50 radius
    symbolNode.pathsInfo = {{path3, color}};
    symbolAnimationConfig->symbolNodes.push_back(symbolNode); // the third node
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2.1 start test
     */
    bool flag1 = symbolAnimation.SetReplaceAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag1 == true);
    /**
     * @tc.steps: step2.2 start test replace two node
     */
    bool flag2 = symbolAnimation.SetReplaceAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2 == true);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetReplaceAnimation002 end";
}

/**
 * @tc.name: ChooseAnimation001
 * @tc.desc: ChooseAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ChooseAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ChooseAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.InitSupportAnimationTable();
    symbolAnimation.SetNode(rootNode);
    auto newCanvasNode = RSCanvasNode::Create();
    std::vector<Drawing::DrawingPiecewiseParameter> parameters;
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    /**
     * @tc.steps: step2.1 test SCALE
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::SCALE;
    bool flag1 = symbolAnimation.ChooseAnimation(newCanvasNode, parameters, symbolAnimationConfig);
    EXPECT_TRUE(flag1 == true);
    /**
     * @tc.steps: step2.2 test VARIABLE_COLOR
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::VARIABLE_COLOR;
    bool flag2 = symbolAnimation.ChooseAnimation(newCanvasNode, parameters, symbolAnimationConfig);
    EXPECT_TRUE(flag2 == false);
    /**
     * @tc.steps: step2.3 test SYMBOL_PULSE
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::PULSE;
    bool flag3 = symbolAnimation.ChooseAnimation(newCanvasNode, parameters, symbolAnimationConfig);
    EXPECT_TRUE(flag3 == false);
        /**
     * @tc.steps: step2.4 test NONE
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::NONE;
    bool flag4 = symbolAnimation.ChooseAnimation(newCanvasNode, parameters, symbolAnimationConfig);
    EXPECT_TRUE(flag4 == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ChooseAnimation001 end";
}

/**
 * @tc.name: InitSupportAnimationTableTest
 * @tc.desc: Verify Init of SupportAnimationTable
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, InitSupportAnimationTableTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest InitSupportAnimationTable start";
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.InitSupportAnimationTable(); // init data
    symbolAnimation.InitSupportAnimationTable(); // if data exists, data will not init again
    EXPECT_FALSE(symbolAnimation.publicSupportAnimations_.empty());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest InitSupportAnimationTable end";
}

/**
 * @tc.name: ReplaceAnimationTest001
 * @tc.desc: Verify the basic ability, ReplaceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ReplaceAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ReplaceAnimationTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> disappearGroupParas =
        {DISAPPEAR_FIRST_PHASE_PARAS, DISAPPEAR_SECOND_PHASE_PARAS};
    std::vector<Drawing::DrawingPiecewiseParameter> appearGroupParas =
        {APPEAR_FIRST_PHASE_PARAS, APPEAR_SECOND_PHASE_PARAS};
    std::shared_ptr<RSCanvasNode> appearCanvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(ANIMATION_START_BOUNDS);
    canvasNode->SetFrame(ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start ReplaceAnimation test
     */
    symbolAnimation.SpliceAnimation(canvasNode, disappearGroupParas);
    symbolAnimation.SpliceAnimation(canvasNode, appearGroupParas);
    EXPECT_FALSE(appearGroupParas.empty());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ReplaceAnimationTest001 end";
}

/**
 * @tc.name: PopNodeFromReplaceListTest001
 * @tc.desc: Verify PopNode From Replace list map
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, PopNodeFromReplaceListTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest001 start";
    /**
     * @tc.steps: step1.1 init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    uint64_t symbolSpanId = 1;
    rootNode->canvasNodesListMap_[symbolSpanId] = {{canvasNode->GetId(), canvasNode}};
    symbolAnimation.PopNodeFromReplaceList(symbolSpanId);
    /**
     * @tc.steps: step1.2 if symbolSpanId not in canvasNodesListMap_
     */
    symbolSpanId = 9999; // random value
    symbolAnimation.PopNodeFromReplaceList(symbolSpanId);
    EXPECT_TRUE(symbolSpanId);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest001 end";
}

/**
 * @tc.name: PopNodeFromReplaceListTest002
 * @tc.desc: Verify PopNode From Replace list map
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, PopNodeFromReplaceListTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest002 start";
    /**
     * @tc.steps: step1 if INVALID_STATUS&APPEAR_STATUS in replaceNodesSwapArr
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    uint64_t symbolSpanId = 1;
    rootNode->canvasNodesListMap_[symbolSpanId] = {{canvasNode->GetId(), canvasNode}};
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    TextEngine::SymbolNode symbolNode;
    symbolNode.symbolData = symbol;
    symbolNode.nodeBoundary = {100, 100, 50, 50};
    AnimationNodeConfig appearNodeConfig = {symbolNode, canvasNode->GetId(), symbolNode.animationIndex};
    rootNode->replaceNodesSwapArr_[APPEAR_STATUS] = {appearNodeConfig};
    rootNode->replaceNodesSwapArr_[INVALID_STATUS] = {appearNodeConfig};
    symbolAnimation.PopNodeFromReplaceList(symbolSpanId);
    EXPECT_TRUE(symbolAnimation.rsNode_);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest002 end";
}

/**
 * @tc.name: BounceAnimationTest001
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS,
                                                                     BOUNCE_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test
     */
    symbolAnimation.SpliceAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    EXPECT_FALSE(oneGroupParas.empty());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest001 end";
}

/**
 * @tc.name: BounceAnimationTest002
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest002 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS,
                                                                     BOUNCE_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test, no animation
     */
    symbolAnimation.SpliceAnimation(nullptr, oneGroupParas);
    NotifyStartAnimation();
    EXPECT_FALSE(oneGroupParas.empty());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest002 end";
}

/**
 * @tc.name: BounceAnimationTest003
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest003 start";
    /**
     * @tc.steps: step1. if parameters.size() < animationStageNum
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test, no animation
     */
    symbolAnimation.SpliceAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    EXPECT_FALSE(oneGroupParas.empty());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest003 end";
}

/**
 * @tc.name: ScaleAnimationBase001
 * @tc.desc: Verify the basic ability, ScaleAnimationBase of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ScaleAnimationBase001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    auto bounceSecondPhaseParas = BOUNCE_SECOND_PHASE_PARAS;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    symbolAnimation.ScaleAnimationBase(canvasNode, scaleProperty, bounceSecondPhaseParas, groupAnimation);
    /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_FALSE(groupAnimation.empty());
    EXPECT_FALSE(groupAnimation[0] == nullptr);
    groupAnimation[0]->Start(canvasNode);
    EXPECT_TRUE(groupAnimation[0]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase001 end";
}

/**
 * @tc.name: ScaleAnimationBase002
 * @tc.desc: Verify the basic ability, ScaleAnimationBase of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ScaleAnimationBase002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    Drawing::DrawingPiecewiseParameter testParas = {};
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    symbolAnimation.ScaleAnimationBase(canvasNode, scaleProperty, testParas, groupAnimation);
    /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_TRUE(groupAnimation.empty());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase002 end";
}

/**
 * @tc.name: ScaleAnimationBase003
 * @tc.desc: Verify the basic ability, ScaleAnimationBase of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ScaleAnimationBase003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase003 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    auto bounceSecondPhaseParas = BOUNCE_SECOND_PHASE_PARAS;
    bounceSecondPhaseParas.curveType = Drawing::DrawingCurveType::SHARP;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    symbolAnimation.ScaleAnimationBase(canvasNode, scaleProperty, bounceSecondPhaseParas, groupAnimation);
    /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_FALSE(groupAnimation.empty());
    EXPECT_FALSE(groupAnimation[0] == nullptr);
    groupAnimation[0]->Start(canvasNode);
    EXPECT_TRUE(groupAnimation[0]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase003 end";
}

/**
 * @tc.name: AppearAnimation001
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS,
                                                                     APPEAR_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test
     */
    symbolAnimation.SpliceAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    EXPECT_FALSE(oneGroupParas.empty());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation001 end";
}

/**
 * @tc.name: AppearAnimation002
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation002 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS,
                                                                     APPEAR_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test, no animation
     */
    symbolAnimation.SpliceAnimation(nullptr, oneGroupParas);
    NotifyStartAnimation();
    EXPECT_FALSE(oneGroupParas.empty());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation002 end";
}

/**
 * @tc.name: AppearAnimation003
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation003 start";
    /**
     * @tc.steps: step1. if parameters.size() < animationStageNum
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test, no animation
     */
    symbolAnimation.SpliceAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    EXPECT_FALSE(oneGroupParas.empty());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation003 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation001
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size(); i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(animation == nullptr);
    EXPECT_FALSE(animation->IsStarted());
    animation->Start(canvasNode);
    EXPECT_TRUE(animation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation001 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation002
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation002 start";
    /**
     * @tc.steps: step1. if no alpha data
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation002 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation003
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation003 start";
    /**
     * @tc.steps: step1. if alphaPropertyStages_.size() != TIME_PERCENTS.size()
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size() - 1; i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(symbolAnimation.alphaPropertyStages_.size() == TIME_PERCENTS.size());
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation003 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation004
 * @tc.desc:Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation004 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size(); i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(nullptr, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation004 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation005
 * @tc.desc: Get Paras for KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation005 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    uint32_t totalDuration = 0;
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.GetKeyframeAlphaAnimationParas(oneGroupParas, totalDuration, timePercents);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(getParasFlag == false);
    EXPECT_FALSE(totalDuration == 0);
    EXPECT_FALSE(timePercents.size() == 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation005 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation006
 * @tc.desc: Get Paras for KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation006 start";
    /**
     * @tc.steps: step1. if oneGroupParas is empty
     */
    auto symbolAnimation = RSSymbolAnimation();
    uint32_t totalDuration = 0;
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {};
    auto getParasFlag = symbolAnimation.GetKeyframeAlphaAnimationParas(oneGroupParas, totalDuration, timePercents);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(getParasFlag);
    EXPECT_FALSE(totalDuration);
    EXPECT_FALSE(timePercents.size());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation006 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation007
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation007 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, ANIMATION_DURATION, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(getParasFlag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation007 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation008
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation008 start";
    /**
     * @tc.steps: step1. if totalDuration <= 0
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, 0, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(getParasFlag == false);
    EXPECT_TRUE(timePercents.size() == 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation008 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation009
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation009 start";
    /**
     * @tc.steps: step1. if interval < 0
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_FIRST_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, ANIMATION_DURATION, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(getParasFlag == false);
    EXPECT_TRUE(timePercents.size() == 2);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation009 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation0010
 * @tc.desc: SetKeyframeAlphaAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation0010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0010 start";
    /**
     * @tc.steps: step1.1 init data if effectStrategy is variablecolor
     */
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>(VARIABLE_COLOR_CONFIG);
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto flag1 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, symbolAnimationConfig);
    /**
     * @tc.steps: step1.2 if node is nullptr
     */
    auto flag2 = symbolAnimation.SetKeyframeAlphaAnimation(nullptr, oneGroupParas, symbolAnimationConfig);
    /**
     * @tc.steps: step1.3 if symbolAnimationConfig is nullptr
     */
    auto flag3 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, nullptr);
    /**
     * @tc.steps: step1.4 if parameters is empty
     */
    std::vector<Drawing::DrawingPiecewiseParameter> emptyParameters = {};
    auto flag4 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, emptyParameters, symbolAnimationConfig);
    /**
     * @tc.steps: step2 start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(flag1 == true);
    EXPECT_TRUE(flag2 == false);
    EXPECT_TRUE(flag3 == false);
    EXPECT_TRUE(flag4 == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0010 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation0011
 * @tc.desc: SetKeyframeAlphaAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation0011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0011 start";
    /**
     * @tc.steps: step1 init data if effectStrategy is pulse
     */
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>(PULSE_CONFIG);
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto flag = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, symbolAnimationConfig);

    /**
     * @tc.steps: step2 start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(flag == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0011 end";
}

/**
 * @tc.name: SetSymbolGeometryTest001
 * @tc.desc: set geometry of a node, SetSymbolGeometry of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolGeometryTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto flag = symbolAnimation.SetSymbolGeometry(canvasNode, ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start SetSymbolGeometry test
     */
    EXPECT_FALSE(flag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest001 end";
}

/**
 * @tc.name: SetSymbolGeometryTest002
 * @tc.desc: set geometry of a node, SetSymbolGeometry of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolGeometryTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto flag = symbolAnimation.SetSymbolGeometry(nullptr, ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start SetSymbolGeometry test
     */
    EXPECT_TRUE(flag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest002 end";
}

/**
 * @tc.name: SetNodePivotTest001
 * @tc.desc: move the scale center to the center of a node
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetNodePivotTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest001 start";
    /**
     * @tc.steps: step1. init data if scale center equals the center of a node
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNodePivot(canvasNode);
    /**
     * @tc.steps: step2. start SetNodePivot test
     */
    NotifyStartAnimation();
    EXPECT_FALSE(symbolAnimation.rsNode_);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest001 end";
}

/**
 * @tc.name: SetNodePivotTest002
 * @tc.desc: move the scale center to the center of a node
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetNodePivotTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest002 start";
    /**
     * @tc.steps: step1. init data if scale center not equals the center of a node
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto newCanvasNode = RSCanvasNode::Create();
    Vector2f pivotOffset = Vector2f(0.1f, 0.1f); // 0.1f is offset on x-axis and y-axis
    auto pivotProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(pivotOffset);
    auto pivotModifier = std::make_shared<RSPivotModifier>(pivotProperty);
    newCanvasNode->AddModifier(pivotModifier);
    symbolAnimation.SetNodePivot(newCanvasNode);
    /**
     * @tc.steps: step2. start SetNodePivot test
     */
    NotifyStartAnimation();
    EXPECT_FALSE(symbolAnimation.rsNode_);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest002 end";
}

/**
 * @tc.name: SpliceAnimation001
 * @tc.desc: implement animation according to type
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SpliceAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SpliceAnimation001 start";
    /**
     * @tc.steps: step1. init data about various type
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> parameters = {BOUNCE_FIRST_PHASE_PARAS, APPEAR_SECOND_PHASE_PARAS};
    symbolAnimation.SpliceAnimation(canvasNode, parameters);
    /**
     * @tc.steps: step2. start SetNodePivot test
     */
    NotifyStartAnimation();
    EXPECT_FALSE(symbolAnimation.rsNode_);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SpliceAnimation001 end";
}

/**
 * @tc.name: DrawPathOnCanvas001
 * @tc.desc: set brush&pen and draw path
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, DrawPathOnCanvas001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest DrawPathOnCanvas001 start";
    /**
     * @tc.steps: step1. init data for symbolNode
     */
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100 y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    Drawing::DrawingSColor color = {1, 255, 255, 0}; // the color 1 A, 255 R, 255 G, 0 B
    TextEngine::NodeLayerInfo layerinfo;
    layerinfo.path = path;
    layerinfo.color = color;
    TextEngine::SymbolNode symbolNode;
    symbolNode.pathsInfo = {layerinfo};
    symbolNode.nodeBoundary = {100, 100, 50, 50}; // 100 x, 100 y, 50 width, 50 height
    symbolNode.symbolData = symbol;
    symbolNode.animationIndex = 1;
    symbolNode.isMask = true;
    Vector4f vector4f = Vector4f(50.f, 50.f, 50.f, 50.f); // the offset of path
    auto recordingCanvas = canvasNode->BeginRecording(CANVAS_NODE_BOUNDS_WIDTH,
                                                      CANVAS_NODE_BOUNDS_HEIGHT);
    auto symbolAnimation = RSSymbolAnimation();
    /**
     * @tc.steps: step2. start DrawPathOnCanvas test on nullptr
     */
    symbolAnimation.DrawPathOnCanvas(nullptr, symbolNode, vector4f);
    /**
     * @tc.steps: step3. start DrawPathOnCanvas test on recordingCanvas
     */
    symbolAnimation.DrawPathOnCanvas(recordingCanvas, symbolNode, vector4f);
    NotifyStartAnimation();
    EXPECT_FALSE(vector4f.IsZero());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest DrawPathOnCanvas001 end";
}

/**
 * @tc.name: AlphaAnimationBase001
 * @tc.desc: Verify the basic ability, AlphaAnimationBase of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AlphaAnimationBase001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AlphaAnimationBase001 start";
    /**
     * @tc.steps: step1. if parameters.size() < animationStageNum
     */
    auto symbolAnimation = RSSymbolAnimation();
    Drawing::DrawingPiecewiseParameter groupPara1 = APPEAR_FIRST_PHASE_PARAS;
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    std::shared_ptr<RSAnimatableProperty<float>> alphaProperty = nullptr;
    /**
     * @tc.steps: step2. start alphaAnimationBase test, no nullptr
     */
    symbolAnimation.AlphaAnimationBase(nullptr, alphaProperty, groupPara1, groupAnimation);
    /**
     * @tc.steps: step2. start alphaAnimationBase test, on the size is 0 of alphas
     */
    symbolAnimation.AlphaAnimationBase(canvasNode, alphaProperty, groupPara1, groupAnimation);
    /**
     * @tc.steps: step2. start alphaAnimationBase test, on the size < 2 of alphas
     */
    Drawing::DrawingPiecewiseParameter groupPara2;
    groupPara2.properties = {{"alpha", {1}}}; // alpha is 1
    symbolAnimation.AlphaAnimationBase(canvasNode, alphaProperty, groupPara2, groupAnimation);
    EXPECT_TRUE(groupAnimation.empty());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AlphaAnimationBase001 end";
}

/**
 * @tc.name: GroupAnimationStart001
 * @tc.desc: test GroupAnimationStart of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, GroupAnimationStart001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest GroupAnimationStart001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto newCanvasNode = RSCanvasNode::Create();
    std::vector<std::shared_ptr<RSAnimation>> animations = {};
    /**
     * @tc.steps: step2.1 test node is nullptr, animations is empty;
     */
    symbolAnimation.GroupAnimationStart(nullptr, animations);
    /**
     * @tc.steps: step2.2 test node is nullptr, animations is empty;
     */
    symbolAnimation.GroupAnimationStart(newCanvasNode, animations);
    /**
     * @tc.steps: step2.3 test node is nullptr, animations not is empty;
     */
    std::shared_ptr<RSAnimation> animation = nullptr;
    animations.push_back(animation);
    symbolAnimation.GroupAnimationStart(nullptr, animations);
    /**
     * @tc.steps: step2.4 test node is nullptr, animations not is empty;
     */
    symbolAnimation.GroupAnimationStart(newCanvasNode, animations);
    EXPECT_FALSE(animations.empty());
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest GroupAnimationStart001 end";
}

/**
 * @tc.name: PopNodeFromFlipListTest001
 * @tc.desc: Verify PopNode From Replace list map
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, PopNodeFromFlipListTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1 init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    uint64_t symbolSpanId = 111; // the 111 is the unique ID of a symbol
    /**
     * @tc.steps: step1 if symbolSpanId not in canvasNodesListMap
     */
    symbolAnimation.PopNodeFromFlipList(symbolSpanId);
    /**
     * @tc.steps: step2 if symbolSpanId in canvasNodesListMap
     */
    rootNode->canvasNodesListMap_[symbolSpanId] = {{INVALID_STATUS, canvasNode}};
    symbolAnimation.PopNodeFromFlipList(symbolSpanId);
    EXPECT_TRUE(rootNode->canvasNodesListMap_[symbolSpanId].empty());
}

/**
 * @tc.name: SetTextFlipAnimation001
 * @tc.desc: Test text flip animation respectively through invalid and valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetTextFlipAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 1; // the 1996 is the unique ID of a symbol
    /**
     * @tc.steps: step2.1 test Text FLip animation with Invalid animation parameters
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::TEXT_FLIP;
    bool flag = symbolAnimation.SetTextFlipAnimation(symbolAnimationConfig);
    EXPECT_FALSE(flag);

    /**
     * @tc.steps: step2.2 test Text FLip animation with valid animation parameters
     */
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    parameters.push_back({DISAPPEAR_FIRST_PHASE_PARAS, DISAPPEAR_SECOND_PHASE_PARAS});
    parameters.push_back({APPEAR_FIRST_PHASE_PARAS, APPEAR_SECOND_PHASE_PARAS});
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::TEXT_FLIP;
    symbolAnimationConfig->parameters = parameters;
    bool flag1 = symbolAnimation.SetTextFlipAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag1);

    symbolAnimationConfig->currentAnimationHasPlayed = true;
    bool flag2 = symbolAnimation.SetTextFlipAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2);
}

/**
 * @tc.name: SetTextFlipAnimation002
 * @tc.desc: Test text flip animation respectively through invalid and valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetTextFlipAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init data
     */
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    parameters.push_back({DISAPPEAR_FIRST_PHASE_PARAS, DISAPPEAR_SECOND_PHASE_PARAS});
    parameters.push_back({APPEAR_FIRST_PHASE_PARAS, APPEAR_SECOND_PHASE_PARAS});
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    uint64_t symbolSpanId = 1995; // the 1995 is the unique ID of a symbol
    symbolAnimationConfig->parameters = parameters;
    symbolAnimationConfig->symbolSpanId = symbolSpanId;
    // setEffectelement
    Drawing::Point offset = {100, 100}; // 100, 100 is the offset
    Drawing::Path path;
    path.AddCircle(100, 100, 40); // 100 x, 100, 40 radius
    symbolAnimationConfig->effectElement.width = 40; // 40 width of path
    symbolAnimationConfig->effectElement.height = 40; // 40 height of path
    symbolAnimationConfig->effectElement.path = path;
    symbolAnimationConfig-> effectElement.offset = offset;
    /**
     * @tc.steps: step2.1 test Text FLip animation with nullptr node
     */
    rootNode->canvasNodesListMap_[symbolSpanId] = {{APPEAR_STATUS, nullptr}};
    symbolAnimationConfig->currentAnimationHasPlayed = true;
    bool flag1 = symbolAnimation.SetTextFlipAnimation(symbolAnimationConfig);
    EXPECT_FALSE(flag1);
    /**
     * @tc.steps: step2.2 test Text Flip animation by valid node
     */
    rootNode->canvasNodesListMap_.erase(symbolSpanId);
    symbolAnimationConfig->currentAnimationHasPlayed = false;
    bool flag2 = symbolAnimation.SetTextFlipAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2);
    symbolAnimation.PopNodeFromFlipList(symbolSpanId);
    bool flag3 = symbolAnimation.SetTextFlipAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag3);
    NotifyStartAnimation();
}

/**
 * @tc.name: SetFlipAppear001
 * @tc.desc: Test SetFlipAppear of text flip animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetFlipAppear001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init data
     */
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    parameters.push_back({DISAPPEAR_FIRST_PHASE_PARAS, DISAPPEAR_SECOND_PHASE_PARAS});
    parameters.push_back({APPEAR_FIRST_PHASE_PARAS, APPEAR_SECOND_PHASE_PARAS});
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 199; // the 199 is the unique ID of a symbol
    symbolAnimationConfig->parameters = parameters;
    // setEffectelement
    Drawing::Point offset = {100, 100}; // 100, 100 is the offset
    Drawing::Path path;
    path.AddCircle(100, 100, 40); // 100 x, 100, 40 radius
    TextEngine::TextEffectElement effectElement;
    effectElement.width = 40; // 40 width of path
    effectElement.height = 40; // 40 height of path
    symbolAnimationConfig->effectElement = effectElement;
    /**
     * @tc.steps: step2.1 test Text FLip animation
     */
    bool flag1 = symbolAnimation.SetFlipAppear(symbolAnimationConfig, rootNode, true);
    EXPECT_TRUE(flag1);
    symbolAnimationConfig->effectElement.path = path;
    symbolAnimationConfig-> effectElement.offset = offset;
    bool flag2 = symbolAnimation.SetFlipAppear(symbolAnimationConfig, rootNode, true);
    EXPECT_TRUE(flag2);
}

/**
 * @tc.name: DrawPathOnCanvas002
 * @tc.desc: Test DrawPathOnCanvas of text flip animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, DrawPathOnCanvas002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init data for effectElement
     */
    Drawing::Point offset = {50, 50}; // 50, 50 is the offset
    Drawing::Path path;
    path.AddCircle(100, 100, 40); // 100 x, 100, 40 radius
    TextEngine::TextEffectElement effectElement;
    effectElement.path = path;
    effectElement.offset = offset;
    Drawing::Color color = Drawing::Color(255, 0, 255, 255); // set color r 255, g 0, b 255, a 255
    auto recordingCanvas = canvasNode->BeginRecording(CANVAS_NODE_BOUNDS_WIDTH,
                                                      CANVAS_NODE_BOUNDS_HEIGHT);
    auto symbolAnimation = RSSymbolAnimation();
    /**
     * @tc.steps: step2. Test whether the program will crash when nullptr pointer are input
     */
    symbolAnimation.DrawPathOnCanvas(nullptr, effectElement, color, offset);
    /**
     * @tc.steps: step3. Test whether the program can ren normally without crashing
     */
    symbolAnimation.DrawPathOnCanvas(recordingCanvas, effectElement, color, offset);
    EXPECT_FALSE(symbolAnimation.rsNode_);
}

/**
 * @tc.name: SpliceAnimation002
 * @tc.desc: Test SpliceAnimation of text flip animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SpliceAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init data about various type
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> parameters = {TRANSITION_FIRST_PARAS, BLUR_FIRST_PARAS};
    /**
     * @tc.steps: step2. Test whether the program can ren normally without crashing
     */
    symbolAnimation.SpliceAnimation(canvasNode, parameters);
    EXPECT_FALSE(symbolAnimation.rsNode_);
    NotifyStartAnimation();
}

/**
 * @tc.name: TranslateAnimationBase
 * @tc.desc: Test TranslateAnimationBase respectively through invalid and valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, TranslateAnimationBase, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init data about various type
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::shared_ptr<RSAnimatableProperty<Vector2f>> translateProperty = nullptr;
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    /**
     * @tc.steps: step2. Test invalid parameter input of translate.
     */
    Drawing::DrawingPiecewiseParameter flipParameter = TRANSITION_FIRST_PARAS;
    flipParameter.properties["ty"] = {};
    symbolAnimation.TranslateAnimationBase(canvasNode, translateProperty, flipParameter, groupAnimation);
    EXPECT_TRUE(groupAnimation.empty());
    /**
     * @tc.steps: step2. Test valid parameter input of translate.
     */
    symbolAnimation.TranslateAnimationBase(canvasNode, translateProperty, TRANSITION_FIRST_PARAS, groupAnimation);
    symbolAnimation.TranslateAnimationBase(canvasNode, translateProperty, TRANSITION_SECOND_PARAS, groupAnimation);
    EXPECT_FALSE(groupAnimation.empty());

    Drawing::DrawingPiecewiseParameter flipParameter1 = TRANSITION_FIRST_PARAS;
    flipParameter1.duration = 0;
    flipParameter1.delay = 300; // 300 is animation delay
    symbolAnimation.TranslateAnimationBase(canvasNode, translateProperty, flipParameter1, groupAnimation);
    EXPECT_FALSE(groupAnimation.empty());
    NotifyStartAnimation();
}

/**
 * @tc.name: BlurAnimationBase
 * @tc.desc: Test BlurAnimationBase respectively through invalid and valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BlurAnimationBase, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init data about various type
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::shared_ptr<RSAnimatableProperty<float>> blurProperty = nullptr;
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    /**
     * @tc.steps: step2. Test invalid parameter input of blur
     */
    Drawing::DrawingPiecewiseParameter flipParameter = BLUR_FIRST_PARAS;
    flipParameter.properties["blur"] = {0};
    symbolAnimation.BlurAnimationBase(canvasNode, blurProperty, flipParameter, groupAnimation);
    EXPECT_TRUE(groupAnimation.empty());
    /**
     * @tc.steps: step2. Test valid parameter input of blur
     */
    symbolAnimation.BlurAnimationBase(canvasNode, blurProperty, BLUR_FIRST_PARAS, groupAnimation);
    symbolAnimation.BlurAnimationBase(canvasNode, blurProperty, BLUR_SECOND_PARAS, groupAnimation);
    EXPECT_FALSE(groupAnimation.empty());

    Drawing::DrawingPiecewiseParameter flipParameter1 = BLUR_FIRST_PARAS;
    flipParameter1.duration = 0;
    flipParameter1.delay = 300; // 300 is animation delay
    symbolAnimation.BlurAnimationBase(canvasNode, blurProperty, flipParameter1, groupAnimation);
    EXPECT_FALSE(groupAnimation.empty());
    NotifyStartAnimation();
}

/**
 * @tc.name: SetSymbolAnimation004
 * @tc.desc: Test SetSymbolAnimation by invalid input
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolAnimation004, TestSize.Level1) {
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolNodes.resize(1); // symbolNodes size is 3
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::DISABLE;
    bool result = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetCreateSameNode001
 * @tc.desc: Test SetCreateSameNode
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetCreateSameNode001, TestSize.Level1) {
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    std::shared_ptr<RSNode> rsNode = nullptr;
    uint64_t symbolId = 115;
    /**
     * @tc.steps: step2. Test invalid input
     */
    rootNode->canvasNodesListMap_.erase(symbolId);
    symbolAnimation.CreateSameNode(symbolId, rsNode, nullptr);
    EXPECT_FALSE(rsNode);
    /**
     * @tc.steps: step2. Test valid input
     */
    symbolAnimation.CreateSameNode(symbolId, rsNode, rootNode);
    EXPECT_NE(rsNode, nullptr);

    rootNode->canvasNodesListMap_[symbolId].erase(symbolId);
    symbolAnimation.CreateSameNode(symbolId, rsNode, rootNode);
    EXPECT_NE(rsNode, nullptr);

    symbolAnimation.CreateSameNode(symbolId, rsNode, rootNode);
    EXPECT_NE(rsNode, nullptr);
}

/**
 * @tc.name: SetCreateNode001
 * @tc.desc: Test SetCreateSymbolNode and CreateSymbolReplaceNode by nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetCreateNode001, TestSize.Level1) {
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    std::shared_ptr<RSCanvasNode> rsNode;
    uint64_t symbolId = 115;
    Vector4f bounds;
    /**
     * @tc.steps: step2. Test invalid input
     */
    symbolAnimation.CreateSymbolNode(bounds, symbolId, 0, nullptr, rsNode);
    EXPECT_FALSE(rsNode);
    symbolAnimation.CreateSymbolReplaceNode(symbolAnimationConfig_, bounds, 0, nullptr, rsNode);
    EXPECT_FALSE(rsNode);
}

/**
 * @tc.name: SetDisableAnimation001
 * @tc.desc: Test SetDisableAnimation by invalid input
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetDisableAnimation001, TestSize.Level1) {
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolNodes.clear();
    /**
     * @tc.steps: step2. test symbolNodes is empty
     */
    bool result = symbolAnimation.SetDisableAnimation(symbolAnimationConfig);
    EXPECT_FALSE(result);
    /**
     * @tc.steps: step3. test GetAnimationGroupParameters failed
     */
    symbolAnimationConfig->symbolNodes.resize(3); // symbolNodes size is 3
    result = symbolAnimation.SetDisableAnimation(symbolAnimationConfig);
    EXPECT_FALSE(result);
    /**
     * @tc.steps: step4. test createSymbol failed
     */
    auto symbolAnimation1 = std::make_shared<RSSymbolAnimation>();
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::DISABLE;
    result = symbolAnimation1->SetDisableAnimation(symbolAnimationConfig);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetDisableAnimation002
 * @tc.desc: Test SetDisableAnimation by symbolNodes with path and parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetDisableAnimation002, TestSize.Level1) {
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    InitSymbolConfigData();
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    /**
     * @tc.steps: step2. test parameters is empty
     */
    symbolAnimationConfig_->symbolSpanId = 8888; // 8888 is symbolId
    bool result = symbolAnimation.SetDisableAnimation(symbolAnimationConfig_, parameters);
    EXPECT_FALSE(result);
    /**
     * @tc.steps: step3. test parameters not is empty
     */
    parameters = {{DISABLE_TRANSLATE_RATIO, DISABLE_CLIP_PROP, DISABLE_ALPHA_PROP}};
    result = symbolAnimation.SetDisableAnimation(symbolAnimationConfig_, parameters);
    EXPECT_TRUE(result);

    ASSERT_TRUE(symbolAnimationConfig_->symbolNodes.size() > 1);
    size_t n = symbolAnimationConfig_->symbolNodes.size() - 1;
    symbolAnimationConfig_->symbolNodes[n].animationIndex = 1; // 1: second layer effect
    result = symbolAnimation.SetDisableAnimation(symbolAnimationConfig_, parameters);
    EXPECT_FALSE(result);

    symbolAnimationConfig_->symbolNodes[n].isMask = true;
    result = symbolAnimation.SetDisableAnimation(symbolAnimationConfig_, parameters);
    EXPECT_FALSE(result);

    uint64_t symbolId = 115;
    rootNode->canvasNodesListMap_.erase(symbolId);
    symbolAnimationConfig_->symbolSpanId = symbolId;
    std::shared_ptr<RSNode> rsNode = RSCanvasNode::Create();
    symbolAnimation.CreateSameNode(symbolId, rsNode, rootNode);
    rsNode->canvasNodesListMap_[symbolId][0] = nullptr;
    result = symbolAnimation.SetDisableAnimation(symbolAnimationConfig_, parameters);
    EXPECT_FALSE(result);

    rootNode->canvasNodesListMap_[symbolId][symbolId] = nullptr;
    result = symbolAnimation.SetDisableAnimation(symbolAnimationConfig_, parameters);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetDisableBaseLayer001
 * @tc.desc: Test SetDisableBaseLayer by canvasNodes is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetDisableBaseLayer001, TestSize.Level1) {
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    InitSymbolConfigData();
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    Vector4f offsets = {10.0f, 10.0f, 15.0f, 15.0f}; // 10.0f 10.0f: first offset, 15.0f 15.0f: second offset
    /**
     * @tc.steps: step2. test canvasNodes is nullptr
     */
    uint64_t symbolId = 1196;
    symbolAnimationConfig_->symbolSpanId = symbolId;
    std::shared_ptr<RSNode> rsNode = RSCanvasNode::Create();
    symbolAnimation.CreateSameNode(symbolId, rsNode, rootNode);
    rsNode->canvasNodesListMap_[symbolId] = {{0, nullptr}};
    parameters = {{DISABLE_TRANSLATE_RATIO, DISABLE_CLIP_PROP, DISABLE_ALPHA_PROP}};
    bool result = symbolAnimation.SetDisableBaseLayer(rsNode, symbolAnimationConfig_, parameters, offsets);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetDisableParameter001
 * @tc.desc: Test SetDisableParameter
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetDisableParameter001, TestSize.Level1) {
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    TextEngine::SymbolNode symbolNode;
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    std::vector<Drawing::DrawingPiecewiseParameter> parameters;
    /**
     * @tc.steps: step2. test parameters is invalid
     */
    symbolAnimation.SetDisableParameter(parameters, symbolAnimationConfig);
    parameters = {DISABLE_ALPHA_PROP};
    symbolAnimation.SetDisableParameter(parameters, symbolAnimationConfig);
    InitSymbolConfigData();
    symbolAnimation.SetDisableParameter(parameters, symbolAnimationConfig_);
    EXPECT_FALSE(parameters[0].properties.count(TRANSLATE_PROP_X) > 0);
    /**
     * @tc.steps: step2. test parameters is valid
     */
    parameters = {DISABLE_TRANSLATE_RATIO};
    symbolAnimation.SetDisableParameter(parameters, symbolAnimationConfig_);
    EXPECT_TRUE(parameters[0].properties.count(TRANSLATE_PROP_X) > 0);
}

/**
 * @tc.name: SetClipAnimation001
 * @tc.desc: Test SetClipAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetClipAnimation001, TestSize.Level1) {
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    InitSymbolConfigData();
    auto symbolId = symbolAnimationConfig_->symbolSpanId;
    std::vector<Drawing::DrawingPiecewiseParameter> parameters;
    Vector4f offsets = {10.0f, 10.0f, 15.0f, 15.0f}; // 10.0f 10.0f: first offset, 15.0f 15.0f: second offset
    uint32_t index = 2;
    /**
     * @tc.steps: step2. test parameters is invalid
     */
    bool result = symbolAnimation.SetClipAnimation(rootNode, symbolAnimationConfig_, parameters, index, offsets);
    EXPECT_FALSE(result);
    /**
     * @tc.steps: step3. test parameters is valid
     */
    parameters = {DISABLE_TRANSLATE_RATIO, DISABLE_CLIP_PROP, DISABLE_ALPHA_PROP};
    rootNode->canvasNodesListMap_[symbolId] = {{index, nullptr}};
    result = symbolAnimation.SetClipAnimation(rootNode, symbolAnimationConfig_, parameters, index, offsets);
    EXPECT_FALSE(result);

    std::shared_ptr<RSNode> rsNode = RSCanvasNode::Create();
    rootNode->canvasNodesListMap_[symbolId][index] = rsNode;
    rsNode->canvasNodesListMap_[symbolId] = {{index, nullptr}};
    result = symbolAnimation.SetClipAnimation(rootNode, symbolAnimationConfig_, parameters, index, offsets);
    EXPECT_FALSE(result);
    rootNode->canvasNodesListMap_.erase(symbolId);

    result = symbolAnimation.SetClipAnimation(rootNode, symbolAnimationConfig_, parameters, index, offsets);
    EXPECT_TRUE(result);

    symbolAnimation.DrawClipOnCanvas(nullptr, symbolAnimationConfig_->symbolNodes[0], offsets);
    result = symbolAnimation.SetClipAnimation(rootNode, symbolAnimationConfig_, parameters, index, offsets);
    EXPECT_TRUE(result);
}
} // namespace Rosen
} // namespace OHOS