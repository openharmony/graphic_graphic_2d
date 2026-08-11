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

#include "drawable/rs_render_node_drawable.h"
#include "gtest/gtest.h"
#include "params/rs_depth_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDepthRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDepthRenderParamsTest::SetUpTestCase() {}
void RSDepthRenderParamsTest::TearDownTestCase() {}
void RSDepthRenderParamsTest::SetUp() {}
void RSDepthRenderParamsTest::TearDown() {}

/**
 * @tc.name: SetDepthImageNeedSync001
 * @tc.desc: Verify SetDepthImage sets needSync_ flag to true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, SetDepthImageNeedSync001, TestSize.Level1)
{
    constexpr NodeId testNodeId = 4;
    RSDepthRenderParams params(testNodeId);
    params.needSync_ = false;
    params.SetDepthImage(nullptr);
    EXPECT_TRUE(params.needSync_);
}

/**
 * @tc.name: OnSync001
 * @tc.desc: Verify OnSync transfers depthImage_ to target params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, OnSync001, TestSize.Level1)
{
    constexpr NodeId testNodeId = 6;
    auto targetDepth = std::make_unique<RSDepthRenderParams>(testNodeId);
    std::unique_ptr<RSRenderParams> target = std::move(targetDepth);
    auto* targetPtr = static_cast<RSDepthRenderParams*>(target.get());
    EXPECT_NE(targetPtr, nullptr);

    RSDepthRenderParams srcParams(testNodeId);
    auto image = std::make_shared<Drawing::Image>();
    srcParams.SetDepthImage(image);

    srcParams.OnSync(target);
    EXPECT_EQ(targetPtr->GetDepthImage(), image);
}

/**
 * @tc.name: OnSync002
 * @tc.desc: Verify OnSync with nullptr depthImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, OnSync002, TestSize.Level1)
{
    constexpr NodeId testNodeId = 7;
    auto targetDepth = std::make_unique<RSDepthRenderParams>(testNodeId);
    std::unique_ptr<RSRenderParams> target = std::move(targetDepth);
    auto* targetPtr = static_cast<RSDepthRenderParams*>(target.get());
    EXPECT_NE(targetPtr, nullptr);

    RSDepthRenderParams srcParams(testNodeId);
    srcParams.SetDepthImage(nullptr);
    srcParams.OnSync(target);
    EXPECT_EQ(targetPtr->GetDepthImage(), nullptr);
}

/**
 * @tc.name: OnSync003
 * @tc.desc: Verify OnSync replaces target's existing depth image
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, OnSync003, TestSize.Level1)
{
    constexpr NodeId testNodeId = 8;
    auto targetDepth = std::make_unique<RSDepthRenderParams>(testNodeId);
    std::unique_ptr<RSRenderParams> target = std::move(targetDepth);
    auto* targetPtr = static_cast<RSDepthRenderParams*>(target.get());
    EXPECT_NE(targetPtr, nullptr);
    auto oldImage = std::make_shared<Drawing::Image>();
    targetPtr->SetDepthImage(oldImage);

    RSDepthRenderParams srcParams(testNodeId);
    auto newImage = std::make_shared<Drawing::Image>();
    srcParams.SetDepthImage(newImage);
    srcParams.OnSync(target);

    EXPECT_EQ(targetPtr->GetDepthImage(), newImage);
    EXPECT_NE(targetPtr->GetDepthImage(), oldImage);
}

/**
 * @tc.name: OnSync004
 * @tc.desc: Verify OnSync with null target
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, OnSync004, TestSize.Level1)
{
    constexpr NodeId testNodeId = 9;
    auto depthParams = std::make_unique<RSDepthRenderParams>(testNodeId);
    EXPECT_NO_FATAL_FAILURE(depthParams->OnSync(nullptr));
}

/**
 * @tc.name: SetAndGetDepthImage001
 * @tc.desc: Verify SetDepthImage & GetDepthImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, SetAndGetDepthImage001, TestSize.Level1)
{
    constexpr NodeId testNodeId = 114;
    RSDepthRenderParams params(testNodeId);

    auto image = std::make_shared<Drawing::Image>();
    params.SetDepthImage(image);
    EXPECT_EQ(params.GetDepthImage(), image);
}

/**
 * @tc.name: SetAndGetDepthCameraPara001
 * @tc.desc: Verify SetDepthCameraPara & GetDepthCameraPara
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, SetAndGetDepthCameraPara001, TestSize.Level1)
{
    constexpr NodeId testNodeId = 114;
    RSDepthRenderParams params(testNodeId);

    DepthCameraPara para;
    para.zNear = 1.0f;
    para.zFar = 1000.0f;
    params.SetDepthCameraPara(para);
    EXPECT_EQ(params.GetDepthCameraPara(), para);
}

/**
 * @tc.name: SetAndGetDepthLightPara001
 * @tc.desc: Verify SetDepthLightPara & GetDepthLightPara
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, SetAndGetDepthLightPara001, TestSize.Level1)
{
    constexpr NodeId testNodeId = 114;
    RSDepthRenderParams params(testNodeId);

    DepthLightPara para;
    para.intensity = 514.0f;
    params.SetDepthLightPara(para);
    EXPECT_EQ(params.GetDepthLightPara(), para);
}

/**
 * @tc.name: SetAndGetImageMatrix001
 * @tc.desc: Verify SetImageMatrix & GetImageMatrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, SetAndGetImageMatrix001, TestSize.Level1)
{
    constexpr NodeId testNodeId = 114;
    RSDepthRenderParams params(testNodeId);

    Matrix3f matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    params.SetImageMatrix(matrix);
    Drawing::Matrix::Buffer buffer{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
    Drawing::Matrix drawingMatrix;
    drawingMatrix.SetAll(buffer);
    EXPECT_EQ(params.GetImageMatrix(), drawingMatrix);
}

/**
 * @tc.name: SetAndGetBackgroundMatrix001
 * @tc.desc: Verify SetBackgroundMatrix & GetBackgroundMatrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderParamsTest, SetAndGetBackgroundMatrix001, TestSize.Level1)
{
    constexpr NodeId testNodeId = 114;
    RSDepthRenderParams params(testNodeId);

    Drawing::Matrix::Buffer buffer{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
    Drawing::Matrix matrix;
    matrix.SetAll(buffer);

    params.SetBackgroundMatrix(matrix);
    EXPECT_EQ(params.GetBackgroundMatrix(), matrix);
}
} // namespace OHOS::Rosen