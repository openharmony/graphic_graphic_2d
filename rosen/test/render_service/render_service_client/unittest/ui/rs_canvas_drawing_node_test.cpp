/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ui/rs_canvas_drawing_node.h"
#include "command/rs_canvas_drawing_node_command.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "ui/rs_canvas_callback_router.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasDrawingNodeTest::SetUpTestCase() {}
void RSCanvasDrawingNodeTest::TearDownTestCase() {}
void RSCanvasDrawingNodeTest::SetUp() {}
void RSCanvasDrawingNodeTest::TearDown() {}

/**
 * @tc.name: CreateTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, CreateTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    RSCanvasDrawingNode::SharedPtr canvasNode = RSCanvasDrawingNode::Create(isRenderServiceNode);
    ASSERT_NE(canvasNode, nullptr);
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    RSCanvasDrawingNode::preAllocateDmaCcm_ = false;
    canvasNode = RSCanvasDrawingNode::Create(isRenderServiceNode);
    ASSERT_NE(canvasNode, nullptr);
    ASSERT_EQ(RSCanvasDrawingNode::preAllocateDmaCcm_, false);

    RSCanvasDrawingNode::preAllocateDmaCcm_ = true;
    canvasNode = RSCanvasDrawingNode::Create(isRenderServiceNode);
    canvasNode->resetSurfaceIndex_ = 1;
    canvasNode->id_ = ((NodeId)1 << 32) | 1;
    ASSERT_NE(canvasNode, nullptr);
    ASSERT_EQ(RSCanvasDrawingNode::preAllocateDmaCcm_, true);
#endif
}

/**
 * @tc.name: ResetSurfaceTest
 * @tc.desc: test results of ResetSurface
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, ResetSurfaceTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    int width = 1;
    int height = 1;
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    RSCanvasDrawingNode::preAllocateDmaCcm_ = false;
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode);
    bool ret = node->ResetSurface(width, height);
    RSCanvasDrawingNode::preAllocateDmaCcm_ = true;
    EXPECT_EQ(ret, true);
#endif
    RSCanvasDrawingNode::SharedPtr canvasNode = RSCanvasDrawingNode::Create(isRenderServiceNode);
    bool res = canvasNode->ResetSurface(width, height);
    EXPECT_EQ(res, true);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    res = canvasNode->ResetSurface(width, height);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_EQ(res, false);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    canvasNode->isNeverOnTree_ = false;
    res = canvasNode->ResetSurface(width, height);
    EXPECT_EQ(res, true);
#endif
}

/**
 * @tc.name: CreateTextureExportRenderNodeInRTTest
 * @tc.desc: test results of CreateRenderNodeForTextureExportSwitch
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasDrawingNodeTest, CreateTextureExportRenderNodeInRTTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    RSCanvasDrawingNode::SharedPtr canvasNode = RSCanvasDrawingNode::Create(isRenderServiceNode);
    canvasNode->CreateRenderNodeForTextureExportSwitch();
    EXPECT_NE(RSTransactionProxy::GetInstance(), nullptr);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasNode->CreateRenderNodeForTextureExportSwitch();
    EXPECT_EQ(RSTransactionProxy::GetInstance(), nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: GetBitmapTest
 * @tc.desc: test results of GetBitmap
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasDrawingNodeTest, GetBitmapTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    auto drawingNode = std::make_shared<RSCanvasDrawingNode>(isRenderServiceNode);
    Drawing::Bitmap bitmap;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
    Drawing::Rect rect;
    bool res = drawingNode->GetBitmap(bitmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);

    RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::isUniRenderEnabled_ = true;
    drawingNode->GetBitmap(bitmap, drawCmdList, &rect);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);

    drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    res = drawingNode->GetBitmap(bitmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: GetPixelmapTest
 * @tc.desc: test results of GetPixelmap
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasDrawingNodeTest, GetPixelmapTest, TestSize.Level1)
{
    auto drawingNode = std::make_shared<RSCanvasDrawingNode>(true);
    std::shared_ptr<Media::PixelMap> pixelmap;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
    Drawing::Rect rect;
    bool res = drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);

    pixelmap = std::make_shared<Media::PixelMap>();
    drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);

    pixelmap = nullptr;
    res = drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);

    RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::isUniRenderEnabled_ = true;
    drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);

    pixelmap = std::make_shared<Media::PixelMap>();
    res = drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);

    auto drawingNode1 = std::make_shared<RSCanvasDrawingNode>(false, true);
    RSCanvasDrawingNodeCommandHelper::Create(RSRenderThread::Instance().GetContext(), drawingNode1->GetId(), true);
    pixelmap = std::make_shared<Media::PixelMap>();
    res = drawingNode1->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: PreAllocateDMABufferTest
 * @tc.desc: test results of PreAllocateDMABuffer
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, PreAllocateDMABufferTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingNode>(true);
    auto nodeId = node->GetId();
    std::weak_ptr<RSCanvasDrawingNode> weakNode = node;
    node->PreAllocateDMABuffer(weakNode, nodeId, 100, 100, 1);
    ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
    node->resetSurfaceIndex_ = RSCanvasDrawingNode::GenerateResetSurfaceIndex();
    node->PreAllocateDMABuffer(weakNode, nodeId, 0, 0, 1);
    ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
    node->PreAllocateDMABuffer(weakNode, nodeId, 100, 100, 1);
    if (RSSystemProperties::GetCanvasDrawingNodePreAllocateDmaEnabled()) {
        ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
    } else {
        ASSERT_NE(node->canvasSurfaceBuffer_, nullptr);
    }
    node->resetSurfaceIndex_ = 10;
    node->canvasSurfaceBuffer_ = nullptr;
    node->PreAllocateDMABuffer(node, nodeId, 0, 0, 10);
    ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
    node->resetSurfaceIndex_ = 0;
    node->canvasSurfaceBuffer_ = nullptr;
    node->PreAllocateDMABuffer(node, nodeId, 100, 100, 0);
    if (RSSystemProperties::GetCanvasDrawingNodePreAllocateDmaEnabled()) {
        ASSERT_NE(node->canvasSurfaceBuffer_, nullptr);
    } else {
        ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
    }
    node->resetSurfaceIndex_ = 0;
    node->canvasSurfaceBuffer_ = nullptr;
    node->PreAllocateDMABuffer(node, nodeId, 100, 100, 0);
    ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
}

/**
 * @tc.name: CheckNodeAndSurfaceBufferStateTest
 * @tc.desc: test results of CheckNodeAndSurfaceBufferState
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, CheckNodeAndSurfaceBufferStateTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingNode>(true);
    node->resetSurfaceIndex_ = 1;
    auto buffer = SurfaceBuffer::Create();
    node->canvasSurfaceBuffer_ = buffer;
    auto nodeId = node->GetId();
    std::weak_ptr<RSCanvasDrawingNode> weakNode = node;
    auto ret = node->CheckNodeAndSurfaceBufferState(weakNode, nodeId, 1);
    ASSERT_FALSE(ret);
    ret = node->CheckNodeAndSurfaceBufferState(weakNode, nodeId, 2);
    ASSERT_FALSE(ret);
    node->canvasSurfaceBuffer_ = nullptr;
    ret = node->CheckNodeAndSurfaceBufferState(weakNode, nodeId, 1);
    ASSERT_TRUE(ret);
    ret = node->CheckNodeAndSurfaceBufferState(weakNode, nodeId, 2);
    ASSERT_FALSE(ret);
    std::shared_ptr<RSCanvasDrawingNode> nullNode = nullptr;
    ret = node->CheckNodeAndSurfaceBufferState(nullNode, nodeId, 2);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: OnSurfaceBufferChangedTest
 * @tc.desc: test results of OnSurfaceBufferChanged
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, OnSurfaceBufferChangedTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingNode>(true);
    node->resetSurfaceIndex_ = 1;
    auto buffer = SurfaceBuffer::Create();
    node->canvasSurfaceBuffer_ = buffer;
    node->OnSurfaceBufferChanged(nullptr, 1);
    ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
    node->OnSurfaceBufferChanged(buffer, 2);
    ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
}

/**
 * @tc.name: SetIsOnTheTreeTest
 * @tc.desc: test results of SetIsOnTheTree
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, SetIsOnTheTreeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingNode>(true);
    node->isNeverOnTree_ = true;
    node->SetIsOnTheTree(true);
    ASSERT_FALSE(node->isNeverOnTree_);
    node->isNeverOnTree_ = true;
    node->resetSurfaceParams_ = std::make_unique<RSCanvasDrawingNode::ResetSurfaceParams>(100, 100, 1);
    node->SetIsOnTheTree(true);
    ASSERT_FALSE(node->isNeverOnTree_);
    ASSERT_EQ(node->resetSurfaceParams_, nullptr);
    node->isNeverOnTree_ = true;
    node->SetIsOnTheTree(false);
    ASSERT_TRUE(node->isNeverOnTree_);
    node->isNeverOnTree_ = false;
    node->SetIsOnTheTree(true);
    ASSERT_FALSE(node->isNeverOnTree_);
    node->isNeverOnTree_ = false;
    node->SetIsOnTheTree(false);
    ASSERT_FALSE(node->isNeverOnTree_);
}

/**
 * @tc.name: DestructorBranchesTest001
 * @tc.desc: Test destructor with resetSurfaceIndex_ > 0
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, DestructorBranchesTest001, TestSize.Level1)
{
    // Test Case 1: resetSurfaceIndex_ > 0 && SubmitCanvasPreAllocatedBuffer fails
    {
        auto node = std::make_shared<RSCanvasDrawingNode>(true);
        node->resetSurfaceIndex_ = 1;
        node->canvasSurfaceBuffer_ = SurfaceBuffer::Create();
        ASSERT_NE(node, nullptr);
        ASSERT_GT(node->resetSurfaceIndex_, 0);
    }
    // Test Case 2: resetSurfaceIndex_ > 0 && SubmitCanvasPreAllocatedBuffer succeeds
    {
        auto node = std::make_shared<RSCanvasDrawingNode>(true);
        node->resetSurfaceIndex_ = 1;
        node->canvasSurfaceBuffer_ = SurfaceBuffer::Create();
        ASSERT_NE(node, nullptr);
        ASSERT_GT(node->resetSurfaceIndex_, 0);
    }
}

/**
 * @tc.name: DestructorBranchesTest002
 * @tc.desc: Test destructor with resetSurfaceIndex_ == 0
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, DestructorBranchesTest002, TestSize.Level1)
{
    // Test Case 3: resetSurfaceIndex_ == 0 && SubmitCanvasPreAllocatedBuffer fails
    {
        auto node = std::make_shared<RSCanvasDrawingNode>(true);
        node->resetSurfaceIndex_ = 0;
        node->canvasSurfaceBuffer_ = SurfaceBuffer::Create();
        ASSERT_NE(node, nullptr);
        ASSERT_EQ(node->resetSurfaceIndex_, 0);
    }
    // Test Case 4: resetSurfaceIndex_ == 0 && SubmitCanvasPreAllocatedBuffer succeeds
    {
        auto node = std::make_shared<RSCanvasDrawingNode>(true);
        node->resetSurfaceIndex_ = 0;
        node->canvasSurfaceBuffer_ = nullptr;
        ASSERT_NE(node, nullptr);
        ASSERT_EQ(node->resetSurfaceIndex_, 0);
    }
    RSCanvasDrawingNode::preAllocateDmaCcm_ = true;
    {
        auto node = std::make_shared<RSCanvasDrawingNode>(true);
        node->resetSurfaceIndex_ = 1;
        ASSERT_NE(node, nullptr);
    }
}

/**
 * @tc.name: DestructorWithDmaBufferTest
 * @tc.desc: Test destructor behavior with DMA buffer scenarios
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, DestructorWithDmaBufferTest, TestSize.Level1)
{
    // Test with resetSurfaceIndex > 0
    {
        RSCanvasDrawingNode::preAllocateDmaCcm_ = true;
        auto node = std::make_shared<RSCanvasDrawingNode>(true);
        node->resetSurfaceIndex_ = 2;
        node->canvasSurfaceBuffer_ = SurfaceBuffer::Create();
        ASSERT_NE(node->canvasSurfaceBuffer_, nullptr);
    }
    // Test with resetSurfaceIndex == 0
    {
        RSCanvasDrawingNode::preAllocateDmaCcm_ = true;
        auto node = std::make_shared<RSCanvasDrawingNode>(true);
        node->resetSurfaceIndex_ = 0;
        node->canvasSurfaceBuffer_ = SurfaceBuffer::Create();
        ASSERT_NE(node->canvasSurfaceBuffer_, nullptr);
    }
    // Test with null buffer
    {
        RSCanvasDrawingNode::preAllocateDmaCcm_ = true;
        auto node = std::make_shared<RSCanvasDrawingNode>(true);
        node->resetSurfaceIndex_ = 1;
        node->canvasSurfaceBuffer_ = nullptr;
        ASSERT_EQ(node->canvasSurfaceBuffer_, nullptr);
    }
}
#endif
} // namespace OHOS::Rosen