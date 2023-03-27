/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <parameter.h>
#include <parameters.h>

#include "gtest/gtest.h"
#include "system/rs_system_parameters.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSystemParametersTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSystemParametersTest::SetUpTestCase() {}
void RSSystemParametersTest::TearDownTestCase() {}
void RSSystemParametersTest::SetUp() {}
void RSSystemParametersTest::TearDown() {}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSSystemParametersTest, GetCalcCostEnabledTest1, TestSize.Level1)
{
    auto result = RSSystemParameters::GetCalcCostEnabled();
    ASSERT_FALSE(result);
    system::SetParameter("rosen.calcCost.enabled", "1");
    result = RSSystemParameters::GetCalcCostEnabled();
    ASSERT_TRUE(result);
    system::SetParameter("rosen.calcCost.enabled", "0");
}

} // namespace OHOS::Rosen