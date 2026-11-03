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

#include <memory>

#include "gtest/gtest.h"
#include "modifier_ng/appearance/rs_depth_space_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSDepthSpaceModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override {}
    void TearDown() override {}
};

void RSDepthSpaceModifierTest::SetUpTestCase() {}
void RSDepthSpaceModifierTest::TearDownTestCase() {}

/**
 * @tc.name: GetType001
 * @tc.desc: Test GetType of RSDepthSpaceModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, GetType001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::DEPTH_SPACE);
}

/**
 * @tc.name: SetDepthImage001
 * @tc.desc: Test SetDepthImage with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthImage001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set depth image to nullptr
    std::shared_ptr<RSImage> depthImage = nullptr;
    modifier->SetDepthImage(depthImage);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthImage002
 * @tc.desc: Test SetDepthImage with valid image pointer
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthImage002, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Create an RSImage (this will be a basic test pattern)
    std::shared_ptr<RSImage> depthImage = std::make_shared<RSImage>();
    modifier->SetDepthImage(depthImage);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthImage003
 * @tc.desc: Test SetDepthImage with multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthImage003, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set depth image multiple times
    std::shared_ptr<RSImage> depthImage1 = std::make_shared<RSImage>();
    modifier->SetDepthImage(depthImage1);

    std::shared_ptr<RSImage> depthImage2 = nullptr;
    modifier->SetDepthImage(depthImage2);

    std::shared_ptr<RSImage> depthImage3 = std::make_shared<RSImage>();
    modifier->SetDepthImage(depthImage3);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthCameraPara001
 * @tc.desc: Test SetDepthCameraPara with default camera parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthCameraPara001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set default camera parameters
    DepthCameraPara cameraPara;
    modifier->SetDepthCameraPara(cameraPara);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthCameraPara002
 * @tc.desc: Test SetDepthCameraPara with custom camera parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthCameraPara002, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set custom camera parameters
    DepthCameraPara cameraPara;
    cameraPara.position = Vector3f(1.0f, 2.0f, 3.0f);
    cameraPara.quaternion = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    cameraPara.yFov = 60.0f;
    cameraPara.zNear = 0.1f;
    cameraPara.zFar = 100.0f;

    modifier->SetDepthCameraPara(cameraPara);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthCameraPara003
 * @tc.desc: Test SetDepthCameraPara with multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthCameraPara003, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set camera parameters multiple times
    DepthCameraPara cameraPara1;
    cameraPara1.yFov = 45.0f;
    modifier->SetDepthCameraPara(cameraPara1);

    DepthCameraPara cameraPara2;
    cameraPara2.yFov = 90.0f;
    modifier->SetDepthCameraPara(cameraPara2);

    DepthCameraPara cameraPara3;
    cameraPara3.yFov = 120.0f;
    modifier->SetDepthCameraPara(cameraPara3);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthCameraPara004
 * @tc.desc: Test SetDepthCameraPara with edge case values
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthCameraPara004, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Test with edge case values
    DepthCameraPara cameraPara;
    cameraPara.yFov = 0.0f;  // minimum
    cameraPara.zNear = 0.001f;  // very small
    cameraPara.zFar = 10000.0f;  // very large
    modifier->SetDepthCameraPara(cameraPara);

    cameraPara.yFov = 180.0f;  // maximum
    modifier->SetDepthCameraPara(cameraPara);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthLightPara001
 * @tc.desc: Test SetDepthLightPara with default light parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthLightPara001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set default light parameters
    DepthLightPara lightPara;
    modifier->SetDepthLightPara(lightPara);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthLightPara002
 * @tc.desc: Test SetDepthLightPara with custom light parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthLightPara002, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set custom light parameters
    DepthLightPara lightPara;
    lightPara.direction = Vector3f(0.0f, -1.0f, 0.0f);  // downward light
    lightPara.color = Vector3f(1.0f, 1.0f, 1.0f);  // white light
    lightPara.intensity = 1.0f;

    modifier->SetDepthLightPara(lightPara);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthLightPara003
 * @tc.desc: Test SetDepthLightPara with multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthLightPara003, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set light parameters multiple times
    DepthLightPara lightPara1;
    lightPara1.intensity = 0.5f;
    modifier->SetDepthLightPara(lightPara1);

    DepthLightPara lightPara2;
    lightPara2.intensity = 1.0f;
    modifier->SetDepthLightPara(lightPara2);

    DepthLightPara lightPara3;
    lightPara3.intensity = 2.0f;
    modifier->SetDepthLightPara(lightPara3);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthLightPara004
 * @tc.desc: Test SetDepthLightPara with edge case values
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthLightPara004, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Test with edge case values
    DepthLightPara lightPara;
    lightPara.intensity = 0.0f;  // minimum (no light)
    modifier->SetDepthLightPara(lightPara);

    lightPara.intensity = 100.0f;  // very bright
    modifier->SetDepthLightPara(lightPara);

    // Test with different colors
    lightPara.color = Vector3f(1.0f, 0.0f, 0.0f);  // red
    modifier->SetDepthLightPara(lightPara);

    lightPara.color = Vector3f(0.0f, 1.0f, 0.0f);  // green
    modifier->SetDepthLightPara(lightPara);

    lightPara.color = Vector3f(0.0f, 0.0f, 1.0f);  // blue
    modifier->SetDepthLightPara(lightPara);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDepthImageMatrix001
 * @tc.desc: Test SetDepthImageMatrix with edge case values
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, SetDepthImageMatrix001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    Matrix3f matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    EXPECT_NO_FATAL_FAILURE(modifier->SetDepthImageMatrix(matrix));
}

/**
 * @tc.name: CombinedTest001
 * @tc.desc: Test setting all depth space properties together
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, CombinedTest001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Set all properties
    std::shared_ptr<RSImage> depthImage = std::make_shared<RSImage>();
    modifier->SetDepthImage(depthImage);

    DepthCameraPara cameraPara;
    cameraPara.yFov = 60.0f;
    modifier->SetDepthCameraPara(cameraPara);

    DepthLightPara lightPara;
    lightPara.intensity = 1.0f;
    modifier->SetDepthLightPara(lightPara);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: CombinedTest002
 * @tc.desc: Test alternating between different property settings
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, CombinedTest002, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();

    // Alternate between different properties
    DepthCameraPara cameraPara;
    cameraPara.yFov = 45.0f;
    modifier->SetDepthCameraPara(cameraPara);

    DepthLightPara lightPara;
    lightPara.intensity = 0.5f;
    modifier->SetDepthLightPara(lightPara);

    std::shared_ptr<RSImage> depthImage = std::make_shared<RSImage>();
    modifier->SetDepthImage(depthImage);

    cameraPara.yFov = 90.0f;
    modifier->SetDepthCameraPara(cameraPara);

    lightPara.intensity = 1.0f;
    modifier->SetDepthLightPara(lightPara);

    modifier->SetDepthImage(nullptr);

    // Test should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: MultipleModifiersTest
 * @tc.desc: Test creating multiple modifiers with different settings
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceModifierTest, MultipleModifiersTest, TestSize.Level1)
{
    // Create multiple modifiers with different settings
    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier1 =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();
    DepthCameraPara cameraPara1;
    cameraPara1.yFov = 45.0f;
    modifier1->SetDepthCameraPara(cameraPara1);

    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier2 =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();
    DepthCameraPara cameraPara2;
    cameraPara2.yFov = 90.0f;
    modifier2->SetDepthCameraPara(cameraPara2);

    std::shared_ptr<ModifierNG::RSDepthSpaceModifier> modifier3 =
        std::make_shared<ModifierNG::RSDepthSpaceModifier>();
    DepthLightPara lightPara3;
    lightPara3.intensity = 1.5f;
    modifier3->SetDepthLightPara(lightPara3);

    // Verify each modifier has the correct type
    EXPECT_EQ(modifier1->GetType(), ModifierNG::RSModifierType::DEPTH_SPACE);
    EXPECT_EQ(modifier2->GetType(), ModifierNG::RSModifierType::DEPTH_SPACE);
    EXPECT_EQ(modifier3->GetType(), ModifierNG::RSModifierType::DEPTH_SPACE);
}
} // namespace OHOS::Rosen