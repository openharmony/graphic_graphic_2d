/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "common/rs_obj_abs_geometry.h"
#include "utils/matrix.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr static float FLOAT_DATA_INIT = 0.5f;
constexpr static float FLOAT_DATA_UPDATE = 1.0f;
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;

class RSObjAbsGeometryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    Drawing::Matrix44 MakeRotMatrix44();
};

void RSObjAbsGeometryTest::SetUpTestCase() {}
void RSObjAbsGeometryTest::TearDownTestCase() {}
void RSObjAbsGeometryTest::SetUp() {}
void RSObjAbsGeometryTest::TearDown() {}
Drawing::Matrix44 RSObjAbsGeometryTest::MakeRotMatrix44()
{
    float x = 0.1f;
    float y = 0.1f;
    float z = 0.1f;
    float w = 1.0f;
    float magnitudeRecp = 1.0f / sqrt((x * x) + (y * y) + (z * z) + (w * w));
    x *= magnitudeRecp;
    y *= magnitudeRecp;
    z *= magnitudeRecp;
    w *= magnitudeRecp;
    Drawing::Matrix44::Buffer buffer = {
        1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w), 0,   // m00 ~ m30
        2.f * (x * y - z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w), 0,   // m01 ~ m31
        2.f * (x * z + y * w), 2.f * (y * z - x * w), 1.f - 2.f * (x * x + y * y), 0,   // m02 ~ m32
        0, 0, 0, 1 };                                                                    // m03 ~ m33
    Drawing::Matrix44 rotMatrix44;
    rotMatrix44.SetMatrix44ColMajor(buffer);
    return rotMatrix44;
}

/**
 * @tc.name: UpdateMatrix001
 * @tc.desc: test results of UpdateMatrix
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateMatrix001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. UpdateMatrix
     */
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    auto parent = nullptr;
    Quaternion quaternion;
    Drawing::Matrix contextMatrix;
    float x = 0.5f;
    float y = 0.5f;
    float w = 0.5f;
    float h = 0.5f;
    float rotationX = 0.5f;
    float rotationY = 0.5f;
    float translateZ = 0.5f;
    float cameraDistance = 0.5f;
    float pivotZ = 0.5f;
    float scaleX = 0.5f;
    float scaleY = 0.5f;
    float scaleZ = 0.5f;
    float skewX = 0.5f;
    float skewY = 0.5f;
    float skewZ = 0.5f;
    float perspX = FLOAT_DATA_INIT;
    float perspY = FLOAT_DATA_INIT;
    float perspZ = FLOAT_DATA_INIT;
    float perspW = FLOAT_DATA_INIT;
    rsObjAbsGeometry->SetRect(x, y, w, h);
    rsObjAbsGeometry->SetQuaternion(quaternion);
    rsObjAbsGeometry->SetRotationY(rotationY);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetRotationX(rotationX);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetTranslateZ(translateZ);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetCameraDistance(cameraDistance);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetPivotZ(pivotZ);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetScaleY(scaleY);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetScaleX(scaleX);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetScaleZ(scaleZ);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetSkewX(skewX);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetSkewY(skewY);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetSkewZ(skewZ);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetPerspX(perspX);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetPerspY(perspY);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetPerspZ(perspZ);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetPerspW(perspW);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetQuaternion(quaternion.Flip());
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    scaleX = 1.f;
    scaleY = 1.f;
    scaleZ = 1.f;
    skewX = 0.5f;
    skewY = 0.5f;
    skewZ = 0.5f;
    perspX = FLOAT_DATA_UPDATE;
    perspY = FLOAT_DATA_UPDATE;
    perspZ = FLOAT_DATA_UPDATE;
    perspW = FLOAT_DATA_UPDATE;
    rsObjAbsGeometry->SetScale(scaleX, scaleY);
    rsObjAbsGeometry->SetSkew(skewX, skewY);
    rsObjAbsGeometry->SetPersp(perspX, perspY);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetContextMatrix(contextMatrix);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);

    rsObjAbsGeometry->SetSkew(skewX, skewY, skewZ);
    rsObjAbsGeometry->SetPersp(perspX, perspY, perspZ, perspW);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry->SetContextMatrix(contextMatrix);
    rsObjAbsGeometry->UpdateMatrix(parent, std::nullopt);
    EXPECT_TRUE(rsObjAbsGeometry != nullptr);
}

