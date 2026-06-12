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

#include "command_modifier/rs_window_keyframe_node_command_modifier.h"
#include "feature/window_keyframe/rs_window_keyframe_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSWindowKeyFrameNodeCommandModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSWindowKeyFrameNodeCommandModifierTest::SetUpTestCase() {}
void RSWindowKeyFrameNodeCommandModifierTest::TearDownTestCase() {}
void RSWindowKeyFrameNodeCommandModifierTest::SetUp() {}
void RSWindowKeyFrameNodeCommandModifierTest::TearDown() {}

/**
 * @tc.name: WkfIsFreezeTest001
 * @tc.desc: Test GetType, SetParam same/different, GetParam, DumpParam
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeCommandModifierTest, WkfIsFreezeTest001, TestSize.Level1)
{
    auto node = RSWindowKeyFrameNode::Create();
    WkfIsFreezeCmdParam param{true, false};
    auto mod = std::make_shared<WkfIsFreezeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::WKF_IS_FREEZE);
    EXPECT_TRUE(mod->GetParam().isFreeze_);
    EXPECT_FALSE(mod->GetParam().isMarkedByUI_);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);

    WkfIsFreezeCmdParam param2{false, true};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isFreeze_);
    EXPECT_TRUE(mod->GetParam().isMarkedByUI_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isFreeze"), std::string::npos);
    EXPECT_NE(out.find("isMarkedByUI"), std::string::npos);
}

/**
 * @tc.name: WkfIsFreezeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeCommandModifierTest, WkfIsFreezeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSWindowKeyFrameNode::Create(); weakNode = n; }
    WkfIsFreezeCmdParam param{true, true};
    auto mod = std::make_shared<WkfIsFreezeCmdModifier>(weakNode, param);
    mod->UpdateToRender();
}

/**
 * @tc.name: WkfLinkedNodeIdTest001
 * @tc.desc: Test GetType, SetParam same/different, GetParam, DumpParam
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeCommandModifierTest, WkfLinkedNodeIdTest001, TestSize.Level1)
{
    auto node = RSWindowKeyFrameNode::Create();
    constexpr NodeId linkedId = 100;
    WkfLinkedNodeIdCmdParam param{linkedId};
    auto mod = std::make_shared<WkfLinkedNodeIdCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::WKF_LINKED_NODE_ID);
    EXPECT_EQ(mod->GetParam().linkedNodeId_, linkedId);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);

    constexpr NodeId linkedId2 = 200;
    WkfLinkedNodeIdCmdParam param2{linkedId2};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().linkedNodeId_, linkedId2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("linkedNodeId"), std::string::npos);
    EXPECT_NE(out.find("200"), std::string::npos);
}

/**
 * @tc.name: WkfLinkedNodeIdTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeCommandModifierTest, WkfLinkedNodeIdTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSWindowKeyFrameNode::Create(); weakNode = n; }
    WkfLinkedNodeIdCmdParam param{0};
    auto mod = std::make_shared<WkfLinkedNodeIdCmdModifier>(weakNode, param);
    mod->UpdateToRender();
}

/**
 * @tc.name: WkfIsFreezeTest003
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeCommandModifierTest, WkfIsFreezeTest003, TestSize.Level1)
{
    auto node = RSWindowKeyFrameNode::Create();
    WkfIsFreezeCmdParam param{true, false};
    auto mod = std::make_shared<WkfIsFreezeCmdModifier>(node, param);
    mod->UpdateToRender();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: WkfLinkedNodeIdTest003
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeCommandModifierTest, WkfLinkedNodeIdTest003, TestSize.Level1)
{
    auto node = RSWindowKeyFrameNode::Create();
    constexpr NodeId linkedId = 100;
    WkfLinkedNodeIdCmdParam param{linkedId};
    auto mod = std::make_shared<WkfLinkedNodeIdCmdModifier>(node, param);
    mod->UpdateToRender();
    EXPECT_TRUE(true);
}

} // namespace Rosen
} // namespace OHOS
