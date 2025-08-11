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

#include <gtest/gtest.h>
#include <test_header.h>

#include "subtree_parallel_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class SubtreeParallelParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SubtreeParallelParamTest::SetUpTestCase() {}
void SubtreeParallelParamTest::TearDownTestCase() {}
void SubtreeParallelParamTest::SetUp() {}
void SubtreeParallelParamTest::TearDown() {}

/**
 * @tc.name: SetSubtreeEnable
 * @tc.desc: Verify the SetSubtreeEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(SubtreeParallelParamTest, SetSubtreeEnable, Function | SmallTest | Level1)
{
    SubtreeParallelParam::SetSubtreeEnable(true);
    ASSERT_EQ(SubtreeParallelParam::GetSubtreeEnable(), true);

    SubtreeParallelParam::SetRBPolicyEnabled(true);
    ASSERT_EQ(SubtreeParallelParam::GetRBPolicyEnabled(), true);

    SubtreeParallelParam::SetMultiWinPolicyEnabled(true);
    ASSERT_EQ(SubtreeParallelParam::GetMultiWinPolicyEnabled(), true);

    SubtreeParallelParam::SetMutliWinSurfaceNum(100);
    ASSERT_EQ(SubtreeParallelParam::GetMutliWinSurfaceNum(), 100);

    SubtreeParallelParam::SetRBChildrenWeight(200);
    ASSERT_EQ(SubtreeParallelParam::GetRBChildrenWeight(), 200);

    SubtreeParallelParam::SetRBSubtreeWeight(300);
    ASSERT_EQ(SubtreeParallelParam::GetRBSubtreeWeight(), 300);

    SubtreeParallelParam::SetSubtreeScene("test", "1");
    ASSERT_EQ(SubtreeParallelParam::GetSubtreeScene()["test"], "1");
}
} // namespace Rosen
} // namespace OHOS