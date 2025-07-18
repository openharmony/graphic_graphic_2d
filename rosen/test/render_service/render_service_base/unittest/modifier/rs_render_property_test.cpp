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
#include "include/modifier/rs_render_modifier.h"
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
    ASSERT_FALSE(RSMarshallingHelper::Marshalling(parcel1, intProp));
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
    props.push_back(std::make_shared<MockRSRenderProperty<std::shared_ptr<RSRenderFilter>>>());
    props.push_back(std::make_shared<MockRSRenderProperty<std::shared_ptr<Drawing::DrawCmdList>>>());

    for (auto& prop : props) {
        MessageParcel parcel;
        ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, prop));
        ASSERT_TRUE(RSRenderPropertyBase::Unmarshalling(parcel, prop));
    }
}

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
        ASSERT_FALSE(RSMarshallingHelper::Marshalling(parcel, prop));
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
    auto prop = std::make_shared<RSRenderProperty<Vector3f>>();
    prop->Set(Vector3f(1.f, 1.f, 1.f)); // 1.f for test
    std::string dumpOut;
    prop->Dump(dumpOut);
    EXPECT_EQ(dumpOut, std::string("[x:1.0 y:1.0 z:1.0]"));
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
 * @tc.name: RSRenderPropertyOnUnmarshalling001
 * @tc.desc: OnUnmarshalling should return true when Unmarshalling succeed
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyOnUnmarshalling001, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;
    int32_t expectId = 42;
    int expectValue = 5;
    parcel.WriteInt32(expectId);
    parcel.WriteInt32(expectValue);

    bool result = RSRenderProperty<int>::OnUnmarshalling(parcel, val);
    EXPECT_TRUE(result);
    EXPECT_NE(val, nullptr);
    auto prop = std::static_pointer_cast<RSRenderProperty<int>>(val);
    EXPECT_EQ(prop->GetId(), expectId);
    EXPECT_EQ(prop->Get(), expectValue);
}

/**
 * @tc.name: RSRenderPropertyOnUnmarshalling002
 * @tc.desc: OnUnmarshalling should return false when Unmarshalling fail on id
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyOnUnmarshalling002, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;

    bool result = RSRenderProperty<int>::OnUnmarshalling(parcel, val);
    EXPECT_FALSE(result);
    EXPECT_EQ(val, nullptr);
}

/**
 * @tc.name: RSRenderPropertyOnUnmarshalling003
 * @tc.desc: OnUnmarshalling should return false when Unmarshalling fail on value
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderPropertyOnUnmarshalling003, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;
    int32_t expectId = 42;
    parcel.WriteInt32(expectId);

    bool result = RSRenderProperty<int>::OnUnmarshalling(parcel, val);
    EXPECT_FALSE(result);
    EXPECT_EQ(val, nullptr);
}

/**
 * @tc.name: RSRenderAnimatablePropertyOnUnmarshalling001
 * @tc.desc: OnUnmarshalling should return false when Unmarshalling succeed
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderAnimatablePropertyOnUnmarshalling001, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;
    int32_t expectId = 42;
    float expectValue = 3.14f;
    uint8_t expectedUnit = 1;
    parcel.WriteInt32(expectId);
    parcel.WriteFloat(expectValue);
    parcel.WriteUint8(expectedUnit);

    bool result = RSRenderAnimatableProperty<float>::OnUnmarshalling(parcel, val);
    EXPECT_TRUE(result);
    EXPECT_NE(val, nullptr);
    auto prop = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(val);
    EXPECT_EQ(prop->GetId(), expectId);
    EXPECT_TRUE(ROSEN_EQ(prop->Get(), expectValue));
    EXPECT_EQ(static_cast<uint8_t>(prop->GetPropertyUnit()), expectValue);
}

/**
 * @tc.name: RSRenderAnimatablePropertyOnUnmarshalling002
 * @tc.desc: OnUnmarshalling should return false when Unmarshalling fail on id
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderAnimatablePropertyOnUnmarshalling002, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;

    bool result = RSRenderAnimatableProperty<float>::OnUnmarshalling(parcel, val);
    EXPECT_FALSE(result);
    EXPECT_EQ(val, nullptr);
}

/**
 * @tc.name: RSRenderAnimatablePropertyOnUnmarshalling003
 * @tc.desc: OnUnmarshalling should return false when Unmarshalling fail on value
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderAnimatablePropertyOnUnmarshalling003, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;
    int32_t expectId = 42;
    parcel.WriteInt32(expectId);

    bool result = RSRenderAnimatableProperty<float>::OnUnmarshalling(parcel, val);
    EXPECT_FALSE(result);
    EXPECT_EQ(val, nullptr);
}

/**
 * @tc.name: RSRenderAnimatablePropertyOnUnmarshalling004
 * @tc.desc: OnUnmarshalling should return false when Unmarshalling fail on PropertyUnit
 * @tc.type: FUNC
 * @tc.require: issueICDSPJ
 */
HWTEST_F(RSRenderPropertyTest, RSRenderAnimatablePropertyOnUnmarshalling004, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;
    int32_t expectId = 42;
    float expectValue = 3.14f;
    parcel.WriteInt32(expectId);
    parcel.WriteFloat(expectValue);

    bool result = RSRenderAnimatableProperty<float>::OnUnmarshalling(parcel, val);
    EXPECT_FALSE(result);
    EXPECT_EQ(val, nullptr);
}
}
