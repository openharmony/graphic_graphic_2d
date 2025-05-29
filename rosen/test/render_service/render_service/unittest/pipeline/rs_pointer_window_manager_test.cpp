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
#include "rs_test_util.h"

#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_pointer_window_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSPointerWindowManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPointerWindowManagerTest::SetUpTestCase() {}
void RSPointerWindowManagerTest::TearDownTestCase() {}
void RSPointerWindowManagerTest::SetUp() {}
void RSPointerWindowManagerTest::TearDown() {}

/**
 * @tc.name: UpdatePointerDirtyToGlobalDirty
 * @tc.desc: Test UpdatePointerDirtyToGlobalDirty, CURSOR_NODE
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSPointerWindowManagerTest, UpdatePointerDirtyToGlobalDirtyTest, TestSize.Level2)
{
    auto rsPointerWindowManager = std::make_shared<RSPointerWindowManager>();
    ASSERT_NE(rsPointerWindowManager, nullptr);
    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config, rsContext->weak_from_this());
    displayNode->InitRenderParams();
    auto node = RSTestUtil::CreateSurfaceNode();
    node->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    node->name_ = "pointer window";
    node->GetDirtyManager()->SetCurrentFrameDirtyRect(RectI{1, 1, 1, 1});
    // run
    rsPointerWindowManager->UpdatePointerDirtyToGlobalDirty(node, displayNode);
    ASSERT_NE(rsPointerWindowManager->IsNeedForceCommitByPointer(), true);
}


/**
 * @tc.name: SetHardCursorNodeInfo001
 * @tc.desc: Test SetHardCursorNodeInfo
 * @tc.type: FUNC
 * @tc.require: #IB1MHP
 */
HWTEST_F(RSPointerWindowManagerTest, SetHardCursorNodeInfoTest001, TestSize.Level2)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetHardCursorNodeInfo(surfaceNode);
    ASSERT_EQ(rsPointerWindowManager.hardCursorNodes_, nullptr);
}

/**
 * @tc.name: SetHardCursorNodeInfo002
 * @tc.desc: Test SetHardCursorNodeInfo
 * @tc.type: FUNC
 * @tc.require: #IB1MHP
 */
HWTEST_F(RSPointerWindowManagerTest, SetHardCursorNodeInfoTest002, TestSize.Level2)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceNode->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    surfaceNode->name_ = "pointer window";
    surfaceNode->isOnTheTree_ = true;
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetHardCursorNodeInfo(surfaceNode);
    ASSERT_NE(rsPointerWindowManager.hardCursorNodeMap_.size(), 0);
}
/**
 * @tc.name: HardCursorCreateLayerForDirect
 * @tc.desc: Test HardCursorCreateLayerForDirect
 * @tc.type: FUNC
 * @tc.require: #IB1MHP
 */
