/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"
#include "common/rs_vector4.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_mask_base.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_render_node.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderPropertyTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, 2.0f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    PropertyId id = 100;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderPropertyTest::SetUpTestCase() {}
void RSRenderPropertyTest::TearDownTestCase() {}
void RSRenderPropertyTest::SetUp() {}
void RSRenderPropertyTest::TearDown() {}

static void GenRSMarshallingParcelHeader(Parcel& parcel)
{
    parcel.WriteInt32(0);
    RSMarshallingHelper::MarshallingTransactionVer(parcel);
    const auto headerLen = parcel.GetWritePosition();
    parcel.SkipBytes(headerLen);
}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyTest, LifeCycle001, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<float>>();
    ASSERT_TRUE(prop != nullptr);
    ASSERT_TRUE(prop->GetId() == 0);

    auto prop2 = std::make_shared<RSRenderProperty<float>>(floatData[0], id);
    ASSERT_TRUE(prop2 != nullptr);
    ASSERT_EQ(prop2->GetId(), id);
}

/**
 * @tc.name: Property001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyTest, Property001, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<float>>(floatData[0], id);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), floatData[0]);

    prop->Set(floatData[1]);
    ASSERT_EQ(prop->Get(), floatData[1]);
}

/**
 * @tc.name: PropertyOp001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyTest, PropertyOp001, TestSize.Level1)
{
    std::shared_ptr<RSRenderAnimatableProperty<float>> prop1 = nullptr;
    auto prop2 = std::make_shared<RSRenderAnimatableProperty<float>>(floatData[0], id);
    auto prop3 = std::make_shared<RSRenderAnimatableProperty<float>>(floatData[1], id + 1);

    prop1 += prop3;
    ASSERT_EQ(prop1, nullptr);

    prop2 += prop3;
    ASSERT_TRUE(ROSEN_EQ(prop2->Get(), floatData[0] + floatData[1]));

    prop1 -= prop3;
    ASSERT_EQ(prop1, nullptr);

    prop2 -= prop3;
    ASSERT_TRUE(ROSEN_EQ(prop2->Get(), floatData[0]));

    ASSERT_EQ(prop1 + prop3, nullptr);
    ASSERT_TRUE(ROSEN_EQ(
        std::static_pointer_cast<RSRenderProperty<float>>(prop2 + prop3)->Get(), floatData[0] + floatData[1]));

    ASSERT_EQ(prop1 - prop3, nullptr);
    ASSERT_TRUE(ROSEN_EQ(
        std::static_pointer_cast<RSRenderProperty<float>>(prop2 - prop3)->Get(), floatData[0] - floatData[1]));

    ASSERT_EQ(prop1 * floatData[1], nullptr);
    ASSERT_TRUE(ROSEN_EQ(
        std::static_pointer_cast<RSRenderProperty<float>>(prop2 * floatData[1])->Get(), floatData[0] * floatData[1]));

    ASSERT_FALSE(prop1 == prop3);
    ASSERT_TRUE(prop1 != prop3);

    ASSERT_FALSE(prop2 == prop3);
    ASSERT_TRUE(prop2 != prop3);
}

template<typename T>
class MockRSRenderProperty : public RSRenderProperty<T> {
public:
    explicit MockRSRenderProperty() : RSRenderProperty<T>() {}
    virtual ~MockRSRenderProperty() = default;
};

template<typename T>
class MockRSRenderAnimatableProperty : public RSRenderAnimatableProperty<T> {
public:
    explicit MockRSRenderAnimatableProperty(const T& value) : RSRenderAnimatableProperty<T>(value) {}
    explicit MockRSRenderAnimatableProperty() : RSRenderAnimatableProperty<T>() {}
    virtual ~MockRSRenderAnimatableProperty() = default;
    float ToFloat() const
    {
        return RSRenderAnimatableProperty<T>::ToFloat();
    }
};
/**
 * @tc.name: PropertyOp002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyTest, PropertyOp002, TestSize.Level1)
{
    auto prop0 = std::make_shared<MockRSRenderAnimatableProperty<float>>(floatData[0]);
    ASSERT_TRUE(ROSEN_EQ(prop0->ToFloat(), floatData[0]));

    auto prop1 = std::make_shared<MockRSRenderAnimatableProperty<Vector4f>>(Vector4f(floatData[1]));
    ASSERT_TRUE(ROSEN_EQ(prop1->ToFloat(), 2 * floatData[1]));

    auto prop2 = std::make_shared<MockRSRenderAnimatableProperty<Vector2f>>(Vector2f(floatData[2], 0.f));
    ASSERT_TRUE(ROSEN_EQ(prop2->ToFloat(), floatData[2]));

    auto prop3 = std::make_shared<MockRSRenderAnimatableProperty<Quaternion>>(Quaternion());
    ASSERT_TRUE(ROSEN_EQ(prop3->ToFloat(), 1.f));
}

/**
 * @tc.name: PropertyIPC001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyTest, PropertyIPC001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> props;
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<float>>());
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Color>>());
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Matrix3f>>());
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Quaternion>>());
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<std::vector<float>>>());
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Vector2f>>());
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Vector3f>>());
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Vector4f>>());
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Vector4<Color>>>());

    for (auto& prop : props) {
        MessageParcel parcel;
        ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, prop));
        ASSERT_TRUE(RSRenderPropertyBase::Unmarshalling(parcel, prop));
    }

    MessageParcel parcel1;
    auto intProp = std::make_shared<RSRenderProperty<int>>();
    std::shared_ptr<RSRenderPropertyBase> tmpProp;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel1, intProp));
    ASSERT_TRUE(RSRenderPropertyBase::Unmarshalling(parcel1, tmpProp));

    MessageParcel parcel2;
    int data = 0;
    parcel2.ParseFrom(data, sizeof(int));
    ASSERT_FALSE(RSMarshallingHelper::Marshalling(parcel2, intProp));
}

/**
 * @tc.name: PropertyIPC002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyTest, PropertyIPC002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> props;
    props.push_back(std::make_shared<MockRSRenderProperty<float>>());
    props.push_back(std::make_shared<MockRSRenderProperty<Color>>());
    props.push_back(std::make_shared<MockRSRenderProperty<Matrix3f>>());
    props.push_back(std::make_shared<MockRSRenderProperty<Quaternion>>());
    props.push_back(std::make_shared<MockRSRenderProperty<Vector2f>>());
    props.push_back(std::make_shared<MockRSRenderProperty<Vector3f>>());
    props.push_back(std::make_shared<MockRSRenderProperty<Vector4f>>());
    props.push_back(std::make_shared<MockRSRenderProperty<Vector4<Color>>>());
    props.push_back(std::make_shared<MockRSRenderProperty<std::shared_ptr<Drawing::DrawCmdList>>>());

    for (auto& prop : props) {
        MessageParcel parcel;
        ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, prop));
        ASSERT_TRUE(RSRenderPropertyBase::Unmarshalling(parcel, prop));
    }
}

/**
 * @tc.name: IsNearEqual
 * @tc.desc: Test IsNearEqual
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, IsNearEqual, TestSize.Level1)
{
    RSRenderAnimatableProperty<float> property;
    Vector2f vector2f(0, 0);
    RSRenderAnimatableProperty<Vector2f> propertyTwo(vector2f);
    Matrix3f matrix3f;
    RSRenderAnimatableProperty<Matrix3f> propertyMatrix3f(matrix3f);
    Color color;
    RSRenderAnimatableProperty<Color> propertyColor(color);
    Vector4<Color> vector4;
    RSRenderAnimatableProperty<Vector4<Color>> propertyVector4Color(vector4);
    RRect rect;
    RSRenderAnimatableProperty<RRect> propertyRect(rect);

    std::shared_ptr<RSRenderPropertyBase> value;
    EXPECT_TRUE(property.IsNearEqual(value, 1.f));
    EXPECT_TRUE(propertyTwo.IsNearEqual(value, 1.f));
    EXPECT_TRUE(propertyMatrix3f.IsNearEqual(value, 1.f));
    EXPECT_TRUE(propertyColor.IsNearEqual(value, 1.f));
    EXPECT_TRUE(propertyVector4Color.IsNearEqual(value, 1.f));
    EXPECT_TRUE(propertyRect.IsNearEqual(value, 1.f));

    std::shared_ptr<RSRenderProperty<float>> floatValue = std::make_shared<RSRenderProperty<float>>();
    float float1 = 0.0;
    floatValue->Set(float1);
    std::shared_ptr<RSRenderProperty<Vector2f>> vector2fValue = std::make_shared<RSRenderProperty<Vector2f>>();
    Vector2f vector2f1(0, 0);
    vector2fValue->Set(vector2f1);
    std::shared_ptr<RSRenderProperty<Matrix3f>> matrix3fValue = std::make_shared<RSRenderProperty<Matrix3f>>();
    Matrix3f matrix3f1;
    matrix3fValue->Set(matrix3f1);
    std::shared_ptr<RSRenderProperty<Color>> colorValue = std::make_shared<RSRenderProperty<Color>>();
    Color color1;
    colorValue->Set(color1);
    std::shared_ptr<RSRenderProperty<Vector4<Color>>> vector4Value =
        std::make_shared<RSRenderProperty<Vector4<Color>>>();
    Vector4<Color> vector41;
    vector4Value->Set(vector41);
    std::shared_ptr<RSRenderProperty<RRect>> rectValue = std::make_shared<RSRenderProperty<RRect>>();
    RRect rect1;
    rectValue->Set(rect1);
    EXPECT_TRUE(property.IsNearEqual(floatValue, 1.f));
    EXPECT_TRUE(propertyTwo.IsNearEqual(vector2fValue, 1.f));
    EXPECT_TRUE(propertyMatrix3f.IsNearEqual(matrix3fValue, 1.f));
    EXPECT_TRUE(propertyColor.IsNearEqual(colorValue, 1.f));
    EXPECT_TRUE(propertyVector4Color.IsNearEqual(vector4Value, 1.f));
    EXPECT_TRUE(propertyRect.IsNearEqual(rectValue, 1.f));
}

/**
 * @tc.name: OnChange001
 * @tc.desc: Test OnChange
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, OnChange001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPropertyBase> base = std::make_shared<RSRenderProperty<bool>>();
    Parcel parcel;
    std::vector<std::shared_ptr<RSRenderPropertyBase>> props;
    props.push_back(std::make_shared<MockRSRenderProperty<Color>>());
    
    const std::shared_ptr<RSRenderPropertyBase> val = nullptr;
    bool ret = RSMarshallingHelper::Marshalling(parcel, val);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: OnChange002
 * @tc.desc: Test OnChange
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, OnChange002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPropertyBase> base = std::make_shared<RSRenderProperty<bool>>();
    Parcel parcel;
    std::vector<std::shared_ptr<RSRenderPropertyBase>> props;
    props.push_back(std::make_shared<MockRSRenderProperty<Color>>());
    for (auto& prop : props) {
        MessageParcel parcel;
        ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, prop));
        ASSERT_TRUE(RSRenderPropertyBase::Unmarshalling(parcel, prop));
    }
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: Test IsNearEqual
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, IsNearEqual001, TestSize.Level1)
{
    RSRenderAnimatableProperty<float> property1;
    RSRenderAnimatableProperty<Vector2f> property2;
    RSRenderAnimatableProperty<Vector3f> property3f;
    RSRenderAnimatableProperty<Quaternion> property3;
    RSRenderAnimatableProperty<Vector4f> property4;
    RSRenderAnimatableProperty<Matrix3f> property5;
    RSRenderAnimatableProperty<Color> property6;
    RSRenderAnimatableProperty<Vector4<Color>> property7;
    RSRenderAnimatableProperty<RRect> property8;
    const std::shared_ptr<RSRenderPropertyBase> value = nullptr;
    float zeroThreshold = 0.0;
    ASSERT_TRUE(property1.IsNearEqual(value, zeroThreshold));
    ASSERT_TRUE(property2.IsNearEqual(value, zeroThreshold));
    ASSERT_TRUE(property3f.IsNearEqual(value, zeroThreshold));
    ASSERT_TRUE(property3.IsNearEqual(value, zeroThreshold));
    ASSERT_TRUE(property4.IsNearEqual(value, zeroThreshold));
    ASSERT_TRUE(property5.IsNearEqual(value, zeroThreshold));
    ASSERT_TRUE(property6.IsNearEqual(value, zeroThreshold));
    ASSERT_TRUE(property7.IsNearEqual(value, zeroThreshold));
    ASSERT_TRUE(property8.IsNearEqual(value, zeroThreshold));
}

/**
 * @tc.name: IsNearEqual002
 * @tc.desc: Test IsNearEqual
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, IsNearEqual002, TestSize.Level1)
{
    RSRenderAnimatableProperty<float> property1;
    Vector2f vector2f;
    RSRenderAnimatableProperty<Vector2f> property2(vector2f);
    Quaternion quaternion;
    RSRenderAnimatableProperty<Quaternion> property3(quaternion);
    Vector4f vector4;
    RSRenderAnimatableProperty<Vector4f> property4(vector4);
    Matrix3f matrix3f;
    RSRenderAnimatableProperty<Matrix3f> property5(matrix3f);
    Color color;
    RSRenderAnimatableProperty<Color> property6(color);
    Vector4<Color> vectorColor;
    RSRenderAnimatableProperty<Vector4<Color>> property7(vectorColor);
    RRect rect;
    RSRenderAnimatableProperty<RRect> property8(rect);
    float zeroThreshold = 0.0001;

    std::shared_ptr<RSRenderProperty<float>> floatValue = std::make_shared<RSRenderProperty<float>>();
    floatValue->Set(0.00001f);
    std::shared_ptr<RSRenderProperty<Vector2f>> vector2fValue = std::make_shared<RSRenderProperty<Vector2f>>();
    vector2fValue->Set(vector2f);
    std::shared_ptr<RSRenderProperty<Quaternion>> quaternionValue = std::make_shared<RSRenderProperty<Quaternion>>();
    quaternionValue->Set(quaternion);
    std::shared_ptr<RSRenderProperty<Vector4f>> vector4Value = std::make_shared<RSRenderProperty<Vector4f>>();
    vector4Value->Set(vector4);
    std::shared_ptr<RSRenderProperty<Matrix3f>> matrix3fValue = std::make_shared<RSRenderProperty<Matrix3f>>();
    matrix3fValue->Set(matrix3f);
    std::shared_ptr<RSRenderProperty<Color>> colorValue = std::make_shared<RSRenderProperty<Color>>();
    colorValue->Set(color);
    std::shared_ptr<RSRenderProperty<Vector4<Color>>> vector4ColorValue =
        std::make_shared<RSRenderProperty<Vector4<Color>>>();
    vector4ColorValue->Set(vectorColor);
    std::shared_ptr<RSRenderProperty<RRect>> rectValue = std::make_shared<RSRenderProperty<RRect>>();
    rectValue->Set(rect);

    EXPECT_TRUE(property1.IsNearEqual(floatValue, zeroThreshold));
    EXPECT_TRUE(property2.IsNearEqual(vector2fValue, zeroThreshold));
    EXPECT_TRUE(property3.IsNearEqual(quaternionValue, zeroThreshold));
    EXPECT_TRUE(property4.IsNearEqual(vector4Value, zeroThreshold));
    EXPECT_TRUE(property5.IsNearEqual(matrix3fValue, zeroThreshold));
    EXPECT_TRUE(property6.IsNearEqual(colorValue, zeroThreshold));
    EXPECT_TRUE(property7.IsNearEqual(vector4ColorValue, zeroThreshold));
    EXPECT_TRUE(property8.IsNearEqual(rectValue, zeroThreshold));
}

/**
 * @tc.name: IsNearEqual003
 * @tc.desc: Test IsNearEqual
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, IsNearEqual003, TestSize.Level1)
{
    Vector3f vector3f;
    RSRenderAnimatableProperty<Vector3f> property3f(vector3f);
    std::shared_ptr<RSRenderProperty<Vector3f>> vector3fValue = std::make_shared<RSRenderProperty<Vector3f>>();
    vector3fValue->Set(vector3f);
    EXPECT_TRUE(property3f.IsNearEqual(vector3fValue, 1e-4));
}

/**
 * @tc.name: dumptest
 * @tc.desc: Test dump
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, dumptest, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSRenderProperty<Vector3f>>();
    prop1->Set(Vector3f(1.f, 1.f, 1.f)); // 1.f for test
    std::string dumpOut1;
    prop1->Dump(dumpOut1);
    EXPECT_EQ(dumpOut1, std::string("[x:1.0 y:1.0 z:1.0]"));

    auto prop2 = std::make_shared<RSRenderProperty<Vector4f>>();
    prop2->Set(Vector4f(1.5f, 1.5f, 10.2f, 10.2f)); // 1.5f and 10.2f for test
    std::string dumpOut2;
    prop2->Dump(dumpOut2);
    EXPECT_EQ(dumpOut2, std::string("[x:1.5 y:1.5 z:10.2 w:10.2]"));
}

/**
 * @tc.name: tofloattest
 * @tc.desc: Test toFloat
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, tofloattest, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector3f> property3f(Vector3f(1.f, 1.f, 1.f)); // 1.f for test
    EXPECT_NEAR(property3f.ToFloat(), 1.73205f, 1e-4);                        // 1.73205.f mod result
}

/**
 * @tc.name: DepthCameraParaDump
 * @tc.desc: Test dump for RSRenderProperty<DepthCameraPara>
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, DepthCameraParaDump, TestSize.Level1)
{
    DepthCameraPara para;
    auto prop = std::make_shared<RSRenderProperty<DepthCameraPara>>();
    std::string out;
    prop->Dump(out);
    EXPECT_EQ(out, "[position: (0.0, 0.0, 0.0), quaternion: (0.0, 0.0, 0.0, 0.0), yFov: 0.0, zNear: 0.1, zFar: 100.0, "
        "offset: (0.0, 0.0)]");
}

/**
 * @tc.name: DepthLightParaDump
 * @tc.desc: Test dump for RSRenderProperty<DepthLightPara>
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, DepthLightParaDump, TestSize.Level1)
{
    DepthLightPara para;
    auto prop = std::make_shared<RSRenderProperty<DepthLightPara>>();
    std::string out;
    prop->Dump(out);
    EXPECT_EQ(out, "[direction: (0.0, 0.0, 0.0), color: (0.0, 0.0, 0.0), intensity: 0.0]");
}

/**
 * @tc.name: RSRenderPropertyBoolOnUnmarshalling
 * @tc.desc: RSRenderPropertyBool On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyBoolOnUnmarshalling, TestSize.Level1)
{
    bool value = false;
    std::shared_ptr<RSRenderProperty<bool>> prop = std::make_shared<RSRenderProperty<bool>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<bool>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<bool>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<bool>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertyIntOnUnmarshalling
 * @tc.desc: RSRenderPropertyInt On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyIntOnUnmarshalling, TestSize.Level1)
{
    int value = 1;
    std::shared_ptr<RSRenderProperty<int>> prop = std::make_shared<RSRenderProperty<int>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<int>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<int>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<int>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertyVector4uint32_tOnUnmarshalling
 * @tc.desc: RSRenderPropertyVector4uint32_t On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyVector4uint32_tOnUnmarshalling, TestSize.Level1)
{
    Vector4<uint32_t> value = { 1, 1, 1, 1 };
    std::shared_ptr<RSRenderProperty<Vector4<uint32_t>>> prop =
        std::make_shared<RSRenderProperty<Vector4<uint32_t>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<Vector4<uint32_t>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<Vector4<uint32_t>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<Vector4<uint32_t>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertyDrawingDrawCmdListPtrOnUnmarshalling
 * @tc.desc: RSRenderPropertyDrawingDrawCmdListPtr On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyDrawingDrawCmdListPtrOnUnmarshalling, TestSize.Level1)
{
    Drawing::DrawCmdListPtr value = {};
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> prop =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<Drawing::DrawCmdListPtr>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<Drawing::DrawCmdListPtr>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<Drawing::DrawCmdListPtr>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrRSShaderOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrRSShader On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrRSShaderOnUnmarshalling, TestSize.Level1)
{
    auto value = std::make_shared<RSShader>();
    std::shared_ptr<RSRenderProperty<std::shared_ptr<RSShader>>> prop =
        std::make_shared<RSRenderProperty<std::shared_ptr<RSShader>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<RSShader>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSShader>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSShader>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrRSImageOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrRSImage On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrRSImageOnUnmarshalling, TestSize.Level1)
{
    auto value = std::make_shared<RSImage>();
    std::shared_ptr<RSRenderProperty<std::shared_ptr<RSImage>>> prop =
        std::make_shared<RSRenderProperty<std::shared_ptr<RSImage>>>(value, 1);

    Parcel parcel;
    GenRSMarshallingParcelHeader(parcel);
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<RSImage>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    parcel.FlushBuffer();
    GenRSMarshallingParcelHeader(parcel);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSImage>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    parcel.FlushBuffer();
    GenRSMarshallingParcelHeader(parcel);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSImage>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);
    EXPECT_FALSE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrRSPathOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrRSPath On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrRSPathOnUnmarshalling, TestSize.Level1)
{
    auto value = std::make_shared<RSPath>();
    std::shared_ptr<RSRenderProperty<std::shared_ptr<RSPath>>> prop =
        std::make_shared<RSRenderProperty<std::shared_ptr<RSPath>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<RSPath>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSPath>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSPath>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertyGravityOnUnmarshalling
 * @tc.desc: RSRenderPropertyGravity On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyGravityOnUnmarshalling, TestSize.Level1)
{
    Gravity value = {};
    std::shared_ptr<RSRenderProperty<Gravity>> prop = std::make_shared<RSRenderProperty<Gravity>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<Gravity>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<Gravity>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<Gravity>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrMotionBlurParamOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrMotionBlurParam On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrMotionBlurParamOnUnmarshalling, TestSize.Level1)
{
    float radius = 0.f;
    Vector2f scaleAnchor = Vector2f(0.f, 0.f);
    auto value = std::make_shared<MotionBlurParam>(radius, scaleAnchor);
    std::shared_ptr<RSRenderProperty<std::shared_ptr<MotionBlurParam>>> prop =
        std::make_shared<RSRenderProperty<std::shared_ptr<MotionBlurParam>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<MotionBlurParam>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<MotionBlurParam>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<MotionBlurParam>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrEmitterUpdaterOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrEmitterUpdater On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrEmitterUpdaterOnUnmarshalling, TestSize.Level1)
{
    std::vector<std::shared_ptr<EmitterUpdater>> value;
    auto item = std::make_shared<EmitterUpdater>(0);
    value.push_back(item);
    std::shared_ptr<RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>> prop =
        std::make_shared<RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrParticleNoiseFieldsOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrParticleNoiseFields On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrParticleNoiseFieldsOnUnmarshalling, TestSize.Level1)
{
    auto field = std::make_shared<ParticleNoiseField>(
        0, ShapeType::RECT, Vector2f(1.f, 1.f), Vector2f(1.f, 1.f), 0, 0.f, 0.f, 0.f);
    auto value = std::make_shared<ParticleNoiseFields>();
    value->AddField(field);
    std::shared_ptr<RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>> prop =
        std::make_shared<RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    parcel.WriteInt32(100);
    parcel.WriteUint32(100000);
    parcel.WriteUint32(100000);
    ret = RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrRSMaskOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrRSMask On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrRSMaskOnUnmarshalling, TestSize.Level1)
{
    auto value = std::make_shared<RSMask>();
    std::shared_ptr<RSRenderProperty<std::shared_ptr<RSMask>>> prop =
        std::make_shared<RSRenderProperty<std::shared_ptr<RSMask>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<RSMask>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSMask>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSMask>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertyRSWaterRippleParaOnUnmarshalling
 * @tc.desc: RSRenderPropertyRSWaterRipplePara On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyRSWaterRippleParaOnUnmarshalling, TestSize.Level1)
{
    RSWaterRipplePara value = {};
    std::shared_ptr<RSRenderProperty<RSWaterRipplePara>> prop =
        std::make_shared<RSRenderProperty<RSWaterRipplePara>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<RSWaterRipplePara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<RSWaterRipplePara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<RSWaterRipplePara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertyRSFlyOutParaOnUnmarshalling
 * @tc.desc: RSRenderPropertyRSFlyOutPara On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyRSFlyOutParaOnUnmarshalling, TestSize.Level1)
{
    RSFlyOutPara value = {};
    std::shared_ptr<RSRenderProperty<RSFlyOutPara>> prop = std::make_shared<RSRenderProperty<RSFlyOutPara>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<RSFlyOutPara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<RSFlyOutPara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<RSFlyOutPara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrPixelMapOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrPixelMap On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrPixelMapOnUnmarshalling, TestSize.Level1)
{
    std::shared_ptr<OHOS::Media::PixelMap> value;
    OHOS::Media::InitializationOptions opts;
    opts.size.width = 100;
    opts.size.height = 100;
    opts.editable = true;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = OHOS::Media::PixelMap::Create(opts);
    value.reset(pixelMap.get());
    pixelMap.release();

    std::shared_ptr<RSRenderProperty<std::shared_ptr<OHOS::Media::PixelMap>>> prop =
        std::make_shared<RSRenderProperty<std::shared_ptr<OHOS::Media::PixelMap>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<OHOS::Media::PixelMap>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<OHOS::Media::PixelMap>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<OHOS::Media::PixelMap>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertyRSDynamicBrightnessParaOnUnmarshalling
 * @tc.desc: RSRenderPropertyRSDynamicBrightnessPara On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyRSDynamicBrightnessParaOnUnmarshalling, TestSize.Level1)
{
    RSDynamicBrightnessPara value = {};
    std::shared_ptr<RSRenderProperty<RSDynamicBrightnessPara>> prop =
        std::make_shared<RSRenderProperty<RSDynamicBrightnessPara>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<RSDynamicBrightnessPara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<RSDynamicBrightnessPara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<RSDynamicBrightnessPara>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyFloatOnUnmarshalling
 * @tc.desc: AnimatablePropertyFloat On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyFloatOnUnmarshalling, TestSize.Level1)
{
    float value = {};
    std::shared_ptr<RSRenderAnimatableProperty<float>> prop =
        std::make_shared<RSRenderAnimatableProperty<float>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<float>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<float>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<float>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<float>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyQuaternionOnUnmarshalling
 * @tc.desc: AnimatablePropertyQuaternion On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyQuaternionOnUnmarshalling, TestSize.Level1)
{
    Quaternion value = {};
    std::shared_ptr<RSRenderAnimatableProperty<Quaternion>> prop =
        std::make_shared<RSRenderAnimatableProperty<Quaternion>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<Quaternion>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Quaternion>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Quaternion>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Quaternion>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyVector2fOnUnmarshalling
 * @tc.desc: AnimatablePropertyVector2f On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyVector2fOnUnmarshalling, TestSize.Level1)
{
    Vector2f value = {};
    std::shared_ptr<RSRenderAnimatableProperty<Vector2f>> prop =
        std::make_shared<RSRenderAnimatableProperty<Vector2f>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<Vector2f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector2f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector2f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector2f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyVector3fOnUnmarshalling
 * @tc.desc: AnimatablePropertyVector3f On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyVector3fOnUnmarshalling, TestSize.Level1)
{
    Vector3f value = {};
    std::shared_ptr<RSRenderAnimatableProperty<Vector3f>> prop =
        std::make_shared<RSRenderAnimatableProperty<Vector3f>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<Vector3f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector3f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector3f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector3f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyVector4fOnUnmarshalling
 * @tc.desc: AnimatablePropertyVector4f On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyVector4fOnUnmarshalling, TestSize.Level1)
{
    Vector4f value = {};
    std::shared_ptr<RSRenderAnimatableProperty<Vector4f>> prop =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<Vector4f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector4f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector4f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector4f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyMatrix3fOnUnmarshalling
 * @tc.desc: AnimatablePropertyMatrix3f On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyMatrix3fOnUnmarshalling, TestSize.Level1)
{
    Matrix3f value = {};
    std::shared_ptr<RSRenderAnimatableProperty<Matrix3f>> prop =
        std::make_shared<RSRenderAnimatableProperty<Matrix3f>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<Matrix3f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Matrix3f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Matrix3f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Matrix3f>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyRRectOnUnmarshalling
 * @tc.desc: AnimatablePropertyRRect On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyRRectOnUnmarshalling, TestSize.Level1)
{
    RRect value = {};
    std::shared_ptr<RSRenderAnimatableProperty<RRect>> prop =
        std::make_shared<RSRenderAnimatableProperty<RRect>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<RRect>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<RRect>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<RRect>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<RRect>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyColorOnUnmarshalling
 * @tc.desc: AnimatablePropertyColor On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyColorOnUnmarshalling, TestSize.Level1)
{
    Color value = {};
    std::shared_ptr<RSRenderAnimatableProperty<Color>> prop =
        std::make_shared<RSRenderAnimatableProperty<Color>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<Color>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Color>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Color>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Color>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyVector4ColorOnUnmarshalling
 * @tc.desc: AnimatablePropertyVector4Color On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyVector4ColorOnUnmarshalling, TestSize.Level1)
{
    Vector4<Color> value = {};
    std::shared_ptr<RSRenderAnimatableProperty<Vector4<Color>>> prop =
        std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<Vector4<Color>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector4<Color>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector4<Color>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<Vector4<Color>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: AnimatablePropertyVectorFloatOnUnmarshalling
 * @tc.desc: AnimatablePropertyVectorFloat On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, AnimatablePropertyVectorFloatOnUnmarshalling, TestSize.Level1)
{
    std::vector<float> value = {};
    std::shared_ptr<RSRenderAnimatableProperty<std::vector<float>>> prop =
        std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderAnimatableProperty<std::vector<float>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<std::vector<float>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<std::vector<float>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetPropertyUnit());
    EXPECT_TRUE(ret);
    ret = RSRenderAnimatableProperty<std::vector<float>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSNGRenderMaskBaseSetOnlyValue
 * @tc.desc: Test Set for RSRenderProperty<std::shared_ptr<RSNGRenderMaskBase>>
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, RSNGRenderMaskBaseSetOnlyValue, TestSize.Level1)
{
    auto nullProp = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderMaskBase>>>();
    nullProp->Set(nullptr, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(nullProp->stagingValue_, nullptr);
    
    auto testShader = std::make_shared<RSNGRenderRippleMask>();
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderMaskBase>>>();
    prop->Set(testShader, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, testShader);

    prop->Set(testShader, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, testShader);
}

/**
 * @tc.name: RSNGRenderShaderBaseSetOnlyValue
 * @tc.desc: Test Set for RSRenderProperty<std::shared_ptr<RSNGRenderShaderBase>>
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, RSNGRenderShaderBaseSetOnlyValue, TestSize.Level1)
{
    auto nullProp = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShaderBase>>>();
    nullProp->Set(nullptr, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(nullProp->stagingValue_, nullptr);
    
    auto testShader = std::make_shared<RSNGRenderContourDiagonalFlowLight>();
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShaderBase>>>();
    prop->Set(testShader, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, testShader);

    prop->Set(testShader, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, testShader);
}

/**
 * @tc.name: RSNGRenderFilterBaseSetOnlyValue
 * @tc.desc: Test Set for RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, RSNGRenderFilterBaseSetOnlyValue, TestSize.Level1)
{
    auto nullProp = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>();
    nullProp->Set(nullptr, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(nullProp->stagingValue_, nullptr);
    
    auto testShader = std::make_shared<RSNGRenderBlurFilter>();
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>();
    prop->Set(testShader, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, testShader);

    prop->Set(testShader, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, testShader);
}

/**
 * @tc.name: RSNGRenderShapeBaseSetOnlyValue
 * @tc.desc: Test Set for RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, RSNGRenderShapeBaseSetOnlyValue, TestSize.Level1)
{
    auto nullProp = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>();
    nullProp->Set(nullptr, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(nullProp->stagingValue_, nullptr);
    
    auto testShader = std::make_shared<RSNGRenderSDFRRectShape>();
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>();
    prop->Set(testShader, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, testShader);

    prop->Set(testShader, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, testShader);
}

// ----------------- SDFShape ownerId lifecycle (owner on shape object) -----------------
// Invariants under test: Attach/SetOwnerId are paired; only the owner property
// (ownerId_ == this) cascades Detach on OnDetach/Set(DETACHATTACH); borrowers using
// ONLY_VALUE never Attach/SetOwnerId and thus never cascade. Sub-properties of an
// SDF_RRECT_SHAPE are registered with id 0, so node->GetProperty(0) observes the
// cascade effect (registered after Attach, gone after owner Detach).

/**
 * @tc.name: ShapeOwnerIdDefaultAndAccessor
 * @tc.desc: ownerId_ defaults to 0 and round-trips via Set/Get
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeOwnerIdDefaultAndAccessor, TestSize.Level1)
{
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shape, nullptr);
    EXPECT_EQ(shape->GetOwnerId(), 0u);
    shape->SetOwnerId(0x1234u);
    EXPECT_EQ(shape->GetOwnerId(), 0x1234u);
}

/**
 * @tc.name: ShapeOnAttachSetsOwnerIdToSelf
 * @tc.desc: OnAttach records the attaching property's this as ownerId and registers sub-properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeOnAttachSetsOwnerIdToSelf, TestSize.Level1)
{
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shape, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shape, id);
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    EXPECT_EQ(shape->GetOwnerId(), reinterpret_cast<uintptr_t>(prop.get()));
    EXPECT_NE(node->GetProperty(0), nullptr); // sub-property registered
    EXPECT_NE(node->GetProperty(id), nullptr); // property itself registered
}

/**
 * @tc.name: ShapeOnAttachNullStagingValueNoOp
 * @tc.desc: OnAttach is a no-op when stagingValue_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeOnAttachNullStagingValueNoOp, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>();
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node); // stagingValue_ null, OnAttach must not touch any shape
    EXPECT_EQ(node->GetProperty(id), nullptr); // property itself still registered
}

/**
 * @tc.name: ShapeOnDetachCascadesWhenOwnerMatches
 * @tc.desc: OnDetach cascades Detach to the shape only when ownerId equals self
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeOnDetachCascadesWhenOwnerMatches, TestSize.Level1)
{
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shape, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shape, id);
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    ASSERT_NE(node->GetProperty(0), nullptr);
    prop->Detach();
    EXPECT_EQ(node->GetProperty(0), nullptr); // cascade unregistered sub-property
}

/**
 * @tc.name: ShapeOnDetachSkipsWhenOwnerMismatch
 * @tc.desc: OnDetach skips cascade when ownerId differs from self, preserving sub-properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeOnDetachSkipsWhenOwnerMismatch, TestSize.Level1)
{
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shape, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shape, id);
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    shape->SetOwnerId(reinterpret_cast<uintptr_t>(prop.get()) + 1); // owned by another instance
    prop->Detach();
    EXPECT_NE(node->GetProperty(0), nullptr); // cascade skipped, sub-property preserved
}

/**
 * @tc.name: ShapeOnDetachNullStagingValueNoOp
 * @tc.desc: OnDetach is a no-op when stagingValue_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeOnDetachNullStagingValueNoOp, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>();
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    prop->Detach(); // no crash, no cascade
    EXPECT_EQ(node->GetProperty(id), nullptr);
}

/**
 * @tc.name: ShapeSetDetachAttachCascadesOldAndAttachesNew
 * @tc.desc: Set(OVERWRITE) detaches the old owner shape and attaches the new one
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeSetDetachAttachCascadesOldAndAttachesNew, TestSize.Level1)
{
    auto shapeA = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    auto shapeB = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shapeA, nullptr);
    ASSERT_NE(shapeB, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shapeA, id);
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    prop->Set(shapeB);
    EXPECT_EQ(prop->stagingValue_, shapeB);
    EXPECT_EQ(shapeB->GetOwnerId(), reinterpret_cast<uintptr_t>(prop.get()));
    EXPECT_NE(node->GetProperty(0), nullptr); // new shape sub-property registered
}

/**
 * @tc.name: ShapeSetDetachAttachSkipsCascadeWhenOwnerMismatch
 * @tc.desc: Set(OVERWRITE) skips detaching the old shape when its ownerId differs from self
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeSetDetachAttachSkipsCascadeWhenOwnerMismatch, TestSize.Level1)
{
    auto shapeA = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    auto shapeB = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shapeA, nullptr);
    ASSERT_NE(shapeB, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shapeA, id);
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    shapeA->SetOwnerId(reinterpret_cast<uintptr_t>(prop.get()) + 1); // not owned by this prop
    prop->Set(shapeB);
    EXPECT_EQ(prop->stagingValue_, shapeB);
    EXPECT_EQ(shapeB->GetOwnerId(), reinterpret_cast<uintptr_t>(prop.get()));
    EXPECT_NE(node->GetProperty(0), nullptr); // old shapeA sub-property kept (cascade skipped)
}

/**
 * @tc.name: ShapeSetDetachAttachNoDetachWhenStagingValueNull
 * @tc.desc: Set(OVERWRITE) does not detach when stagingValue_ is null, then attaches the new value
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeSetDetachAttachNoDetachWhenStagingValueNull, TestSize.Level1)
{
    auto shapeB = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shapeB, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>();
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node); // stagingValue_ null, OnAttach no-op
    prop->Set(shapeB);
    EXPECT_EQ(prop->stagingValue_, shapeB);
    EXPECT_EQ(shapeB->GetOwnerId(), reinterpret_cast<uintptr_t>(prop.get()));
    EXPECT_NE(node->GetProperty(0), nullptr);
}

/**
 * @tc.name: ShapeSetDetachAttachNoDetachNoAttachWhenNodeNull
 * @tc.desc: Set(OVERWRITE) neither detaches nor attaches when the node is null
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeSetDetachAttachNoDetachNoAttachWhenNodeNull, TestSize.Level1)
{
    auto shapeA = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    auto shapeB = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shapeA, nullptr);
    ASSERT_NE(shapeB, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shapeA, id);
    prop->Set(shapeB); // never Attached, node_ null
    EXPECT_EQ(prop->stagingValue_, shapeB);
    EXPECT_EQ(shapeB->GetOwnerId(), 0u); // SetOwnerId not called
}

/**
 * @tc.name: ShapeSetDetachAttachNoAttachWhenValueNull
 * @tc.desc: Set(OVERWRITE) detaches the old shape but does not attach a null value
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeSetDetachAttachNoAttachWhenValueNull, TestSize.Level1)
{
    auto shapeA = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shapeA, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shapeA, id);
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    ASSERT_NE(node->GetProperty(0), nullptr);
    prop->Set(nullptr);
    EXPECT_EQ(prop->stagingValue_, nullptr);
    EXPECT_EQ(node->GetProperty(0), nullptr); // old shapeA sub-property unregistered
}

/**
 * @tc.name: ShapeSetEarlyReturnWhenValueEqualsStaging
 * @tc.desc: Set returns early when value equals stagingValue_, leaving ownerId untouched
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeSetEarlyReturnWhenValueEqualsStaging, TestSize.Level1)
{
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shape, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shape, id);
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    auto ownerBefore = shape->GetOwnerId();
    prop->Set(shape); // same value, early return
    EXPECT_EQ(prop->stagingValue_, shape);
    EXPECT_EQ(shape->GetOwnerId(), ownerBefore);
}

/**
 * @tc.name: ShapeSetOnlyValueEarlyReturn
 * @tc.desc: Set(ONLY_VALUE) updates stagingValue_ without attaching or setting ownerId
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeSetOnlyValueEarlyReturn, TestSize.Level1)
{
    auto shapeA = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    auto shapeB = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shapeA, nullptr);
    ASSERT_NE(shapeB, nullptr);
    auto prop = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shapeA, id);
    auto node = std::make_shared<RSRenderNode>(1);
    prop->Attach(*node);
    prop->Set(shapeB, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    EXPECT_EQ(prop->stagingValue_, shapeB);
    EXPECT_EQ(shapeB->GetOwnerId(), 0u); // borrower path, no SetOwnerId
    EXPECT_EQ(shapeA->GetOwnerId(), reinterpret_cast<uintptr_t>(prop.get())); // old owner unchanged
}

/**
 * @tc.name: ShapeBorrowerDetachPreservesOwnerSubProperties
 * @tc.desc: Borrower (ONLY_VALUE) Detach skips cascade and keeps owner's sub-properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, ShapeBorrowerDetachPreservesOwnerSubProperties, TestSize.Level1)
{
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(shape, nullptr);
    auto node = std::make_shared<RSRenderNode>(1);
    auto ownerProp = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>(shape, id);
    ownerProp->Attach(*node);
    ASSERT_EQ(shape->GetOwnerId(), reinterpret_cast<uintptr_t>(ownerProp.get()));
    ASSERT_NE(node->GetProperty(0), nullptr);
    // borrower shares the same shape via ONLY_VALUE: no Attach, no SetOwnerId
    auto borrowerProp = std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderShapeBase>>>();
    borrowerProp->Set(shape, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    borrowerProp->Detach(); // must skip cascade
    EXPECT_NE(node->GetProperty(0), nullptr); // owner sub-properties preserved
    EXPECT_EQ(shape->GetOwnerId(), reinterpret_cast<uintptr_t>(ownerProp.get()));
    ownerProp->Detach(); // owner now cascades
    EXPECT_EQ(node->GetProperty(0), nullptr);
}

/**
 * @tc.name: CastToPropertyOfTypeMatch001
 * @tc.desc: CastToPropertyOf returns valid pointer when type matches, nullptr when mismatch
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, CastToPropertyOfTypeMatch001, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<float>>(1.0f, id);
    auto matched = prop->CastToPropertyOf<float>(__func__);
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(matched->Get(), 1.0f);

    auto mismatched = prop->CastToPropertyOf<Color>(__func__);
    EXPECT_EQ(mismatched, nullptr);
}

/**
 * @tc.name: CastToAnimatablePropertyOfTypeMatch001
 * @tc.desc: CastToAnimatablePropertyOfType returns valid pointer when type matches, nullptr when mismatch
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, CastToAnimatablePropertyOfTypeMatch001, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, id);
    auto matched = prop->CastToAnimatablePropertyOf<float>(__func__);
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(matched->Get(), 1.0f);

    auto mismatched = prop->CastToAnimatablePropertyOf<Vector4f>(__func__);
    EXPECT_EQ(mismatched, nullptr);
}

/**
 * @tc.name: IsAbsNearEqualFloat001
 * @tc.desc: Test IsAbsNearEqual for float type with valid target and threshold
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualFloat001, TestSize.Level1)
{
    RSRenderAnimatableProperty<float> property(10.0f);
    auto target = std::make_shared<RSRenderAnimatableProperty<float>>(12.0f);
    auto threshold = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    // |10 - 12| = 2 <= |5| -> true
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    // |10 - 12| = 2 > |1| -> false
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // exact match
    auto sameTarget = std::make_shared<RSRenderAnimatableProperty<float>>(10.0f);
    auto zeroThreshold = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    EXPECT_TRUE(property.IsAbsNearEqual(sameTarget, zeroThreshold));
}

/**
 * @tc.name: IsAbsNearEqualFloat002
 * @tc.desc: Test IsAbsNearEqual for float with nullptr target/threshold
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualFloat002, TestSize.Level1)
{
    RSRenderAnimatableProperty<float> property(10.0f);
    auto validThreshold = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    // nullptr target -> false
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, validThreshold));

    // nullptr threshold -> false
    auto validTarget = std::make_shared<RSRenderAnimatableProperty<float>>(12.0f);
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(validTarget, nullThreshold));

    // both nullptr -> false
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, nullThreshold));
}

/**
 * @tc.name: IsAbsNearEqualVector2f001
 * @tc.desc: Test IsAbsNearEqual for Vector2f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualVector2f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector2f> property(Vector2f(10.0f, 20.0f));
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(12.0f, 18.0f));
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(5.0f, 5.0f));
    // |10-12|=2 <= 5, |20-18|=2 <= 5 -> true
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(1.0f, 1.0f));
    // |10-12|=2 > 1 -> false
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // nullptr target
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, threshold));

    // nullptr threshold
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(target, nullThreshold));
}

/**
 * @tc.name: IsAbsNearEqualVector3f001
 * @tc.desc: Test IsAbsNearEqual for Vector3f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualVector3f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector3f> property(Vector3f(10.0f, 20.0f, 30.0f));
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(Vector3f(12.0f, 18.0f, 33.0f));
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(Vector3f(5.0f, 5.0f, 5.0f));
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(Vector3f(1.0f, 1.0f, 1.0f));
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // nullptr target
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, threshold));

    // nullptr threshold
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(target, nullThreshold));
}

/**
 * @tc.name: IsAbsNearEqualVector4f001
 * @tc.desc: Test IsAbsNearEqual for Vector4f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualVector4f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector4f> property(Vector4f(10.0f, 20.0f, 30.0f, 40.0f));
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(12.0f, 18.0f, 33.0f, 38.0f));
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(5.0f, 5.0f, 5.0f, 5.0f));
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // nullptr target
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, threshold));

    // nullptr threshold
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(target, nullThreshold));
}

/**
 * @tc.name: IsAbsNearEqualQuaternion001
 * @tc.desc: Test IsAbsNearEqual for Quaternion type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualQuaternion001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Quaternion> property(Quaternion(10.0f, 20.0f, 30.0f, 40.0f));
    auto target = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion(12.0f, 18.0f, 33.0f, 38.0f));
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion(5.0f, 5.0f, 5.0f, 5.0f));
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion(1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // nullptr target
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, threshold));

    // nullptr threshold
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(target, nullThreshold));
}

/**
 * @tc.name: IsAbsNearEqualMatrix3f001
 * @tc.desc: Test IsAbsNearEqual for Matrix3f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualMatrix3f001, TestSize.Level1)
{
    Matrix3f propVal(10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f);
    RSRenderAnimatableProperty<Matrix3f> property(propVal);
    Matrix3f tgtVal(12.0f, 18.0f, 33.0f, 38.0f, 48.0f, 58.0f, 68.0f, 78.0f, 88.0f);
    auto target = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>(tgtVal);
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>(
        Matrix3f(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f));
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>(
        Matrix3f(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // nullptr target
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, threshold));

    // nullptr threshold
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(target, nullThreshold));
}

/**
 * @tc.name: IsAbsNearEqualColor001
 * @tc.desc: Test IsAbsNearEqual for Color type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualColor001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Color> property(Color(100, 100, 100, 100));
    auto target = std::make_shared<RSRenderAnimatableProperty<Color>>(Color(105, 95, 110, 90));
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Color>>(Color(10, 10, 10, 10));
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<Color>>(Color(2, 2, 2, 2));
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // nullptr target
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, threshold));

    // nullptr threshold
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(target, nullThreshold));
}

/**
 * @tc.name: IsAbsNearEqualColor002
 * @tc.desc: Test IsAbsNearEqual for Color with BT2020 cross-color-space conversion path
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualColor002, TestSize.Level1)
{
    // BT2020 vs SRGB: different colorspace triggers BT2020 conversion in RSColor::IsAbsNearEqual
    Color bt2020Color(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSRenderAnimatableProperty<Color> property(bt2020Color);
    Color srgbColor(128, 128, 128, 128, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto target = std::make_shared<RSRenderAnimatableProperty<Color>>(srgbColor);
    // large threshold -> close after conversion -> true
    Color thrColor(0.1f, 0.1f, 0.1f, 0.1f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Color>>(thrColor);
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    // zero threshold -> not exactly equal after conversion -> false
    Color zeroThr(0.0f, 0.0f, 0.0f, 0.0f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    auto zeroThreshold = std::make_shared<RSRenderAnimatableProperty<Color>>(zeroThr);
    EXPECT_FALSE(property.IsAbsNearEqual(target, zeroThreshold));
}

/**
 * @tc.name: IsAbsNearEqualVector4Color001
 * @tc.desc: Test IsAbsNearEqual for Vector4<Color> type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualVector4Color001, TestSize.Level1)
{
    Color baseColor(100, 100, 100, 100);
    Vector4<Color> val(baseColor, baseColor, baseColor, baseColor);
    RSRenderAnimatableProperty<Vector4<Color>> property(val);
    Color tgtColor(105, 95, 110, 90);
    Vector4<Color> tgtVal(tgtColor, tgtColor, tgtColor, tgtColor);
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(tgtVal);
    Color thrColor(10, 10, 10, 10);
    Vector4<Color> thrVal(thrColor, thrColor, thrColor, thrColor);
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(thrVal);
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    Color smallThrColor(2, 2, 2, 2);
    Vector4<Color> smallThrVal(smallThrColor, smallThrColor, smallThrColor, smallThrColor);
    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(smallThrVal);
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // nullptr target
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, threshold));

    // nullptr threshold
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(target, nullThreshold));
}

/**
 * @tc.name: IsAbsNearEqualVector4Color002
 * @tc.desc: Test IsAbsNearEqual for Vector4<Color> with BT2020 cross-color-space path
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualVector4Color002, TestSize.Level1)
{
    Color bt2020Color(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    Vector4<Color> bt2020Val(bt2020Color, bt2020Color, bt2020Color, bt2020Color);
    RSRenderAnimatableProperty<Vector4<Color>> property(bt2020Val);
    Color srgbColor(128, 128, 128, 128, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    Vector4<Color> srgbVal(srgbColor, srgbColor, srgbColor, srgbColor);
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(srgbVal);
    Color thrColor(0.1f, 0.1f, 0.1f, 0.1f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    Vector4<Color> thrVal(thrColor, thrColor, thrColor, thrColor);
    auto threshold = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(thrVal);
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    Color zeroThr(0.0f, 0.0f, 0.0f, 0.0f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    Vector4<Color> zeroVal(zeroThr, zeroThr, zeroThr, zeroThr);
    auto zeroThreshold = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(zeroVal);
    EXPECT_FALSE(property.IsAbsNearEqual(target, zeroThreshold));
}

/**
 * @tc.name: IsAbsNearEqualRRect001
 * @tc.desc: Test IsAbsNearEqual for RRect type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsAbsNearEqualRRect001, TestSize.Level1)
{
    RectF rect1;
    rect1.SetAll(10.0f, 20.0f, 30.0f, 40.0f);
    RRect rrect1(rect1, 5.0f, 5.0f);
    RSRenderAnimatableProperty<RRect> property(rrect1);
    RectF rect2;
    rect2.SetAll(12.0f, 18.0f, 33.0f, 38.0f);
    RRect rrect2(rect2, 7.0f, 3.0f);
    auto target = std::make_shared<RSRenderAnimatableProperty<RRect>>(rrect2);
    RectF rect3;
    rect3.SetAll(5.0f, 5.0f, 5.0f, 5.0f);
    RRect rrect3(rect3, 5.0f, 5.0f);
    auto threshold = std::make_shared<RSRenderAnimatableProperty<RRect>>(rrect3);
    EXPECT_TRUE(property.IsAbsNearEqual(target, threshold));

    RectF rect4;
    rect4.SetAll(1.0f, 1.0f, 1.0f, 1.0f);
    RRect rrect4(rect4, 1.0f, 1.0f);
    auto smallThreshold = std::make_shared<RSRenderAnimatableProperty<RRect>>(rrect4);
    EXPECT_FALSE(property.IsAbsNearEqual(target, smallThreshold));

    // nullptr target
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(nullTarget, threshold));

    // nullptr threshold
    const std::shared_ptr<RSRenderPropertyBase> nullThreshold = nullptr;
    EXPECT_FALSE(property.IsAbsNearEqual(target, nullThreshold));
}

/**
 * @tc.name: TakeAbsMaxFromFloat001
 * @tc.desc: Test TakeAbsMaxFrom for float type with larger target
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromFloat001, TestSize.Level1)
{
    RSRenderAnimatableProperty<float> property(10.0f);
    auto target = std::make_shared<RSRenderAnimatableProperty<float>>(50.0f);
    property.TakeAbsMaxFrom(target);
    // |10| < |50| -> takes 50
    EXPECT_EQ(property.Get(), 50.0f);
}

/**
 * @tc.name: TakeAbsMaxFromFloat002
 * @tc.desc: Test TakeAbsMaxFrom for float with smaller target (no change)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromFloat002, TestSize.Level1)
{
    RSRenderAnimatableProperty<float> property(100.0f);
    auto target = std::make_shared<RSRenderAnimatableProperty<float>>(10.0f);
    property.TakeAbsMaxFrom(target);
    // |100| > |10| -> stays 100
    EXPECT_EQ(property.Get(), 100.0f);
}

/**
 * @tc.name: TakeAbsMaxFromFloat003
 * @tc.desc: Test TakeAbsMaxFrom for float with nullptr target
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromFloat003, TestSize.Level1)
{
    RSRenderAnimatableProperty<float> property(10.0f);
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    property.TakeAbsMaxFrom(nullTarget);
    // nullptr -> no change
    EXPECT_EQ(property.Get(), 10.0f);
}

/**
 * @tc.name: TakeAbsMaxFromVector2f001
 * @tc.desc: Test TakeAbsMaxFrom for Vector2f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromVector2f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector2f> property(Vector2f(10.0f, 20.0f));
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(50.0f, 5.0f));
    property.TakeAbsMaxFrom(target);
    // x: |10| < |50| -> 50; y: |20| > |5| -> stays 20
    EXPECT_FLOAT_EQ(property.Get().x_, 50.0f);
    EXPECT_FLOAT_EQ(property.Get().y_, 20.0f);
}

/**
 * @tc.name: TakeAbsMaxFromVector3f001
 * @tc.desc: Test TakeAbsMaxFrom for Vector3f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromVector3f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector3f> property(Vector3f(10.0f, 20.0f, 30.0f));
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(Vector3f(50.0f, 5.0f, 60.0f));
    property.TakeAbsMaxFrom(target);
    EXPECT_FLOAT_EQ(property.Get().x_, 50.0f);
    EXPECT_FLOAT_EQ(property.Get().y_, 20.0f);
    EXPECT_FLOAT_EQ(property.Get().z_, 60.0f);
}

/**
 * @tc.name: TakeAbsMaxFromVector4f001
 * @tc.desc: Test TakeAbsMaxFrom for Vector4f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromVector4f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector4f> property(Vector4f(10.0f, 20.0f, 30.0f, 40.0f));
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(50.0f, 5.0f, 60.0f, 3.0f));
    property.TakeAbsMaxFrom(target);
    EXPECT_FLOAT_EQ(property.Get().x_, 50.0f);
    EXPECT_FLOAT_EQ(property.Get().y_, 20.0f);
    EXPECT_FLOAT_EQ(property.Get().z_, 60.0f);
    EXPECT_FLOAT_EQ(property.Get().w_, 40.0f);
}

/**
 * @tc.name: TakeAbsMaxFromQuaternion001
 * @tc.desc: Test TakeAbsMaxFrom for Quaternion type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromQuaternion001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Quaternion> property(Quaternion(10.0f, 20.0f, 30.0f, 40.0f));
    auto target = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion(50.0f, 5.0f, 60.0f, 3.0f));
    property.TakeAbsMaxFrom(target);
    EXPECT_FLOAT_EQ(property.Get().x_, 50.0f);
    EXPECT_FLOAT_EQ(property.Get().y_, 20.0f);
    EXPECT_FLOAT_EQ(property.Get().z_, 60.0f);
    EXPECT_FLOAT_EQ(property.Get().w_, 40.0f);
}

/**
 * @tc.name: TakeAbsMaxFromMatrix3f001
 * @tc.desc: Test TakeAbsMaxFrom for Matrix3f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromMatrix3f001, TestSize.Level1)
{
    Matrix3f propVal(10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f);
    RSRenderAnimatableProperty<Matrix3f> property(propVal);
    auto target = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>(
        Matrix3f(100.0f, 5.0f, 110.0f, 5.0f, 120.0f, 5.0f, 130.0f, 5.0f, 140.0f));
    property.TakeAbsMaxFrom(target);
    const float* data = property.Get().GetConstData();
    EXPECT_FLOAT_EQ(data[0], 100.0f);
    EXPECT_FLOAT_EQ(data[1], 20.0f);
    EXPECT_FLOAT_EQ(data[2], 110.0f);
    EXPECT_FLOAT_EQ(data[3], 40.0f);
    EXPECT_FLOAT_EQ(data[4], 120.0f);
    EXPECT_FLOAT_EQ(data[5], 60.0f);
    EXPECT_FLOAT_EQ(data[6], 130.0f);
    EXPECT_FLOAT_EQ(data[7], 80.0f);
    EXPECT_FLOAT_EQ(data[8], 140.0f);
}

/**
 * @tc.name: TakeAbsMaxFromColor001
 * @tc.desc: Test TakeAbsMaxFrom for Color type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromColor001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Color> property(Color(10, 20, 30, 40));
    auto target = std::make_shared<RSRenderAnimatableProperty<Color>>(Color(50, 5, 60, 3));
    property.TakeAbsMaxFrom(target);
    EXPECT_EQ(property.Get().GetRed(), 50);
    EXPECT_EQ(property.Get().GetGreen(), 20);
    EXPECT_EQ(property.Get().GetBlue(), 60);
    EXPECT_EQ(property.Get().GetAlpha(), 40);
}

/**
 * @tc.name: TakeAbsMaxFromVector4Color001
 * @tc.desc: Test TakeAbsMaxFrom for Vector4<Color> type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromVector4Color001, TestSize.Level1)
{
    Vector4<Color> val(Color(10, 20, 30, 40), Color(10, 20, 30, 40), Color(10, 20, 30, 40), Color(10, 20, 30, 40));
    RSRenderAnimatableProperty<Vector4<Color>> property(val);
    Vector4<Color> tgtVal(Color(50, 5, 60, 3), Color(50, 5, 60, 3), Color(50, 5, 60, 3), Color(50, 5, 60, 3));
    auto target = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(tgtVal);
    property.TakeAbsMaxFrom(target);
    for (uint32_t i = 0; i < Vector4<Color>::V4SIZE; i++) {
        EXPECT_EQ(property.Get().data_[i].GetRed(), 50);
        EXPECT_EQ(property.Get().data_[i].GetGreen(), 20);
        EXPECT_EQ(property.Get().data_[i].GetBlue(), 60);
        EXPECT_EQ(property.Get().data_[i].GetAlpha(), 40);
    }
}

/**
 * @tc.name: TakeAbsMaxFromRRect001
 * @tc.desc: Test TakeAbsMaxFrom for RRect type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromRRect001, TestSize.Level1)
{
    RectF rect1;
    rect1.SetAll(10.0f, 20.0f, 30.0f, 40.0f);
    RRect rrect1(rect1, 5.0f, 10.0f);
    RSRenderAnimatableProperty<RRect> property(rrect1);
    RectF rect2;
    rect2.SetAll(100.0f, 5.0f, 110.0f, 3.0f);
    RRect rrect2(rect2, 50.0f, 3.0f);
    auto target = std::make_shared<RSRenderAnimatableProperty<RRect>>(rrect2);
    property.TakeAbsMaxFrom(target);
    EXPECT_FLOAT_EQ(property.Get().rect_.GetLeft(), 100.0f);
    EXPECT_FLOAT_EQ(property.Get().rect_.GetTop(), 20.0f);
    EXPECT_FLOAT_EQ(property.Get().rect_.GetWidth(), 110.0f);
    EXPECT_FLOAT_EQ(property.Get().rect_.GetHeight(), 40.0f);
    // radius: |5| < |50| -> 50; |10| > |3| -> stays 10
    for (int i = 0; i < 4; i++) {
        EXPECT_FLOAT_EQ(property.Get().radius_[i].x_, 50.0f);
        EXPECT_FLOAT_EQ(property.Get().radius_[i].y_, 10.0f);
    }
}

/**
 * @tc.name: TakeAbsMaxFromNullptr001
 * @tc.desc: Test TakeAbsMaxFrom with nullptr for various types
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, TakeAbsMaxFromNullptr001, TestSize.Level1)
{
    const std::shared_ptr<RSRenderPropertyBase> nullTarget = nullptr;
    RSRenderAnimatableProperty<Vector2f> prop2(Vector2f(10.0f, 20.0f));
    prop2.TakeAbsMaxFrom(nullTarget);
    EXPECT_FLOAT_EQ(prop2.Get().x_, 10.0f);

    RSRenderAnimatableProperty<Color> propColor(Color(10, 20, 30, 40));
    propColor.TakeAbsMaxFrom(nullTarget);
    EXPECT_EQ(propColor.Get().GetRed(), 10);

    RSRenderAnimatableProperty<Vector4<Color>> propV4C(
        Vector4<Color>(Color(10, 20, 30, 40), Color(10, 20, 30, 40), Color(10, 20, 30, 40), Color(10, 20, 30, 40)));
    propV4C.TakeAbsMaxFrom(nullTarget);
    EXPECT_EQ(propV4C.Get().data_[0].GetRed(), 10);

    RSRenderAnimatableProperty<Vector3f> propV3(Vector3f(10.0f, 20.0f, 30.0f));
    propV3.TakeAbsMaxFrom(nullTarget);
    EXPECT_FLOAT_EQ(propV3.Get().x_, 10.0f);

    RSRenderAnimatableProperty<Vector4f> propV4(Vector4f(10.0f, 20.0f, 30.0f, 40.0f));
    propV4.TakeAbsMaxFrom(nullTarget);
    EXPECT_FLOAT_EQ(propV4.Get().x_, 10.0f);

    RSRenderAnimatableProperty<Quaternion> propQ(Quaternion(10.0f, 20.0f, 30.0f, 40.0f));
    propQ.TakeAbsMaxFrom(nullTarget);
    EXPECT_FLOAT_EQ(propQ.Get().x_, 10.0f);

    RSRenderAnimatableProperty<Matrix3f> propM(Matrix3f(10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f));
    propM.TakeAbsMaxFrom(nullTarget);
    EXPECT_FLOAT_EQ(propM.Get().GetConstData()[0], 10.0f);

    RectF rect;
    rect.SetAll(10.0f, 20.0f, 30.0f, 40.0f);
    RRect rrect(rect, 5.0f, 10.0f);
    RSRenderAnimatableProperty<RRect> propRRect(rrect);
    propRRect.TakeAbsMaxFrom(nullTarget);
    EXPECT_FLOAT_EQ(propRRect.Get().rect_.GetLeft(), 10.0f);
}

/**
 * @tc.name: IsReachProgressFloat001
 * @tc.desc: Test IsReachProgress for float type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressFloat001, TestSize.Level1)
{
    // start=0, end=100, threshold=0.5 -> need |value-end| <= |end-start| * 0.5 = 50
    // If value=60: |60-100|=40 <= 50 -> true
    // If value=10: |10-100|=90 > 50 -> false
    RSRenderAnimatableProperty<float> property(60.0f);
    auto start = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto end = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    RSRenderAnimatableProperty<float> property2(10.0f);
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));

    // threshold=1.0 -> need |value-end| <= |end-start| * 1.0 = 100
    // value=10: |10-100|=90 <= 100 -> true
    EXPECT_TRUE(property2.IsReachProgress(start, end, 1.0f));
}

/**
 * @tc.name: IsReachProgressVector2f001
 * @tc.desc: Test IsReachProgress for Vector2f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressVector2f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector2f> property(Vector2f(60.0f, 60.0f));
    auto start = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(0.0f, 0.0f));
    auto end = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(100.0f, 100.0f));
    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    RSRenderAnimatableProperty<Vector2f> property2(Vector2f(10.0f, 10.0f));
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));
}

/**
 * @tc.name: IsReachProgressVector3f001
 * @tc.desc: Test IsReachProgress for Vector3f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressVector3f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector3f> property(Vector3f(60.0f, 60.0f, 60.0f));
    auto start = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(Vector3f(0.0f, 0.0f, 0.0f));
    auto end = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(Vector3f(100.0f, 100.0f, 100.0f));
    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    RSRenderAnimatableProperty<Vector3f> property2(Vector3f(10.0f, 10.0f, 10.0f));
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));
}

/**
 * @tc.name: IsReachProgressVector4f001
 * @tc.desc: Test IsReachProgress for Vector4f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressVector4f001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Vector4f> property(Vector4f(60.0f, 60.0f, 60.0f, 60.0f));
    auto start = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    auto end = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(100.0f, 100.0f, 100.0f, 100.0f));
    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    RSRenderAnimatableProperty<Vector4f> property2(Vector4f(10.0f, 10.0f, 10.0f, 10.0f));
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));
}

/**
 * @tc.name: IsReachProgressQuaternion001
 * @tc.desc: Test IsReachProgress for Quaternion type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressQuaternion001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Quaternion> property(Quaternion(60.0f, 60.0f, 60.0f, 60.0f));
    auto start = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion(0.0f, 0.0f, 0.0f, 0.0f));
    auto end = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion(100.0f, 100.0f, 100.0f, 100.0f));
    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    // value far from end -> false
    RSRenderAnimatableProperty<Quaternion> property2(Quaternion(10.0f, 10.0f, 10.0f, 10.0f));
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));
}

/**
 * @tc.name: IsReachProgressMatrix3f001
 * @tc.desc: Test IsReachProgress for Matrix3f type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressMatrix3f001, TestSize.Level1)
{
    Matrix3f propM(60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f);
    RSRenderAnimatableProperty<Matrix3f> property(propM);
    Matrix3f startM(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    auto start = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>(startM);
    Matrix3f endM(100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f);
    auto end = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>(endM);
    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    // value far from end -> false
    Matrix3f farM(10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f);
    RSRenderAnimatableProperty<Matrix3f> property2(farM);
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));
}

/**
 * @tc.name: IsReachProgressColor001
 * @tc.desc: Test IsReachProgress for Color type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressColor001, TestSize.Level1)
{
    RSRenderAnimatableProperty<Color> property(Color(60, 60, 60, 60));
    auto start = std::make_shared<RSRenderAnimatableProperty<Color>>(Color(0, 0, 0, 0));
    auto end = std::make_shared<RSRenderAnimatableProperty<Color>>(Color(100, 100, 100, 100));
    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    RSRenderAnimatableProperty<Color> property2(Color(10, 10, 10, 10));
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));
}

/**
 * @tc.name: IsReachProgressVector4Color001
 * @tc.desc: Test IsReachProgress for Vector4<Color> type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressVector4Color001, TestSize.Level1)
{
    Color midC(60, 60, 60, 60);
    Vector4<Color> val(midC, midC, midC, midC);
    RSRenderAnimatableProperty<Vector4<Color>> property(val);
    Color startC(0, 0, 0, 0);
    Vector4<Color> startVal(startC, startC, startC, startC);
    auto start = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(startVal);
    Color endC(100, 100, 100, 100);
    Vector4<Color> endVal(endC, endC, endC, endC);
    auto end = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(endVal);
    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    Color farC(10, 10, 10, 10);
    Vector4<Color> farVal(farC, farC, farC, farC);
    RSRenderAnimatableProperty<Vector4<Color>> property2(farVal);
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));
}

/**
 * @tc.name: IsReachProgressRRect001
 * @tc.desc: Test IsReachProgress for RRect type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressRRect001, TestSize.Level1)
{
    RectF rect60;
    rect60.SetAll(60.0f, 60.0f, 60.0f, 60.0f);
    RRect rrect60(rect60, 60.0f, 60.0f);
    RSRenderAnimatableProperty<RRect> property(rrect60);

    RectF rect0;
    rect0.SetAll(0.0f, 0.0f, 0.0f, 0.0f);
    RRect rrect0(rect0, 0.0f, 0.0f);
    auto start = std::make_shared<RSRenderAnimatableProperty<RRect>>(rrect0);

    RectF rect100;
    rect100.SetAll(100.0f, 100.0f, 100.0f, 100.0f);
    RRect rrect100(rect100, 100.0f, 100.0f);
    auto end = std::make_shared<RSRenderAnimatableProperty<RRect>>(rrect100);

    EXPECT_TRUE(property.IsReachProgress(start, end, 0.5f));

    RectF rect10;
    rect10.SetAll(10.0f, 10.0f, 10.0f, 10.0f);
    RRect rrect10(rect10, 10.0f, 10.0f);
    RSRenderAnimatableProperty<RRect> property2(rrect10);
    EXPECT_FALSE(property2.IsReachProgress(start, end, 0.5f));
}

/**
 * @tc.name: IsReachProgressColor002
 * @tc.desc: Test IsReachProgress for Color with BT2020 cross-color-space (operator- BT2020 path)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPropertyTest, IsReachProgressColor002, TestSize.Level1)
{
    // property=BT2020, end=SRGB: IsAbsNearEqual triggers BT2020 cross-color-space path
    // operator- on (end - start) uses BT2020 conversion when either operand is BT2020
    Color bt2020Color(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSRenderAnimatableProperty<Color> property(bt2020Color);
    Color srgbStart(0, 0, 0, 0, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto start = std::make_shared<RSRenderAnimatableProperty<Color>>(srgbStart);
    Color srgbEnd(128, 128, 128, 128, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto end = std::make_shared<RSRenderAnimatableProperty<Color>>(srgbEnd);
    // threshold=1.0: threshold_value = (end - start) * 1.0 ≈ end
    // |property_BT2020 - end_BT2020| is small -> true
    EXPECT_TRUE(property.IsReachProgress(start, end, 1.0f));

    // threshold=0.0: threshold_value = 0, exact match required -> false
    EXPECT_FALSE(property.IsReachProgress(start, end, 0.0f));
}
} // namespace OHOS::Rosen
