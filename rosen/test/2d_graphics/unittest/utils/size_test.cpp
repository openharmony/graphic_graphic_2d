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
#include "utils/size.h"
#include "utils/scalar.h"
#include "utils/rect.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SizeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SizeTest::SetUpTestCase() {}
void SizeTest::TearDownTestCase() {}
void SizeTest::SetUp() {}
void SizeTest::TearDown() {}

/**
 * @tc.name: SizeCreateAndDestory001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeCreateAndDestory001, TestSize.Level1)
{
    // The best way to create SizeF.
    std::unique_ptr<SizeF> sizeF = std::make_unique<SizeF>();
    ASSERT_TRUE(sizeF != nullptr);
    std::unique_ptr<SizeI> sizeI = std::make_unique<SizeI>();
    ASSERT_TRUE(sizeI != nullptr);
}

/**
 * @tc.name: SizeCreateAndDestory002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeCreateAndDestory002, TestSize.Level1)
{
    // The best way to create SizeF.
    const SizeF sizef1;
    std::unique_ptr<SizeF> sizeF = std::make_unique<SizeF>(sizef1);
    ASSERT_TRUE(sizeF != nullptr);
    const SizeI sizei1;
    std::unique_ptr<SizeI> sizeI = std::make_unique<SizeI>(sizei1);
    ASSERT_TRUE(sizeI != nullptr);
}

/**
 * @tc.name: SizeFIsZeroTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFIsZeroTest001, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    ASSERT_TRUE(size->IsZero());
}

/**
 * @tc.name: SizeFIsZeroTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFIsZeroTest002, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    ASSERT_FALSE(! size->IsZero());
}

/**
 * @tc.name: SizeFIsEmptyTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFIsEmptyTest001, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    ASSERT_TRUE(size->IsEmpty());
}

/**
 * @tc.name: SizeFIsEmptyTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFIsEmptyTest002, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    ASSERT_FALSE(! size->IsEmpty());
}

/**
 * @tc.name: SizeFGetAndSetWidth001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetWidth001, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    size->SetWidth(92.5f);
    float width = size->Width();
    EXPECT_EQ(width, 92.5f);
}

/**
 * @tc.name: SizeFGetAndSetWidth002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetWidth002, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    size->SetWidth(162.5f);
    float width = size->Width();
    EXPECT_EQ(width, 162.5f);
}

/**
 * @tc.name: SizeFGetAndSetWidth003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetWidth003, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    size->SetWidth(102.5f);
    float width = size->Width();
    EXPECT_EQ(width, 102.5f);
}

/**
 * @tc.name: SizeFGetAndSetHeight001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetHeight001, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    size->SetHeight(102.5f);
    float height = size->Height();
    EXPECT_EQ(height, 102.5f);
}

/**
 * @tc.name: SizeFGetAndSetHeight002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetHeight002, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    size->SetHeight(131.5f);
    float height = size->Height();
    EXPECT_EQ(height, 131.5f);
}

/**
 * @tc.name: SizeFGetAndSetHeight003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetHeight003, TestSize.Level1)
{
    std::unique_ptr<SizeF> size = std::make_unique<SizeF>();
    ASSERT_TRUE(size != nullptr);
    size->SetHeight(50.0f);
    int height = size->Height();
    EXPECT_EQ(height, 50.0f);
}

/**
 * @tc.name: SizeOperatorEqualTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeOperatorEqualTest001, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizeF = std::make_unique<SizeF>();
    ASSERT_TRUE(sizeF != nullptr);
    std::unique_ptr<SizeI> sizeI = std::make_unique<SizeI>();
    ASSERT_TRUE(sizeI != nullptr);
    EXPECT_TRUE(sizeF == sizeF);
    EXPECT_TRUE(sizeI == sizeI);
}

/**
 * @tc.name: SizeOperatorEqualTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeOperatorEqualTest002, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizeF = std::make_unique<SizeF>();
    std::unique_ptr<SizeF> sizeF2 = std::make_unique<SizeF>(1.0f, 1.1f);
    ASSERT_TRUE(sizeF != nullptr);
    ASSERT_TRUE(sizeF2 != nullptr);
    std::unique_ptr<SizeI> sizeI = std::make_unique<SizeI>();
    std::unique_ptr<SizeI> sizeI2 = std::make_unique<SizeI>(2.0f, 3.0f);
    ASSERT_TRUE(sizeI != nullptr);
    ASSERT_TRUE(sizeI2 != nullptr);
    EXPECT_TRUE(sizeF != sizeF2);
    EXPECT_TRUE(sizeI != sizeI2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
