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
#include "feature/video_3d/rs_tv_shutter_3d_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_screen_render_params.h"
#include "screen_manager/rs_screen_property.h"
#include "surface_buffer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSTvShutter3DManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSSurfaceRenderNode> CreateSurfaceNode(NodeId id);
    std::shared_ptr<RSScreenRenderNode> CreateScreenNode(NodeId id, ScreenId screenId);
    void SetupScreenProperty(RSScreenRenderNode& screenNode, uint32_t width, uint32_t height,
        ScreenConnectionType connectionType);
    void SetupSurfaceBufferWithDimType(RSSurfaceRenderNode& node, VideoDimType dimType);

    std::shared_ptr<RSContext> context_;
};

void RSTvShutter3DManagerTest::SetUpTestCase() {}
void RSTvShutter3DManagerTest::TearDownTestCase() {}

void RSTvShutter3DManagerTest::SetUp()
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_2D);
    context_ = std::make_shared<RSContext>();
    context_->Initialize();
}

void RSTvShutter3DManagerTest::TearDown()
{
    context_.reset();
}

std::shared_ptr<RSSurfaceRenderNode> RSTvShutter3DManagerTest::CreateSurfaceNode(NodeId id)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context_);
    context_->GetMutableNodeMap().RegisterRenderNode(node);
    return node;
}

std::shared_ptr<RSScreenRenderNode> RSTvShutter3DManagerTest::CreateScreenNode(NodeId id, ScreenId screenId)
{
    auto node = std::make_shared<RSScreenRenderNode>(id, screenId, context_);
    context_->GetMutableNodeMap().RegisterRenderNode(node);
    return node;
}

void RSTvShutter3DManagerTest::SetupScreenProperty(RSScreenRenderNode& screenNode, uint32_t width,
    uint32_t height, ScreenConnectionType connectionType)
{
    auto& screenProperty = const_cast<RSScreenProperty&>(screenNode.GetScreenProperty());
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::make_pair(width, height));
    screenProperty.Set<ScreenPropertyType::CONNECTION_TYPE>(
        static_cast<uint32_t>(connectionType));
}

void RSTvShutter3DManagerTest::SetupSurfaceBufferWithDimType(RSSurfaceRenderNode& node, VideoDimType dimType)
{
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferVideoDimensionType(dimType);
}

/**
 * @tc.name: SetVideoDimType_001
 * @tc.desc: Test SetVideoDimType with valid VideoDimType values
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, SetVideoDimType_001, TestSize.Level1)
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_2D);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_2D);

    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_TAB);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_TAB);
}

/**
 * @tc.name: SetVideoDimType_002
 * @tc.desc: Test SetVideoDimType with invalid VideoDimType values
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, SetVideoDimType_002, TestSize.Level1)
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_2D);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_2D);

    RSTvShutter3DManager::Instance().SetVideoDimType(static_cast<VideoDimType>(100));
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), static_cast<VideoDimType>(100));
}

/**
 * @tc.name: GetVideoDimType_001
 * @tc.desc: Test GetVideoDimType returns default value when not set
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, GetVideoDimType_001, TestSize.Level1)
{
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_2D);
}

/**
 * @tc.name: SetVideoDimType_003
 * @tc.desc: Test SetVideoDimType and GetVideoDimType consistency
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, SetVideoDimType_003, TestSize.Level1)
{
    VideoDimType types[] = {
        VideoDimType::VIDEO_DIM_TYPE_2D,
        VideoDimType::VIDEO_DIM_TYPE_3D_SBS,
        VideoDimType::VIDEO_DIM_TYPE_3D_TAB,
    };

    for (auto type : types) {
        RSTvShutter3DManager::Instance().SetVideoDimType(type);
        EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), type);
    }
}

/**
 * @tc.name: Instance_001
 * @tc.desc: Test Instance returns singleton
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Instance_001, TestSize.Level1)
{
    RSTvShutter3DManager& instance1 = RSTvShutter3DManager::Instance();
    RSTvShutter3DManager& instance2 = RSTvShutter3DManager::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_001
 * @tc.desc: Test UpdateSurfaceNodeCompositionType with null surfaceNode
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nullSurfaceNode = nullptr;
    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(nullSurfaceNode, UIMode3D::MODE_2D);
    EXPECT_EQ(nullSurfaceNode, nullptr);
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_002
 * @tc.desc: Test UpdateSurfaceNodeCompositionType with MODE_SHUTTER_3D and no buffer
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_002, TestSize.Level1)
{
    auto surfaceNode = CreateSurfaceNode(1);
    surfaceNode->isOnTheTree_ = true;
    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(surfaceNode, UIMode3D::MODE_SHUTTER_3D);
    EXPECT_EQ(surfaceNode->GetCompositionType(), CompositionType::COMPOSITION_DEFAULT);
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_003
 * @tc.desc: Test UpdateSurfaceNodeCompositionType with MODE_2D
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_003, TestSize.Level1)
{
    auto surfaceNode = CreateSurfaceNode(2);
    surfaceNode->isOnTheTree_ = true;
    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(surfaceNode, UIMode3D::MODE_2D);
    EXPECT_EQ(surfaceNode->GetCompositionType(), CompositionType::COMPOSITION_DEFAULT);
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_004
 * @tc.desc: Test UpdateSurfaceNodeCompositionType with MODE_GLASSESFREE_3D
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_004, TestSize.Level1)
{
    auto surfaceNode = CreateSurfaceNode(3);
    surfaceNode->isOnTheTree_ = true;
    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(surfaceNode, UIMode3D::MODE_GLASSESFREE_3D);
    EXPECT_EQ(surfaceNode->GetCompositionType(), CompositionType::COMPOSITION_DEFAULT);
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_005
 * @tc.desc: Test UpdateSurfaceNodeCompositionType sets COMPOSITION_3D_SHUTTER
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_005, TestSize.Level1)
{
    auto surfaceNode = CreateSurfaceNode(4);
    surfaceNode->isOnTheTree_ = true;
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    ASSERT_EQ(surfaceNode->GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(surfaceNode, UIMode3D::MODE_SHUTTER_3D);
    EXPECT_EQ(surfaceNode->GetCompositionType(), CompositionType::COMPOSITION_3D_SHUTTER);
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_006
 * @tc.desc: Test UpdateSurfaceNodeCompositionType with 2D buffer reset to DEFAULT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_006, TestSize.Level1)
{
    auto surfaceNode = CreateSurfaceNode(5);
    surfaceNode->isOnTheTree_ = true;
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_2D);

    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(surfaceNode, UIMode3D::MODE_SHUTTER_3D);
    EXPECT_EQ(surfaceNode->GetCompositionType(), CompositionType::COMPOSITION_DEFAULT);
}

/**
 * @tc.name: Prepare3DForDraw_001
 * @tc.desc: Test Prepare3DForDraw with null drSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_001, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_SHUTTER_3D);
    std::shared_ptr<Drawing::Surface> drSurface = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Prepare3DForDraw_002
 * @tc.desc: Test Prepare3DForDraw with MODE_2D resets dimType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_002, TestSize.Level1)
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    ASSERT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_2D);
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_2D);
}

/**
 * @tc.name: Prepare3DForDraw_003
 * @tc.desc: Test Prepare3DForDraw with MODE_SHUTTER_3D resets dimType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_003, TestSize.Level1)
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_TAB);
    ASSERT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_TAB);

    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_GLASSESFREE_3D);
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_2D);
}

/**
 * @tc.name: Prepare3DForDraw_004
 * @tc.desc: Test Prepare3DForDraw with MODE_SHUTTER_3D and null curCanvas
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_004, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_SHUTTER_3D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Prepare3DForDraw_005
 * @tc.desc: Test Prepare3DForDraw with MODE_SHUTTER_3D and valid canvas
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_005, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_SHUTTER_3D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    auto originalCanvas = curCanvas;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_TRUE(RSTvShutter3DManager::Instance().Is3DEnabled(UIMode3D::MODE_SHUTTER_3D));
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetBackupCanvas(), originalCanvas);
    EXPECT_NE(curCanvas, originalCanvas);

    RSTvShutter3DManager::Instance().Release3DContext();
}

/**
 * @tc.name: Prepare3DForDraw_006
 * @tc.desc: Test Prepare3DForDraw propagates VideoDimType from params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_006, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_SHUTTER_3D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_TAB);
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_TAB);

    RSTvShutter3DManager::Instance().Release3DContext();
}

/**
 * @tc.name: Prepare3DForDraw_007
 * @tc.desc: Test Prepare3DForDraw with dimType gets upgrade to SBS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_007, TestSize.Level1)
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_2D);

    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_SHUTTER_3D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_2D);
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    RSTvShutter3DManager::Instance().Release3DContext();
}

/**
 * @tc.name: Prepare3DForDraw_008
 * @tc.desc: Test Prepare3DForDraw with zero-size drSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_008, TestSize.Level2)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_SHUTTER_3D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{0, 0, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Process3DForFlush_001
 * @tc.desc: Test Process3DForFlush with MODE_2D
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DForFlush_001, TestSize.Level1)
{
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Process3DForFlush(UIMode3D::MODE_2D, curCanvas);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Process3DForFlush_002
 * @tc.desc: Test Process3DForFlush with MODE_SHUTTER_3D and disabled 3D context
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DForFlush_002, TestSize.Level1)
{
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Process3DForFlush(UIMode3D::MODE_SHUTTER_3D, curCanvas);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Process3DForFlush_003
 * @tc.desc: Test Process3DForFlush with MODE_GLASSESFREE_3D
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DForFlush_003, TestSize.Level1)
{
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Process3DForFlush(UIMode3D::MODE_GLASSESFREE_3D, curCanvas);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Process3DForFlush_004
 * @tc.desc: Test Process3DForFlush with invalid UIMode3D value
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DForFlush_004, TestSize.Level1)
{
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Process3DForFlush(
        static_cast<UIMode3D>(100), curCanvas);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: IsFullScreen_001
 * @tc.desc: Test IsFullScreen return false when node is not on the tree
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_001, TestSize.Level1)
{
    auto surfaceNode = CreateSurfaceNode(1);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_002
 * @tc.desc: Test IsFullScreen return false when composition type is not COMPOSITION_3D_SHUTTER
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_002, TestSize.Level1)
{
    auto surfaceNode = CreateSurfaceNode(1);
    surfaceNode->isOnTheTree_ = true;
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_003
 * @tc.desc: Test IsFullScreen return false when context weak_ptr is expired
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_003, TestSize.Level2)
{
    auto tempContext = std::make_shared<RSContext>();
    tempContext->Initialize();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, tempContext);
    tempContext->GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    surfaceNode->isOnTheTree_ = true;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    tempContext.reset();
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_004
 * @tc.desc: Test IsFullScreen return false when screenNode is not found
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_004, TestSize.Level2)
{
    const NodeId surfaceNodeId = 1001;
    const NodeId screenNodeId = 2001;

    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);
    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_005
 * @tc.desc: Test IsFullScreen return false when videoDimtype is 2D
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_005, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1002;
    const NodeId screenNodeId = 2002;
    const ScreenId screenId = 3002;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    surfaceNode->dstRect_ = RectI(0, 0, 1920, 1080);

    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_006
 * @tc.desc: Test IsFullScreen with zero-size screen property
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_006, TestSize.Level2)
{
    const NodeId surfaceNodeId = 1003;
    const NodeId screenNodeId = 2003;
    const ScreenId screenId = 3003;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    surfaceNode->dstRect_ = RectI(0, 0, 1920, 1080);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_007
 * @tc.desc: Test IsFullScreen SBS boundary: width < 99% of screen width
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_007, TestSize.Level2)
{
    const NodeId surfaceNodeId = 1004;
    const NodeId screenNodeId = 2004;
    const ScreenId screenId = 3004;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);

    surfaceNode->dstRect_ = RectI(0, 0, 960, 1080);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_008
 * @tc.desc: Test IsFullScreen SBS boundary: width at 99% threshold
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_008, TestSize.Level2)
{
    const NodeId surfaceNodeId = 1005;
    const NodeId screenNodeId = 2005;
    const ScreenId screenId = 3005;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    surfaceNode->dstRect_ = RectI(0, 0, 1901, 1080);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_009
 * @tc.desc: Test IsFullScreen TAB boundary: height < 99% of screen height
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_009, TestSize.Level2)
{
    const NodeId surfaceNodeId = 1006;
    const NodeId screenNodeId = 2006;
    const ScreenId screenId = 3006;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    surfaceNode->dstRect_ = RectI(0, 0, 1920, 540);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_010
 * @tc.desc: Test IsFullScreen return true for SBS full-screen
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_010, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1007;
    const NodeId screenNodeId = 2007;
    const ScreenId screenId = 3007;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    surfaceNode->dstRect_ = RectI(0, 0, 1920, 1080);
    EXPECT_TRUE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_011
 * @tc.desc: Test IsFullScreen return false for SBS when left offset too large
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_011, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1008;
    const NodeId screenNodeId = 2008;
    const ScreenId screenId = 3008;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    surfaceNode->dstRect_ = RectI(100, 0, 1820, 1080);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_012
 * @tc.desc: Test IsFullScreen return false for SBS when width < 99%
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_012, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1009;
    const NodeId screenNodeId = 2009;
    const ScreenId screenId = 3009;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    surfaceNode->dstRect_ = RectI(0, 0, 960, 1080);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_013
 * @tc.desc: Test IsFullScreen return true for TAB full-screen
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_013, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1010;
    const NodeId screenNodeId = 2010;
    const ScreenId screenId = 3010;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_TAB);

    surfaceNode->dstRect_ = RectI(0, 0, 1920, 1080);
    EXPECT_TRUE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_014
 * @tc.desc: Test IsFullScreen returns false for TAB when top offset too large
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_014, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1011;
    const NodeId screenNodeId = 2011;
    const ScreenId screenId = 3011;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_TAB);

    surfaceNode->dstRect_ = RectI(0, 100, 1920, 980);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: IsFullScreen_015
 * @tc.desc: Test IsFullScreen returns false for TAB when height < 99%
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, IsFullScreen_015, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1012;
    const NodeId screenNodeId = 2012;
    const ScreenId screenId = 3012;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    surfaceNode->isOnTheTree_ = true;
    surfaceNode->screenNodeId_ = screenNodeId;
    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_TAB);

    surfaceNode->dstRect_ = RectI(0, 100, 1920, 540);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().IsFullScreen(*surfaceNode));
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_001
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer with non-SHUTTER_3D mode
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_001, TestSize.Level1)
{
    const NodeId screenNodeId = 3001;
    const ScreenId screenId = 4001;
    auto screenNode = CreateScreenNode(screenNodeId, screenId);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(*screenNode, UIMode3D::MODE_2D);
    EXPECT_NE(screenNode->GetUIMode3D(), UIMode3D::MODE_SHUTTER_3D);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_002
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer skips external displays
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_002, TestSize.Level2)
{
    const NodeId screenNodeId = 3002;
    const ScreenId screenId = 4002;
    auto screenNode = CreateScreenNode(screenNodeId, screenId);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);
    EXPECT_EQ(screenNode->GetUIMode3D(), UIMode3D::MODE_SHUTTER_3D);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_003
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer with empty nodes
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_003, TestSize.Level1)
{
    const NodeId screenNodeId = 3003;
    const ScreenId screenId = 4003;
    auto screenNode = CreateScreenNode(screenNodeId, screenId);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);
    EXPECT_EQ(screenNode->GetUIMode3D(), UIMode3D::MODE_SHUTTER_3D);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_004
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer sets UIMode3D on screenNode
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_004, TestSize.Level1)
{
    const NodeId screenNodeId = 3004;
    const ScreenId screenId = 4004;
    auto screenNode = CreateScreenNode(screenNodeId, screenId);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);

    EXPECT_EQ(screenNode->GetUIMode3D(), UIMode3D::MODE_SHUTTER_3D);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_005
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer skips expired weak_ptr
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_005, TestSize.Level2)
{
    const NodeId screenNodeId = 3005;
    const ScreenId screenId = 4005;
    auto screenNode = CreateScreenNode(screenNodeId, screenId);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);

    std::weak_ptr<RSSurfaceRenderNode> expiredPtr;
    {
        auto tempNode = std::make_shared<RSSurfaceRenderNode>(9999);
        expiredPtr = tempNode;
    }
    screenNode->childHwcNodes_.push_back(expiredPtr);
    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);
    EXPECT_EQ(screenNode->GetUIMode3D(), UIMode3D::MODE_SHUTTER_3D);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_006
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer forces disabled for non-3D SHUTTER nodes.
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_006, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1101;
    const NodeId screenNodeId = 3006;
    const ScreenId screenId = 4006;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);
    surfaceNode->isOnTheTree_ = true;

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    screenNode->childHwcNodes_.push_back(surfaceNode);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);

    EXPECT_TRUE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_007
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer skips off-tree nodes
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_007, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1102;
    const NodeId screenNodeId = 3007;
    const ScreenId screenId = 4007;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    screenNode->childHwcNodes_.push_back(surfaceNode);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);

    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_008
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer forces disabled for non-full-screen 3D_SHUTTER node
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_008, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1103;
    const NodeId screenNodeId = 3008;
    const ScreenId screenId = 4008;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);
    surfaceNode->isOnTheTree_ = true;

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);

    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);
    surfaceNode->dstRect_ = RectI(0, 0, 960, 540);
    surfaceNode->screenNodeId_ = screenNodeId;

    screenNode->childHwcNodes_.push_back(surfaceNode);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);

    EXPECT_TRUE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_009
 * @tc.desc: Test MODE_GLASSESFREE_3D does not trigger shutter 3D processing
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_009, TestSize.Level1)
{
    const NodeId screenNodeId = 3009;
    const ScreenId screenId = 4009;
    auto screenNode = CreateScreenNode(screenNodeId, screenId);

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_GLASSESFREE_3D);
    EXPECT_NE(screenNode->GetUIMode3D(), UIMode3D::MODE_SHUTTER_3D);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_010
 * @tc.desc: Test UpdateHwcNodeEnableByShutter3DLayer with mixed composition types
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_010, TestSize.Level2)
{
    const NodeId surfaceNodeId1 = 1110;
    const NodeId surfaceNodeId2 = 1111;
    const NodeId screenNodeId = 3010;
    const ScreenId screenId = 4010;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode1 = CreateSurfaceNode(surfaceNodeId1);
    auto surfaceNode2 = CreateSurfaceNode(surfaceNodeId2);
    surfaceNode1->isOnTheTree_ = true;
    surfaceNode2->isOnTheTree_ = true;

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);

    auto* params1 = static_cast<RSSurfaceRenderParams*>(surfaceNode1->GetStagingRenderParams().get());
    ASSERT_NE(params1, nullptr);
    params1->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);
    surfaceNode1->dstRect_ = RectI(0, 0, 960, 540);
    surfaceNode1->screenNodeId_ = screenNodeId;

    screenNode->childHwcNodes_.push_back(surfaceNode1);
    screenNode->childHwcNodes_.push_back(surfaceNode2);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);

    EXPECT_TRUE(surfaceNode1->isHardwareForcedDisabled_);
    EXPECT_TRUE(surfaceNode2->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_011
 * @tc.desc: Test full-screen SBS 3D_SHUTTER node is not forced disabled
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_011, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1113;
    const NodeId screenNodeId = 3011;
    const ScreenId screenId = 4011;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);
    surfaceNode->isOnTheTree_ = true;

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);
    surfaceNode->dstRect_ = RectI(0, 0, 1920, 1080);
    surfaceNode->screenNodeId_ = screenNodeId;

    screenNode->childHwcNodes_.push_back(surfaceNode);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);

    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
    EXPECT_EQ(screenNode->GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
}

/**
 * @tc.name: UpdateHwcNodeEnableByShutter3DLayer_012
 * @tc.desc: Test full-screen TAB 3D_SHUTTER node is not forced disabled
 * @tc.type: FUNC
 * @tc.require
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateHwcNodeEnableByShutter3DLayer_012, TestSize.Level1)
{
    const NodeId surfaceNodeId = 1114;
    const NodeId screenNodeId = 3012;
    const ScreenId screenId = 4012;

    auto screenNode = CreateScreenNode(screenNodeId, screenId);
    auto surfaceNode = CreateSurfaceNode(surfaceNodeId);
    surfaceNode->isOnTheTree_ = true;

    SetupScreenProperty(*screenNode, 1920, 1080, ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    SetupSurfaceBufferWithDimType(*surfaceNode, VideoDimType::VIDEO_DIM_TYPE_3D_TAB);

    auto* surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetCompositionType(CompositionType::COMPOSITION_3D_SHUTTER);
    surfaceNode->dstRect_ = RectI(0, 0, 1920, 1080);
    surfaceNode->screenNodeId_ = screenNodeId;

    screenNode->childHwcNodes_.push_back(surfaceNode);

    RSTvShutter3DManager::Instance().UpdateHwcNodeEnableByShutter3DLayer(
        *screenNode, UIMode3D::MODE_SHUTTER_3D);

    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
    EXPECT_EQ(screenNode->GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_TAB);
}

/**
 * @tc.name: Process3DImage_001
 * @tc.desc: Test Process3DImage with null targetCanvas
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DImage_001, TestSize.Level1)
{
    std::shared_ptr<RSPaintFilterCanvas> targetCanvas = nullptr;
    std::shared_ptr<Drawing::Image> snapshot = nullptr;
    RSTvShutter3DManager::Instance().Process3DImage(targetCanvas, snapshot, VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    EXPECT_TRUE(targetCanvas == nullptr);
}

/**
 * @tc.name: Process3DImage_002
 * @tc.desc: Test Process3DImage with null snapshot
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DImage_002, TestSize.Level1)
{
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<RSPaintFilterCanvas> targetCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    std::shared_ptr<Drawing::Image> snapshot = nullptr;
    RSTvShutter3DManager::Instance().Process3DImage(targetCanvas, snapshot, VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    EXPECT_NE(targetCanvas, nullptr);
    EXPECT_TRUE(snapshot == nullptr);
}

/**
 * @tc.name: Process3DImage_003
 * @tc.desc: Test Process3DImage with VIDEO_DIM_TYPE_3D_SBS
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DImage_003, TestSize.Level1)
{
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<RSPaintFilterCanvas> targetCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    auto snapshotSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<Drawing::Image> snapshot = snapshotSurface->GetImageSnapshot();
    RSTvShutter3DManager::Instance().Process3DImage(targetCanvas, snapshot, VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    EXPECT_NE(targetCanvas, nullptr);
    EXPECT_NE(snapshot, nullptr);
}

/**
 * @tc.name: Process3DImage_004
 * @tc.desc: Test Process3DImage with VIDEO_DIM_TYPE_3D_TAB
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DImage_004, TestSize.Level1)
{
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<RSPaintFilterCanvas> targetCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    auto snapshotSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<Drawing::Image> snapshot = snapshotSurface->GetImageSnapshot();
    RSTvShutter3DManager::Instance().Process3DImage(targetCanvas, snapshot, VideoDimType::VIDEO_DIM_TYPE_3D_TAB);
    EXPECT_NE(targetCanvas, nullptr);
    EXPECT_NE(snapshot, nullptr);
}

/**
 * @tc.name: Process3DImage_005
 * @tc.desc: Test Process3DImage with VIDEO_DIM_TYPE_2D
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Process3DImage_005, TestSize.Level1)
{
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<RSPaintFilterCanvas> targetCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    auto snapshotSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    std::shared_ptr<Drawing::Image> snapshot = snapshotSurface->GetImageSnapshot();
    RSTvShutter3DManager::Instance().Process3DImage(targetCanvas, snapshot, VideoDimType::VIDEO_DIM_TYPE_2D);
    EXPECT_NE(targetCanvas, nullptr);
    EXPECT_NE(snapshot, nullptr);
}

/**
 * @tc.name: Init3DContext_001
 * @tc.desc: Test Init3DContext with null curCanvas
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Init3DContext_001, TestSize.Level1)
{
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Init3DContext(100, 100, curCanvas);
    EXPECT_FALSE(result);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().Is3DEnabled(UIMode3D::MODE_SHUTTER_3D));
}

/**
 * @tc.name: Init3DContext_002
 * @tc.desc: Test Init3DContext with vaild canvas
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Init3DContext_002, TestSize.Level1)
{
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    auto originalCanvas = curCanvas;

    bool result = RSTvShutter3DManager::Instance().Init3DContext(100, 100, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_TRUE(RSTvShutter3DManager::Instance().Is3DEnabled(UIMode3D::MODE_SHUTTER_3D));
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetBackupCanvas(), originalCanvas);
    EXPECT_NE(curCanvas, originalCanvas);
    EXPECT_NE(RSTvShutter3DManager::Instance().GetOffscreenCanvas(), nullptr);

    RSTvShutter3DManager::Instance().Release3DContext();
}

/**
 * @tc.name: Init3DContext_003
 * @tc.desc: Test Init3DContext upgrades VIDEO_DIM_TYPE_2D to VIDEO_DIM_TYPE_3D_SBS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Init3DContext_003, TestSize.Level1)
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_2D);

    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());

    bool result = RSTvShutter3DManager::Instance().Init3DContext(100, 100, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_SBS);

    RSTvShutter3DManager::Instance().Release3DContext();
}

/**
 * @tc.name: Init3DContext_004
 * @tc.desc: Test Init3DContext preserves VIDEO_DIM_TYPE_3D_TAB
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Init3DContext_004, TestSize.Level1)
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_TAB);

    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());

    bool result = RSTvShutter3DManager::Instance().Init3DContext(100, 100, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_TAB);

    RSTvShutter3DManager::Instance().Release3DContext();
}

/**
 * @tc.name: Init3DContext_005
 * @tc.desc: Test Init3DContext fails with zero width/height
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Init3DContext_005, TestSize.Level2)
{
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());

    bool result = RSTvShutter3DManager::Instance().Init3DContext(0, 0, curCanvas);
    EXPECT_FALSE(result);
    EXPECT_FALSE(RSTvShutter3DManager::Instance().Is3DEnabled(UIMode3D::MODE_SHUTTER_3D));
}

/**
 * @tc.name: Init3DContext_006
 * @tc.desc: Test Init3DContext called twice replace context
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Init3DContext_006, TestSize.Level2)
{
    auto drSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo{100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL});
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());

    bool result1 = RSTvShutter3DManager::Instance().Init3DContext(100, 100, curCanvas);
    EXPECT_TRUE(result1);
    auto firstOffscreen = RSTvShutter3DManager::Instance().GetOffscreenCanvas();
    EXPECT_NE(firstOffscreen, nullptr);

    auto savedFirstOffscreen = curCanvas;
    auto curCanvas2 = curCanvas;
    bool result2 = RSTvShutter3DManager::Instance().Init3DContext(200, 200, curCanvas2);
    EXPECT_TRUE(result2);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetBackupCanvas(), savedFirstOffscreen);
    EXPECT_NE(curCanvas2, savedFirstOffscreen);
    EXPECT_NE(RSTvShutter3DManager::Instance().GetOffscreenCanvas(), firstOffscreen);
    RSTvShutter3DManager::Instance().Release3DContext();
}

/**
 * @tc.name: Release3DContext_001
 * @tc.desc: Test Release3DContext resets context state
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTvShutter3DManagerTest, Release3DContext_001, TestSize.Level1)
{
    RSTvShutter3DManager::Instance().Release3DContext();
    EXPECT_FALSE(RSTvShutter3DManager::Instance().Is3DEnabled(UIMode3D::MODE_SHUTTER_3D));
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetOffscreenCanvas(), nullptr);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetBackupCanvas(), nullptr);
}

} // namespace OHOS::Rosen
