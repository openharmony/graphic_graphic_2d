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
#include "feature/video_3d/rs_tv_shutter_3d_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSTvShutter3DManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
};

void RSTvShutter3DManagerTest::SetUpTestCase() {}
void RSTvShutter3DManagerTest::TearDownTestCase() {}
void RSTvShutter3DManagerTest::SetUp()
{
    RSTvShutter3DManager::Instance().SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_2D);
}
void RSTvShutter3DManagerTest::TearDown() {}

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
}

/**
 * @tc.name: Prepare3DForDraw_001
 * @tc.desc: Test Prepare3DForDraw with null drSurface
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_001, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    std::shared_ptr<Drawing::Surface> drSurface = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Prepare3DForDraw_002
 * @tc.desc: Test Prepare3DForDraw with MODE_2D mode and valid drSurface
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_002, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_2D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_2D);
    std::shared_ptr<Drawing::Surface> drSurface = std::make_shared<Drawing::Surface>();
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Prepare3DForDraw_003
 * @tc.desc: Test Prepare3DForDraw with MODE_SHUTTER_3D mode and null canvas
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_003, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_SHUTTER_3D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    std::shared_ptr<Drawing::Surface> drSurface = std::make_shared<Drawing::Surface>();
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Prepare3DForDraw_004
 * @tc.desc: Test Prepare3DForDraw with MODE_GLASSESFREE_3D mode
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_004, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_GLASSESFREE_3D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    std::shared_ptr<Drawing::Surface> drSurface = std::make_shared<Drawing::Surface>();
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_TRUE(result);
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
 * @tc.name: SetVideoDimType_003
 * @tc.desc: Test SetVideoDimType and GetVideoDimType consistency
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, SetVideoDimType_003, TestSize.Level1)
{
    VideoDimType types[] = {
        VideoDimType::VIDEO_DIM_TYPE_2D,
        VideoDimType::VIDEO_DIM_TYPE_3D_SBS,
        VideoDimType::VIDEO_DIM_TYPE_3D_TAB
    };

    for (auto type : types) {
        RSTvShutter3DManager::Instance().SetVideoDimType(type);
        EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), type);
    }
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_002
 * @tc.desc: Test UpdateSurfaceNodeCompositionType with MODE_SHUTTER_3D
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(surfaceNode, UIMode3D::MODE_SHUTTER_3D);
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_003
 * @tc.desc: Test UpdateSurfaceNodeCompositionType with MODE_2D
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_003, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(surfaceNode, UIMode3D::MODE_2D);
}

/**
 * @tc.name: UpdateSurfaceNodeCompositionType_004
 * @tc.desc: Test UpdateSurfaceNodeCompositionType with MODE_GLASSESFREE_3D
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, UpdateSurfaceNodeCompositionType_004, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    RSTvShutter3DManager::Instance().UpdateSurfaceNodeCompositionType(surfaceNode, UIMode3D::MODE_GLASSESFREE_3D);
}

/**
 * @tc.name: Prepare3DForDraw_005
 * @tc.desc: Test Prepare3DForDraw updates videoDimType correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSTvShutter3DManagerTest, Prepare3DForDraw_005, TestSize.Level1)
{
    RSScreenRenderParams params(0);
    params.SetUIMode3D(UIMode3D::MODE_2D);
    params.SetVideoDimType(VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
    std::shared_ptr<Drawing::Surface> drSurface = std::make_shared<Drawing::Surface>();
    std::shared_ptr<RSPaintFilterCanvas> curCanvas = nullptr;

    bool result = RSTvShutter3DManager::Instance().Prepare3DForDraw(params, drSurface, curCanvas);
    EXPECT_TRUE(result);
    EXPECT_EQ(RSTvShutter3DManager::Instance().GetVideoDimType(), VideoDimType::VIDEO_DIM_TYPE_3D_SBS);
}
}
