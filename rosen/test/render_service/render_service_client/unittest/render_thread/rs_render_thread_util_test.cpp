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

#include "render_thread/rs_render_thread_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderThreadUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderThreadUtilTest::SetUpTestCase() {}
void RSRenderThreadUtilTest::TearDownTestCase() {}
void RSRenderThreadUtilTest::SetUp() {}
void RSRenderThreadUtilTest::TearDown() {}

/**
 * @tc.name: SrcRectScaleDownTest
 * @tc.desc: test results of SrcRectScaleDown
 * @tc.type:FUNC
 * @tc.require: issueIB9LAF
 */
HWTEST_F(RSRenderThreadUtilTest, SrcRectScaleDownTest, TestSize.Level1)
{
    TextureExportBufferDrawParam params;
    params.srcRect = Drawing::Rect(0, 0, 100, 100);
    RectF localBounds = RectF(0, 0, 200, 100);
    RSRenderThreadUtil::SrcRectScaleDown(params, localBounds);
    EXPECT_EQ(params.srcRect.GetWidth(), 100);
    EXPECT_LT(params.srcRect.GetHeight(), 100);

    localBounds = RectF(0, 0, 100, 200);
    params.srcRect = Drawing::Rect(0, 0, 100, 100);
    RSRenderThreadUtil::SrcRectScaleDown(params, localBounds);
    EXPECT_LT(params.srcRect.GetWidth(), 100);
    EXPECT_EQ(params.srcRect.GetHeight(), 100);

    localBounds = RectF(0, 0, 100, 100);
    params.srcRect = Drawing::Rect(0, 0, 100, 100);
    RSRenderThreadUtil::SrcRectScaleDown(params, localBounds);
    EXPECT_EQ(params.srcRect.GetWidth(), 100);
    EXPECT_EQ(params.srcRect.GetHeight(), 100);
}

/**
 * @tc.name: GetFlipTransform
 * @tc.desc: test results of GetFlipTransform
 * @tc.type:FUNC
 * @tc.require: issueIB9LAF
 */
