/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "utils/rect.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RectTest::SetUpTestCase() {}
void RectTest::TearDownTestCase() {}
void RectTest::SetUp() {}
void RectTest::TearDown() {}

/**
 * @tc.name: RectFCreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestroy001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    EXPECT_EQ(0.0f, rectF->GetLeft());
    EXPECT_EQ(0.0f, rectF->GetTop());
    EXPECT_EQ(0.0f, rectF->GetRight());
    EXPECT_EQ(0.0f, rectF->GetBottom());
}

/**
 * @tc.name: RectFCreateAndDestroy002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestroy002, TestSize.Level1)
{
    RectF rectf1;
    rectf1.SetLeft(1.0f);
    RectF rectf2(rectf1);
    EXPECT_EQ(rectf1.GetLeft(), rectf2.GetLeft());
}

/**
 * @tc.name: RectFCreateAndDestroy003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestroy003, TestSize.Level1)
{
    RectF rectf1;
    rectf1.SetLeft(2.0f);
    RectF rectf2(rectf1);
    EXPECT_EQ(rectf1.GetLeft(), rectf2.GetLeft());
}

/**
 * @tc.name: RectFCreateAndDestroy004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestroy004, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(1.0f, rectF->GetLeft());
    EXPECT_EQ(2.0f, rectF->GetTop());
    EXPECT_EQ(3.0f, rectF->GetRight());
    EXPECT_EQ(4.0f, rectF->GetBottom());
}

/**
 * @tc.name: RectFCreateAndDestroy005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestroy005, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(4.0f, 3.0f, 2.0f, 1.0f);
    EXPECT_EQ(4.0f, rectF->GetLeft());
    EXPECT_EQ(3.0f, rectF->GetTop());
    EXPECT_EQ(2.0f, rectF->GetRight());
    EXPECT_EQ(1.0f, rectF->GetBottom());
}

/**
 * @tc.name: RectFIsValid001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFIsValid001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(4.0f, 3.0f, 2.0f, 1.0f);
    EXPECT_FALSE(rectF->IsValid());
}

/**
 * @tc.name: RectFIsValid002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFIsValid002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE(rectF->IsValid());
}

/**
 * @tc.name: RectFSetAndGetLeft001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFSetAndGetLeft001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetLeft(1.0f);
    EXPECT_EQ(1.0f, rectF->GetLeft());
}

/**
 * @tc.name: RectFSetAndGetLeft002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFSetAndGetLeft002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetLeft(2.0f);
    EXPECT_EQ(2.0f, rectF->GetLeft());
}

/**
 * @tc.name: RectFSetAndGetRight001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFSetAndGetRight001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetRight(1.0f);
    EXPECT_EQ(1.0f, rectF->GetRight());
}

/**
 * @tc.name: RectFSetAndGetRight002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFSetAndGetRight002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetRight(2.0f);
    EXPECT_EQ(2.0f, rectF->GetRight());
}

/**
 * @tc.name: RectFSetAndGetTop001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFSetAndGetTop001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetTop(1.0f);
    EXPECT_EQ(1.0f, rectF->GetTop());
}

/**
 * @tc.name: RectFSetAndGetTop002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFSetAndGetTop002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetTop(2.0f);
    EXPECT_EQ(2.0f, rectF->GetTop());
}

/**
 * @tc.name: RectFSetAndGetBottom001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFSetAndGetBottom001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetBottom(1.0f);
    EXPECT_EQ(1.0f, rectF->GetBottom());
}

/**
 * @tc.name: RectFSetAndGetBottom002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFSetAndGetBottom002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetBottom(2.0f);
    EXPECT_EQ(2.0f, rectF->GetBottom());
}

/**
 * @tc.name: RectFOffset001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFOffset001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    rectF->Offset(1.0f, 2.0f);
    EXPECT_EQ(2.0f, rectF->GetLeft());
    EXPECT_EQ(4.0f, rectF->GetRight());
    EXPECT_EQ(4.0f, rectF->GetTop());
    EXPECT_EQ(6.0f, rectF->GetBottom());
}

/**
 * @tc.name: RectFOffset002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFOffset002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    rectF->Offset(2.0f, 1.0f);
    EXPECT_EQ(3.0f, rectF->GetLeft());
    EXPECT_EQ(5.0f, rectF->GetRight());
    EXPECT_EQ(3.0f, rectF->GetTop());
    EXPECT_EQ(5.0f, rectF->GetBottom());
}

/**
 * @tc.name: RectFMakeOutset001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFMakeOutset001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    rectF->MakeOutset(1.0f, 2.0f);
    EXPECT_EQ(0.0f, rectF->GetLeft());
    EXPECT_EQ(4.0f, rectF->GetRight());
    EXPECT_EQ(0.0f, rectF->GetTop());
    EXPECT_EQ(6.0f, rectF->GetBottom());
}

/**
 * @tc.name: RectFMakeOutset002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFMakeOutset002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    rectF->MakeOutset(2.0f, 1.0f);
    EXPECT_EQ(-1.0f, rectF->GetLeft());
    EXPECT_EQ(5.0f, rectF->GetRight());
    EXPECT_EQ(1.0f, rectF->GetTop());
    EXPECT_EQ(5.0f, rectF->GetBottom());
}


/**
 * @tc.name: RectFEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFEqual001, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    rectf1.SetLeft(1.0f);
    EXPECT_FALSE(rectf1 == rectf2);
}

/**
 * @tc.name: RectFEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFEqual002, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    EXPECT_TRUE(rectf1 == rectf2);
}

/**
 * @tc.name: RectFNotEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFNotEqual001, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    EXPECT_FALSE(rectf1 != rectf2);
}

/**
 * @tc.name: RectFNotEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectFNotEqual002, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    rectf2.SetLeft(2.0f);
    EXPECT_TRUE(rectf1 != rectf2);
}

/**
 * @tc.name: RectIntersect001
 * @tc.desc: test for seting RectF to intersection.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIntersect001, TestSize.Level1)
{
    RectF rectf1(1.0f, 2.0f, 3.0f, 4.0f);
    RectF rectf2;
    EXPECT_FALSE(rectf2.Intersect(rectf1));
}

/**
 * @tc.name: RectIntersect002
 * @tc.desc: test for seting RectF to intersection.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIntersect002, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    EXPECT_FALSE(rectf2.Intersect(rectf1));
}

/**
 * @tc.name: RectIntersect003
 * @tc.desc: test for seting RectF to intersection.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIntersect003, TestSize.Level1)
{
    RectF rectf1(1.0f, 2.0f, 3.0f, 4.0f);
    RectF rectf2(1.0f, 2.0f, 3.0f, 5.0f);
    EXPECT_TRUE(rectf2.Intersect(rectf1));
}

/**
 * @tc.name: RectJoin001
 * @tc.desc: test for seting RectF to the union of itself and other.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectJoin001, TestSize.Level1)
{
    RectF rectf1(1.0f, 2.0f, 3.0f, 4.0f);
    RectF rectf2;
    EXPECT_TRUE(rectf2.Join(rectf1));
}

/**
 * @tc.name: RectJoin002
 * @tc.desc: test for seting RectF to the union of itself and other.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectJoin002, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    EXPECT_FALSE(rectf2.Join(rectf1));
}

/**
 * @tc.name: RectJoin003
 * @tc.desc: test for seting RectF to the union of itself and other.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectJoin003, TestSize.Level1)
{
    RectF rectf1(2.0f, 4.0f, 6.0f, 8.0f);
    RectF rectf2(1.0f, 2.0f, 3.0f, 5.0f);
    EXPECT_TRUE(rectf2.Join(rectf1));
}

/**
 * @tc.name: RectICreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestroy001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    EXPECT_EQ(0.0f, rectI->GetLeft());
    EXPECT_EQ(0.0f, rectI->GetTop());
    EXPECT_EQ(0.0f, rectI->GetRight());
    EXPECT_EQ(0.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectICreateAndDestroy002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestroy002, TestSize.Level1)
{
    RectI recti1;
    recti1.SetLeft(1.0f);
    RectI recti2(recti1);
    EXPECT_EQ(recti1.GetLeft(), recti2.GetLeft());
}

/**
 * @tc.name: RectICreateAndDestroy003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestroy003, TestSize.Level1)
{
    RectI recti1;
    recti1.SetLeft(2.0f);
    RectI recti2(recti1);
    EXPECT_EQ(recti1.GetLeft(), recti2.GetLeft());
}

/**
 * @tc.name: RectICreateAndDestroy004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestroy004, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(1.0f, rectI->GetLeft());
    EXPECT_EQ(2.0f, rectI->GetTop());
    EXPECT_EQ(3.0f, rectI->GetRight());
    EXPECT_EQ(4.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectICreateAndDestroy005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestroy005, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(4.0f, 3.0f, 2.0f, 1.0f);
    EXPECT_EQ(4.0f, rectI->GetLeft());
    EXPECT_EQ(3.0f, rectI->GetTop());
    EXPECT_EQ(2.0f, rectI->GetRight());
    EXPECT_EQ(1.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectIIsValid001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectIIsValid001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(4.0f, 3.0f, 2.0f, 1.0f);
    EXPECT_FALSE(rectI->IsValid());
}

/**
 * @tc.name: RectIIsValid002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectIIsValid002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE(rectI->IsValid());
}

/**
 * @tc.name: RectISetAndGetLeft001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectISetAndGetLeft001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetLeft(1.0f);
    EXPECT_EQ(1.0f, rectI->GetLeft());
}

/**
 * @tc.name: RectISetAndGetLeft002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectISetAndGetLeft002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetLeft(2.0f);
    EXPECT_EQ(2.0f, rectI->GetLeft());
}

/**
 * @tc.name: RectISetAndGetRight001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectISetAndGetRight001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetRight(1.0f);
    EXPECT_EQ(1.0f, rectI->GetRight());
}

/**
 * @tc.name: RectISetAndGetRight002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectISetAndGetRight002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetRight(2.0f);
    EXPECT_EQ(2.0f, rectI->GetRight());
}

/**
 * @tc.name: RectISetAndGetTop001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectISetAndGetTop001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetTop(1.0f);
    EXPECT_EQ(1.0f, rectI->GetTop());
}

/**
 * @tc.name: RectISetAndGetTop002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectISetAndGetTop002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetTop(2.0f);
    EXPECT_EQ(2.0f, rectI->GetTop());
}

/**
 * @tc.name: RectISetAndGetBottom001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectISetAndGetBottom001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetBottom(1.0f);
    EXPECT_EQ(1.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectISetAndGetBottom002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectISetAndGetBottom002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetBottom(2.0f);
    EXPECT_EQ(2.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectIOffset001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectIOffset001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    rectI->Offset(1.0f, 2.0f);
    EXPECT_EQ(2.0f, rectI->GetLeft());
    EXPECT_EQ(4.0f, rectI->GetRight());
    EXPECT_EQ(4.0f, rectI->GetTop());
    EXPECT_EQ(6.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectIOffset002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectIOffset002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    rectI->Offset(2.0f, 1.0f);
    EXPECT_EQ(3.0f, rectI->GetLeft());
    EXPECT_EQ(5.0f, rectI->GetRight());
    EXPECT_EQ(3.0f, rectI->GetTop());
    EXPECT_EQ(5.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectIMakeOutset001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectIMakeOutset001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    rectI->MakeOutset(1.0f, 2.0f);
    EXPECT_EQ(0.0f, rectI->GetLeft());
    EXPECT_EQ(4.0f, rectI->GetRight());
    EXPECT_EQ(0.0f, rectI->GetTop());
    EXPECT_EQ(6.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectIMakeOutset002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectIMakeOutset002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    rectI->MakeOutset(2.0f, 1.0f);
    EXPECT_EQ(-1.0f, rectI->GetLeft());
    EXPECT_EQ(5.0f, rectI->GetRight());
    EXPECT_EQ(1.0f, rectI->GetTop());
    EXPECT_EQ(5.0f, rectI->GetBottom());
}

/**
 * @tc.name: RectIEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectIEqual001, TestSize.Level1)
{
    RectI recti1;
    RectI recti2;
    recti1.SetLeft(1.0f);
    EXPECT_FALSE(recti1 == recti2);
}

/**
 * @tc.name: RectIEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectIEqual002, TestSize.Level1)
{
    RectI recti1;
    RectI recti2;
    EXPECT_TRUE(recti1 == recti2);
}

/**
 * @tc.name: RectINotEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectINotEqual001, TestSize.Level1)
{
    RectI recti1;
    RectI recti2;
    EXPECT_FALSE(recti1 != recti2);
}

/**
 * @tc.name: RectINotEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, RectINotEqual002, TestSize.Level1)
{
    RectI recti1;
    RectI recti2;
    recti2.SetLeft(2.0f);
    EXPECT_TRUE(recti1 != recti2);
}

/**
 * @tc.name: RectIIntersect001
 * @tc.desc: test for seting RectI to intersection.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIIntersect001, TestSize.Level1)
{
    RectI rectI1(1, 2, 3, 4);
    RectI rectI2;
    EXPECT_FALSE(rectI2.Intersect(rectI1));
}

/**
 * @tc.name: RectIIntersect002
 * @tc.desc: test for seting RectI to intersection.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIIntersect002, TestSize.Level1)
{
    RectI rectI1;
    RectI rectI2;
    EXPECT_FALSE(rectI2.Intersect(rectI1));
}

/**
 * @tc.name: RectIIntersect003
 * @tc.desc: test for seting RectI to intersection.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIIntersect003, TestSize.Level1)
{
    RectI rectI1(1, 2, 3, 4);
    RectI rectI2(1, 2, 3, 5);
    EXPECT_TRUE(rectI2.Intersect(rectI1));
}

/**
 * @tc.name: RectIJoin001
 * @tc.desc: test for seting RectI to the union of itself and other.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIJoin001, TestSize.Level1)
{
    RectI rectI1(1, 2, 3, 4);
    RectI rectI2;
    EXPECT_TRUE(rectI2.Join(rectI1));
}

/**
 * @tc.name: RectIJoin002
 * @tc.desc: test for seting RectI to the union of itself and other.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIJoin002, TestSize.Level1)
{
    RectI rectI1;
    RectI rectI2;
    EXPECT_FALSE(rectI2.Join(rectI1));
}

/**
 * @tc.name: RectIJoin003
 * @tc.desc: test for seting RectI to the union of itself and other.
 * @tc.type: FUNC
 * @tc.require: I6ZMMK
 */
HWTEST_F(RectTest, RectIJoin003, TestSize.Level1)
{
    RectI rectI1(2, 4, 6, 8);
    RectI rectI2(1, 2, 3, 5);
    EXPECT_TRUE(rectI2.Join(rectI1));
    EXPECT_EQ(rectI2.GetLeft(), 1);
    EXPECT_EQ(rectI2.GetBottom(), 8);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS