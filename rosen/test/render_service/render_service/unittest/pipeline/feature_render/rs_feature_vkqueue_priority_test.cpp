/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "feature/uifirst/rs_sub_thread.h"
#include "gtest/gtest.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFeatureVkQueuePriorityTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFeatureVkQueuePriorityTest::SetUpTestCase() {}
void RSFeatureVkQueuePriorityTest::TearDownTestCase() {}
void RSFeatureVkQueuePriorityTest::SetUp() {}
void RSFeatureVkQueuePriorityTest::TearDown() {}

/*
 * @tc.name: Init
 * @tc.desc: Test For Init stage
 * @tc.type: FUNC
 * @tc.require: issueIAKPNB
 */
HWTEST_F(RSFeatureVkQueuePriorityTest, Init_001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    auto ret1 = rsVulkanInterface.CreateDrawingContext(false);
    EXPECT_TRUE(ret1 != nullptr);

    auto ret2 = rsVulkanInterface.CreateDrawingContext(true);
    EXPECT_TRUE(ret2 != nullptr);

    auto curThread = std::make_shared<RSSubThread>(nullptr, 0);
    ASSERT_TRUE(curThread != nullptr);
    curThread->ResetGrContext();
}
} // namespace OHOS::Rosen