/**
 * @tc.name: UpdateMatrix002
 * @tc.desc: test results of UpdateMatrix
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateMatrix002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. UpdateMatrix
     */
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    Drawing::Matrix matrix;
    auto parent = &matrix;
    float offsetX = 0.f;
    float offsetY = 0.f;
    float x = 0.f;
    float y = 0.f;
    float w = 3.5f;
    float h = 3.5f;
    Drawing::Point offset = Drawing::Point(offsetX, offsetY);
    rsObjAbsGeometry->SetRect(x, y, w, h);
    rsObjAbsGeometry->UpdateMatrix(parent, offset);
    x = 0.5f;
    y = 0.5f;
    w = 1.5f;
    h = 1.5f;
    rsObjAbsGeometry->SetRect(x, y, w, h);
    rsObjAbsGeometry->UpdateMatrix(parent, offset);
    EXPECT_TRUE(rsObjAbsGeometry != nullptr);
}

/**
 * @tc.name: UpdateByMatrixFromSelf001
 * @tc.desc: test results of UpdateByMatrixFromSelf
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateByMatrixFromSelf001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. UpdateByMatrixFromSelf
     */
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    Quaternion quaternion;
    Drawing::Matrix contextMatrix;
    float x = 0.5f;
    float y = 0.5f;
    float w = 0.5f;
    float h = 0.5f;
    float rotationX = 0.5f;
    float rotationY = 0.5f;
    float translateZ = 0.5f;
    rsObjAbsGeometry->SetRect(x, y, w, h);
    rsObjAbsGeometry->SetQuaternion(quaternion.Flip());
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    rsObjAbsGeometry->SetRotationY(rotationY);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    rsObjAbsGeometry->SetRotationX(rotationX);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    rsObjAbsGeometry->SetTranslateZ(translateZ);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    rsObjAbsGeometry->SetContextMatrix(contextMatrix);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry != nullptr);
}

