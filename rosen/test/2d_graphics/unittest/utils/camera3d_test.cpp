/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "utils/camera3d.h"
#include "utils/scalar.h"
#include "utils/matrix.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Camera3DTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Camera3DTest::SetUpTestCase() {}
void Camera3DTest::TearDownTestCase() {}
void Camera3DTest::SetUp() {}
void Camera3DTest::TearDown() {}

/**
 * @tc.name: Camera3DCreateAndDestory001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DCreateAndDestory001, TestSize.Level1)
{
    // The best way to create Camera3D.
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
}

/**
 * @tc.name: Camera3DCreateAndDestory002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DCreateAndDestory002, TestSize.Level1)
{
    // The best way to create Camera3D.
    Camera3D camera3d;
}

/**
 * @tc.name: Camera3DTranslateTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DTranslateTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->Translate(131.5f, 121.0f, 60.0f);
}

/**
 * @tc.name: Camera3DTranslateTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DTranslateTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->Translate(23.6f, 7.9f, 800.9f);
}

/**
 * @tc.name: Camera3DRotateXDegreesTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateXDegreesTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateXDegrees(800.6f);
}

/**
 * @tc.name: Camera3DRotateXDegreesTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateXDegreesTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateXDegrees(300.0f);
}

/**
 * @tc.name: Camera3DRotateXDegreesTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateXDegreesTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateXDegrees(300.29f);
}

/**
 * @tc.name: Camera3DRotateYDegreesTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateYDegreesTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateYDegrees(300.29f);
}

/**
 * @tc.name: Camera3DRotateYDegreesTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateYDegreesTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateYDegrees(300);
}

/**
 * @tc.name: Camera3DRotateYDegreesTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateYDegreesTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateYDegrees(300.29f);
}

/**
 * @tc.name: Camera3DRotateZDegreesTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateZDegreesTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateZDegrees(300.29f);
}

/**
 * @tc.name: Camera3DRotateZDegreesTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateZDegreesTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateZDegrees(300.0f);
}

/**
 * @tc.name: Camera3DRotateZDegreesTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DRotateZDegreesTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateZDegrees(300.29f);
}

/**
 * @tc.name: Camera3DSetAndGetCameraPosTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DSetAndGetCameraPosTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->SetCameraPos(8.2f, 9.1f, 8.2f);
    EXPECT_EQ(8.2f, camera3d->GetCameraPosX());
    EXPECT_EQ(9.1f, camera3d->GetCameraPosY());
    EXPECT_EQ(8.2f, camera3d->GetCameraPosZ());
    camera3d->Save();
    camera3d->Restore();
}

/**
 * @tc.name: Camera3DSetAndGetCameraPosTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DSetAndGetCameraPosTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->SetCameraPos(8.0f, 8.1f, 8.2f);
    EXPECT_EQ(8.0f, camera3d->GetCameraPosX());
    EXPECT_EQ(8.1f, camera3d->GetCameraPosY());
    EXPECT_EQ(8.2f, camera3d->GetCameraPosZ());
    camera3d->Save();
    camera3d->Restore();
}

/**
 * @tc.name: Camera3DApplyToMatrixTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DApplyToMatrixTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    Matrix matrix;
    camera3d->ApplyToMatrix(matrix);
}

/**
 * @tc.name: Camera3DApplyToMatrixTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Camera3DApplyToMatrixTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    Matrix matrix2;
    camera3d->ApplyToMatrix(matrix2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS