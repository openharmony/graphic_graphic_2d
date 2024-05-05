/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "effect/blur_draw_looper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class BlurDrawLooperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BlurDrawLooperTest::SetUpTestCase() {}
void BlurDrawLooperTest::TearDownTestCase() {}
void BlurDrawLooperTest::SetUp() {}
void BlurDrawLooperTest::TearDown() {}

/**
 * @tc.name: CreateDashPathEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 * @tc.author:
 */
HWTEST_F(BlurDrawLooperTest, CreateBlurDrawLooperTest001, TestSize.Level1)
{
    // 1.f 2.f  3.f and 0x12345678 is setted to compare.
    float radius = 1.f;
    Point point{2.f, 3.f};
    Color color = Color(0x12345678);
    std::shared_ptr<BlurDrawLooper> blurDrawLooper = BlurDrawLooper::CreateBlurDrawLooper(radius,
        point.GetX(), point.GetY(), color);
    EXPECT_NE(blurDrawLooper, nullptr);
    EXPECT_TRUE(IsScalarAlmostEqual(blurDrawLooper->GetRadius(), radius));
    EXPECT_TRUE(IsScalarAlmostEqual(blurDrawLooper->GetXOffset(), point.GetX()));
    EXPECT_TRUE(IsScalarAlmostEqual(blurDrawLooper->GetYOffset(), point.GetY()));
    EXPECT_TRUE(color == blurDrawLooper->GetColor());

    // except branch
    std::shared_ptr<BlurDrawLooper> blurDrawLooper2 = BlurDrawLooper::CreateBlurDrawLooper(-1.f,
        2.f, 3.f, Color(0x12345678));
    EXPECT_EQ(blurDrawLooper2, nullptr);
}

/**
 * @tc.name: BlurDrawLooperEqualTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 * @tc.author:
 */
HWTEST_F(BlurDrawLooperTest, BlurDrawLooperEqualTest002, TestSize.Level1)
{
    // 1.f 2.f  3.f and 0x12345678 is setted to compare.
    float radius = 1.f;
    Point point{2.f, 3.f};
    Color color = Color(0x12345678);
    std::shared_ptr<BlurDrawLooper> blurDrawLooper1 = BlurDrawLooper::CreateBlurDrawLooper(radius,
        point.GetX(), point.GetY(), color);
    EXPECT_NE(blurDrawLooper1, nullptr);

    std::shared_ptr<BlurDrawLooper> blurDrawLooper2 = BlurDrawLooper::CreateBlurDrawLooper(radius,
        point.GetX(), point.GetY(), color);
    EXPECT_NE(blurDrawLooper2, nullptr);
    // Color(0x87654321) and color, not same
    std::shared_ptr<BlurDrawLooper> blurDrawLooper3 = BlurDrawLooper::CreateBlurDrawLooper(radius,
        point.GetX(), point.GetY(), Color(0x87654321));
    EXPECT_NE(blurDrawLooper3, nullptr);
    // Point{1.f, -1.f} and point, not same
    std::shared_ptr<BlurDrawLooper> blurDrawLooper4 = BlurDrawLooper::CreateBlurDrawLooper(radius,
        1.f, -1.f, color);
    EXPECT_NE(blurDrawLooper4, nullptr);
    // 3.f and radius, not same
    std::shared_ptr<BlurDrawLooper> blurDrawLooper5 = BlurDrawLooper::CreateBlurDrawLooper(3.f,
        point.GetX(), point.GetY(), color);
    EXPECT_NE(blurDrawLooper5, nullptr);

    EXPECT_TRUE(*blurDrawLooper1 == *blurDrawLooper2);
    EXPECT_TRUE(*blurDrawLooper1 != *blurDrawLooper3);
    EXPECT_TRUE(*blurDrawLooper1 != *blurDrawLooper4);
    EXPECT_TRUE(*blurDrawLooper1 != *blurDrawLooper5);
}

/**
 * @tc.name: BlurDrawLooperSerializeTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 * @tc.author:
 */
HWTEST_F(BlurDrawLooperTest, BlurDrawLooperSerializeTest003, TestSize.Level1)
{
    // 1.f 2.f  3.f and 0x12345678 is setted to compare.
    float radius = 1.f;
    Point point{2.f, 3.f};
    Color color = Color(0x12345678);
    std::shared_ptr<BlurDrawLooper> blurDrawLooper1 = BlurDrawLooper::CreateBlurDrawLooper(radius,
        point.GetX(), point.GetY(), color);
    EXPECT_NE(blurDrawLooper1, nullptr);

    // serialize
    std::shared_ptr<Data> data = blurDrawLooper1->Serialize();
    EXPECT_NE(data, nullptr);
    EXPECT_NE(data->GetData(), nullptr);
    EXPECT_NE(data->GetSize(), 0);

    // deserialize
    std::shared_ptr<BlurDrawLooper> blurDrawLooper2 = BlurDrawLooper::Deserialize(data);
    EXPECT_NE(blurDrawLooper2, nullptr);
    EXPECT_NE(blurDrawLooper2->GetMaskFilter(), nullptr);
    EXPECT_TRUE(IsScalarAlmostEqual(blurDrawLooper2->GetRadius(), radius));
    EXPECT_TRUE(IsScalarAlmostEqual(blurDrawLooper2->GetXOffset(), point.GetX()));
    EXPECT_TRUE(IsScalarAlmostEqual(blurDrawLooper2->GetYOffset(), point.GetY()));
    EXPECT_TRUE(blurDrawLooper2->GetColor() == color);

    // compare
    EXPECT_TRUE(*blurDrawLooper1 == *blurDrawLooper2);

    //except branch
    EXPECT_EQ(BlurDrawLooper::Deserialize(nullptr), nullptr);
    auto emptyData = std::make_shared<Data>();
    EXPECT_EQ(BlurDrawLooper::Deserialize(emptyData), nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
