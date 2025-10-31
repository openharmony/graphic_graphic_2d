/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "feature/window_keyframe/rs_window_keyframe_node.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_surface_node.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSWindowKeyFrameNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSWindowKeyFrameNodeTest::SetUpTestCase() {}
void RSWindowKeyFrameNodeTest::TearDownTestCase() {}
void RSWindowKeyFrameNodeTest::SetUp() {}
void RSWindowKeyFrameNodeTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeTest, Create001, TestSize.Level1)
{
	// return shared_ptr
    RSWindowKeyFrameNode::SharedPtr keyframeNode = RSWindowKeyFrameNode::Create();
    ASSERT_NE(keyframeNode, nullptr);

    auto rsUiCtx = std::make_shared<RSUIContext>();
    keyframeNode = RSWindowKeyFrameNode::Create(false, false, rsUiCtx);
    ASSERT_NE(keyframeNode, nullptr);
}

/**
 * @tc.name: Create002
 * @tc.desc: test Create on feature disabled
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeTest, Create002, TestSize.Level1)
{
    bool bEnable = RSSystemProperties::GetWindowKeyFrameEnabled();

    system::SetParameter("rosen.graphic.windowkeyframe.enabled", "0");
    RSWindowKeyFrameNode::SharedPtr keyframeNode = RSWindowKeyFrameNode::Create();
    EXPECT_EQ(keyframeNode, nullptr);
    system::SetParameter("rosen.graphic.windowkeyframe.enabled", bEnable ? "1" : "0");

    bool bUniRenderEnableBak = RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::isUniRenderEnabled_ = false;
    keyframeNode = RSWindowKeyFrameNode::Create();
    RSSystemProperties::isUniRenderEnabled_ = bUniRenderEnableBak;
    EXPECT_EQ(keyframeNode, nullptr);
}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSSurfaceNode and add RSWindowKeyFrameNode as child
     */
    RSSurfaceNodeConfig surfaceNodeConfig;
    RSSurfaceNode::SharedPtr appNode = RSSurfaceNode::Create(surfaceNodeConfig);
    ASSERT_TRUE(appNode != nullptr);

    RSWindowKeyFrameNode::SharedPtr keyframeNode = RSWindowKeyFrameNode::Create(false);
    ASSERT_NE(keyframeNode, nullptr);
    appNode->AddChild(keyframeNode, -1);

    /**
     * @tc.steps: step2. remove child
     */
    appNode->RemoveChild(keyframeNode);
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSSurfaceNode and add RSWindowKeyFrameNode as child
     */
    RSSurfaceNodeConfig surfaceNodeConfig;
    RSSurfaceNode::SharedPtr appNode = RSSurfaceNode::Create(surfaceNodeConfig);
    ASSERT_TRUE(appNode != nullptr);

    RSWindowKeyFrameNode::SharedPtr keyframeNode = RSWindowKeyFrameNode::Create(false);
    ASSERT_NE(keyframeNode, nullptr);
    appNode->AddChild(keyframeNode, -1);

    /**
     * @tc.steps: step2. remove child
     */
    keyframeNode->RemoveFromTree();
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameNodeTest, SetFreeze, TestSize.Level1)
{
    RSWindowKeyFrameNode::SharedPtr keyframeNode = RSWindowKeyFrameNode::Create();
    ASSERT_NE(keyframeNode, nullptr);
    keyframeNode->SetFreeze(true);
    EXPECT_EQ(keyframeNode->GetType(), RSUINodeType::WINDOW_KEYFRAME_NODE);
}

/**
 * @tc.name: WriteToParcel
 * @tc.desc: test WriteToParcel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameNodeTest, WriteToParcel, TestSize.Level1)
{
    RSWindowKeyFrameNode::SharedPtr keyframeNode = RSWindowKeyFrameNode::Create();
    ASSERT_NE(keyframeNode, nullptr);

    Parcel parcel;
    EXPECT_TRUE(keyframeNode->WriteToParcel(parcel));
}

/**
 * @tc.name: ReadFromParcel
 * @tc.desc: test ReadFromParcel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameNodeTest, ReadFromParcel, TestSize.Level1)
{
    Parcel parcel;
    auto keyframeNode = RSWindowKeyFrameNode::ReadFromParcel(parcel);
    EXPECT_EQ(keyframeNode, nullptr);

    NodeId nodeId = 1;
    NodeId linkedNodeId = 2;
    parcel.WriteUint64(nodeId);
    keyframeNode = RSWindowKeyFrameNode::ReadFromParcel(parcel);
    EXPECT_EQ(keyframeNode, nullptr);

    parcel.ClearObjects();
    parcel.WriteUint64(nodeId);
    parcel.WriteBool(false);
    keyframeNode = RSWindowKeyFrameNode::ReadFromParcel(parcel);
    EXPECT_EQ(keyframeNode, nullptr);

    parcel.ClearObjects();
    parcel.WriteUint64(nodeId);
    parcel.WriteBool(false);
    parcel.WriteUint64(linkedNodeId);
    keyframeNode = RSWindowKeyFrameNode::ReadFromParcel(parcel);
    ASSERT_NE(keyframeNode, nullptr);
    EXPECT_EQ(keyframeNode->IsRenderServiceNode(), false);
    EXPECT_EQ(keyframeNode->GetId(), nodeId);
    EXPECT_EQ(keyframeNode->GetLinkedNodeId(), linkedNodeId);

    parcel.ClearObjects();
    parcel.WriteUint64(nodeId);
    parcel.WriteBool(false);
    parcel.WriteUint64(linkedNodeId);
    auto keyframeNodeNap = RSWindowKeyFrameNode::ReadFromParcel(parcel);
    EXPECT_EQ(keyframeNode, keyframeNodeNap);
}

/**
 * @tc.name: RegisterNodeMap
 * @tc.desc: test RegisterNodeMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameNodeTest, RegisterNodeMap, TestSize.Level1)
{
    auto rsUiCtx = std::make_shared<RSUIContext>();
    RSWindowKeyFrameNode::SharedPtr keyframeNode = std::make_shared<RSWindowKeyFrameNode>(false);
    ASSERT_NE(keyframeNode, nullptr);

    keyframeNode->RegisterNodeMap();

    keyframeNode->rsUIContext_ = rsUiCtx;
    keyframeNode->RegisterNodeMap();

    auto& nodeMap = rsUiCtx->GetNodeMap();
    auto nodeInMap = nodeMap.GetNode(keyframeNode->GetId());
    EXPECT_EQ(keyframeNode, nodeInMap);
}

/**
 * @tc.name: SetLinkedNodeId
 * @tc.desc: test SetLinkedNodeId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameNodeTest, SetLinkedNodeId, TestSize.Level1)
{
    auto rsUiCtx = std::make_shared<RSUIContext>();
    RSWindowKeyFrameNode::SharedPtr keyframeNode = std::make_shared<RSWindowKeyFrameNode>(false);
    ASSERT_NE(keyframeNode, nullptr);

    NodeId tmpId = 0xFF;
    keyframeNode->SetLinkedNodeId(tmpId);
    EXPECT_EQ(keyframeNode->GetLinkedNodeId(), tmpId);
}

} // namespace OHOS::Rosen
