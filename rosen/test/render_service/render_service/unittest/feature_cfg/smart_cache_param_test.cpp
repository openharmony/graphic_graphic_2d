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

#include "smart_cache_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class SmartCacheParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SmartCacheParamTest::SetUpTestCase() {}
void SmartCacheParamTest::TearDownTestCase() {}
void SmartCacheParamTest::SetUp() {}
void SmartCacheParamTest::TearDown() {}

/**
 * @tc.name: SetEnabled
 * @tc.desc: Verify the SetEnabled function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SmartCacheParamTest, SetEnabled, Function | SmallTest | Level1)
{
    SmartCacheParam::SetEnabled("true");
    ASSERT_EQ(SmartCacheParam::IsEnabled(), "true");
    SmartCacheParam::SetEnabled("false");
    ASSERT_EQ(SmartCacheParam::IsEnabled(), "false");
}

/**
 * @tc.name: SetUMDPoolSize
 * @tc.desc: Verify the SetUMDPoolSize function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SmartCacheParamTest, SetUMDPoolSize, Function | SmallTest | Level1)
{
    SmartCacheParam::SetUMDPoolSize("100");
    ASSERT_EQ(SmartCacheParam::GetUMDPoolSize(), "100");
}

/**
 * @tc.name: SetTimeInterval
 * @tc.desc: Verify the SetTimeInterval function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SmartCacheParamTest, SetTimeInterval, Function | SmallTest | Level1)
{
    SmartCacheParam::SetTimeInterval("200");
    ASSERT_EQ(SmartCacheParam::GetTimeInterval(), "200");
}
} // namespace Rosen
} // namespace OHOS