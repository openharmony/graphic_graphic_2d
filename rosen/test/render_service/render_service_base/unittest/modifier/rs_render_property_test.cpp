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
#include "pipeline/rs_render_node.h"

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

#ifndef MODIFIER_NG
/**
 * @tc.name: OnChange
 * @tc.desc: Test OnChange and Marshalling
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRenderPropertyTest, OnChange, TestSize.Level1)
{
    std::shared_ptr<RSRenderPropertyBase> base = std::make_shared<RSRenderProperty<bool>>();
    RSRenderNode node(1);
    base->OnChange();
    base->Attach(node);
    base->modifierType_ = RSModifierType::FOREGROUND_COLOR;
    base->OnChange();
    base->modifierType_ = RSModifierType::POSITION_Z;
    base->OnChange();
    base->modifierType_ = RSModifierType::FOREGROUND_STYLE;
    base->OnChange();

    base->UpdatePropertyUnit(RSModifierType::FRAME);
    base->UpdatePropertyUnit(RSModifierType::SCALE);
    base->UpdatePropertyUnit(RSModifierType::ROTATION_X);
    base->UpdatePropertyUnit(RSModifierType::CLIP_BOUNDS);
    ASSERT_EQ(base->node_.lock(), nullptr);
}
#endif

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
    EXPECT_NEAR(property3f.ToFloat(), 1.73205f, 1e-4);  // 1.73205.f mod result
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
 * @tc.name: RSRenderPropertySkMatrixOnUnmarshalling
 * @tc.desc: RSRenderPropertySkMatrix On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySkMatrixOnUnmarshalling, TestSize.Level1)
{
    SkMatrix value = {};
    std::shared_ptr<RSRenderProperty<SkMatrix>> prop = std::make_shared<RSRenderProperty<SkMatrix>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<SkMatrix>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<SkMatrix>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<SkMatrix>::OnUnmarshalling(parcel, receivedProp);
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
 * @tc.name: RSRenderPropertyForegroundColorStrategyTypeOnUnmarshalling
 * @tc.desc: RSRenderPropertyForegroundColorStrategyType On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyForegroundColorStrategyTypeOnUnmarshalling, TestSize.Level1)
{
    ForegroundColorStrategyType value = {};
    std::shared_ptr<RSRenderProperty<ForegroundColorStrategyType>> prop =
        std::make_shared<RSRenderProperty<ForegroundColorStrategyType>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<ForegroundColorStrategyType>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<ForegroundColorStrategyType>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<ForegroundColorStrategyType>::OnUnmarshalling(parcel, receivedProp);
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
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<RSImage>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSImage>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSImage>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
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
 * @tc.name: RSRenderPropertyDrawingMatrixOnUnmarshalling
 * @tc.desc: RSRenderPropertyDrawingMatrix On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyDrawingMatrixOnUnmarshalling, TestSize.Level1)
{
    Drawing::Matrix value = {};
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> prop =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<Drawing::Matrix>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<Drawing::Matrix>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<Drawing::Matrix>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(receivedProp != nullptr);
}

/**
 * @tc.name: RSRenderPropertySharedPtrRSMagnifierParamsOnUnmarshalling
 * @tc.desc: RSRenderPropertySharedPtrRSMagnifierParams On Unmarshalling Test
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertySharedPtrRSMagnifierParamsOnUnmarshalling, TestSize.Level1)
{
    auto value = std::make_shared<RSMagnifierParams>();
    std::shared_ptr<RSRenderProperty<std::shared_ptr<RSMagnifierParams>>> prop =
        std::make_shared<RSRenderProperty<std::shared_ptr<RSMagnifierParams>>>(value, 1);

    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> receivedProp;
    bool ret = RSRenderProperty<std::shared_ptr<RSMagnifierParams>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSMagnifierParams>>::OnUnmarshalling(parcel, receivedProp);
    EXPECT_FALSE(ret);

    ret = RSMarshallingHelper::Marshalling(parcel, prop->GetId());
    EXPECT_TRUE(ret);
    ret = RSMarshallingHelper::Marshalling(parcel, prop->Get());
    EXPECT_TRUE(ret);
    ret = RSRenderProperty<std::shared_ptr<RSMagnifierParams>>::OnUnmarshalling(parcel, receivedProp);
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
} // namespace OHOS::Rosen
