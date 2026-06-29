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

#include <gtest/gtest.h>

#include "composer/composer_service/external_depend/engine/rs_base_render_engine.h"

#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int32_t TEST_NODE_ID = 12345;
constexpr int32_t TEST_PARENT_ID = 67890;
constexpr int32_t TEST_WIDTH = 1920;
constexpr int32_t TEST_HEIGHT = 1080;
} // anonymous namespace

class RSSplitSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<RSSurfaceRenderNode> CreateNode(NodeId nodeId);
    std::shared_ptr<RSSplitSurfaceBuffer> CreateBuffer(const std::string& name, int32_t layerId);

protected:
    std::unique_ptr<SplitSurface> splitSurface_;
};

void RSSplitSurfaceTest::SetUpTestCase() {}
void RSSplitSurfaceTest::TearDownTestCase() {}
void RSSplitSurfaceTest::SetUp()
{
    splitSurface_ = std::make_unique<SplitSurface>(TEST_WIDTH, TEST_HEIGHT);
}
void RSSplitSurfaceTest::TearDown()
{
    splitSurface_ = nullptr;
}

std::shared_ptr<RSSurfaceRenderNode> RSSplitSurfaceTest::CreateNode(NodeId nodeId)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    node->InitRenderParams();
    node->isOnTheTree_ = false;
    return node;
}

std::shared_ptr<RSSplitSurfaceBuffer> RSSplitSurfaceTest::CreateBuffer(const std::string& name, int32_t layerId)
{
    return std::make_shared<RSSplitSurfaceBuffer>(name, layerId, TEST_WIDTH, TEST_HEIGHT);
}

