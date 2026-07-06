/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "gtest/hwext/gtest-tag.h"

#include "command_modifier/rs_canvas_node_command_modifier.h"
#include "pipeline/rs_simple_draw_cmd_list.h"
#include "recording/draw_cmd.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSCanvasNodeCommandModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasNodeCommandModifierTest::SetUpTestCase() {}
void RSCanvasNodeCommandModifierTest::TearDownTestCase() {}
void RSCanvasNodeCommandModifierTest::SetUp() {}
void RSCanvasNodeCommandModifierTest::TearDown() {}

/**
 * @tc.name: HdrPresentTest001
 * @tc.desc: Test all methods of HdrPresentCmdModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, HdrPresentTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    HdrPresentCmdParam param{true};
    auto mod = std::make_shared<HdrPresentCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::HDR_PRESENT);
    EXPECT_TRUE(mod->GetParam().hdrPresent_);

    HdrPresentCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().hdrPresent_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("hdrPresent"), std::string::npos);
}

/**
 * @tc.name: HdrPresentTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, HdrPresentTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    HdrPresentCmdParam param{true};
    auto mod = std::make_shared<HdrPresentCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ColorGamutTest001
 * @tc.desc: Test all methods of ColorGamutCmdModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, ColorGamutTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    constexpr uint32_t gamutVal = 5;
    ColorGamutCmdParam param{gamutVal};
    auto mod = std::make_shared<ColorGamutCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::COLOR_GAMUT);
    EXPECT_EQ(mod->GetParam().colorGamut_, gamutVal);

    constexpr uint32_t gamutVal2 = 10;
    ColorGamutCmdParam param2{gamutVal2};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().colorGamut_, gamutVal2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("colorGamut"), std::string::npos);
}

/**
 * @tc.name: ColorGamutTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, ColorGamutTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    ColorGamutCmdParam param{0};
    auto mod = std::make_shared<ColorGamutCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsFreezeTest001
 * @tc.desc: Test all methods of IsFreezeCmdModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, IsFreezeTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsFreezeCmdParam param{true, false};
    auto mod = std::make_shared<IsFreezeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_FREEZE);
    EXPECT_TRUE(mod->GetParam().isFreeze_);
    EXPECT_FALSE(mod->GetParam().isMarkedByUI_);

    IsFreezeCmdParam param2{false, true};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isFreeze_);
    EXPECT_TRUE(mod->GetParam().isMarkedByUI_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isFreeze"), std::string::npos);
}

/**
 * @tc.name: IsFreezeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, IsFreezeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    IsFreezeCmdParam param{true, true};
    auto mod = std::make_shared<IsFreezeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ClearRecordingTest001
 * @tc.desc: Test all methods of ClearRecordingCmdModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, ClearRecordingTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ClearRecordingCmdParam param{100, 200};
    auto mod = std::make_shared<ClearRecordingCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::CLEAR_RECORDING);
    EXPECT_EQ(mod->GetParam().width_, 100);
    EXPECT_EQ(mod->GetParam().height_, 200);

    ClearRecordingCmdParam param2{300, 400};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().width_, 300);
    EXPECT_EQ(mod->GetParam().height_, 400);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("width"), std::string::npos);
    EXPECT_NE(out.find("height"), std::string::npos);
}

/**
 * @tc.name: ClearRecordingTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, ClearRecordingTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    ClearRecordingCmdParam param{0, 0};
    auto mod = std::make_shared<ClearRecordingCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: FinishRecordTest001
 * @tc.desc: Test all methods of FinishRecordCmdModifier with valid drawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, FinishRecordTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    FinishRecordCmdParam param{0, std::make_shared<Drawing::DrawCmdList>()};
    auto mod = std::make_shared<FinishRecordCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::FINISH_RECORD);
    ASSERT_TRUE(mod->GetParam().drawingCmdList_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("drawCmdList"), std::string::npos);
}

/**
 * @tc.name: FinishRecordTest002
 * @tc.desc: Test DumpParam with null drawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, FinishRecordTest002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    FinishRecordCmdParam param{0, nullptr};
    auto mod = std::make_shared<FinishRecordCmdModifier>(node, param);
    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("null"), std::string::npos);
}

/**
 * @tc.name: FinishRecordTest003
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, FinishRecordTest003, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    FinishRecordCmdParam param{0, nullptr};
    auto mod = std::make_shared<FinishRecordCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: FinishRecordTest004
 * @tc.desc: Test SetParam same and different
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, FinishRecordTest004, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    FinishRecordCmdParam param{1, drawCmdList};
    auto mod = std::make_shared<FinishRecordCmdModifier>(node, param);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);

    FinishRecordCmdParam param2{2, nullptr};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().modifierType_, 2);
    EXPECT_FALSE(mod->GetParam().drawingCmdList_);
}

/**
 * @tc.name: DrawOnNodeTest001
 * @tc.desc: Test all methods of DrawOnNodeCmdModifier with valid drawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawOnNodeCmdParam param{0, std::make_shared<Drawing::DrawCmdList>()};
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::DRAW_ON_NODE);
    ASSERT_TRUE(mod->GetParam().drawingCmdList_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("drawCmdList"), std::string::npos);
}

/**
 * @tc.name: DrawOnNodeTest002
 * @tc.desc: Test DumpParam with null drawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawOnNodeCmdParam param{0, nullptr};
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("null"), std::string::npos);
}

/**
 * @tc.name: DrawOnNodeTest003
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest003, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    DrawOnNodeCmdParam param{0, nullptr};
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DrawOnNodeTest004
 * @tc.desc: Test UpdateToRenderWithResult with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest004, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    DrawOnNodeCmdParam param{0, nullptr};
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(weakNode, param);
    auto result = mod->UpdateToRenderWithResult();
    bool val = std::get<bool>(result);
    EXPECT_FALSE(val);
}

/**
 * @tc.name: DrawOnNodeTest005
 * @tc.desc: Test SetParam same and different
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest005, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawOnNodeCmdParam param{1, std::make_shared<Drawing::DrawCmdList>()};
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);

    DrawOnNodeCmdParam param2{2, nullptr};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().modifierType_, 2);
}

/**
 * @tc.name: HdrPresentTest003
 * @tc.desc: Test SetParam same param
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, HdrPresentTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    HdrPresentCmdParam param{true};
    auto mod = std::make_shared<HdrPresentCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().hdrPresent_);
}

/**
 * @tc.name: ColorGamutTest003
 * @tc.desc: Test SetParam same param
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, ColorGamutTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    constexpr uint32_t gamutVal = 5;
    ColorGamutCmdParam param{gamutVal};
    auto mod = std::make_shared<ColorGamutCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().colorGamut_, gamutVal);
}

/**
 * @tc.name: IsFreezeTest003
 * @tc.desc: Test SetParam same param
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, IsFreezeTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsFreezeCmdParam param{true, false};
    auto mod = std::make_shared<IsFreezeCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isFreeze_);
    EXPECT_FALSE(mod->GetParam().isMarkedByUI_);
}

/**
 * @tc.name: ClearRecordingTest003
 * @tc.desc: Test SetParam same param
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, ClearRecordingTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ClearRecordingCmdParam param{100, 200};
    auto mod = std::make_shared<ClearRecordingCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().width_, 100);
    EXPECT_EQ(mod->GetParam().height_, 200);
}

/**
 * @tc.name: HdrPresentTest004
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, HdrPresentTest004, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    HdrPresentCmdParam param{true};
    auto mod = std::make_shared<HdrPresentCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ColorGamutTest004
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, ColorGamutTest004, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ColorGamutCmdParam param{5};
    auto mod = std::make_shared<ColorGamutCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsFreezeTest004
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, IsFreezeTest004, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsFreezeCmdParam param{true, false};
    auto mod = std::make_shared<IsFreezeCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ClearRecordingTest004
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, ClearRecordingTest004, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ClearRecordingCmdParam param{100, 200};
    auto mod = std::make_shared<ClearRecordingCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: FinishRecordTest005
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, FinishRecordTest005, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    FinishRecordCmdParam param{0, std::make_shared<Drawing::DrawCmdList>()};
    auto mod = std::make_shared<FinishRecordCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DrawOnNodeTest006
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest006, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawOnNodeCmdParam param{0, std::make_shared<Drawing::DrawCmdList>()};
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DrawOnNodeTest007
 * @tc.desc: Test UpdateToRenderWithResult with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest007, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawOnNodeCmdParam param{0, std::make_shared<Drawing::DrawCmdList>()};
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    auto result = mod->UpdateToRenderWithResult();
    bool val = std::get<bool>(result);
    EXPECT_FALSE(val);
}

/**
 * @tc.name: FinishRecordTest006
 * @tc.desc: Test UpdateToRender with simpleDrawCmdList_ only (drawingCmdList_ is null)
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, FinishRecordTest006, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 100, opItems);
 
    FinishRecordCmdParam param{0, nullptr};
    param.simpleDrawCmdList_ = simpleDrawCmdList;
    auto mod = std::make_shared<FinishRecordCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}
 
/**
 * @tc.name: FinishRecordTest007
 * @tc.desc: Test UpdateToRender with both drawingCmdList_ and simpleDrawCmdList_ as null
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, FinishRecordTest007, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    FinishRecordCmdParam param{0, nullptr};
    param.simpleDrawCmdList_ = nullptr;
    auto mod = std::make_shared<FinishRecordCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}
 
/**
 * @tc.name: DrawOnNodeTest008
 * @tc.desc: Test UpdateToRender with simpleDrawCmdList_ only (drawingCmdList_ is null)
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest008, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 100, opItems);
 
    DrawOnNodeCmdParam param{0, nullptr};
    param.simpleDrawCmdList_ = simpleDrawCmdList;
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}
 
/**
 * @tc.name: DrawOnNodeTest009
 * @tc.desc: Test UpdateToRender with both drawingCmdList_ and simpleDrawCmdList_ as null
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest009, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawOnNodeCmdParam param{0, nullptr};
    param.simpleDrawCmdList_ = nullptr;
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}
 
/**
 * @tc.name: DrawOnNodeTest010
 * @tc.desc: Test UpdateToRenderWithResult with simpleDrawCmdList_ only (drawingCmdList_ is null)
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest010, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 100, opItems);
 
    DrawOnNodeCmdParam param{0, nullptr};
    param.simpleDrawCmdList_ = simpleDrawCmdList;
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    auto result = mod->UpdateToRenderWithResult();
    bool val = std::get<bool>(result);
    EXPECT_TRUE(val);
}
 
/**
 * @tc.name: DrawOnNodeTest011
 * @tc.desc: Test UpdateToRenderWithResult with both drawingCmdList_ and simpleDrawCmdList_ as null
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest011, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawOnNodeCmdParam param{0, nullptr};
    param.simpleDrawCmdList_ = nullptr;
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    auto result = mod->UpdateToRenderWithResult();
    bool val = std::get<bool>(result);
    EXPECT_TRUE(val);
}
 
/**
 * @tc.name: FinishRecordTest008
 * @tc.desc: Test DumpParam with simpleDrawCmdList_ only
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, FinishRecordTest008, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 100, opItems);
 
    FinishRecordCmdParam param{0, nullptr};
    param.simpleDrawCmdList_ = simpleDrawCmdList;
    auto mod = std::make_shared<FinishRecordCmdModifier>(node, param);
    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("simple size"), std::string::npos);
}
 
/**
 * @tc.name: DrawOnNodeTest012
 * @tc.desc: Test DumpParam with simpleDrawCmdList_ only
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandModifierTest, DrawOnNodeTest012, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 100, opItems);
 
    DrawOnNodeCmdParam param{0, nullptr};
    param.simpleDrawCmdList_ = simpleDrawCmdList;
    auto mod = std::make_shared<DrawOnNodeCmdModifier>(node, param);
    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("simple size"), std::string::npos);
}

} // namespace Rosen
} // namespace OHOS
