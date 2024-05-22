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

#include "gtest/gtest.h"

#include "common/rs_thread_looper.h"

#include "common/rs_thread_looper_generic.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSThreadLooperGenericTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSThreadLooperGenericTest::SetUpTestCase() {}
void RSThreadLooperGenericTest::TearDownTestCase() {}
void RSThreadLooperGenericTest::SetUp() {}
void RSThreadLooperGenericTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: test results of Create
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSThreadLooperGenericTest, Create001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create
     */
    auto rsThreadLooperGeneric = RSThreadLooper::Create();
    ASSERT_NE(rsThreadLooperGeneric, nullptr);
}

/**
 * @tc.name: ProcessOneMessageTest
 * @tc.desc: Verify function ProcessOneMessage
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperGenericTest, ProcessOneMessageTest, TestSize.Level1)
{
    auto rsThreadLooperGeneric = std::make_shared<RSThreadLooperGeneric>();
    rsThreadLooperGeneric->ProcessOneMessage(0);
    EXPECT_NE(rsThreadLooperGeneric->looper_, nullptr);
}

/**
 * @tc.name: ProcessAllMessagesTest
 * @tc.desc: Verify function ProcessAllMessages
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperGenericTest, ProcessAllMessagesTest, TestSize.Level1)
{
    auto rsThreadLooperGeneric = std::make_shared<RSThreadLooperGeneric>();
    rsThreadLooperGeneric->ProcessAllMessages(0);
    EXPECT_NE(rsThreadLooperGeneric->looper_, nullptr);
}

/**
 * @tc.name: WakeUpTest
 * @tc.desc: Verify function WakeUp
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperGenericTest, WakeUpTest, TestSize.Level1)
{
    auto rsThreadLooperGeneric = std::make_shared<RSThreadLooperGeneric>();
    rsThreadLooperGeneric->WakeUp();
    EXPECT_TRUE(rsThreadLooperGeneric->looper_->wakeup_);
}
} // namespace OHOS::Rosen