/**
 * @tc.name: ConcatMatrixTest
 * @tc.desc: Verify function ConcatMatrix
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSObjAbsGeometryTest, ConcatMatrixTest, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    Drawing::Matrix matrix;
    rsObjAbsGeometry->ConcatMatrix(matrix);
    matrix.matrixImplPtr->Translate(10, 10);
    rsObjAbsGeometry->ConcatMatrix(matrix);
    rsObjAbsGeometry->absMatrix_ = std::make_optional<Drawing::Matrix>();
    rsObjAbsGeometry->ConcatMatrix(matrix);
    EXPECT_TRUE(rsObjAbsGeometry->absMatrix_.has_value());
}

/**
 * @tc.name: IsNeedClientComposeTest
 * @tc.desc: Verify function IsNeedClientCompose
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSObjAbsGeometryTest, IsNeedClientComposeTest, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    EXPECT_TRUE(rsObjAbsGeometry->trans_);
    EXPECT_FALSE(rsObjAbsGeometry->IsNeedClientCompose());
}

/**
 * @tc.name: GetDataRangeTest
 * @tc.desc: Verify function GetDataRange
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSObjAbsGeometryTest, GetDataRangeTest, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    EXPECT_EQ(rsObjAbsGeometry->GetDataRange(0.0f, 1.0f, -1.0f, 1.0f).x_, -1.0f);
}

/**
 * @tc.name: UpdateAbsMatrix2DTest
 * @tc.desc: Verify function UpdateAbsMatrix2D
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DTest, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->rotation_ = 0.5f;
    rsObjAbsGeometry->trans_->scaleX_ = 0.f;
    rsObjAbsGeometry->trans_->scaleY_ = 0.f;
    rsObjAbsGeometry->trans_->scaleZ_ = 0.f;
    rsObjAbsGeometry->UpdateAbsMatrix2D();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());
}


/**
 * @tc.name: UpdateAbsMatrix2DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix2D with throughout UT
 *           Case 1 - Scale transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DAdvancedTest001, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->scaleX_ = 1.6f;
    rsObjAbsGeometry->trans_->scaleY_ = 1.5f;
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.SetScale(rsObjAbsGeometry->trans_->scaleX_, rsObjAbsGeometry->trans_->scaleY_);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix2DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix2D with throughout UT
 *           Case 2 - Skew transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DAdvancedTest002, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->skewX_ = 1.6f;
    rsObjAbsGeometry->trans_->skewY_ = 1.5f;
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.SetSkew(rsObjAbsGeometry->trans_->skewX_, rsObjAbsGeometry->trans_->skewY_);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix2DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix2D with throughout UT
 *           Case 3 - Rotation transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DAdvancedTest003, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->rotation_ = 0.5f;
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreRotate(0.5f, 0, 0);
    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix2DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix2D with throughout UT
 *           Case 4 - Translation transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DAdvancedTest004, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->translateX_ = -5.5f;
    rsObjAbsGeometry->trans_->translateY_ = 11.5f;
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreTranslate(rsObjAbsGeometry->trans_->translateX_, rsObjAbsGeometry->trans_->translateY_);
    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix2DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix2D with throughout UT
 *           Case 5 - Perspective transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DAdvancedTest005, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->perspX_ = 0.336f;
    rsObjAbsGeometry->trans_->perspY_ = 0.557f;
    rsObjAbsGeometry->trans_->perspW_ = 1.02f;
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix expectedMatrix;
    expectedMatrix.SetMatrix(
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        0.336, 0.557f, 1.02f);
    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix2DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix2D with throughout UT
 *           Case 6 - Global Transforms Only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DAdvancedTest006, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->x_ = 40.5f;
    rsObjAbsGeometry->y_ = 320.5f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreTranslate(rsObjAbsGeometry->x_, rsObjAbsGeometry->y_);
    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix2DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix2D with throughout UT
 *           Case 7 - Local Transforms All
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DAdvancedTest007, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->scaleX_ = 1.6f;
    rsObjAbsGeometry->trans_->scaleY_ = 1.5f;
    rsObjAbsGeometry->trans_->skewX_ = 1.6f;
    rsObjAbsGeometry->trans_->skewY_ = 1.5f;
    rsObjAbsGeometry->trans_->rotation_ = 0.5f;
    rsObjAbsGeometry->trans_->translateX_ = -5.5f;
    rsObjAbsGeometry->trans_->translateY_ = 11.5f;
    rsObjAbsGeometry->trans_->perspX_ = 0.336f;
    rsObjAbsGeometry->trans_->perspY_ = 0.557f;
    rsObjAbsGeometry->trans_->perspW_ = 0.8f;
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.SetMatrix(
        1.5789955f, 2.3868189f, -5.5f,
        2.41387f, 1.5208866f, 11.5f,
        1.8750684f, 1.6491049f, 5.3575f);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix2DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix2D with throughout UT
 *           Case 8 - Local + Global Transforms
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix2DAdvancedTest008, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->scaleX_ = 1.6f;
    rsObjAbsGeometry->trans_->scaleY_ = 1.5f;
    rsObjAbsGeometry->trans_->skewX_ = 1.6f;
    rsObjAbsGeometry->trans_->skewY_ = 1.5f;
    rsObjAbsGeometry->trans_->rotation_ = 0.5f;
    rsObjAbsGeometry->trans_->translateX_ = -5.5f;
    rsObjAbsGeometry->trans_->translateY_ = 11.5f;
    rsObjAbsGeometry->trans_->perspX_ = 0.336f;
    rsObjAbsGeometry->trans_->perspY_ = 0.557f;
    rsObjAbsGeometry->trans_->perspW_ = 0.8f;
    rsObjAbsGeometry->x_ = 40.5f;
    rsObjAbsGeometry->y_ = 320.5f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.SetMatrix(
        77.5192719f, 69.1755598f, 211.4787598f,
        603.3733521f, 530.0589560f, 1728.57869f,
        1.8750686f, 1.6491047f, 5.3575f);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix3DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix3D with throughout UT
 *           Case 1 - Scale + Rotation(Quaternion) transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix3DAdvancedTest001, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->scaleX_ = 1.6f;
    rsObjAbsGeometry->trans_->scaleY_ = 1.5f;
    rsObjAbsGeometry->trans_->scaleZ_ = 0.28f;
    rsObjAbsGeometry->trans_->quaternion_ = Quaternion(0.1f, 0.1f, 0.1f, 1.0f);
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->z_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix44 resMat, scaleMat;
    scaleMat.PreScale(
        rsObjAbsGeometry->trans_->scaleX_,
        rsObjAbsGeometry->trans_->scaleY_,
        rsObjAbsGeometry->trans_->scaleZ_);
    Drawing::Matrix44 rotMatrix44 = MakeRotMatrix44();
    resMat = resMat * rotMatrix44 * scaleMat;
    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreConcat(resMat);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix3DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix3D with throughout UT
 *           Case 2 - Skew + Rotation(Quaternion) transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix3DAdvancedTest002, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->skewX_ = 1.6f;
    rsObjAbsGeometry->trans_->skewY_ = 1.5f;
    rsObjAbsGeometry->trans_->skewZ_ = 0.66f;
    rsObjAbsGeometry->trans_->quaternion_ = Quaternion(0.1f, 0.1f, 0.1f, 1.0f);
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->z_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix44 resMat, skewMat;
    skewMat.SetMatrix44RowMajor(
        {1.f, rsObjAbsGeometry->trans_->skewX_, 0.f, 0.f,
            rsObjAbsGeometry->trans_->skewY_, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f});
    Drawing::Matrix44 rotMatrix44 = MakeRotMatrix44();
    resMat = resMat * rotMatrix44 * skewMat;

    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreConcat(resMat);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix3DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix3D with throughout UT
 *           Case 3 - Rotation(Quaternion) transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix3DAdvancedTest003, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->quaternion_ = Quaternion(0.1f, 0.1f, 0.1f, 1.0f);
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->z_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix44 rotMatrix44 = MakeRotMatrix44();
    Drawing::Matrix44 resMat;
    resMat = resMat * rotMatrix44;
    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreConcat(resMat);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix3DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix3D with throughout UT
 *           Case 4 - Local Translation + Rotation(Quaternion) transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix3DAdvancedTest004, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->translateX_ = -5.5f;
    rsObjAbsGeometry->trans_->translateY_ = 11.5f;
    rsObjAbsGeometry->trans_->translateZ_ = 2.1f;
    rsObjAbsGeometry->trans_->quaternion_ = Quaternion(0.1f, 0.1f, 0.1f, 1.0f);
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->z_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix44 resMat, localTransMat;
    localTransMat.PreTranslate(
        rsObjAbsGeometry->trans_->translateX_,
        rsObjAbsGeometry->trans_->translateY_,
        rsObjAbsGeometry->trans_->translateZ_);
    Drawing::Matrix44 rotMatrix44 = MakeRotMatrix44();
    resMat = resMat * localTransMat * rotMatrix44;
    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreConcat(resMat);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix3DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix3D with throughout UT
 *           Case 5 - Perspective + Rotation(Quaternion) transform only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix3DAdvancedTest005, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->perspX_ = 0.336f;
    rsObjAbsGeometry->trans_->perspY_ = 0.557f;
    rsObjAbsGeometry->trans_->perspZ_ = 1.1f;
    rsObjAbsGeometry->trans_->perspW_ = 1.02f;
    rsObjAbsGeometry->trans_->quaternion_ = Quaternion(0.1f, 0.1f, 0.1f, 1.0f);
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->z_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix44 resMat, perspM44;
    perspM44.SetMatrix44RowMajor({1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        rsObjAbsGeometry->trans_->perspX_, rsObjAbsGeometry->trans_->perspY_,
        rsObjAbsGeometry->trans_->perspZ_, rsObjAbsGeometry->trans_->perspW_});
    Drawing::Matrix44 rotMatrix44 = MakeRotMatrix44();
    resMat = resMat * perspM44 * rotMatrix44;
    Drawing::Matrix expectedMatrix;
    expectedMatrix.PreConcat(resMat);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix3DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix3D with throughout UT
 *           Case 6 - Global + Rotation(Quaternion) Transforms Only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix3DAdvancedTest006, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->perspX_ = 0.336f;
    rsObjAbsGeometry->trans_->perspY_ = 0.557f;
    rsObjAbsGeometry->trans_->perspZ_ = 1.1f;
    rsObjAbsGeometry->trans_->perspW_ = 1.02f;
    rsObjAbsGeometry->trans_->quaternion_ = Quaternion(0.1f, 0.1f, 0.1f, 1.0f);
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->z_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix44 resMat, perspM44;
    perspM44.SetMatrix44RowMajor({1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        rsObjAbsGeometry->trans_->perspX_, rsObjAbsGeometry->trans_->perspY_,
        rsObjAbsGeometry->trans_->perspZ_, rsObjAbsGeometry->trans_->perspW_});
    Drawing::Matrix44 rotMatrix44 = MakeRotMatrix44();
    resMat = resMat * perspM44 * rotMatrix44;
    Drawing::Matrix expectedMatrix;
    expectedMatrix.PreConcat(resMat);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix3DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix3D with throughout UT
 *           Case 7 - Global + Rotation(Quaternion) Transforms Only
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix3DAdvancedTest007, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->scaleX_ = 1.6f;
    rsObjAbsGeometry->trans_->scaleY_ = 1.5f;
    rsObjAbsGeometry->trans_->scaleZ_ = 0.28f;
    rsObjAbsGeometry->trans_->skewX_ = 1.6f;
    rsObjAbsGeometry->trans_->skewY_ = 1.5f;
    rsObjAbsGeometry->trans_->skewZ_ = 0.66f;
    rsObjAbsGeometry->trans_->quaternion_ = Quaternion(0.1f, 0.1f, 0.1f, 1.0f);
    rsObjAbsGeometry->trans_->translateX_ = -5.5f;
    rsObjAbsGeometry->trans_->translateY_ = 11.5f;
    rsObjAbsGeometry->trans_->translateZ_ = 2.1f;
    rsObjAbsGeometry->trans_->perspX_ = 0.336f;
    rsObjAbsGeometry->trans_->perspY_ = 0.557f;
    rsObjAbsGeometry->trans_->perspZ_ = 1.1f;
    rsObjAbsGeometry->trans_->perspW_ = 1.02f;
    rsObjAbsGeometry->x_ = 0.f;
    rsObjAbsGeometry->y_ = 0.f;
    rsObjAbsGeometry->z_ = 0.f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix44 matrix44;
    matrix44.SetMatrix44RowMajor(
        {1.118447f, 2.044660f, 0.059806f, -5.500000f,
        2.648544f, 1.954369f, -0.048932f, 11.500000f,
        0.233010f, -0.099029f, 0.269126f, 2.100000f,
        2.107348f, 1.666657f, 0.288878f, 7.887500f});
    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreConcat(matrix44);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: UpdateAbsMatrix3DAdvancedTest
 * @tc.desc: Verify function UpdateAbsMatrix3D with throughout UT
 *           Case 8 - Local + Global Transforms
 * @tc.type: FUNC
 * @tc.require: issuesIAP7XJ
 */