HWTEST_F(RSRenderThreadUtilTest, GetFlipTransformTest, TestSize.Level1)
{
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_FLIP_H_ROT90;
    auto transform = RSRenderThreadUtil::GetFlipTransform(transformType);
    EXPECT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_H);
    transformType = GraphicTransformType::GRAPHIC_FLIP_H_ROT180;
    transform = RSRenderThreadUtil::GetFlipTransform(transformType);
    EXPECT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_H);
    transformType = GraphicTransformType::GRAPHIC_FLIP_H_ROT270;
    transform = RSRenderThreadUtil::GetFlipTransform(transformType);
    EXPECT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_H);
    transformType = GraphicTransformType::GRAPHIC_FLIP_V_ROT90;
    transform = RSRenderThreadUtil::GetFlipTransform(transformType);
    EXPECT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_V);
    transformType = GraphicTransformType::GRAPHIC_FLIP_V_ROT180;
    transform = RSRenderThreadUtil::GetFlipTransform(transformType);
    EXPECT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_V);
    transformType = GraphicTransformType::GRAPHIC_FLIP_V_ROT270;
    transform = RSRenderThreadUtil::GetFlipTransform(transformType);
    EXPECT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_V);
    transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    transform = RSRenderThreadUtil::GetFlipTransform(transformType);
    EXPECT_EQ(transform, GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/**
 * @tc.name: FlipMatrix
 * @tc.desc: test results of FlipMatrix
 * @tc.type:FUNC
 * @tc.require: issueIB9LAF
 */
HWTEST_F(RSRenderThreadUtilTest, FlipMatrixTest, TestSize.Level1)
{
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    TextureExportBufferDrawParam params;
    RSRenderThreadUtil::FlipMatrix(transformType, params);
    EXPECT_TRUE(params.matrix.IsIdentity());
    transformType = GraphicTransformType::GRAPHIC_FLIP_H_ROT270;
    RSRenderThreadUtil::FlipMatrix(transformType, params);
    EXPECT_FALSE(params.matrix.IsIdentity());
    transformType = GraphicTransformType::GRAPHIC_FLIP_V_ROT90;
    RSRenderThreadUtil::FlipMatrix(transformType, params);
    EXPECT_FALSE(params.matrix.IsIdentity());
}

/**
 * @tc.name: DealWithSurfaceRotationAndGravity
 * @tc.desc: test results of DealWithSurfaceRotationAndGravity
 * @tc.type:FUNC
 * @tc.require: issueIB9LAF
 */
HWTEST_F(RSRenderThreadUtilTest, DealWithSurfaceRotationAndGravityTest, TestSize.Level1)
{
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    TextureExportBufferDrawParam params;
    RectF localBounds(0.f, 0.f, 100.f, 200.f);
    RSRenderThreadUtil::DealWithSurfaceRotationAndGravity(transformType, Gravity::TOP_LEFT, localBounds, params);
    EXPECT_EQ(params.dstRect.GetWidth(), 0);
    EXPECT_EQ(params.dstRect.GetHeight(), 0);
    transformType = GraphicTransformType::GRAPHIC_ROTATE_90;
    RSRenderThreadUtil::DealWithSurfaceRotationAndGravity(transformType, Gravity::TOP_LEFT, localBounds, params);
    EXPECT_EQ(localBounds.GetWidth(), 200);
    EXPECT_EQ(localBounds.GetHeight(), 100);
}

/**
 * @tc.name: GetSurfaceTransformMatrix
 * @tc.desc: test results of GetSurfaceTransformMatrix
 * @tc.type:FUNC
 * @tc.require: issueIB9LAF
 */
HWTEST_F(RSRenderThreadUtilTest, GetSurfaceTransformMatrixTest, TestSize.Level1)
{
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    RectF bufferBounds = RectF(0, 0, 100, 100);
    auto matrix = RSRenderThreadUtil::GetSurfaceTransformMatrix(transformType, bufferBounds);
    EXPECT_TRUE(matrix.IsIdentity());
    transformType = GraphicTransformType::GRAPHIC_ROTATE_90;
    matrix = RSRenderThreadUtil::GetSurfaceTransformMatrix(transformType, bufferBounds);
    EXPECT_FALSE(matrix.IsIdentity());
    transformType = GraphicTransformType::GRAPHIC_ROTATE_180;
    matrix = RSRenderThreadUtil::GetSurfaceTransformMatrix(transformType, bufferBounds);
    EXPECT_FALSE(matrix.IsIdentity());
    transformType = GraphicTransformType::GRAPHIC_ROTATE_270;
    matrix =  RSRenderThreadUtil::GetSurfaceTransformMatrix(transformType, bufferBounds);
    EXPECT_FALSE(matrix.IsIdentity());
}

/**
 * @tc.name: GetGravityMatrix
 * @tc.desc: test results of GetGravityMatrix
 * @tc.type:FUNC
 * @tc.require: issueIB9LAF
 */
HWTEST_F(RSRenderThreadUtilTest, GetGravityMatrixTest, TestSize.Level1)
{
    RectF bufferBounds = RectF(0, 0, 100, 100);
    RectF bounds = RectF(0, 0, 100, 100);
    auto matrix = RSRenderThreadUtil::GetGravityMatrix(Gravity::TOP_LEFT, bufferBounds, bounds);
    EXPECT_TRUE(matrix.IsIdentity());
    bounds = RectF(0, 0, 200, 100);
    matrix = RSRenderThreadUtil::GetGravityMatrix(Gravity::TOP_LEFT, bufferBounds, bounds);
    EXPECT_TRUE(matrix.IsIdentity());
    matrix = RSRenderThreadUtil::GetGravityMatrix(Gravity::RESIZE, bufferBounds, bounds);
    EXPECT_FALSE(matrix.IsIdentity());
}

/**
 * @tc.name: GetRotateTransform
 * @tc.desc: test results of GetRotateTransform
 * @tc.type:FUNC
 * @tc.require: issueIB9LAF
 */
HWTEST_F(RSRenderThreadUtilTest, GetRotateTransformTest, TestSize.Level1)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_FLIP_H;
    EXPECT_EQ(RSRenderThreadUtil::GetRotateTransform(transform), GraphicTransformType::GRAPHIC_ROTATE_NONE);
    transform = GraphicTransformType::GRAPHIC_FLIP_V;
    EXPECT_EQ(RSRenderThreadUtil::GetRotateTransform(transform), GraphicTransformType::GRAPHIC_ROTATE_NONE);
    transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT90;
    EXPECT_EQ(RSRenderThreadUtil::GetRotateTransform(transform), GraphicTransformType::GRAPHIC_ROTATE_90);
    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT90;
    EXPECT_EQ(RSRenderThreadUtil::GetRotateTransform(transform), GraphicTransformType::GRAPHIC_ROTATE_90);
    transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT180;
    EXPECT_EQ(RSRenderThreadUtil::GetRotateTransform(transform), GraphicTransformType::GRAPHIC_ROTATE_180);
    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT180;
    EXPECT_EQ(RSRenderThreadUtil::GetRotateTransform(transform), GraphicTransformType::GRAPHIC_ROTATE_180);
    transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT270;
    EXPECT_EQ(RSRenderThreadUtil::GetRotateTransform(transform), GraphicTransformType::GRAPHIC_ROTATE_270);
    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT270;
    EXPECT_EQ(RSRenderThreadUtil::GetRotateTransform(transform), GraphicTransformType::GRAPHIC_ROTATE_270);
}

/**
 * @tc.name: CreateTextureExportBufferDrawParams
 * @tc.desc: test results of CreateTextureExportBufferDrawParams
 * @tc.type:FUNC
 * @tc.require: issueIB9LAF
 */
HWTEST_F(RSRenderThreadUtilTest, CreateTextureExportBufferDrawParamsTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    config.name += std::to_string(1);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    auto params = RSRenderThreadUtil::CreateTextureExportBufferDrawParams(
        *surfaceNode, GraphicTransformType::GRAPHIC_ROTATE_NONE, nullptr);
    EXPECT_EQ(params.buffer, nullptr);
}

} // namespace OHOS::Rosen