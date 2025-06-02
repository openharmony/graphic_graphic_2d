/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"
#include "gtest/gtest.h"
#include "feature/hdr/rs_hdr_util.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_test_util.h"
#include "v2_1/cm_color_space.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "colorspace_converter_display.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSHdrUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHdrUtilTest::SetUpTestCase() {}
void RSHdrUtilTest::TearDownTestCase() {}
void RSHdrUtilTest::SetUp() {}
void RSHdrUtilTest::TearDown() {}

/**
 * @tc.name: CheckIsHdrSurface
 * @tc.desc: Test CheckIsHdrSurface
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, CheckIsHdrSurfaceTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_TRUE(node->GetRSSurfaceHandler() != nullptr);
    auto buffer = node->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_TRUE(buffer != nullptr);
    ASSERT_TRUE(buffer->GetBufferHandle() != nullptr);
    node->SetIsOnTheTree(false);
    HdrStatus ret = RSHdrUtil::CheckIsHdrSurface(*node);
    ASSERT_EQ(ret, HdrStatus::NO_HDR);
    node->SetIsOnTheTree(true);
    ret = RSHdrUtil::CheckIsHdrSurface(*node);
    ASSERT_EQ(ret, HdrStatus::NO_HDR);
}

/**
 * @tc.name: CheckIsHdrSurfaceBuffer
 * @tc.desc: Test CheckIsHdrSurfaceBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, CheckIsHdrSurfaceBufferTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_TRUE(node->GetRSSurfaceHandler() != nullptr);
    sptr<SurfaceBuffer> buffer = nullptr;
    HdrStatus ret = RSHdrUtil::CheckIsHdrSurfaceBuffer(buffer);
    ASSERT_EQ(ret, HdrStatus::NO_HDR);
    buffer = node->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_TRUE(buffer != nullptr);
    ASSERT_TRUE(buffer->GetBufferHandle() != nullptr);
    buffer->GetBufferHandle()->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP;
    ret = RSHdrUtil::CheckIsHdrSurfaceBuffer(buffer);
    ASSERT_EQ(ret, HdrStatus::NO_HDR);
    buffer->GetBufferHandle()->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_P010;
    ret = RSHdrUtil::CheckIsHdrSurfaceBuffer(buffer);
    ASSERT_EQ(ret, HdrStatus::NO_HDR);
}

/**
 * @tc.name: CheckIsSurfaceWithMetadata
 * @tc.desc: Test CheckIsSurfaceWithMetadata
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, CheckIsSurfaceWithMetadataTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_TRUE(node->GetRSSurfaceHandler() != nullptr);
    auto buffer = node->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_TRUE(buffer != nullptr);
    ASSERT_TRUE(buffer->GetBufferHandle() != nullptr);
    node->SetIsOnTheTree(false);
    bool ret = RSHdrUtil::CheckIsSurfaceWithMetadata(*node);
    ASSERT_EQ(ret, false);
    node->SetIsOnTheTree(true);
    ret = RSHdrUtil::CheckIsSurfaceWithMetadata(*node);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CheckIsSurfaceBufferWithMetadata
 * @tc.desc: Test CheckIsSurfaceBufferWithMetadata
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, CheckIsSurfaceBufferWithMetadataTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_TRUE(node->GetRSSurfaceHandler() != nullptr);
    auto buffer = node->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_TRUE(buffer != nullptr);
    ASSERT_TRUE(buffer->GetBufferHandle() != nullptr);
    bool ret = RSHdrUtil::CheckIsSurfaceBufferWithMetadata(buffer);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: UpdateSurfaceNodeNit
 * @tc.desc: Test UpdateSurfaceNodeNit
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, UpdateSurfaceNodeNitTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    RSHdrUtil::UpdateSurfaceNodeNit(*node, 0);
    node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    node->SetVideoHdrStatus(HdrStatus::HDR_VIDEO);
    RSHdrUtil::UpdateSurfaceNodeNit(*node, 0);
}

/**
 * @tc.name: UpdateSurfaceNodeNit001
 * @tc.desc: Test UpdateSurfaceNodeNit
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, UpdateSurfaceNodeNitTest001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    node->SetVideoHdrStatus(HdrStatus::HDR_VIDEO);
    EXPECT_EQ(node->GetVideoHdrStatus(), HdrStatus::HDR_VIDEO);
    RSHdrUtil::UpdateSurfaceNodeNit(*node, 0);
}

/**
 * @tc.name: UpdateSurfaceNodeNit002
 * @tc.desc: Test UpdateSurfaceNodeNit
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, UpdateSurfaceNodeNitTest002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    node->OnRegister(context);
    node->SetVideoHdrStatus(HdrStatus::HDR_VIDEO);
    EXPECT_EQ(node->GetVideoHdrStatus(), HdrStatus::HDR_VIDEO);
    RSHdrUtil::UpdateSurfaceNodeNit(*node, 0);

    auto& nodeMap = context->GetMutableNodeMap();
    NodeId displayRenderNodeId = 1;
    struct RSDisplayNodeConfig config;
    auto displayRenderNode = std::make_shared<RSDisplayRenderNode>(displayRenderNodeId, config);
    bool res = nodeMap.RegisterRenderNode(displayRenderNode);
    ASSERT_EQ(res, true);
    RSHdrUtil::UpdateSurfaceNodeNit(*node, 0); // displayNode is nullptr
    node->displayNodeId_ = displayRenderNodeId;
    auto displayNode = context->GetNodeMap().GetRenderNode<RSDisplayRenderNode>(node->GetDisplayNodeId());
    ASSERT_NE(displayNode, nullptr);
    RSHdrUtil::UpdateSurfaceNodeNit(*node, 0); // displayNode is nullptr
}

/**
 * @tc.name: UpdateSurfaceNodeLayerLinearMatrix
 * @tc.desc: Test UpdateSurfaceNodeLayerLinearMatrix
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, UpdateSurfaceNodeLayerLinearMatrixTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    RSHdrUtil::UpdateSurfaceNodeLayerLinearMatrix(*node, 0);
    node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    node->SetVideoHdrStatus(HdrStatus::HDR_VIDEO);
    RSHdrUtil::UpdateSurfaceNodeLayerLinearMatrix(*node, 0);
}

/**
 * @tc.name: UpdatePixelFormatAfterHwcCalc
 * @tc.desc: Test UpdatePixelFormatAfterHwcCalc
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, UpdatePixelFormatAfterHwcCalcTest, TestSize.Level1)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(selfDrawingNode, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    selfDrawingNode->SetAncestorDisplayNode(displayNode);
    selfDrawingNode->SetIsOnTheTree(true);
    selfDrawingNode->SetHardwareForcedDisabledState(true);
    displayNode->SetIsOnTheTree(true);

    ASSERT_NE(selfDrawingNode->GetRSSurfaceHandler(), nullptr);
    auto bufferHandle = selfDrawingNode->GetRSSurfaceHandler()->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102;
    RSHdrUtil::UpdatePixelFormatAfterHwcCalc(*displayNode);
}

/**
 * @tc.name: CheckPixelFormatWithSelfDrawingNode
 * @tc.desc: Test CheckPixelFormatWithSelfDrawingNode
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, CheckPixelFormatWithSelfDrawingNodeTest, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(false);
    RSHdrUtil::CheckPixelFormatWithSelfDrawingNode(*surfaceNode, *displayNode);
    surfaceNode->SetIsOnTheTree(true);
    RSHdrUtil::CheckPixelFormatWithSelfDrawingNode(*surfaceNode, *displayNode);
    surfaceNode->SetHardwareForcedDisabledState(false);
    RSHdrUtil::CheckPixelFormatWithSelfDrawingNode(*surfaceNode, *displayNode);
    surfaceNode->SetHardwareForcedDisabledState(true);
    RSHdrUtil::CheckPixelFormatWithSelfDrawingNode(*surfaceNode, *displayNode);
}

/**
 * @tc.name: SetHDRParam
 * @tc.desc: Test SetHDRParam
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSHdrUtilTest, SetHDRParamTest, TestSize.Level1)
{
    NodeId id = 0;
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(parentNode, nullptr);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    id = 1;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(childNode, nullptr);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parentNode->AddChild(childNode);
    parentNode->SetIsOnTheTree(true);
    childNode->SetIsOnTheTree(true);
    RSHdrUtil::SetHDRParam(*childNode, true);
}

} // namespace OHOS::Rosen