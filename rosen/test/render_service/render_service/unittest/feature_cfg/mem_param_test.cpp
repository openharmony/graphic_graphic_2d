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

#include "mem_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class MEMParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MEMParamTest::SetUpTestCase() {}
void MEMParamTest::TearDownTestCase() {}
void MEMParamTest::SetUp() {}
void MEMParamTest::TearDown() {}

/**
 * @tc.name: SetRSWatchPoint
 * @tc.desc: Verify the SetRSWatchPoint function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(MEMParamTest, SetRSWatchPoint, Function | SmallTest | Level1)
{
    MEMParam memParam;
    memParam.SetRSWatchPoint("RsWatchPoint");
    EXPECT_EQ(memParam.GetRSWatchPoint(), "RsWatchPoint");
}
} // namespace Rosen
} // namespace OHOS