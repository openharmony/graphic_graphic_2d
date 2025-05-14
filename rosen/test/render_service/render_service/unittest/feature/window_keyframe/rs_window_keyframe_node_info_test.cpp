/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "feature/window_keyframe/rs_window_keyframe_node_info.h"
#include "params/rs_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSWindowKeyframeNodeInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSWindowKeyframeNodeInfoTest::SetUpTestCase() {}
void RSWindowKeyframeNodeInfoTest::TearDownTestCase() {}
void RSWindowKeyframeNodeInfoTest::SetUp() {}
void RSWindowKeyframeNodeInfoTest::TearDown() {}

/**
 * @tc.name: UpdateLinkedNodeId
 * @tc.desc: Test UpdateLinkedNodeId
 * @tc.type: FUNC
 * @tc.require: #IBPVN9
 */
HWTEST_F(RSWindowKeyframeNodeInfoTest, UpdateLinkedNodeId, TestSize.Level2)
{
    auto info = std::make_shared<RSWindowKeyframeNodeInfo>();
    ASSERT_NE(info, nullptr);

    NodeId rootNodeId = INVALID_NODEID;
    NodeId canvasNodeId = INVALID_NODEID;
    info->UpdateLinkedNodeId(canvasNodeId, rootNodeId);
    EXPECT_EQ(info->GetLinkedNodeCount(), 0);
 
    rootNodeId = static_cast<NodeId>(1);
    canvasNodeId = static_cast<NodeId>(2);
    info->UpdateLinkedNodeId(canvasNodeId, rootNodeId);
    EXPECT_EQ(info->GetLinkedNodeCount(), 1);
    info->ClearLinkedNodeInfo();
    EXPECT_EQ(info->GetLinkedNodeCount(), 0);
}

/**
 * @tc.name: ClearLinkedNodeInfo
 * @tc.desc: Test ClearLinkedNodeInfo
 * @tc.type: FUNC
 * @tc.require: #IC1LJK
 */
HWTEST_F(RSWindowKeyframeNodeInfoTest, ClearLinkedNodeInfo, TestSize.Level2)
{
    RSWindowKeyframeNodeInfo info;
    EXPECT_EQ(info.GetLinkedNodeCount(), 0);
    info.linkedNodeMap_[1] = 1;
    EXPECT_EQ(info.GetLinkedNodeCount(), 1);
    info.ClearLinkedNodeInfo();
    EXPECT_EQ(info.GetLinkedNodeCount(), 0);
}

/**
 * @tc.name: GetLinkedNodeCount
 * @tc.desc: Test GetLinkedNodeCount
 * @tc.type: FUNC
 * @tc.require: #IC1LJK
 */
HWTEST_F(RSWindowKeyframeNodeInfoTest, GetLinkedNodeCount, TestSize.Level2)
{
    RSWindowKeyframeNodeInfo info;
    EXPECT_EQ(info.GetLinkedNodeCount(), 0);
    info.linkedNodeMap_[1] = 1;
    EXPECT_EQ(info.GetLinkedNodeCount(), 1);
}

/*
 * @tc.name: PrepareRootNodeOffscreen
 * @tc.desc: Test PrepareRootNodeOffscreen
 * @tc.type: FUNC
 * @tc.require: #IBPVN9
 */
HWTEST_F(RSWindowKeyframeNodeInfoTest, PrepareRootNodeOffscreen, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto appNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(appNode, nullptr);
    appNode->InitRenderParams();

    auto info = std::make_shared<RSWindowKeyframeNodeInfo>();
    ASSERT_NE(info, nullptr);
    EXPECT_FALSE(info->PrepareRootNodeOffscreen(*appNode));

    info->linkedNodeMap_[1] = 1;
    EXPECT_TRUE(info->PrepareRootNodeOffscreen(*appNode));

    NodeId rootNodeId = static_cast<NodeId>(1);
    auto rootNode = std::make_shared<RSRootRenderNode>(rootNodeId, rsContext->weak_from_this());
    ASSERT_NE(rootNode, nullptr);

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(rootNode);
    NodeId canvasNodeId = static_cast<NodeId>(2);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(canvasNodeId, rsContext->weak_from_this());
    ASSERT_NE(canvasNode, nullptr);

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(canvasNode);
    info->UpdateLinkedNodeId(canvasNodeId, rootNodeId);
    EXPECT_TRUE(info->PrepareRootNodeOffscreen(*appNode));

    rootNode->parent_ = appNode;
    rootNode->renderDrawable_ = nullptr;
    rootNode->stagingRenderParams_ = nullptr;
    canvasNode->stagingRenderParams_ = nullptr;
    EXPECT_TRUE(info->PrepareRootNodeOffscreen(*appNode));

    rootNode->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rootNode);
    rootNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rootNodeId);
    canvasNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(canvasNodeId);
    EXPECT_TRUE(info->PrepareRootNodeOffscreen(*appNode));
}

} // namespace OHOS::Rosen