HWTEST_F(RSPointerWindowManagerTest, HardCursorCreateLayerForDirectTest, TestSize.Level2)
{
    auto processor = std::make_shared<RSUniRenderProcessor>();
    ASSERT_NE(processor, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode->name_ = "pointer window";
    surfaceNode->isOnTheTree_ = true;
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetHardCursorNodeInfo(surfaceNode);
    auto& hardCursorNodes = rsPointerWindowManager.GetHardCursorNode();
    ASSERT_NE(hardCursorNodes.size(), 0);
    for (auto& [_, drawable] :  hardCursorNodes) {
        auto hardCursorNode = drawable;
        hardCursorNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(1);
    }
    rsPointerWindowManager.HardCursorCreateLayerForDirect(processor);
}

/**
 * @tc.name: SetIsPointerEnableHwcTest
 * @tc.desc: Test SetIsPointerEnableHwc and GetIsPointerEnableHwc
 * @tc.type: FUNC
 * @tc.require: issueIB2O0L
 */
HWTEST_F(RSPointerWindowManagerTest, SetIsPointerEnableHwcTest, TestSize.Level1)
{
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetIsPointerEnableHwc(true);
    EXPECT_EQ(rsPointerWindowManager.GetIsPointerEnableHwc(), true);
    rsPointerWindowManager.SetIsPointerEnableHwc(false);
    EXPECT_EQ(rsPointerWindowManager.GetIsPointerEnableHwc(), false);
}

/**
 * @tc.name: SetIsPointerCanSkipFrameTest
 * @tc.desc: Test SetIsPointerCanSkipFrame, GetIsPointerCanSkipFrame and IsPointerCanSkipFrameCompareChange
 * @tc.type: FUNC
 * @tc.require: issueIB2O0L
 */
HWTEST_F(RSPointerWindowManagerTest, SetIsPointerCanSkipFrameTest, TestSize.Level1)
{
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetIsPointerCanSkipFrame(true);
    EXPECT_EQ(rsPointerWindowManager.GetIsPointerCanSkipFrame(), true);
    rsPointerWindowManager.SetIsPointerCanSkipFrame(false);
    EXPECT_EQ(rsPointerWindowManager.GetIsPointerCanSkipFrame(), false);
    rsPointerWindowManager.IsPointerCanSkipFrameCompareChange(false, true);
    EXPECT_EQ(rsPointerWindowManager.GetIsPointerCanSkipFrame(), true);
}

/**
 * @tc.name: SetRsNodeIdTest
 * @tc.desc: Test SetRsNodeId and GetRsNodeId
 * @tc.type: FUNC
 * @tc.require: issueIB2O0L
 */
HWTEST_F(RSPointerWindowManagerTest, SetRsNodeIdTest, TestSize.Level1)
{
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetRsNodeId(1);
    EXPECT_EQ(rsPointerWindowManager.GetRsNodeId(), 1);
    rsPointerWindowManager.SetRsNodeId(99);
    EXPECT_EQ(rsPointerWindowManager.GetRsNodeId(), 99);
}

/**
 * @tc.name: SetBoundHasUpdateTest
 * @tc.desc: Test SetBoundHasUpdate, GetBoundHasUpdate and BoundHasUpdateCompareChange
 * @tc.type: FUNC
 * @tc.require: issueIB2O0L
 */
HWTEST_F(RSPointerWindowManagerTest, SetBoundHasUpdateTest, TestSize.Level1)
{
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetBoundHasUpdate(true);
    EXPECT_EQ(rsPointerWindowManager.GetBoundHasUpdate(), true);
    rsPointerWindowManager.SetBoundHasUpdate(false);
    EXPECT_EQ(rsPointerWindowManager.GetBoundHasUpdate(), false);
    rsPointerWindowManager.BoundHasUpdateCompareChange(false, true);
    EXPECT_EQ(rsPointerWindowManager.GetBoundHasUpdate(), true);
}

/**
 * @tc.name: SetBoundTest
 * @tc.desc: Test SetBound and GetBound
 * @tc.type: FUNC
 * @tc.require: issueIB2O0L
 */
HWTEST_F(RSPointerWindowManagerTest, SetBoundTest, TestSize.Level1)
{
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetBound({1.0f, 1.0f, 1.0f, 1.0f});
    EXPECT_EQ(rsPointerWindowManager.GetBound().x, 1.0f);
    EXPECT_EQ(rsPointerWindowManager.GetBound().y, 1.0f);
    EXPECT_EQ(rsPointerWindowManager.GetBound().z, 1.0f);
    EXPECT_EQ(rsPointerWindowManager.GetBound().w, 1.0f);
}

/**
 * @tc.name: UpdatePointerInfoTest
 * @tc.desc: Test UpdatePointerInfo
 * @tc.type: FUNC
 * @tc.require: issueIB2O0L
 */
HWTEST_F(RSPointerWindowManagerTest, UpdatePointerInfoTest, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto node = surfaceNode->ReinterpretCastTo<RSRenderNode>();
    node->GetMutableRenderProperties().SetBounds({1.0f, 1.0f, 1.0f, 1.0f});
    EXPECT_EQ(node->GetMutableRenderProperties().GetBounds().x_, 1.0f);
    EXPECT_EQ(node->GetMutableRenderProperties().GetBounds().y_, 1.0f);
    EXPECT_EQ(node->GetMutableRenderProperties().GetBounds().z_, 1.0f);
    EXPECT_EQ(node->GetMutableRenderProperties().GetBounds().w_, 1.0f);
}
} // OHOS::Rosen