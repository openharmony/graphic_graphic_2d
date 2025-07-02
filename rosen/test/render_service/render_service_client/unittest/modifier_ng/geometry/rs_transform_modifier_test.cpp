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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/geometry/rs_transform_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSTransformModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSTransformModifierTest
 * @tc.desc: Test Set/Get functions of RSTransformModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSTransformModifierNGTypeTest, RSTransformModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSTransformModifier> modifier = std::make_shared<ModifierNG::RSTransformModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::TRANSFORM);

    std::optional<Vector2f> sandBoxValue = std::make_optional<Vector2f>(Vector2f(1.0f, 2.0f));
    modifier->SetSandBox(sandBoxValue);
    EXPECT_EQ(modifier->GetSandBox(), sandBoxValue);

    float positionZValue = 1.0f;
    modifier->SetPositionZ(positionZValue);
    EXPECT_EQ(modifier->GetPositionZ(), positionZValue);

    bool positionZApplicableCamera3DValue = true;
    modifier->SetPositionZApplicableCamera3D(positionZApplicableCamera3DValue);
    EXPECT_EQ(modifier->GetPositionZApplicableCamera3D(), positionZApplicableCamera3DValue);

    Vector2f pivotValue(1.0f, 2.0f);
    modifier->SetPivot(pivotValue);
    EXPECT_EQ(modifier->GetPivot(), pivotValue);
    bool animatable = false;
    modifier->SetPivot(pivotValue, animatable);
    EXPECT_EQ(modifier->GetPivot(), pivotValue);

    float pivotZValue = 1.0f;
    modifier->SetPivotZ(pivotZValue);
    EXPECT_EQ(modifier->GetPivotZ(), pivotZValue);

    Quaternion quaternionValue(1.0f, 2.0f, 3.0f, 4.0f);
    modifier->SetQuaternion(quaternionValue);
    EXPECT_EQ(modifier->GetQuaternion(), quaternionValue);

    float cameraDistanceValue = 1.0f;
    modifier->SetCameraDistance(cameraDistanceValue);
    EXPECT_EQ(modifier->GetCameraDistance(), cameraDistanceValue);

    Vector2f translateValue(1.0f, 2.0f);
    modifier->SetTranslate(translateValue);
    EXPECT_EQ(modifier->GetTranslate(), translateValue);

    float translateZValue = 1.0f;
    modifier->SetTranslateZ(translateZValue);
    EXPECT_EQ(modifier->GetTranslateZ(), translateZValue);

    Vector2f scaleValue(1.0f, 2.0f);
    modifier->SetScale(scaleValue);
    EXPECT_EQ(modifier->GetScale(), scaleValue);

    float scaleZValue = 1.0f;
    modifier->SetScaleZ(scaleZValue);
    EXPECT_EQ(modifier->GetScaleZ(), scaleZValue);

    Vector3f skewValue(1.0f, 2.0f, 3.0f);
    modifier->SetSkew(skewValue);
    EXPECT_EQ(modifier->GetSkew(), skewValue);

    Vector4f perspValue(1.0f, 2.0f, 3.0f, 4.0f);
    modifier->SetPersp(perspValue);
    EXPECT_EQ(modifier->GetPersp(), perspValue);
}

/**
 * @tc.name: GetRotationTest
 * @tc.desc: test set and get functions of rotation properties
 * @tc.type: FUNC
 */
HWTEST_F(RSTransformModifierNGTypeTest, GetRotationTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSTransformModifier> modifier = std::make_shared<ModifierNG::RSTransformModifier>();

    float rotationValue = 1.0f;
    modifier->SetRotation(rotationValue);
    EXPECT_EQ(modifier->GetRotation(), rotationValue);

    float rotationXValue = 1.0f;
    modifier->SetRotationX(rotationXValue);
    EXPECT_EQ(modifier->GetRotationX(), rotationXValue);

    float rotationYValue = 1.0f;
    modifier->SetRotationY(rotationYValue);
    EXPECT_EQ(modifier->GetRotationY(), rotationYValue);
}

/**
 * @tc.name: ApplyGeometryTest
 * @tc.desc: Test the function ApplyGeometry
 * @tc.type: FUNC
 */
HWTEST_F(RSTransformModifierNGTypeTest, ApplyGeometryTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSTransformModifier>();
    std::shared_ptr<RSObjAbsGeometry> geometry = nullptr;
    modifier->ApplyGeometry(geometry);
    EXPECT_EQ(geometry, nullptr);

    // geometry is not null, but all properties is null
    geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);
    EXPECT_NE(geometry, nullptr);

    // geometry and property is not null
    // 1.0f 2.0f 3.0f 4.0f : incremental change initial value
    Vector2f pivotValue(1.0f, 2.0f);
    float pivotZValue = 1.0f;
    Quaternion quaternionValue(1.0f, 2.0f, 3.0f, 4.0f);
    float rotationValue = 1.0f;
    float rotationXValue = 1.0f;
    float rotationYValue = 1.0f;
    float cameraDistanceValue = 1.0f;
    Vector2f scaleValue(1.0f, 2.0f);
    float scaleZValue = 1.0f;
    Vector3f skewValue(1.0f, 2.0f, 3.0f);
    Vector4f perspValue(1.0f, 2.0f, 3.0f, 4.0f);
    Vector2f translateValue(1.0f, 2.0f);
    float translateZValue = 1.0f;
    modifier->SetPivot(pivotValue);
    modifier->SetPivotZ(pivotZValue);
    modifier->SetQuaternion(quaternionValue);
    modifier->SetRotation(rotationValue);
    modifier->SetRotationX(rotationXValue);
    modifier->SetRotationY(rotationYValue);
    modifier->SetCameraDistance(cameraDistanceValue);
    modifier->SetScale(scaleValue);
    modifier->SetScaleZ(scaleZValue);
    modifier->SetSkew(skewValue);
    modifier->SetPersp(perspValue);
    modifier->SetTranslate(translateValue);
    modifier->SetTranslateZ(translateZValue);
    modifier->ApplyGeometry(geometry);
    EXPECT_NE(geometry, nullptr);
}
} // namespace OHOS::Rosen