/**
 * @tc.name: Init_NodeNotNull
 * @tc.desc: Test Init when splitSurfaceNode_ != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, Init_NodeNotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto surfaceNode = CreateNode(TEST_NODE_ID);
    ASSERT_NE(surfaceNode, nullptr);
    splitSurface_->splitSurfaceNode_ = surfaceNode;
    splitSurface_->splitSurfaceNode_->InitRenderParams();
    auto parentNode = CreateNode(TEST_PARENT_ID);
    ASSERT_NE(parentNode, nullptr);

    splitSurface_->Init(parentNode);
    ASSERT_NE(splitSurface_->splitSurfaceNode_, nullptr);
}

/**
 * @tc.name: Init_NodeNullContextNotNull
 * @tc.desc: Test Init when splitSurfaceNode_ == nullptr && context != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, Init_NodeNullContextNotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto parentNode = CreateNode(TEST_PARENT_ID);
    ASSERT_NE(parentNode, nullptr);

    splitSurface_->Init(parentNode);
    ASSERT_NE(splitSurface_->splitSurfaceNode_, nullptr);
}

/**
 * @tc.name: GetSurfaceNodeId_Null
 * @tc.desc: Test GetSurfaceNodeId when splitSurfaceNode_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, GetSurfaceNodeId_Null, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    splitSurface_->splitSurfaceNode_ = nullptr;

    NodeId result = splitSurface_->GetSurfaceNodeId();
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: GetSurfaceNodeId_NodeNull
 * @tc.desc: Test GetSurfaceNodeId when splitSurfaceNode_ != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, GetSurfaceNodeId_NodeNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    ASSERT_NE(node, nullptr);
    splitSurface_->splitSurfaceNode_ = node;

    NodeId result = splitSurface_->GetSurfaceNodeId();
    ASSERT_EQ(result, TEST_NODE_ID);
}

/**
 * @tc.name: CheckParentOnTree_NotNull
 * @tc.desc: Test CheckParentNodeOnTheTree when splitSurfaceNode_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, CheckParentOnTree_NotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    splitSurface_->splitSurfaceNode_ = nullptr;

    bool result = splitSurface_->CheckParentNodeOnTheTree();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CheckParentOnTree_ParentNull
 * @tc.desc: Test CheckParentNodeOnTheTree when parent == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, CheckParentOnTree_ParentNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    ASSERT_NE(node, nullptr);
    splitSurface_->splitSurfaceNode_ = node;
    splitSurface_->splitSurfaceNode_->parent_.reset();

    bool result = splitSurface_->CheckParentNodeOnTheTree();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CheckParentOnTree_ParentNotOnTree
 * @tc.desc: Test CheckParentNodeOnTheTree when parent != nullptr && !parent->IsOnTheTree()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, CheckParentOnTree_ParentNotOnTree, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    auto parent = CreateNode(TEST_PARENT_ID);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(parent, nullptr);
    parent->isOnTheTree_ = false;
    splitSurface_->splitSurfaceNode_ = node;
    splitSurface_->splitSurfaceNode_->parent_ = std::weak_ptr<RSRenderNode>(parent);

    bool result = splitSurface_->CheckParentNodeOnTheTree();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CheckParentOnTree_ParentOnTree
 * @tc.desc: Test CheckParentNodeOnTheTree when parent != nullptr && parent->IsOnTheTree()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, CheckParentOnTree_ParentOnTree, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    auto parent = CreateNode(TEST_PARENT_ID);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(parent, nullptr);
    parent->isOnTheTree_ = true;
    splitSurface_->splitSurfaceNode_ = node;
    splitSurface_->splitSurfaceNode_->parent_ = std::weak_ptr<RSRenderNode>(parent);

    bool result = splitSurface_->CheckParentNodeOnTheTree();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: Unregister_NodeNull
 * @tc.desc: Test UnregisterSplitSurfaceNode when splitSurfaceNode_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, Unregister_NodeNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    splitSurface_->splitSurfaceNode_ = nullptr;

    splitSurface_->UnregisterSplitSurfaceNode();
    ASSERT_EQ(splitSurface_->surfaceStatus_, SurfaceStatus::UNREGISTER);
}

/**
 * @tc.name: Unregister_ParentNotNull
 * @tc.desc: Test UnregisterSplitSurfaceNode when parent != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, Unregister_ParentNotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    auto parent = CreateNode(TEST_PARENT_ID);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(parent, nullptr);
    parent->isOnTheTree_ = true;
    splitSurface_->splitSurfaceNode_ = node;
    splitSurface_->splitSurfaceNode_->parent_ = std::weak_ptr<RSRenderNode>(parent);
    parent->AddChild(node);
    auto childCountBefore = parent->children_.size();
    ASSERT_GT(childCountBefore, 0);

    splitSurface_->UnregisterSplitSurfaceNode();
    ASSERT_EQ(splitSurface_->surfaceStatus_, SurfaceStatus::UNREGISTER);
    ASSERT_EQ(parent->children_.size(), childCountBefore - 1);
}

/**
 * @tc.name: Unregister_ParentNull
 * @tc.desc: Test UnregisterSplitSurfaceNode when parent == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, Unregister_ParentNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    ASSERT_NE(node, nullptr);
    splitSurface_->splitSurfaceNode_ = node;
    splitSurface_->splitSurfaceNode_->parent_.reset();
    splitSurface_->UnregisterSplitSurfaceNode();

    ASSERT_EQ(splitSurface_->surfaceStatus_, SurfaceStatus::UNREGISTER);
}

/**
 * @tc.name: IsBufferConsumed_NodeNull
 * @tc.desc: Test IsBufferConsumed when splitSurfaceNode_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferConsumed_NodeNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    splitSurface_->splitSurfaceNode_ = nullptr;

    bool result = splitSurface_->IsBufferConsumed();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsBufferConsumed_NodeNotNullHandlerNull
 * @tc.desc: Test IsBufferConsumed when splitSurfaceNode_  != nullptr && surfaceHandler_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferConsumed_NodeNotNullHandlerNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    ASSERT_NE(node, nullptr);
    splitSurface_->splitSurfaceNode_ = node;
    splitSurface_->splitSurfaceNode_->surfaceHandler_ = nullptr;

    bool result = splitSurface_->IsBufferConsumed();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsBufferConsumed_HandlerNotNull
 * @tc.desc: Test IsBufferConsumed when surfaceHandler_ != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferConsumed_HandlerNotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    ASSERT_NE(node, nullptr);
    splitSurface_->splitSurfaceNode_ = node;
    splitSurface_->splitSurfaceNode_->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);

    bool result = splitSurface_->IsBufferConsumed();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsBufferConsumed_HandlerNotNullBufferNull
 * @tc.desc: Test IsBufferConsumed when surfaceHandler_ != nullptr && buffer == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferConsumed_HandlerNotNullBufferNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    ASSERT_NE(node, nullptr);
    splitSurface_->splitSurfaceNode_ = node;
    auto handler = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);
    splitSurface_->splitSurfaceNode_->surfaceHandler_ = handler;
    splitSurface_->splitSurfaceNode_->surfaceHandler_->buffer_.buffer = nullptr;

    bool result = splitSurface_->IsBufferConsumed();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsBufferConsumed_BothNotNull
 * @tc.desc: Test IsBufferConsumed when surfaceHandler_ != nullptr && buffer != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferConsumed_BothNotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    ASSERT_NE(node, nullptr);
    splitSurface_->splitSurfaceNode_ = node;
    auto handler = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);
    splitSurface_->splitSurfaceNode_->surfaceHandler_ = handler;
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    splitSurface_->splitSurfaceNode_->surfaceHandler_->SetBuffer(buffer, nullptr, Rect(), 0, nullptr);

    bool result = splitSurface_->IsBufferConsumed();
    ASSERT_TRUE(result);   
}

/**
 * @tc.name: IsBufferReleased_BufferNull
 * @tc.desc: Test IsBufferReleased when splitSurfaceBuffer_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferReleased_BufferNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    splitSurface_->splitSurfaceBuffer_ = nullptr;

    bool result = splitSurface_->IsBufferReleased();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsBufferReleased_SurfaceNotCreated
 * @tc.desc: Test IsBufferReleased when splitSurfaceBuffer_ != nullptr && !IsSurfaceCreated()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferReleased_SurfaceNotCreated, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto buffer = CreateBuffer("test", TEST_NODE_ID);
    splitSurface_->splitSurfaceBuffer_ = buffer;
    splitSurface_->splitSurfaceBuffer_->surfaceCreated_ = false;

    bool result = splitSurface_->IsBufferReleased();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsBufferReleased_ConsumerBufferAvailable
 * @tc.desc: Test IsBufferReleased when IsSurfaceCreated() && !consumer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferReleased_ConsumerBufferAvailable, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto buffer = CreateBuffer("test", TEST_NODE_ID);
    splitSurface_->splitSurfaceBuffer_ = buffer;
    splitSurface_->splitSurfaceBuffer_->surfaceCreated_ = true;
    splitSurface_->splitSurfaceBuffer_->surfaceHandler_->SetConsumer(nullptr);

    bool result = splitSurface_->IsBufferReleased();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsBufferReleased_AllConditionsMet
 * @tc.desc: Test IsBufferReleased when all conditions are met
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, IsBufferReleased_AllConditionsMet, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto buffer = CreateBuffer("test", TEST_NODE_ID);
    splitSurface_->splitSurfaceBuffer_ = buffer;
    splitSurface_->splitSurfaceBuffer_->surfaceCreated_ = true;

    bool result = splitSurface_->IsBufferReleased();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: SetBufferNull_HandlerNull
 * @tc.desc: Test SetBufferNull when surfaceHandler == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, SetBufferNull_HandlerNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto buffer = CreateBuffer("test", TEST_NODE_ID);
    ASSERT_NE(buffer, nullptr);
    buffer->surfaceHandler_ = nullptr;
    splitSurface_->splitSurfaceBuffer_ = buffer;

    splitSurface_->SetBufferNull();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetBufferNull_HandlerNotNull
 * @tc.desc: Test SetBufferNull when surfaceHandler != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, SetBufferNull_HandlerNotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto node = CreateNode(TEST_NODE_ID);
    ASSERT_NE(node, nullptr);
    splitSurface_->splitSurfaceNode_ = node;
    auto buffer = CreateBuffer("test", TEST_NODE_ID);
    ASSERT_NE(buffer, nullptr);
    splitSurface_->splitSurfaceBuffer_ = buffer;

    splitSurface_->SetBufferNull();
}

/**
 * @tc.name: DrawDfx_CanvasNull
 * @tc.desc: Test DrawDfx when splitCanvas_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, DrawDfx_CanvasNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    splitSurface_->splitCanvas_ = nullptr;

    splitSurface_->DrawDfx(Drawing::Color::COLOR_RED);
    ASSERT_EQ(splitSurface_->splitCanvas_, nullptr);
}

/**
 * @tc.name: DrawDfx_CanvasNotNull
 * @tc.desc: Test DrawDfx when splitCanvas_ != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, DrawDfx_CanvasNotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    Drawing::Canvas drawingCanvas;
    splitSurface_->splitCanvas_ = std::make_shared<RSPaintFilterCanvas>(&drawingCanvas);

    splitSurface_->DrawDfx(Drawing::Color::COLOR_RED);
    ASSERT_NE(splitSurface_->splitCanvas_, nullptr);
}

/**
 * @tc.name: RequestFrame_ReleasedFrameNull
 * @tc.desc: Test RequestFrame when IsBufferReleased == true && splitRenderFrame_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, RequestFrame_ReleasedFrameNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto buffer = CreateBuffer("test", TEST_NODE_ID);
    splitSurface_->splitSurfaceBuffer_ = buffer;
    splitSurface_->splitSurfaceBuffer_->surfaceCreated_ = true;
    splitSurface_->splitSurfaceBuffer_->GetSurfaceHandler()->SetConsumer(nullptr);

    RSScreenRenderParams params(0);
    bool result = splitSurface_->RequestFrame(params);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: RequestFrame_ReleasedFrameNotNull
 * @tc.desc: Test RequestFrame when IsBufferReleased == true && splitRenderFrame_ != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceTest, RequestFrame_ReleasedFrameNotNull, TestSize.Level1)
{
    ASSERT_NE(splitSurface_, nullptr);
    auto buffer = CreateBuffer("test", TEST_NODE_ID);
    ASSERT_NE(buffer, nullptr);
    splitSurface_->splitSurfaceBuffer_ = buffer;
    buffer->surfaceCreated_ = true;
    buffer->GetSurfaceHandler()->SetConsumer(nullptr);

    auto surface = std::make_shared<RSSurfaceOhosRaster>(nullptr);
    ASSERT_NE(surface, nullptr);
    auto surfaceFrame = std::make_unique<RSSurfaceFrameOhosRaster>(TEST_WIDTH, TEST_HEIGHT);
    ASSERT_NE(surfaceFrame, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(surface, std::move(surfaceFrame));
    ASSERT_NE(renderFrame, nullptr);
    splitSurface_->splitRenderFrame_ = std::move(renderFrame);

    RSScreenRenderParams params(0);
    bool result = splitSurface_->RequestFrame(params);
    ASSERT_FALSE(result);
}
} // namespace OHOS::Rosen