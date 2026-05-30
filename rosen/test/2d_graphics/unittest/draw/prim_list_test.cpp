/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "draw/prim_list.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PrimListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PrimListTest::SetUpTestCase() {}
void PrimListTest::TearDownTestCase() {}
void PrimListTest::SetUp() {}
void PrimListTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc: Test for creating and destroying PrimList.
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PrimListTest, CreateAndDestroy001, TestSize.Level1)
{
    auto primList = std::make_unique<PrimList>();
    ASSERT_TRUE(primList != nullptr);
}

/**
 * @tc.name: UpdateAlphaFactor001
 * @tc.desc: Test for updating alpha factor with normal value.
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PrimListTest, UpdateAlphaFactor001, TestSize.Level1)
{
    auto primList = std::make_unique<PrimList>();
    ASSERT_TRUE(primList != nullptr);
    primList->UpdateAlphaFactor(0.5f);
}

/**
 * @tc.name: UpdateAlphaFactor002
 * @tc.desc: Test for updating alpha factor with boundary value 0.
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PrimListTest, UpdateAlphaFactor002, TestSize.Level1)
{
    auto primList = std::make_unique<PrimList>();
    ASSERT_TRUE(primList != nullptr);
    primList->UpdateAlphaFactor(0.0f);
}

/**
 * @tc.name: UpdateAlphaFactor003
 * @tc.desc: Test for updating alpha factor with boundary value 1.
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PrimListTest, UpdateAlphaFactor003, TestSize.Level1)
{
    auto primList = std::make_unique<PrimList>();
    ASSERT_TRUE(primList != nullptr);
    primList->UpdateAlphaFactor(1.0f);
}

/**
 * @tc.name: GetPrimCount001
 * @tc.desc: Test for getting prim count from empty PrimList.
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PrimListTest, GetPrimCount001, TestSize.Level1)
{
    auto primList = std::make_unique<PrimList>();
    ASSERT_TRUE(primList != nullptr);
    EXPECT_EQ(primList->GetPrimCount(), 0);
}

/**
 * @tc.name: GetPrimCountPerType001
 * @tc.desc: Test for getting prim count per type from empty PrimList.
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PrimListTest, GetPrimCountPerType001, TestSize.Level1)
{
    auto primList = std::make_unique<PrimList>();
    ASSERT_TRUE(primList != nullptr);
    auto counts = primList->GetPrimCountPerType();
    EXPECT_TRUE(counts.empty());
}

/**
 * @tc.name: MultipleOperations001
 * @tc.desc: Test for multiple operations on PrimList.
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PrimListTest, MultipleOperations001, TestSize.Level1)
{
    auto primList = std::make_unique<PrimList>();
    ASSERT_TRUE(primList != nullptr);
    primList->UpdateAlphaFactor(0.5f);
    size_t count = primList->GetPrimCount();
    auto counts = primList->GetPrimCountPerType();
    EXPECT_EQ(count, 0);
    EXPECT_TRUE(counts.empty());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
