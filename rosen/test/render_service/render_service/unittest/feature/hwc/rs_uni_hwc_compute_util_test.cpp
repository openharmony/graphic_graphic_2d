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
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "surface_buffer_impl.h"
#include "surface_type.h"
 
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "feature/hwc/rs_uni_hwc_compute_util.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/mock/mock_meta_data_helper.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/hwc/rs_uni_hwc_visitor.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pixel_map.h"
#include "property/rs_properties_def.h"
#include "render/rs_material_filter.h"
#include "render/rs_shadow.h"
 
using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;
 
namespace OHOS::Rosen {
class RSUniHwcComputeUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniHwcComputeUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniHwcComputeUtilTest::TearDownTestCase() {}
void RSUniHwcComputeUtilTest::SetUp() {}
void RSUniHwcComputeUtilTest::TearDown() {}

/*
 * @tc.name: CalcSrcRectBufferFlip
 * @tc.desc: Verify function CalcSrcRectBufferFlip
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, CalcSrcRectBufferFlipTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.surfaceHandler_ = std::make_shared<RSSurfaceHandler>(id);
    ASSERT_NE(node.surfaceHandler_, nullptr);
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    constexpr uint32_t DEFAULT_FRAME_WIDTH = 800;
    constexpr uint32_t DEFAULT_FRAME_HEIGHT = 600;
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(DEFAULT_FRAME_WIDTH);
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(DEFAULT_FRAME_HEIGHT);
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_FLIP_H);
    ScreenInfo screenInfo;
    screenInfo.width = 1440;
    screenInfo.height = 1080;
    int left = 10;
    int top = 20;
    int width = 30;
    int height = 40;
    RectI dstRect(left, top, width, height);
    RectI srcRect(left, top, width, height);
    node.SetDstRect(dstRect);
    node.SetSrcRect(srcRect);
    RSUniHwcComputeUtil::CalcSrcRectByBufferFlip(node, screenInfo);
    RectI newSrcRect = node.GetSrcRect();
    RectI desiredSrcRect(DEFAULT_FRAME_WIDTH - left - width, top, width, height);
    EXPECT_EQ(newSrcRect, desiredSrcRect);
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    node.SetSrcRect({left, top, width, height});
    RSUniHwcComputeUtil::CalcSrcRectByBufferFlip(node, screenInfo);
    newSrcRect = node.GetSrcRect();
    EXPECT_EQ(newSrcRect, desiredSrcRect);
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_FLIP_V);
    node.SetSrcRect({left, top, width, height});
    RSUniHwcComputeUtil::CalcSrcRectByBufferFlip(node, screenInfo);
    newSrcRect = node.GetSrcRect();
    desiredSrcRect = {left, DEFAULT_FRAME_HEIGHT - top - height, width, height};
    EXPECT_EQ(newSrcRect, desiredSrcRect);
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    node.SetSrcRect({left, top, width, height});
    RSUniHwcComputeUtil::CalcSrcRectByBufferFlip(node, screenInfo);
    newSrcRect = node.GetSrcRect();
    EXPECT_EQ(newSrcRect, desiredSrcRect);
}

/*
 * @tc.name: CalcSrcRectByBufferRotationTest_001
 * @tc.desc: Verify function CalcSrcRectByBufferRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, CalcSrcRectByBufferRotationTest_001, Function | SmallTest | Level2)
{
    constexpr uint32_t default_frame_width = 800;
    constexpr uint32_t default_frame_height = 600;
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    surfaceBuffer->SetSurfaceBufferWidth(default_frame_width);
    surfaceBuffer->SetSurfaceBufferHeight(default_frame_height);
    GraphicTransformType consumerTransformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    int left = 1;
    int top = 1;
    int width = 1;
    int height = 1;
    Drawing::Rect srcRect(left, top, left + width, top + height);
    Drawing::Rect newSrcRect;
    newSrcRect = RSUniHwcComputeUtil::CalcSrcRectByBufferRotation(*surfaceBuffer, consumerTransformType, srcRect);
    ASSERT_EQ(newSrcRect, srcRect);
}

/*
 * @tc.name: CalcSrcRectByBufferRotationTest_002
 * @tc.desc: Verify function CalcSrcRectByBufferRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, CalcSrcRectByBufferRotationTest_002, Function | SmallTest | Level2)
{
    constexpr uint32_t default_frame_width = 800;
    constexpr uint32_t default_frame_height = 600;
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    surfaceBuffer->SetSurfaceBufferWidth(default_frame_width);
    surfaceBuffer->SetSurfaceBufferHeight(default_frame_height);
    GraphicTransformType consumerTransformType = GraphicTransformType::GRAPHIC_ROTATE_90;
    int left = 1;
    int top = 1;
    int width = 1;
    int height = 1;
    Drawing::Rect srcRect(left, top, left + width, top + height);
    Drawing::Rect newSrcRect = RSUniHwcComputeUtil::CalcSrcRectByBufferRotation(
        *surfaceBuffer, consumerTransformType, srcRect);
    Drawing::Rect expectedSrcRect(798.0f, 1.0f, 799.0f, 2.0f);
    ASSERT_EQ(newSrcRect, expectedSrcRect);
}

/*
 * @tc.name: CalcSrcRectByBufferRotationTest_003
 * @tc.desc: Verify function CalcSrcRectByBufferRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, CalcSrcRectByBufferRotationTest_003, Function | SmallTest | Level2)
{
    constexpr uint32_t default_frame_width = 800;
    constexpr uint32_t default_frame_height = 600;
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    surfaceBuffer->SetSurfaceBufferWidth(default_frame_width);
    surfaceBuffer->SetSurfaceBufferHeight(default_frame_height);
    GraphicTransformType consumerTransformType = GraphicTransformType::GRAPHIC_ROTATE_180;
    int left = 1;
    int top = 1;
    int width = 1;
    int height = 1;
    Drawing::Rect srcRect(left, top, left + width, top + height);
    Drawing::Rect newSrcRect = RSUniHwcComputeUtil::CalcSrcRectByBufferRotation(
        *surfaceBuffer, consumerTransformType, srcRect);
    Drawing::Rect expectedSrcRect(798.0f, 598.0f, 799.0f, 599.0f);
    ASSERT_EQ(newSrcRect, expectedSrcRect);
}

/*
 * @tc.name: CalcSrcRectByBufferRotationTest_004
 * @tc.desc: Verify function CalcSrcRectByBufferRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, CalcSrcRectByBufferRotationTest_004, Function | SmallTest | Level2)
{
    constexpr uint32_t default_frame_width = 800;
    constexpr uint32_t default_frame_height = 600;
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    surfaceBuffer->SetSurfaceBufferWidth(default_frame_width);
    surfaceBuffer->SetSurfaceBufferHeight(default_frame_height);
    GraphicTransformType consumerTransformType = GraphicTransformType::GRAPHIC_ROTATE_270;
    int left = 1;
    int top = 1;
    int width = 1;
    int height = 1;
    Drawing::Rect srcRect(left, top, left + width, top + height);
    Drawing::Rect newSrcRect = RSUniHwcComputeUtil::CalcSrcRectByBufferRotation(
        *surfaceBuffer, consumerTransformType, srcRect);
    Drawing::Rect expectedSrcRect(1.0f, 598.0f, 2.0f, 599.0f);
    ASSERT_EQ(newSrcRect, expectedSrcRect);
}

/*
 * @tc.name: CheckForceHardwareAndUpdateDstRectTest
 * @tc.desc: Verify function CheckForceHardwareAndUpdateDstRectTest
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, CheckForceHardwareAndUpdateDstRectTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    RSUniHwcComputeUtil::CheckForceHardwareAndUpdateDstRect(node);
    node.isFixRotationByUser_ = true;
    RSUniHwcComputeUtil::CheckForceHardwareAndUpdateDstRect(node);
    EXPECT_TRUE(node.GetOriginalDstRect().IsEmpty());
}

/*
 * @tc.name: CheckForceHardwareAndUpdateDstRectTest002
 * @tc.desc: Test CheckForceHardwareAndUpdateDstRect without nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, CheckForceHardwareAndUpdateDstRectTest002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.isFixRotationByUser_ = true;
    node.isInFixedRotation_ = true;
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    RSUniHwcComputeUtil::CheckForceHardwareAndUpdateDstRect(node);
    EXPECT_TRUE(node.GetOriginalDstRect().IsEmpty());
}

/*
 * @tc.name: DealWithNodeGravityOldVersionTest
 * @tc.desc: Verify function DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityOldVersionTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::RESIZE;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::DEFAULT;
    node.renderContent_->renderProperties_.boundsGeo_->SetHeight(-1.0f);
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    node.renderContent_->renderProperties_.boundsGeo_->SetWidth(-1.0f);
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    screenInfo.rotation = ScreenRotation::ROTATION_270;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: DealWithNodeGravityOldVersionTest002
 * @tc.desc: Test DealWithNodeGravityOldVersion when buffer is nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityOldVersionTest002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    node.GetRSSurfaceHandler()->buffer_.buffer = nullptr;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: DealWithNodeGravityOldVersionTest003
 * @tc.desc: Test DealWithNodeGravityOldVersion when screenInfo.rotation is modify
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityOldVersionTest003, Function | SmallTest | Level2)
{
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    ScreenInfo screenInfo;
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    screenInfo.rotation = ScreenRotation::ROTATION_270;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    screenInfo.rotation = ScreenRotation::ROTATION_180;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: DealWithNodeGravityOldVersionTest004
 * @tc.desc: Test DealWithNodeGravityOldVersion
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityOldVersionTest004, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node.renderContent_->renderProperties_.SetBoundsHeight(1653);
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::CENTER;
    node.SetDstRect({0, 1106, 1080, 1135});
    node.SetSrcRect({0, 0, 1080, 1135});
    node.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1, 0, 0, 0, 1, 1106, 0, 0, 1);
    node.totalMatrix_ = totalMatrix;
    ScreenInfo screenInfo;
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, screenInfo);
    RectI expectedDstRect = {0, 1106, 1080, 1135};
    RectI expectedSrcRect = {0, 0, 1080, 1135};
    EXPECT_TRUE(node.GetDstRect() == expectedDstRect);
    EXPECT_TRUE(node.GetSrcRect() == expectedSrcRect);
}

/*
 * @tc.name: DealWithNodeGravityTest
 * @tc.desc: Verify function DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest, Function | SmallTest | Level2)
{
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::RESIZE;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::DEFAULT;
    node.renderContent_->renderProperties_.boundsGeo_->SetHeight(-1.0f);
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    node.renderContent_->renderProperties_.boundsGeo_->SetWidth(-1.0f);
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    screenInfo.rotation = ScreenRotation::ROTATION_270;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: DealWithNodeGravityTest002
 * @tc.desc: Test DealWithNodeGravity when buffer is nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest002, Function | SmallTest | Level2)
{
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    node.GetRSSurfaceHandler()->buffer_.buffer = nullptr;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: DealWithNodeGravityTest003
 * @tc.desc: Test DealWithNodeGravity when screenInfo.rotation is modify
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest003, Function | SmallTest | Level2)
{
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    ScreenInfo screenInfo;
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    screenInfo.rotation = ScreenRotation::ROTATION_270;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    screenInfo.rotation = ScreenRotation::ROTATION_180;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, matrix);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: DealWithNodeGravityTest004
 * @tc.desc: Test DealWithNodeGravity when a component totalMatrix/bound/frame changed
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest004, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node1(id);
    node1.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node1.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node1.renderContent_->renderProperties_.SetBoundsHeight(1653);
    node1.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node1.SetDstRect({0, 1106, 1080, 1135});
    node1.SetSrcRect({0, 0, 1080, 1135});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1, 0, 0, 0, 1, 1106, 0, 0, 1);
    ScreenInfo screenInfo;
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniHwcComputeUtil::DealWithNodeGravity(node1, totalMatrix);
    RectI expectedDstRect = {0, 1106, 1080, 1135};
    RectI expectedSrcRect = {0, 0, 1080, 1135};
    EXPECT_TRUE(node1.GetDstRect() == expectedDstRect);
    EXPECT_TRUE(node1.GetSrcRect() == expectedSrcRect);

    id = 2;
    RSSurfaceRenderNode node2(id);
    node2.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1647);
    node2.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node2.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node2.renderContent_->renderProperties_.SetBoundsHeight(1647);
    node2.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node2.SetDstRect({873, 75, 358, 699});
    node2.SetSrcRect({0, 0, 1080, 554});
    node2.isFixRotationByUser_ = false;
    totalMatrix.SetMatrix(0.0f, 0.646842, 873.114075, -0.646842, 0.0f, 774.0f, 0.0f, 0.0f, 1.0f);
    screenInfo.rotation = ScreenRotation::ROTATION_180;
    RSUniHwcComputeUtil::DealWithNodeGravity(node2, totalMatrix);
    expectedDstRect = {873, 75, 358, 699};
    expectedSrcRect = {0, 0, 1080, 554};
    EXPECT_TRUE(node2.GetDstRect() == expectedDstRect);
    EXPECT_TRUE(node2.GetSrcRect() == expectedSrcRect);
}

/*
 * @tc.name: DealWithNodeGravityTest005
 * @tc.desc: Test DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest005, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    Drawing::Matrix totalMatrix;
    node.SetDstRect({0, 0, 100, 100});
    node.surfaceHandler_ = nullptr;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, totalMatrix);
    RectI expectedDstRect = {0, 0, 100, 100};
    EXPECT_TRUE(node.GetDstRect() == expectedDstRect);
}

/*
 * @tc.name: DealWithNodeGravityTest006
 * @tc.desc: Test DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest006, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    Drawing::Matrix totalMatrix;
    node.SetDstRect({0, 0, 100, 100});
    node.GetRSSurfaceHandler()->buffer_.buffer = nullptr;
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    RSUniHwcComputeUtil::DealWithNodeGravity(node, totalMatrix);
    RectI expectedDstRect = {0, 0, 100, 100};
    EXPECT_TRUE(node.GetDstRect() == expectedDstRect);
}

/*
 * @tc.name: DealWithNodeGravityTest007
 * @tc.desc: Test DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest007, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    Drawing::Matrix totalMatrix;
    node.SetDstRect({0, 0, 100, 100});
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = nullptr;
    RSUniHwcComputeUtil::DealWithNodeGravity(node, totalMatrix);
    RectI expectedDstRect = {0, 0, 100, 100};
    EXPECT_TRUE(node.GetDstRect() == expectedDstRect);
}

/*
 * @tc.name: DealWithNodeGravityTest008
 * @tc.desc: Test DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest008, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    Drawing::Matrix totalMatrix;
    node.SetDstRect({0, 0, 100, 100});
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1647);
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node.renderContent_->renderProperties_.SetBoundsHeight(1647);
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::RESIZE;
    node.SetDstRect({0, 0, 100, 100});
    RSUniHwcComputeUtil::DealWithNodeGravity(node, totalMatrix);
    RectI expectedDstRect = {0, 0, 100, 100};
    EXPECT_TRUE(node.GetDstRect() == expectedDstRect);
}

/*
 * @tc.name: DealWithNodeGravityTest009
 * @tc.desc: Test DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest009, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node1(id);
    node1.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node1.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node1.renderContent_->renderProperties_.SetBoundsHeight(1653);
    node1.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node1.SetDstRect({0, 1106, 1080, 1135});
    node1.SetSrcRect({0, 0, 1080, 1135});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1, 0, 0, 0, 1, 1106, 0, 0, 1);
    ScreenInfo screenInfo;
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniHwcComputeUtil::DealWithNodeGravity(node1, totalMatrix);
    RectI expectedDstRect = {0, 1106, 1080, 1080};
    RectI expectedSrcRect = {0, 0, 1080, 1080};
    EXPECT_TRUE(node1.GetDstRect() == expectedDstRect);
    EXPECT_TRUE(node1.GetSrcRect() == expectedSrcRect);

    RSSurfaceRenderNode node2(++id);
    node2.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_270);
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node2.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node2.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node2.renderContent_->renderProperties_.SetBoundsHeight(1653);
    node2.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node2.SetDstRect({0, 1106, 1080, 1135});
    node2.SetSrcRect({0, 0, 1080, 1135});
    node2.isFixRotationByUser_ = false;
    totalMatrix.SetMatrix(1, 0, 0, 0, 1, 1106, 0, 0, 1);
    screenInfo.rotation = ScreenRotation::ROTATION_180;
    RSUniHwcComputeUtil::DealWithNodeGravity(node2, totalMatrix);
    expectedDstRect = {0, 1106, 1080, 1080};
    expectedSrcRect = {0, 573, 1080, 1080};
    EXPECT_TRUE(node2.GetDstRect() == expectedDstRect);
    EXPECT_TRUE(node2.GetSrcRect() == expectedSrcRect);
}

/*
 * @tc.name: DealWithScalingMode
 * @tc.desc: Verify function DealWithScalingMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithScalingModeTest, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node1(id);
    node1.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferScalingMode(ScalingMode::SCALING_MODE_SCALE_TO_WINDOW);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node1.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node1.renderContent_->renderProperties_.SetBoundsHeight(1653);
    node1.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node1.SetDstRect({0, 1106, 1080, 1135});
    node1.SetSrcRect({0, 0, 1080, 1135});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1, 0, 0, 0, 1, 1106, 0, 0, 1);
    RSUniHwcComputeUtil::DealWithScalingMode(node1, totalMatrix);
    RectI expectedDstRect = {0, 1106, 1080, 1135};
    RectI expectedSrcRect = {0, 0, 1080, 1135};
    EXPECT_TRUE(node1.GetDstRect() == expectedDstRect);
    EXPECT_TRUE(node1.GetSrcRect() == expectedSrcRect);
}

/*
 * @tc.name: IsHwcEnabledByGravity
 * @tc.desc: Verify function IsHwcEnabledByGravity
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, IsHwcEnabledByGravityTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    EXPECT_TRUE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::RESIZE));
    EXPECT_TRUE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::TOP_LEFT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::CENTER));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::TOP));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::BOTTOM));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::LEFT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::RIGHT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::TOP_RIGHT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::BOTTOM_LEFT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::BOTTOM_RIGHT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::RESIZE_ASPECT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::RESIZE_ASPECT_TOP_LEFT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::RESIZE_ASPECT_BOTTOM_RIGHT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::RESIZE_ASPECT_FILL));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::RESIZE_ASPECT_FILL_TOP_LEFT));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByGravity(node, Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT));
}

/*
 * @tc.name: IsHwcEnabledByScalingMode
 * @tc.desc: Verify function IsHwcEnabledByScalingMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, IsHwcEnabledByScalingModeTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(node, ScalingMode::SCALING_MODE_FREEZE));
    EXPECT_TRUE(RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(node, ScalingMode::SCALING_MODE_SCALE_TO_WINDOW));
    EXPECT_TRUE(RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(node, ScalingMode::SCALING_MODE_SCALE_CROP));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(node, ScalingMode::SCALING_MODE_NO_SCALE_CROP));
    EXPECT_TRUE(RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(node, ScalingMode::SCALING_MODE_SCALE_FIT));
}

/*
 * @tc.name: LayerCropTest
 * @tc.desc: Verify function LayerCrop
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, LayerCropTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    RSUniHwcComputeUtil::LayerCrop(node, screenInfo);

    auto dstRect = node.GetDstRect();
    dstRect.left_ = 1;
    RSUniHwcComputeUtil::LayerCrop(node, screenInfo);
    EXPECT_EQ(screenInfo.width, 0);
}

/*
 * @tc.name: LayerCrop002
 * @tc.desc: LayerCrop test when resDstRect != dstRectI
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, LayerCrop002, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;

    auto dstRect = node.GetDstRect();
    dstRect.left_ = -1;
    RSUniHwcComputeUtil::LayerCrop(node, screenInfo);
    EXPECT_EQ(screenInfo.width, 0);
}

/*
 * @tc.name: LayerCrop003
 * @tc.desc: LayerCrop test when isInFixedRotation_ is true
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, LayerCrop003, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.isInFixedRotation_ = true;
    ScreenInfo screenInfo;

    auto dstRect = node.GetDstRect();
    dstRect.left_ = -1;
    RSUniHwcComputeUtil::LayerCrop(node, screenInfo);
    EXPECT_EQ(screenInfo.width, 0);
}

/*
 * @tc.name: LayerCrop004
 * @tc.desc: LayerCrop test when isInFixedRotation_ is false
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, LayerCrop004, TestSize.Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    node.isInFixedRotation_ = false;
    ScreenInfo screenInfo;

    auto dstRect = node.GetDstRect();
    dstRect.left_ = -1;
    RSUniHwcComputeUtil::LayerCrop(node, screenInfo);
    EXPECT_EQ(screenInfo.width, 0);
}

/*
 * @tc.name: LayerRotateTest
 * @tc.desc: Verify function LayerRotate
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, LayerRotateTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniHwcComputeUtil::LayerRotate(node, screenInfo);

    screenInfo.rotation = ScreenRotation::ROTATION_180;
    RSUniHwcComputeUtil::LayerRotate(node, screenInfo);

    screenInfo.rotation = ScreenRotation::ROTATION_270;
    RSUniHwcComputeUtil::LayerRotate(node, screenInfo);

    screenInfo.rotation = ScreenRotation::ROTATION_0;
    RSUniHwcComputeUtil::LayerRotate(node, screenInfo);
    EXPECT_EQ(screenInfo.width, 0);
}

/*
 * @tc.name: SrcRectRotateTransformTest
 * @tc.desc: Verify function SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, SrcRectRotateTransformTest, Function | SmallTest | Level2)
{
    constexpr uint32_t DEFAULT_FRAME_WIDTH = 800;
    constexpr uint32_t DEFAULT_FRAME_HEIGHT = 600;
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    surfaceBuffer->SetSurfaceBufferWidth(DEFAULT_FRAME_WIDTH);
    surfaceBuffer->SetSurfaceBufferHeight(DEFAULT_FRAME_HEIGHT);
    int left = 10;
    int top = 20;
    int width = 30;
    int height = 40;
    RectI srcRect(left, top, width, height);
    RectI newSrcRect;
    GraphicTransformType bufferRotateTransformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    newSrcRect = RSUniHwcComputeUtil::SrcRectRotateTransform(*surfaceBuffer, bufferRotateTransformType, srcRect);
    ASSERT_EQ(newSrcRect, srcRect);
    bufferRotateTransformType = GraphicTransformType::GRAPHIC_ROTATE_90;
    newSrcRect = RSUniHwcComputeUtil::SrcRectRotateTransform(*surfaceBuffer, bufferRotateTransformType, srcRect);
    RectI desiredSrcRect(760, 20, 30, 40);
    ASSERT_EQ(newSrcRect, desiredSrcRect);
    bufferRotateTransformType = GraphicTransformType::GRAPHIC_ROTATE_180;
    newSrcRect = RSUniHwcComputeUtil::SrcRectRotateTransform(*surfaceBuffer, bufferRotateTransformType, srcRect);
    desiredSrcRect = {760, 540, 30, 40};
    ASSERT_EQ(newSrcRect, desiredSrcRect);
    bufferRotateTransformType = GraphicTransformType::GRAPHIC_ROTATE_270;
    newSrcRect = RSUniHwcComputeUtil::SrcRectRotateTransform(*surfaceBuffer, bufferRotateTransformType, srcRect);
    desiredSrcRect = {10, 540, 30, 40};
    ASSERT_EQ(newSrcRect, desiredSrcRect);
}

/*
 * @tc.name: UpdateRealSrcRectTest
 * @tc.desc: Verify function UpdateRealSrcRect
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, UpdateRealSrcRectTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    RectI absRect;
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    RSUniHwcComputeUtil::UpdateRealSrcRect(node, absRect);

    absRect = RectI(1, 1, 1, 1);
    RSUniHwcComputeUtil::UpdateRealSrcRect(node, absRect);
    EXPECT_TRUE(node.GetRSSurfaceHandler()->buffer_.buffer);
}

/*
 * @tc.name: UpdateRealSrcRectTest002
 * @tc.desc: Test UpdateRealSrcRect when consumer is nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, UpdateRealSrcRectTest002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(node.GetId());
    RectI absRect;
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = nullptr;
    RSUniHwcComputeUtil::UpdateRealSrcRect(node, absRect);

    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node.GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    RSUniHwcComputeUtil::UpdateRealSrcRect(node, absRect);
    node.GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    RSUniHwcComputeUtil::UpdateRealSrcRect(node, absRect);

    constexpr uint32_t default_canvas_width = 800;
    constexpr uint32_t default_canvas_height = 600;
    int left = 1;
    int top = 1;
    int right = 1;
    int bottom = 1;
    absRect = RectI(left, top, right, bottom);
    node.GetMutableRenderProperties().SetBoundsWidth(default_canvas_width);
    node.GetMutableRenderProperties().SetBoundsHeight(default_canvas_height);
    node.GetMutableRenderProperties().SetFrameGravity(Gravity::TOP_RIGHT);
    RSUniHwcComputeUtil::UpdateRealSrcRect(node, absRect);
    EXPECT_TRUE(node.GetRSSurfaceHandler()->buffer_.buffer);
}

/*
 * @tc.name: GetConsumerTransformTest
 * @tc.desc: Verify function GetConsumerTransform
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, GetConsumerTransformTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    auto consumerTransform = RSUniHwcComputeUtil::GetConsumerTransform(node,
        node.GetRSSurfaceHandler()->buffer_.buffer, node.GetRSSurfaceHandler()->consumer_);
    ASSERT_EQ(consumerTransform, GRAPHIC_ROTATE_NONE);
}

/*
 * @tc.name: GetMatrix_001
 * @tc.desc: test GetMatrix with nullptr Node
 * @tc.type: FUNC
 * @tc.require: issueIAVIB4
 */
HWTEST_F(RSUniHwcComputeUtilTest, GetMatrix_001, TestSize.Level2)
{
    std::shared_ptr<RSRenderNode> node = nullptr;
    ASSERT_EQ(node, nullptr);
    ASSERT_EQ(RSUniHwcComputeUtil::GetMatrix(node), std::nullopt);
}

/*
 * @tc.name: GetMatrix_002
 * @tc.desc: test GetMatrix with boundsGeo_
 * @tc.type: FUNC
 * @tc.require: issueIAVIB4
 */
HWTEST_F(RSUniHwcComputeUtilTest, GetMatrix_002, TestSize.Level2)
{
    NodeId id = 1;
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    ASSERT_EQ(RSUniHwcComputeUtil::GetMatrix(node), node->renderContent_->renderProperties_.boundsGeo_->GetMatrix());
}

/*
 * @tc.name: GetMatrix_003
 * @tc.desc: test GetMatrix sandbox hasValue and parent is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAVIB4
 */
HWTEST_F(RSUniHwcComputeUtilTest, GetMatrix_003, TestSize.Level2)
{
    NodeId id = 1;
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    node->renderContent_->renderProperties_.sandbox_ = std::make_unique<Sandbox>();
    node->renderContent_->renderProperties_.sandbox_->position_ = std::make_optional<Vector2f>(1.0f, 1.0f);
    ASSERT_EQ(RSUniHwcComputeUtil::GetMatrix(node), std::nullopt);
}

/*
 * @tc.name: GetMatrix_004
 * @tc.desc: test GetMatrix sandbox hasvalue and parent has geo
 * @tc.type: FUNC
 * @tc.require: issueIAVIB4
 */
HWTEST_F(RSUniHwcComputeUtilTest, GetMatrix_004, TestSize.Level2)
{
    NodeId parentId = 0;
    std::shared_ptr<RSRenderNode> parentNode = std::make_shared<RSRenderNode>(parentId);
    ASSERT_NE(parentNode, nullptr);
    NodeId id = 1;
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    node->renderContent_->renderProperties_.sandbox_ = std::make_unique<Sandbox>();
    node->renderContent_->renderProperties_.sandbox_->position_ = std::make_optional<Vector2f>(1.0f, 1.0f);
    node->SetParent(parentNode);
    Drawing::Matrix invertAbsParentMatrix;
    parentNode->renderContent_->renderProperties_.boundsGeo_->GetAbsMatrix().Invert(invertAbsParentMatrix);
    auto assertResult = node->renderContent_->renderProperties_.boundsGeo_->GetAbsMatrix();
    assertResult.PostConcat(invertAbsParentMatrix);
    ASSERT_EQ(RSUniHwcComputeUtil::GetMatrix(node), assertResult);
}

/*
 * @tc.name: GetLayerTransformTest
 * @tc.desc: Verify function GetLayerTransform
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniHwcComputeUtilTest, GetLayerTransformTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    GraphicTransformType type = RSUniHwcComputeUtil::GetLayerTransform(node, screenInfo);

    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    type = RSUniHwcComputeUtil::GetLayerTransform(node, screenInfo);
    EXPECT_TRUE(type == GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
 * @tc.name: GetLayerTransformTest002
 * @tc.desc: Test GetLayerTransform when consumer and buffer is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniHwcComputeUtilTest, GetLayerTransformTest002, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.surfaceHandler_ = nullptr;
    ScreenInfo screenInfo;
    GraphicTransformType type = RSUniHwcComputeUtil::GetLayerTransform(node, screenInfo);

    node.surfaceHandler_ = std::make_shared<RSSurfaceHandler>(id);
    ASSERT_NE(node.surfaceHandler_, nullptr);
    node.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    type = RSUniHwcComputeUtil::GetLayerTransform(node, screenInfo);
    EXPECT_TRUE(type == GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
 * @tc.name: UpdateHwcNodeProperty_001
 * @tc.desc: Test UpdateHwcNodeProperty
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, UpdateHwcNodeProperty_001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(id);
    leashWindowNode->InitRenderParams();
    leashWindowNode->renderContent_->renderProperties_.boundsGeo_->SetRect(-10, 10, 20, 30);
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    leashWindowNode->SetDstRect({10, 10, 450, 400});
    Drawing::Matrix leashWindowMatrix;
    leashWindowMatrix.SetMatrix(1.f, 0.f, 50.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
    leashWindowNode->renderContent_->renderProperties_.boundsGeo_->ConcatMatrix(leashWindowMatrix);
    auto rootNode = std::make_shared<RSRootRenderNode>(++id);
    rootNode->InitRenderParams();
    rootNode->renderContent_->renderProperties_.boundsGeo_->SetRect(-10, 10, 20 ,30);
    rootNode->renderContent_->renderProperties_.clipToBounds_ = true;
    rootNode->renderContent_->renderProperties_.clipToFrame_ = true;
    auto canvasNode1 = std::make_shared<RSCanvasRenderNode>(++id);
    canvasNode1->InitRenderParams();
    canvasNode1->renderContent_->renderProperties_.boundsGeo_->SetRect(0, 10, 20, 30);
    canvasNode1->renderContent_->renderProperties_.clipToBounds_ = true;
    auto canvasNode2 = std::make_shared<RSCanvasRenderNode>(++id);
    canvasNode2->InitRenderParams();
    canvasNode2->renderContent_->renderProperties_.boundsGeo_->SetRect(-10, 10, 20, 30);
    canvasNode2->renderContent_->renderProperties_.clipToBounds_ = true;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(++id);
    surfaceNode->InitRenderParams();
    surfaceNode->renderContent_->renderProperties_.frameGravity_ = Gravity::RESIZE;
    Drawing::Matrix surfaceMatrix;
    surfaceMatrix.SetMatrix(1.f, 0.f, 0.f, 0.f, 1.f, 100.f, 0.f, 0.f, 1.f);
    surfaceNode->renderContent_->renderProperties_.boundsGeo_->ConcatMatrix(surfaceMatrix);
    surfaceNode->renderContent_->renderProperties_.boundsGeo_->SetRect(-10, 10, 20, 10);
    surfaceNode->renderContent_->renderProperties_.clipToBounds_ = true;
    surfaceNode->renderContent_->renderProperties_.clipToFrame_ = true;
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer->
        SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer->
        SetSurfaceBufferScalingMode(ScalingMode::SCALING_MODE_SCALE_TO_WINDOW);
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    surfaceNode->GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    surfaceNode->isFixRotationByUser_ = false;
    surfaceNode->SetIsOnTheTree(true);
    surfaceNode->SetCalcRectInPrepare(false);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->isHardwareEnabledNode_ = true;
    leashWindowNode->childHardwareEnabledNodes_.emplace_back(surfaceNode);
    leashWindowNode->AddChild(rootNode);
    rootNode->AddChild(canvasNode1);
    canvasNode1->AddChild(canvasNode2);
    canvasNode2->AddChild(surfaceNode);
    RSUniHwcComputeUtil::UpdateHwcNodeProperty(surfaceNode);
    ASSERT_TRUE(surfaceNode->IsHardwareEnabledType());
}

/*
 * @tc.name: HasNonZRotationTransform_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, HasNonZRotationTransform_001, Function | SmallTest | Level2)
{
    bool hasNonZRotationTransform;
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    hasNonZRotationTransform = RSUniHwcComputeUtil::HasNonZRotationTransform(matrix);
    ASSERT_FALSE(hasNonZRotationTransform);
}

/*
 * @tc.name: HasNonZRotationTransform_002
 * @tc.desc: test HasNonZRotationTransform with ScaleX and ScaleY have same sign
 * @tc.type: FUNC
 * @tc.require: #IANUEG
 */
HWTEST_F(RSUniHwcComputeUtilTest, HasNonZRotationTransform_002, Function | SmallTest | Level2)
{
    bool hasNonZRotationTransform;
    Drawing::Matrix matrix;
    matrix.SetMatrix(-1, 0, 0, 0, -1, 0, 0, 0, 1);
    hasNonZRotationTransform = RSUniHwcComputeUtil::HasNonZRotationTransform(matrix);
    ASSERT_FALSE(hasNonZRotationTransform);
}

/*
 * @tc.name: HasNonZRotationTransform_003
 * @tc.desc: Test HasNonZRotationTransform when ScaleX and ScaleY have different sign
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniHwcComputeUtilTest, HasNonZRotationTransform_003, Function | SmallTest | Level2)
{
    bool hasNonZRotationTransform;
    Drawing::Matrix matrix;
    matrix.SetMatrix(-1, 0, 0, 0, 1, 0, 0, 0, 1);
    hasNonZRotationTransform = RSUniHwcComputeUtil::HasNonZRotationTransform(matrix);
    ASSERT_TRUE(hasNonZRotationTransform);
}

/**
 * @tc.name: IntersectRect_001
 * @tc.desc: test for seting RectF to intersection.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IntersectRect_001, Function | SmallTest | Level1)
{
    Drawing::Rect rectf1(1.0f, 2.0f, 3.0f, 4.0f);
    Drawing::Rect rectf2;
    RSUniHwcComputeUtil::IntersectRect(rectf1, rectf2);
    EXPECT_EQ(0.0f, rectf1.GetLeft());
    EXPECT_EQ(0.0f, rectf1.GetTop());
    EXPECT_EQ(0.0f, rectf1.GetRight());
    EXPECT_EQ(0.0f, rectf1.GetBottom());
}

/**
 * @tc.name: IntersectRect_002
 * @tc.desc: test for seting RectF to intersection.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IntersectRect_002, Function | SmallTest | Level1)
{
    Drawing::Rect rectf1(1.0f, 2.0f, 3.0f, 4.0f);
    Drawing::Rect rectf2(1.0f, 2.0f, 3.0f, 5.0f);
    RSUniHwcComputeUtil::IntersectRect(rectf1, rectf2);
    EXPECT_EQ(1.0f, rectf1.GetLeft());
    EXPECT_EQ(2.0f, rectf1.GetTop());
    EXPECT_EQ(3.0f, rectf1.GetRight());
    EXPECT_EQ(4.0f, rectf1.GetBottom());
}

/**
 * @tc.name: IS_NULLPTR_001
 * @tc.desc: test for IS_NULLPTR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IS_NULLPTR_001, Function | SmallTest | Level1)
{
    int* ptr1 = nullptr;
    auto result = RSUniHwcComputeUtil::IS_NULLPTR(ptr1);
    EXPECT_TRUE(result);
    delete ptr1;
    ptr1 = nullptr;
}

/**
 * @tc.name: IS_NULLPTR_002
 * @tc.desc: test for IS_NULLPTR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IS_NULLPTR_002, Function | SmallTest | Level1)
{
    int* ptr1 = new int;
    auto result = RSUniHwcComputeUtil::IS_NULLPTR(ptr1);
    EXPECT_FALSE(result);
    delete ptr1;
    ptr1 = nullptr;
}

/**
 * @tc.name: IS_NULLPTR_003
 * @tc.desc: test for IS_NULLPTR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IS_NULLPTR_003, Function | SmallTest | Level1)
{
    std::unique_ptr<int> ptr1 = std::make_unique<int>(5);
    auto result = RSUniHwcComputeUtil::IS_NULLPTR(std::move(ptr1));
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IS_ANY_NULLPTR_001
 * @tc.desc: test for IS_ANY_NULLPTR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IS_ANY_NULLPTR_001, Function | SmallTest | Level1)
{
    uint8_t* ptr1 = nullptr;
    char* ptr2 = nullptr;
    float* ptr3 = nullptr;
    auto result = RSUniHwcComputeUtil::IS_ANY_NULLPTR(ptr1, ptr2, ptr3);
    EXPECT_TRUE(result);
    delete ptr1;
    delete ptr2;
    delete ptr3;
    ptr1 = nullptr;
    ptr2 = nullptr;
    ptr3 = nullptr;
}

/**
 * @tc.name: IS_ANY_NULLPTR_002
 * @tc.desc: test for IS_ANY_NULLPTR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IS_ANY_NULLPTR_002, Function | SmallTest | Level1)
{
    uint8_t* ptr1 = new uint8_t;
    char* ptr2 = nullptr;
    float* ptr3 = new float;
    auto result = RSUniHwcComputeUtil::IS_ANY_NULLPTR(ptr1, ptr2, ptr3);
    EXPECT_TRUE(result);
    delete ptr1;
    delete ptr2;
    delete ptr3;
    ptr1 = nullptr;
    ptr2 = nullptr;
    ptr3 = nullptr;
}

/**
 * @tc.name: IS_ANY_NULLPTR_003
 * @tc.desc: test for IS_ANY_NULLPTR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IS_ANY_NULLPTR_003, Function | SmallTest | Level1)
{
    uint8_t* ptr1 = new uint8_t;
    char* ptr2 = new char;
    float* ptr3 = new float;
    auto result = RSUniHwcComputeUtil::IS_ANY_NULLPTR(ptr1, ptr2, ptr3);
    EXPECT_FALSE(result);
    delete ptr1;
    delete ptr2;
    delete ptr3;
    ptr1 = nullptr;
    ptr2 = nullptr;
    ptr3 = nullptr;
}

/**
 * @tc.name: IS_ANY_NULLPTR_004
 * @tc.desc: test for IS_ANY_NULLPTR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcComputeUtilTest, IS_ANY_NULLPTR_004, Function | SmallTest | Level1)
{
    uint8_t* ptr1 = new uint8_t;
    char* ptr2 = new char;
    std::unique_ptr<int> ptr3 = std::make_unique<int>(5);
    auto result = RSUniHwcComputeUtil::IS_ANY_NULLPTR(ptr1, ptr2, std::move(ptr3));
    EXPECT_FALSE(result);
    delete ptr1;
    delete ptr2;
    ptr1 = nullptr;
    ptr2 = nullptr;
}

/*
 * @tc.name: IsBlendNeedFilter_001
 * @tc.desc: Test IsBlendNeedFilter
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, IsBlendNeedFilter_001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSRenderNode node(id);
    node.renderContent_->renderProperties_.needFilter_ = true;
    EXPECT_TRUE(RSUniHwcComputeUtil::IsBlendNeedFilter(node));

    node.renderContent_->renderProperties_.needFilter_ = false;
    node.GetHwcRecorder().SetBlendWithBackground(true);
    EXPECT_TRUE(RSUniHwcComputeUtil::IsBlendNeedFilter(node));
}

/*
 * @tc.name: IsBlendNeedBackground_001
 * @tc.desc: Test IsBlendNeedBackground
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, IsBlendNeedBackground_001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSRenderNode node(id);
    node.renderContent_->renderProperties_.needHwcFilter_ = true;
    EXPECT_TRUE(RSUniHwcComputeUtil::IsBlendNeedBackground(node));

    node.renderContent_->renderProperties_.needHwcFilter_ = false;
    node.GetHwcRecorder().SetBlendWithBackground(true);
    EXPECT_TRUE(RSUniHwcComputeUtil::IsBlendNeedBackground(node));
}

/*
 * @tc.name: IsBlendNeedChildNode_001
 * @tc.desc: Test IsBlendNeedChildNode
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, IsBlendNeedChildNode_001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSRenderNode node(id);
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    node.renderContent_->renderProperties_.SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_TRUE(RSUniHwcComputeUtil::IsBlendNeedChildNode(node));
}

/*
 * @tc.name: IsForegroundColorStrategyValid_001
 * @tc.desc: Test IsForegroundColorStrategyValid
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, IsForegroundColorStrategyValid_001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSRenderNode node(id);
    auto result = RSUniHwcComputeUtil::IsForegroundColorStrategyValid(node);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: IsForegroundColorStrategyValid_002
 * @tc.desc: Test IsForegroundColorStrategyValid
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, IsForegroundColorStrategyValid_002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSRenderNode node(id);
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };
    node.renderContent_->drawCmdModifiers_.emplace(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, list);

    auto result = RSUniHwcComputeUtil::IsForegroundColorStrategyValid(node);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: IsDangerousBlendMode_001
 * @tc.desc: Test IsDangerousBlendMode
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, IsDangerousBlendMode_001, Function | SmallTest | Level2)
{
    EXPECT_FALSE(RSUniHwcComputeUtil::IsDangerousBlendMode(0, 0));
    EXPECT_TRUE(RSUniHwcComputeUtil::IsDangerousBlendMode(1, 0));
    EXPECT_TRUE(RSUniHwcComputeUtil::IsDangerousBlendMode(1, 1));
}

/*
 * @tc.name: GetFloatRotationDegreeFromMatrix_001
 * @tc.desc: Test GetFloatRotationDegreeFromMatrix
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcComputeUtilTest, GetFloatRotationDegreeFromMatrix_001, Function | SmallTest | Level2)
{
    Drawing::Matrix matrix;
    matrix.SetMatrix(1.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);

    float result = RSUniHwcComputeUtil::GetFloatRotationDegreeFromMatrix(matrix);
    EXPECT_NEAR(result, -45.0f, 1e-6);
}

} // namespace OHOS::Rosen