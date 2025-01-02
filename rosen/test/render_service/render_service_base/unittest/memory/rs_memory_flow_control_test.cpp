/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "memory/rs_memory_flow_control.h"

namespace OHOS::Rosen {
class RSMemoryFlowControlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMemoryFlowControlTest::SetUpTestCase() {}
void RSMemoryFlowControlTest::TearDownTestCase() {}
void RSMemoryFlowControlTest::SetUp() {}
void RSMemoryFlowControlTest::TearDown() {}

/**
 * @tc.name: AddAshmemStatisticTest001
 * @tc.desc: test MemoryFlowControl functions
 * @tc.type: FUNC
 * @tc.require: issue#IB97OM
 */
HWTEST_F(RSMemoryFlowControlTest, AddAshmemStatisticTest001, testing::ext::TestSize.Level1)
{
    MemoryFlowControl& instance = MemoryFlowControl::Instance();
    pid_t callingPid = 0;
    uint32_t bufferSize = 0;
    EXPECT_TRUE(instance.AddAshmemStatistic(callingPid, bufferSize));
    callingPid = 1;
    EXPECT_TRUE(instance.AddAshmemStatistic(callingPid, bufferSize));
    bufferSize = std::numeric_limits<uint32_t>::max();
    EXPECT_FALSE(instance.AddAshmemStatistic(callingPid, bufferSize));
    callingPid = 2;
    const uint32_t ashmemFlowControlThreshold = MemoryFlowControl::GetAshmemFlowControlThreshold();
    EXPECT_TRUE(ashmemFlowControlThreshold >= 12u);
    bufferSize = ashmemFlowControlThreshold / 4u;
    EXPECT_TRUE(instance.AddAshmemStatistic(callingPid, bufferSize));
    bufferSize = ashmemFlowControlThreshold / 3u;
    EXPECT_TRUE(instance.AddAshmemStatistic(callingPid, bufferSize));
    bufferSize = ashmemFlowControlThreshold / 2u;
    EXPECT_FALSE(instance.AddAshmemStatistic(callingPid, bufferSize));
}

/**
 * @tc.name: RemoveAshmemStatisticTest001
 * @tc.desc: test MemoryFlowControl functions
 * @tc.type: FUNC
 * @tc.require: issue#IB97OM
 */
HWTEST_F(RSMemoryFlowControlTest, RemoveAshmemStatisticTest001, testing::ext::TestSize.Level1)
{
    MemoryFlowControl& instance = MemoryFlowControl::Instance();
    pid_t callingPid = 0;
    uint32_t bufferSize = 0;
    instance.RemoveAshmemStatistic(callingPid, bufferSize);
    callingPid = 1;
    instance.RemoveAshmemStatistic(callingPid, bufferSize);
    const uint32_t ashmemFlowControlThreshold = MemoryFlowControl::GetAshmemFlowControlThreshold();
    EXPECT_TRUE(ashmemFlowControlThreshold >= 12u);
    bufferSize = ashmemFlowControlThreshold / 4u;
    instance.RemoveAshmemStatistic(callingPid, bufferSize);
    EXPECT_TRUE(instance.AddAshmemStatistic(callingPid, bufferSize));
    instance.RemoveAshmemStatistic(callingPid, bufferSize);
    EXPECT_TRUE(instance.AddAshmemStatistic(callingPid, bufferSize));
    bufferSize = ashmemFlowControlThreshold / 3u;
    EXPECT_TRUE(instance.AddAshmemStatistic(callingPid, bufferSize));
    instance.RemoveAshmemStatistic(callingPid, bufferSize);
}

/**
 * @tc.name: CheckOverflowAndCreateInstanceTest001
 * @tc.desc: test AshmemFlowControlUnit functions
 * @tc.type: FUNC
 * @tc.require: issue#IBABHE
 */
HWTEST_F(RSMemoryFlowControlTest, CheckOverflowAndCreateInstanceTest001, testing::ext::TestSize.Level1)
{
    pid_t callingPid = 0;
    uint32_t bufferSize = 0;
    auto ashmemFlowControlUnit = AshmemFlowControlUnit::CheckOverflowAndCreateInstance(callingPid, bufferSize);
    EXPECT_TRUE(ashmemFlowControlUnit);
}

/**
 * @tc.name: CheckOverflowAndCreateInstanceTest002
 * @tc.desc: test AshmemFlowControlUnit functions
 * @tc.type: FUNC
 * @tc.require: issue#IBABHE
 */
HWTEST_F(RSMemoryFlowControlTest, CheckOverflowAndCreateInstanceTest002, testing::ext::TestSize.Level1)
{
    pid_t callingPid = 1;
    uint32_t bufferSize = std::numeric_limits<uint32_t>::max();
    auto ashmemFlowControlUnit = AshmemFlowControlUnit::CheckOverflowAndCreateInstance(callingPid, bufferSize);
    EXPECT_FALSE(ashmemFlowControlUnit);
}
} // namespace OHOS::Rosen