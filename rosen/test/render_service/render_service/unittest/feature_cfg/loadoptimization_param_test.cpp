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

#include "loadoptimization_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class LoadOptimizationParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void LoadOptimizationParamTest::SetUpTestCase() {}
void LoadOptimizationParamTest::TearDownTestCase() {}
void LoadOptimizationParamTest::SetUp() {}
void LoadOptimizationParamTest::TearDown() {}

/**
 * @tc.name: SetMergeFenceSkipped001
 * @tc.desc: Verify the SetMergeFenceSkipped function
 * @tc.type: FUNC
 * @tc.require: #IC2UGT
 */
HWTEST_F(LoadOptimizationParamTest, SetMergeFenceSkipped001, Function | SmallTest | Level1)
{
    LoadOptimizationParam::SetMergeFenceSkipped(false);
    EXPECT_EQ(LoadOptimizationParam::IsMergeFenceSkipped(), false);
    LoadOptimizationParam::SetMergeFenceSkipped(true);
    EXPECT_EQ(LoadOptimizationParam::IsMergeFenceSkipped(), true);
}
} // namespace Rosen
} // namespace OHOS