HWTEST_F(RSObjAbsGeometryTest, UpdateAbsMatrix3DAdvancedTest008, TestSize.Level1)
{
    auto rsObjAbsGeometry = std::make_shared<RSObjAbsGeometry>();
    rsObjAbsGeometry->trans_ = std::make_optional<RSTransform>();
    rsObjAbsGeometry->trans_->scaleX_ = 1.6f;
    rsObjAbsGeometry->trans_->scaleY_ = 1.5f;
    rsObjAbsGeometry->trans_->scaleZ_ = 0.28f;
    rsObjAbsGeometry->trans_->skewX_ = 1.6f;
    rsObjAbsGeometry->trans_->skewY_ = 1.5f;
    rsObjAbsGeometry->trans_->skewZ_ = 0.66f;
    rsObjAbsGeometry->trans_->quaternion_ = Quaternion(0.1f, 0.1f, 0.1f, 1.0f);
    rsObjAbsGeometry->trans_->translateX_ = -5.5f;
    rsObjAbsGeometry->trans_->translateY_ = 11.5f;
    rsObjAbsGeometry->trans_->translateZ_ = 2.1f;
    rsObjAbsGeometry->trans_->perspX_ = 0.336f;
    rsObjAbsGeometry->trans_->perspY_ = 0.557f;
    rsObjAbsGeometry->trans_->perspZ_ = 1.1f;
    rsObjAbsGeometry->trans_->perspW_ = 1.02f;
    rsObjAbsGeometry->x_ = 40.5f;
    rsObjAbsGeometry->y_ = 320.5f;
    rsObjAbsGeometry->z_ = 5.5f;
    rsObjAbsGeometry->trans_->pivotX_ = 0.f;
    rsObjAbsGeometry->trans_->pivotY_ = 0.f;
    rsObjAbsGeometry->SetSize(100, 100);
    rsObjAbsGeometry->UpdateByMatrixFromSelf();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());

    Drawing::Matrix44 matrix44;
    matrix44.SetMatrix44RowMajor(
        {86.466026f, 69.5442811f, 11.759384f, 313.943756f,
        678.053528f, 536.117981f, 92.536613f, 2539.443848f,
        11.823421f, 9.067585f, 1.857958f, 45.481251f,
        2.107348f, 1.666657f, 0.288878f, 7.887500f});
    Drawing::Matrix expectedMatrix;
    expectedMatrix.Reset();
    expectedMatrix.PreConcat(matrix44);

    for (int i = 0; i < 9; ++i) {
        EXPECT_NEAR(expectedMatrix.Get(i), rsObjAbsGeometry->GetMatrix().Get(i), FLOAT_DATA_EPSILON);
    }
}

/**
 * @tc.name: DeflateToRectI001
 * @tc.desc: Verify function DeflateToRectI
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, DeflateToRectI001, TestSize.Level1)
{
    constexpr float srcRectLeft = 1.1f;
    constexpr float srcRectTop = 2.01f;
    constexpr float srcRectRight = 3.001f;
    constexpr float srcRectBottom = 4.0001f;
    constexpr int expectDstRectLeft = 2;
    constexpr int expectDstRectTop = 3;
    constexpr int expectDstRectRight = 3;
    constexpr int expectDstRectBottom = 4;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    RectI dstRect = RSObjAbsGeometry::DeflateToRectI(srcRect);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: DeflateToRectI002
 * @tc.desc: Verify function DeflateToRectI
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, DeflateToRectI002, TestSize.Level1)
{
    constexpr float srcRectLeft = 1.5f;
    constexpr float srcRectTop = 2.5f;
    constexpr float srcRectRight = 3.5f;
    constexpr float srcRectBottom = 4.5f;
    constexpr int expectDstRectLeft = 2;
    constexpr int expectDstRectTop = 3;
    constexpr int expectDstRectRight = 3;
    constexpr int expectDstRectBottom = 4;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    RectI dstRect = RSObjAbsGeometry::DeflateToRectI(srcRect);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: DeflateToRectI003
 * @tc.desc: Verify function DeflateToRectI
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, DeflateToRectI003, TestSize.Level1)
{
    constexpr float srcRectLeft = 1.9f;
    constexpr float srcRectTop = 2.99f;
    constexpr float srcRectRight = 3.999f;
    constexpr float srcRectBottom = 4.9999f;
    constexpr int expectDstRectLeft = 2;
    constexpr int expectDstRectTop = 3;
    constexpr int expectDstRectRight = 3;
    constexpr int expectDstRectBottom = 4;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    RectI dstRect = RSObjAbsGeometry::DeflateToRectI(srcRect);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: InflateToRectI001
 * @tc.desc: Verify function InflateToRectI
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, InflateToRectI001, TestSize.Level1)
{
    constexpr float srcRectLeft = 1.1f;
    constexpr float srcRectTop = 2.01f;
    constexpr float srcRectRight = 3.001f;
    constexpr float srcRectBottom = 4.0001f;
    constexpr int expectDstRectLeft = 1;
    constexpr int expectDstRectTop = 2;
    constexpr int expectDstRectRight = 4;
    constexpr int expectDstRectBottom = 5;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    RectI dstRect = RSObjAbsGeometry::InflateToRectI(srcRect);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: InflateToRectI002
 * @tc.desc: Verify function InflateToRectI
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, InflateToRectI002, TestSize.Level1)
{
    constexpr float srcRectLeft = 1.5f;
    constexpr float srcRectTop = 2.5f;
    constexpr float srcRectRight = 3.5f;
    constexpr float srcRectBottom = 4.5f;
    constexpr int expectDstRectLeft = 1;
    constexpr int expectDstRectTop = 2;
    constexpr int expectDstRectRight = 4;
    constexpr int expectDstRectBottom = 5;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    RectI dstRect = RSObjAbsGeometry::InflateToRectI(srcRect);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: InflateToRectI003
 * @tc.desc: Verify function InflateToRectI
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, InflateToRectI003, TestSize.Level1)
{
    constexpr float srcRectLeft = 1.9f;
    constexpr float srcRectTop = 2.99f;
    constexpr float srcRectRight = 3.999f;
    constexpr float srcRectBottom = 4.9999f;
    constexpr int expectDstRectLeft = 1;
    constexpr int expectDstRectTop = 2;
    constexpr int expectDstRectRight = 4;
    constexpr int expectDstRectBottom = 5;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    RectI dstRect = RSObjAbsGeometry::InflateToRectI(srcRect);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: MapRectWithoutRoundingI001
 * @tc.desc: Verify function MapRectWithoutRounding
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, MapRectWithoutRounding001, TestSize.Level1)
{
    constexpr float srcRectLeft = 0.f;
    constexpr float srcRectTop = 0.f;
    constexpr float srcRectRight = 100.f;
    constexpr float srcRectBottom = 200.f;
    constexpr float expectDstRectLeft = 0.f;
    constexpr float expectDstRectTop = 0.f;
    constexpr float expectDstRectRight = 100.f;
    constexpr float expectDstRectBottom = 200.f;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    Drawing::Matrix matrix;
    RectF dstRect = RSObjAbsGeometry::MapRectWithoutRounding(srcRect, matrix);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: MapRectWithoutRoundingI002
 * @tc.desc: Verify function MapRectWithoutRounding
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, MapRectWithoutRounding002, TestSize.Level1)
{
    constexpr float srcRectLeft = 0.f;
    constexpr float srcRectTop = 0.f;
    constexpr float srcRectRight = 100.f;
    constexpr float srcRectBottom = 200.f;
    constexpr float expectDstRectLeft = 10.1f;
    constexpr float expectDstRectTop = -29.5f;
    constexpr float expectDstRectRight = 110.1f;
    constexpr float expectDstRectBottom = 170.5f;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    Drawing::Matrix matrix;
    matrix.Set(Drawing::Matrix::TRANS_X, 10.1f);
    matrix.Set(Drawing::Matrix::TRANS_Y, -29.5f);
    RectF dstRect = RSObjAbsGeometry::MapRectWithoutRounding(srcRect, matrix);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: MapRectWithoutRoundingI003
 * @tc.desc: Verify function MapRectWithoutRounding
 * @tc.type: FUNC
 * @tc.require: issuesIBOEYP
 */
HWTEST_F(RSObjAbsGeometryTest, MapRectWithoutRounding003, TestSize.Level1)
{
    constexpr float srcRectLeft = 0.f;
    constexpr float srcRectTop = 0.f;
    constexpr float srcRectRight = 100.f;
    constexpr float srcRectBottom = 200.f;
    constexpr float expectDstRectLeft = 0.f;
    constexpr float expectDstRectTop = 0.f;
    constexpr float expectDstRectRight = 12.5f;
    constexpr float expectDstRectBottom = 100.f;

    RectF srcRect(srcRectLeft, srcRectTop, srcRectRight - srcRectLeft, srcRectBottom - srcRectTop);
    Drawing::Matrix matrix;
    matrix.Set(Drawing::Matrix::SCALE_X, 0.125f);
    matrix.Set(Drawing::Matrix::SCALE_Y, 0.5f);
    RectF dstRect = RSObjAbsGeometry::MapRectWithoutRounding(srcRect, matrix);
    EXPECT_TRUE(dstRect.GetLeft() == expectDstRectLeft);
    EXPECT_TRUE(dstRect.GetTop() == expectDstRectTop);
    EXPECT_TRUE(dstRect.GetRight() == expectDstRectRight);
    EXPECT_TRUE(dstRect.GetBottom() == expectDstRectBottom);
}

