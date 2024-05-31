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

class RSObjAbsGeometryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSObjAbsGeometryTest::SetUpTestCase() {}
void RSObjAbsGeometryTest::TearDownTestCase() {}
void RSObjAbsGeometryTest::SetUp() {}
void RSObjAbsGeometryTest::TearDown() {}

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
    RSObjAbsGeometry rsObjAbsGeometry;
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
    float skewX = 0.5f;
    float skewY = 0.5f;
    float perspX = FLOAT_DATA_INIT;
    float perspY = FLOAT_DATA_INIT;
    rsObjAbsGeometry.SetRect(x, y, w, h);
    rsObjAbsGeometry.SetQuaternion(quaternion);
    rsObjAbsGeometry.SetRotationY(rotationY);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetRotationX(rotationX);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetTranslateZ(translateZ);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetCameraDistance(cameraDistance);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetPivotZ(pivotZ);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetScaleY(scaleY);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetScaleX(scaleX);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetSkewX(skewX);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetSkewY(skewY);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetPerspX(perspX);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetPerspY(perspY);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetQuaternion(quaternion.Flip());
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    scaleX = 1.f;
    scaleY = 1.f;
    skewX = 0.5f;
    skewY = 0.5f;
    perspX = FLOAT_DATA_UPDATE;
    perspY = FLOAT_DATA_UPDATE;
    rsObjAbsGeometry.SetScale(scaleX, scaleY);
    rsObjAbsGeometry.SetSkew(skewX, skewY);
    rsObjAbsGeometry.SetPersp(perspX, perspY);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
    rsObjAbsGeometry.SetContextMatrix(contextMatrix);
    rsObjAbsGeometry.UpdateMatrix(parent, std::nullopt);
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
    RSObjAbsGeometry rsObjAbsGeometry;
    Drawing::Matrix matrix;
    auto parent = &matrix;
    float offsetX = 0.f;
    float offsetY = 0.f;
    float x = 0.f;
    float y = 0.f;
    float w = 3.5f;
    float h = 3.5f;
    Drawing::Point offset = Drawing::Point(offsetX, offsetY);
    rsObjAbsGeometry.SetRect(x, y, w, h);
    rsObjAbsGeometry.UpdateMatrix(parent, offset);
    x = 0.5f;
    y = 0.5f;
    w = 1.5f;
    h = 1.5f;
    rsObjAbsGeometry.SetRect(x, y, w, h);
    rsObjAbsGeometry.UpdateMatrix(parent, offset);
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
    RSObjAbsGeometry rsObjAbsGeometry;
    Quaternion quaternion;
    Drawing::Matrix contextMatrix;
    float x = 0.5f;
    float y = 0.5f;
    float w = 0.5f;
    float h = 0.5f;
    float rotationX = 0.5f;
    float rotationY = 0.5f;
    float translateZ = 0.5f;
    rsObjAbsGeometry.SetRect(x, y, w, h);
    rsObjAbsGeometry.SetQuaternion(quaternion.Flip());
    rsObjAbsGeometry.UpdateByMatrixFromSelf();
    rsObjAbsGeometry.SetRotationY(rotationY);
    rsObjAbsGeometry.UpdateByMatrixFromSelf();
    rsObjAbsGeometry.SetRotationX(rotationX);
    rsObjAbsGeometry.UpdateByMatrixFromSelf();
    rsObjAbsGeometry.SetTranslateZ(translateZ);
    rsObjAbsGeometry.UpdateByMatrixFromSelf();
    rsObjAbsGeometry.SetContextMatrix(contextMatrix);
    rsObjAbsGeometry.UpdateByMatrixFromSelf();
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
    rsObjAbsGeometry->trans_ = std::make_optional<Transform>();
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
    rsObjAbsGeometry->trans_ = std::make_optional<Transform>();
    rsObjAbsGeometry->trans_->rotation_ = 0.5f;
    rsObjAbsGeometry->trans_->scaleX_ = 0.f;
    rsObjAbsGeometry->trans_->scaleY_ = 0.f;
    rsObjAbsGeometry->UpdateAbsMatrix2D();
    EXPECT_TRUE(rsObjAbsGeometry->trans_.has_value());
}
} // namespace OHOS::Rosen