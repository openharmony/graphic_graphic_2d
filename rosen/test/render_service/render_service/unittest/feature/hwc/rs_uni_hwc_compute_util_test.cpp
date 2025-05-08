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
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
 * @tc.name: CalcSrcRectByBufferRotationTest
 * @tc.desc: Verify function CalcSrcRectByBufferRotation
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, CalcSrcRectByBufferRotationTest, Function | SmallTest | Level2)
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
 * @tc.name: CheckForceHardwareAndUpdateDstRectTest
 * @tc.desc: Verify function CheckForceHardwareAndUpdateDstRectTest
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, CheckForceHardwareAndUpdateDstRectTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
 * @tc.name: DealWithNodeGravityTest
 * @tc.desc: Verify function DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithNodeGravityTest, Function | SmallTest | Level2)
{
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
    Drawing::Matrix matrix = Drawing::Matrix();
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
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
    node1.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = OHOS::IConsumerSurface::Create();
    node1.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node1.renderContent_->renderProperties_.SetBoundsHeight(1653);
    node1.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node1.SetDstRect({0, 1106, 1080, 1135});
    node1.SetSrcRect({0, 0, 1080, 1135});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix = Drawing::Matrix();
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
    node2.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node2.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1647);
    node2.GetRSSurfaceHandler()->consumer_ = OHOS::IConsumerSurface::Create();
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
 * @tc.name: DealWithScalingMode
 * @tc.desc: Verify function DealWithScalingMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcComputeUtilTest, DealWithScalingModeTest, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node1(id);
    node1.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferScalingMode(ScalingMode::SCALING_MODE_SCALE_TO_WINDOW);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = OHOS::IConsumerSurface::Create();
    node1.renderContent_->renderProperties_.SetBoundsWidth(1080);
    node1.renderContent_->renderProperties_.SetBoundsHeight(1653);
    node1.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node1.SetDstRect({0, 1106, 1080, 1135});
    node1.SetSrcRect({0, 0, 1080, 1135});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix = Drawing::Matrix();
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
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(node, ScalingMode::SCALING_MODE_SCALE_CROP));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(node, ScalingMode::SCALING_MODE_NO_SCALE_CROP));
    EXPECT_FALSE(RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(node, ScalingMode::SCALING_MODE_SCALE_FIT));
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
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = OHOS::IConsumerSurface::Create();
    auto consumerTransform = RSUniHwcComputeUtil::GetConsumerTransform(node,
        node.GetRSSurfaceHandler()->buffer_.buffer, node.GetRSSurfaceHandler()->consumer_);
    ASSERT_EQ(consumerTransform, GRAPHIC_ROTATE_NONE);
}
}