/**
 * @tc.name: MapRegion001
 * @tc.desc: Verify function MapRegion, expecting behavior: map with inflate.
 * @tc.type: FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSObjAbsGeometryTest, MapRegion001, TestSize.Level1)
{
    constexpr float scaleX = 0.125f;
    constexpr float scaleY = 0.5f;
    Drawing::Matrix matrix;
    matrix.Set(Drawing::Matrix::SCALE_X, scaleX);
    matrix.Set(Drawing::Matrix::SCALE_Y, scaleY);

    const RectI srcRect1 = { 0, 0, 100, 200 };
    const RectI srcRect2 = { 100, 200, 100, 100 };
    const RectI expectRect1 = RSObjAbsGeometry::MapRect(srcRect1.ConvertTo<float>(), matrix);
    const RectI expectRect2 = RSObjAbsGeometry::MapRect(srcRect2.ConvertTo<float>(), matrix);

    Occlusion::Region srcRegion;
    srcRegion.OrSelf(Occlusion::Region(Occlusion::Rect(srcRect1)));
    srcRegion.OrSelf(Occlusion::Region(Occlusion::Rect(srcRect2)));

    Occlusion::Region dstRegion = RSObjAbsGeometry::MapRegion(srcRegion, matrix);
    for (auto rect : dstRegion.GetRegionRectIs()) {
        EXPECT_TRUE(rect == expectRect1 || rect == expectRect2);
    }
}
} // namespace OHOS::Rosen