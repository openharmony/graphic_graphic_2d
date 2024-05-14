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

#include "command/rs_message_processor.h"
#include "include/command/rs_command_templates.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSMessageProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMessageProcessorTest::SetUpTestCase() {}
void RSMessageProcessorTest::TearDownTestCase() {}
void RSMessageProcessorTest::SetUp() {}
void RSMessageProcessorTest::TearDown() {}

/**
 * @tc.name: testing
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSMessageProcessorTest, testing, TestSize.Level1)
{
    uint32_t pid = 1;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());

    RSMessageProcessor::Instance().HasTransaction(pid);
    EXPECT_NE(nullptr, RSMessageProcessor::Instance().GetTransaction(pid));
    RSMessageProcessor::Instance().GetAllTransactions();

    RSMessageProcessor::Instance().GetTransaction(0);
    EXPECT_EQ(nullptr, RSMessageProcessor::Instance().GetTransaction(0));
}

/**
 * @tc.name: ReInitializeMovedMap001
 * @tc.desc: test results of ReInitializeMovedMap
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSMessageProcessorTest, ReInitializeMovedMap001, TestSize.Level1)
{
    RSMessageProcessor::Instance().ReInitializeMovedMap();
    EXPECT_EQ(false, RSMessageProcessor::Instance().HasTransaction());
}

/**
 * @tc.name: AddUIMessage001
 * @tc.desc: test results of AddUIMessage
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSMessageProcessorTest, AddUIMessage001, TestSize.Level1)
{
    uint32_t pid = 1;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());
    EXPECT_EQ(nullptr, RSMessageProcessor::Instance().GetTransaction(0));

    pid = 0;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());
}
} // namespace Rosen
} // namespace OHOS