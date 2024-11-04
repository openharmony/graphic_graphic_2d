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

#include "pipeline/rs_context.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "pipeline/rs_uni_render_processor.h"

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
 * @tc.desc: Test UpdatePointerDirtyToGlobalDirty, SELF_DRAWING_WINDOW_NODE
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
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
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
    auto rsPointerWindowManager = RSPointerWindowManager::Instance();
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
    surfaceNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode->name_ = "pointer window";
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetHardCursorNodeInfo(surfaceNode);
    ASSERT_NE(rsPointerWindowManager.hardCursorNodes_, nullptr);
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
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    auto& hardCursorNodes = rsPointerWindowManager.GetHardCursorNode();
    ASSERT_NE(hardCursorNodes, nullptr);
    hardCursorNodes->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(1);
    rsPointerWindowManager.HardCursorCreateLayerForDirect(processor);
}
} // OHOS::Rosen