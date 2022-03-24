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
#include "utils/point.h"
#include "utils/scalar.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PointTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PointTest::SetUpTestCase() {}
void PointTest::TearDownTestCase() {}
void PointTest::SetUp() {}
void PointTest::TearDown() {}

/**
 * @tc.name: PointPointFCreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFCreateAndDestroy001, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    EXPECT_EQ(0.0f, pointf->GetX());
    EXPECT_EQ(0.0f, pointf->GetY());
}

/**
 * @tc.name: PointPointFCreateAndDestroy002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFCreateAndDestroy002, TestSize.Level1)
{
    PointF pointf1;
    pointf1.SetX(1.0f);
    PointF pointf2(pointf1);
    EXPECT_EQ(pointf1.GetX(), pointf2.GetX());
}

/**
 * @tc.name: PointPointFCreateAndDestroy003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFCreateAndDestroy003, TestSize.Level1)
{
    PointF pointf1;
    pointf1.SetX(2.0f);
    PointF pointf2(pointf1);
    EXPECT_EQ(pointf1.GetX(), pointf2.GetX());
}

/**
 * @tc.name: PointPointFCreateAndDestroy004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFCreateAndDestroy004, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>(1.0f, 2.0f);
    EXPECT_EQ(1.0f, pointf->GetX());
    EXPECT_EQ(2.0f, pointf->GetY());
}

/**
 * @tc.name: PointPointFCreateAndDestroy005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFCreateAndDestroy005, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>(2.0f, 1.0f);
    EXPECT_EQ(2.0f, pointf->GetX());
    EXPECT_EQ(1.0f, pointf->GetY());
}

/**
 * @tc.name: PointPointFSetAndGetX001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFSetAndGetX001, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    pointf->SetX(1.0f);
    EXPECT_EQ(1.0f, pointf->GetX());
}

/**
 * @tc.name: PointPointFSetAndGetX002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFSetAndGetX002, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    pointf->SetX(2.0f);
    EXPECT_EQ(2.0f, pointf->GetX());
}

/**
 * @tc.name: PointPointFSetAndGetY001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFSetAndGetY001, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    pointf->SetY(1.0f);
    EXPECT_EQ(1.0f, pointf->GetY());
}

/**
 * @tc.name: PointPointFSetAndGetY002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFSetAndGetY002, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    pointf->SetY(2.0f);
    EXPECT_EQ(2.0f, pointf->GetY());
}

/**
 * @tc.name: PointPointFAddEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFAddEqual001, TestSize.Level1)
{
    PointF pointf1;
    PointF pointf2(1.0f, 2.0f);
    pointf1 += pointf2;
    EXPECT_EQ(1.0f, pointf1.GetX());
    EXPECT_EQ(2.0f, pointf1.GetY());
}

/**
 * @tc.name: PointPointFAddEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFAddEqual002, TestSize.Level1)
{
    PointF pointf1;
    PointF pointf2(2.0f, 1.0f);
    pointf1 += pointf2;
    EXPECT_EQ(2.0f, pointf1.GetX());
    EXPECT_EQ(1.0f, pointf1.GetY());
}

/**
 * @tc.name: PointPointFMinusEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMinusEqual001, TestSize.Level1)
{
    PointF pointf1;
    PointF pointf2(1.0f, 2.0f);
    pointf1 -= pointf2;
    EXPECT_EQ(-1.0f, pointf1.GetX());
    EXPECT_EQ(-2.0f, pointf1.GetY());
}

/**
 * @tc.name: PointPointFMinusEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMinusEqual002, TestSize.Level1)
{
    PointF pointf1;
    PointF pointf2(2.0f, 1.0f);
    pointf1 -= pointf2;
    EXPECT_EQ(-2.0f, pointf1.GetX());
    EXPECT_EQ(-1.0f, pointf1.GetY());
}

/**
 * @tc.name: PointPointFMultiplyEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMultiplyEqual001, TestSize.Level1)
{
    PointF pointf1(2.0f, 3.0f);
    pointf1 *= 2.0f;
    EXPECT_EQ(4.0f, pointf1.GetX());
    EXPECT_EQ(6.0f, pointf1.GetY());
}

/**
 * @tc.name: PointPointFMultiplyEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMultiplyEqual002, TestSize.Level1)
{
    PointF pointf1(4.0f, 5.0f);
    pointf1 *= 2;
    EXPECT_EQ(8.0f, pointf1.GetX());
    EXPECT_EQ(10.0f, pointf1.GetY());
}

/**
 * @tc.name: PointPointFDevidEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFDevidEqual001, TestSize.Level1)
{
    PointF pointf1(4.0f, 6.0f);
    pointf1 /= 2.0f;
    EXPECT_EQ(2.0f, pointf1.GetX());
    EXPECT_EQ(3.0f, pointf1.GetY());
}

/**
 * @tc.name: PointPointFDevidEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFDevidEqual002, TestSize.Level1)
{
    PointF pointf1(6.0f, 8.0f);
    pointf1 /= 2;
    EXPECT_EQ(3.0f, pointf1.GetX());
    EXPECT_EQ(4.0f, pointf1.GetY());
}

/**
 * @tc.name: PointPointFAdd001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFAdd001, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(2.0f, 1.0f);
    PointF pointf3 = pointf1 + pointf2;
    EXPECT_EQ(3.0f, pointf3.GetX());
    EXPECT_EQ(3.0f, pointf3.GetY());
}

/**
 * @tc.name: PointPointFAdd002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFAdd002, TestSize.Level1)
{
    PointF pointf1(2.0f, 3.0f);
    PointF pointf2(4.0f, 5.0f);
    PointF pointf3 = pointf1 + pointf2;
    EXPECT_EQ(5.0f, pointf3.GetX());
    EXPECT_EQ(9.0f, pointf3.GetY());
}

/**
 * @tc.name: PointPointFMinus001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMinus001, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(2.0f, 1.0f);
    PointF pointf3 = pointf1 - pointf2;
    EXPECT_EQ(-1.0f, pointf3.GetX());
    EXPECT_EQ(1.0f, pointf3.GetY());
}

/**
 * @tc.name: PointPointFMinus002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMinus002, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(3.0f, 4.0f);
    PointF pointf3 = pointf1 - pointf2;
    EXPECT_EQ(-2.0f, pointf3.GetX());
    EXPECT_EQ(-2.0f, pointf3.GetY());
}

/**
 * @tc.name: PointPointFMultiply001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMultiply001, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = 3.0f * pointf1;
    EXPECT_EQ(3.0f, pointf2.GetX());
    EXPECT_EQ(6.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFMultiply002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMultiply002, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = 4.0f * pointf1;
    EXPECT_EQ(4.0f, pointf2.GetX());
    EXPECT_EQ(8.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFMultiply003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMultiply003, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = pointf1 * 3.0f;
    EXPECT_EQ(3.0f, pointf2.GetX());
    EXPECT_EQ(6.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFMultiply004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFMultiply004, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = pointf1 * 4.0f;
    EXPECT_EQ(4.0f, pointf2.GetX());
    EXPECT_EQ(8.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFDevide001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFDevide001, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = pointf1 / 1.0f;
    EXPECT_EQ(1.0f, pointf2.GetX());
    EXPECT_EQ(2.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFDevide002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFDevide002, TestSize.Level1)
{
    PointF pointf1(4.0f, 8.0f);
    PointF pointf2 = pointf1 / 2.0f;
    EXPECT_EQ(2.0f, pointf2.GetX());
    EXPECT_EQ(4.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFPositive001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFPositive001, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = +pointf1;
    EXPECT_EQ(1.0f, pointf2.GetX());
    EXPECT_EQ(2.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFPositive002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFPositive002, TestSize.Level1)
{
    PointF pointf1(3.0f, 4.0f);
    PointF pointf2 = +pointf1;
    EXPECT_EQ(3.0f, pointf2.GetX());
    EXPECT_EQ(4.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFNegative001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFNegative001, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = -pointf1;
    EXPECT_EQ(-1.0f, pointf2.GetX());
    EXPECT_EQ(-2.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFNegative002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFNegative002, TestSize.Level1)
{
    PointF pointf1(2.0f, 3.0f);
    PointF pointf2 = -pointf1;
    EXPECT_EQ(-2.0f, pointf2.GetX());
    EXPECT_EQ(-3.0f, pointf2.GetY());
}

/**
 * @tc.name: PointPointFEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFEqual001, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(1.0f, 2.0f);
    EXPECT_TRUE(pointf1 == pointf2);
}

/**
 * @tc.name: PointPointFEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFEqual002, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(2.0f, 3.0f);
    EXPECT_FALSE(pointf1 == pointf2);
}

/**
 * @tc.name: PointPointFNotEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFNotEqual001, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(2.0f, 3.0f);
    EXPECT_TRUE(pointf1 != pointf2);
}

/**
 * @tc.name: PointPointFNotEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointFNotEqual002, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(1.0f, 2.0f);
    EXPECT_FALSE(pointf1 != pointf2);
}

/**
 * @tc.name: PointPointICreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointICreateAndDestroy001, TestSize.Level1)
{
    PointI pointi1;
    pointi1.SetX(1.0f);
    PointI pointi2(pointi1);
    EXPECT_EQ(pointi1.GetX(), pointi2.GetX());
}

/**
 * @tc.name: PointPointICreateAndDestroy002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointICreateAndDestroy002, TestSize.Level1)
{
    PointI pointi1;
    pointi1.SetX(2.0f);
    PointI pointi2(pointi1);
    EXPECT_EQ(pointi1.GetX(), pointi2.GetX());
}

/**
 * @tc.name: PointPointICreateAndDestroy003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointICreateAndDestroy003, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>(1.0f, 2.0f);
    EXPECT_EQ(1.0f, pointi->GetX());
    EXPECT_EQ(2.0f, pointi->GetY());
}

/**
 * @tc.name: PointPointICreateAndDestroy004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointICreateAndDestroy004, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>(2.0f, 1.0f);
    EXPECT_EQ(2.0f, pointi->GetX());
    EXPECT_EQ(1.0f, pointi->GetY());
}

/**
 * @tc.name: PointPointICreateAndDestroy005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointINotEqual005, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    EXPECT_EQ(0.0f, pointi->GetX());
    EXPECT_EQ(0.0f, pointi->GetY());
}

/**
 * @tc.name: PointPointISetAndGetX001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointISetAndGetX001, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    pointi->SetX(1.0f);
    EXPECT_EQ(1.0f, pointi->GetX());
}

/**
 * @tc.name: PointPointISetAndGetX002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointISetAndGetX002, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    pointi->SetX(2.0f);
    EXPECT_EQ(2.0f, pointi->GetX());
}

/**
 * @tc.name: PointPointISetAndGetY001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointISetAndGetY001, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    pointi->SetY(1.0f);
    EXPECT_EQ(1.0f, pointi->GetY());
}

/**
 * @tc.name: PointPointISetAndGetY002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointISetAndGetY002, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    pointi->SetY(2.0f);
    EXPECT_EQ(2.0f, pointi->GetY());
}

/**
 * @tc.name: PointPointIAddEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIAddEqual001, TestSize.Level1)
{
    PointI pointi1;
    PointI pointi2(1.0f, 2.0f);
    pointi1 += pointi2;
    EXPECT_EQ(1.0f, pointi1.GetX());
    EXPECT_EQ(2.0f, pointi1.GetY());
}

/**
 * @tc.name: PointPointIAddEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIAddEqual002, TestSize.Level1)
{
    PointI pointi1;
    PointI pointi2(2.0f, 1.0f);
    pointi1 += pointi2;
    EXPECT_EQ(2.0f, pointi1.GetX());
    EXPECT_EQ(1.0f, pointi1.GetY());
}

/**
 * @tc.name: PointPointIMinusEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMinusEqual001, TestSize.Level1)
{
    PointI pointi1;
    PointI pointi2(1.0f, 2.0f);
    pointi1 -= pointi2;
    EXPECT_EQ(-1.0f, pointi1.GetX());
    EXPECT_EQ(-2.0f, pointi1.GetY());
}

/**
 * @tc.name: PointPointIMinusEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMinusEqual002, TestSize.Level1)
{
    PointI pointi1;
    PointI pointi2(2.0f, 1.0f);
    pointi1 -= pointi2;
    EXPECT_EQ(-2.0f, pointi1.GetX());
    EXPECT_EQ(-1.0f, pointi1.GetY());
}

/**
 * @tc.name: PointPointIMultiplyEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMultiplyEqual001, TestSize.Level1)
{
    PointI pointi1(2.0f, 3.0f);
    pointi1 *= 2;
    EXPECT_EQ(4.0f, pointi1.GetX());
    EXPECT_EQ(6.0f, pointi1.GetY());
}

/**
 * @tc.name: PointPointIMultiplyEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMultiplyEqual002, TestSize.Level1)
{
    PointI pointi1(4.0f, 5.0f);
    pointi1 *= 2.0f;
    EXPECT_EQ(8.0f, pointi1.GetX());
    EXPECT_EQ(10.0f, pointi1.GetY());
}

/**
 * @tc.name: PointPointIDevideEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIDevideEqual001, TestSize.Level1)
{
    PointI pointi1(4.0f, 6.0f);
    pointi1 /= 2.0f;
    EXPECT_EQ(2.0f, pointi1.GetX());
    EXPECT_EQ(3.0f, pointi1.GetY());
}

/**
 * @tc.name: PointPointIDevideEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIDevideEqual002, TestSize.Level1)
{
    PointI pointi1(6.0f, 8.0f);
    pointi1 /= 2.0f;
    EXPECT_EQ(3.0f, pointi1.GetX());
    EXPECT_EQ(4.0f, pointi1.GetY());
}

/**
 * @tc.name: PointPointIAdd001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIAdd001, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(2.0f, 1.0f);
    PointI pointi3 = pointi1 + pointi2;
    EXPECT_EQ(3.0f, pointi3.GetX());
    EXPECT_EQ(3.0f, pointi3.GetY());
}

/**
 * @tc.name: PointPointIAdd002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIAdd002, TestSize.Level1)
{
    PointI pointi1(2.0f, 3.0f);
    PointI pointi2(4.0f, 5.0f);
    PointI pointi3 = pointi1 + pointi2;
    EXPECT_EQ(5.0f, pointi3.GetX());
    EXPECT_EQ(9.0f, pointi3.GetY());
}

/**
 * @tc.name: PointPointIMinus001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMinus001, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(2.0f, 1.0f);
    PointI pointi3 = pointi1 - pointi2;
    EXPECT_EQ(-1.0f, pointi3.GetX());
    EXPECT_EQ(1.0f, pointi3.GetY());
}

/**
 * @tc.name: PointPointIMinus002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMinus002, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(3.0f, 4.0f);
    PointI pointi3 = pointi1 - pointi2;
    EXPECT_EQ(-2.0f, pointi3.GetX());
    EXPECT_EQ(-2.0f, pointi3.GetY());
}

/**
 * @tc.name: PointPointIMultiply001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMultiply001, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = 3.0f * pointi1;
    EXPECT_EQ(3.0f, pointi2.GetX());
    EXPECT_EQ(6.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointIMultiply002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMultiply002, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = 4.0f * pointi1;
    EXPECT_EQ(4.0f, pointi2.GetX());
    EXPECT_EQ(8.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointIMultiply003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMultiply003, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = pointi1 * 3.0f;
    EXPECT_EQ(3.0f, pointi2.GetX());
    EXPECT_EQ(6.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointIMultiply004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIMultiply004, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = pointi1 * 4.0f;
    EXPECT_EQ(4.0f, pointi2.GetX());
    EXPECT_EQ(8.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointIDevide001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIDevide001, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = pointi1 / 1.0f;
    EXPECT_EQ(1.0f, pointi2.GetX());
    EXPECT_EQ(2.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointIDevide002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIDevide002, TestSize.Level1)
{
    PointI pointi1(4.0f, 8.0f);
    PointI pointi2 = pointi1 / 2.0f;
    EXPECT_EQ(2.0f, pointi2.GetX());
    EXPECT_EQ(4.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointIPositive001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIPositive001, TestSize.Level1)
{    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = +pointi1;
    EXPECT_EQ(1.0f, pointi2.GetX());
    EXPECT_EQ(2.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointIPositive002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIPositive002, TestSize.Level1)
{
    PointI pointi1(3.0f, 4.0f);
    PointI pointi2 = +pointi1;
    EXPECT_EQ(3.0f, pointi2.GetX());
    EXPECT_EQ(4.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointINegative001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointINegative001, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = -pointi1;
    EXPECT_EQ(-1.0f, pointi2.GetX());
    EXPECT_EQ(-2.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointINegative002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointINegative002, TestSize.Level1)
{
    PointI pointi1(2.0f, 3.0f);
    PointI pointi2 = -pointi1;
    EXPECT_EQ(-2.0f, pointi2.GetX());
    EXPECT_EQ(-3.0f, pointi2.GetY());
}

/**
 * @tc.name: PointPointIEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIEqual001, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(1.0f, 2.0f);
    EXPECT_TRUE(pointi1 == pointi2);
}

/**
 * @tc.name: PointPointIEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointIEqual002, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(2.0f, 3.0f);
    EXPECT_FALSE(pointi1 == pointi2);
}

/**
 * @tc.name: PointPointINotEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointINotEqual001, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(2.0f, 3.0f);
    EXPECT_TRUE(pointi1 != pointi2);
}

/**
 * @tc.name: PointPointINotEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, PointPointINotEqual002, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(1.0f, 2.0f);
    EXPECT_FALSE(pointi1 != pointi2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

