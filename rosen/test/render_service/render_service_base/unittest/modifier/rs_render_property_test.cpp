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
    explicit MockRSRenderProperty(const RSRenderPropertyType type) : RSRenderProperty<T>(), type_(type) {}
    virtual ~MockRSRenderProperty() = default;
protected:
    RSRenderPropertyType type_;
};

template<typename T>
class MockRSRenderAnimatableProperty : public RSRenderAnimatableProperty<T> {
public:
    explicit MockRSRenderAnimatableProperty(const T& value) : RSRenderAnimatableProperty<T>(value) {}
    explicit MockRSRenderAnimatableProperty(const RSRenderPropertyType type) : RSRenderAnimatableProperty<T>()
    {
        RSRenderAnimatableProperty<T>::SetPropertyType(type);
    }
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
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<float>>(
        RSRenderPropertyType::PROPERTY_FLOAT));
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Color>>(
        RSRenderPropertyType::PROPERTY_COLOR));
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Matrix3f>>(
        RSRenderPropertyType::PROPERTY_MATRIX3F));
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Quaternion>>(
        RSRenderPropertyType::PROPERTY_QUATERNION));
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(
        RSRenderPropertyType::PROPERTY_FILTER));
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Vector2f>>(
        RSRenderPropertyType::PROPERTY_VECTOR2F));
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Vector4f>>(
        RSRenderPropertyType::PROPERTY_VECTOR4F));
    props.push_back(std::make_shared<MockRSRenderAnimatableProperty<Vector4<Color>>>(
        RSRenderPropertyType::PROPERTY_VECTOR4_COLOR));

    for (auto& prop : props) {
        MessageParcel parcel;
        ASSERT_TRUE(RSRenderPropertyBase::Marshalling(parcel, prop));
        ASSERT_TRUE(RSRenderPropertyBase::Unmarshalling(parcel, prop));
    }

    MessageParcel parcel1;
    auto intProp = std::make_shared<RSRenderAnimatableProperty<int>>();
    std::shared_ptr<RSRenderPropertyBase> tmpProp;
    ASSERT_FALSE(RSRenderPropertyBase::Marshalling(parcel1, intProp));
    ASSERT_TRUE(RSRenderPropertyBase::Unmarshalling(parcel1, tmpProp));

    MessageParcel parcel2;
    int data = 0;
    parcel2.ParseFrom(data, sizeof(int));
    ASSERT_FALSE(RSRenderPropertyBase::Marshalling(parcel2, intProp));
}

/**
 * @tc.name: PropertyIPC002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyTest, PropertyIPC002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> props;
    props.push_back(std::make_shared<MockRSRenderProperty<float>>(
        RSRenderPropertyType::PROPERTY_MATRIX3F));
    props.push_back(std::make_shared<MockRSRenderProperty<Color>>(
        RSRenderPropertyType::PROPERTY_FLOAT));
    props.push_back(std::make_shared<MockRSRenderProperty<Matrix3f>>(
        RSRenderPropertyType::PROPERTY_FLOAT));
    props.push_back(std::make_shared<MockRSRenderProperty<Quaternion>>(
        RSRenderPropertyType::PROPERTY_FLOAT));
    props.push_back(std::make_shared<MockRSRenderProperty<std::shared_ptr<RSFilter>>>(
        RSRenderPropertyType::PROPERTY_FLOAT));
    props.push_back(std::make_shared<MockRSRenderProperty<Vector2f>>(
        RSRenderPropertyType::PROPERTY_FLOAT));
    props.push_back(std::make_shared<MockRSRenderProperty<Vector4f>>(
        RSRenderPropertyType::PROPERTY_FLOAT));
    props.push_back(std::make_shared<MockRSRenderProperty<Vector4<Color>>>(
        RSRenderPropertyType::PROPERTY_FLOAT));

    for (auto& prop : props) {
        MessageParcel parcel;
        ASSERT_FALSE(RSRenderPropertyBase::Marshalling(parcel, prop));
        ASSERT_TRUE(RSRenderPropertyBase::Unmarshalling(parcel, prop));
    }
}
}
