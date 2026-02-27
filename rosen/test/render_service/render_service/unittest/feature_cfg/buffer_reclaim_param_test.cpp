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

#include "buffer_reclaim_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class BufferReclaimParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BufferReclaimParamTest::SetUpTestCase() {}
void BufferReclaimParamTest::TearDownTestCase() {}
void BufferReclaimParamTest::SetUp() {}
void BufferReclaimParamTest::TearDown() {}

/**
 * @tc.name: SetBufferReclaimEnable
 * @tc.desc: Verify the SetBufferReclaimEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BufferReclaimParamTest, SetBufferReclaimEnable, Function | SmallTest | Level1)
{
    BufferReclaimParam::GetInstance().SetBufferReclaimEnable(true);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), true);
    BufferReclaimParam::GetInstance().SetBufferReclaimEnable(false);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);
}
} // namespace Rosen
} // namespace OHOS