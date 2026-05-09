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

#include <gtest/gtest.h>
#include <test_header.h>

#include "behind_window_filter_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class BehindWindowFilterParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BehindWindowFilterParamTest::SetUpTestCase() {}
void BehindWindowFilterParamTest::TearDownTestCase() {}
void BehindWindowFilterParamTest::SetUp()
{
    BehindWindowFilterParam::SetBehindWindowFilterEnable(true);
}
void BehindWindowFilterParamTest::TearDown() {}

/**
 * @tc.name: BehindWindowFilterEnableTest001
 * @tc.desc: Verify SetBehindWindowFilterEnable can re-enable the filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BehindWindowFilterParamTest, BehindWindowFilterEnableTest001, Function | SmallTest | Level1)
{
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), true);
    BehindWindowFilterParam::SetBehindWindowFilterEnable(false);
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), false);
    BehindWindowFilterParam::SetBehindWindowFilterEnable(true);
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), true);
}
} // namespace Rosen
} // namespace OHOS
