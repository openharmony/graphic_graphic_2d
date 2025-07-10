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

#include "gtest/gtest.h"

#include "transaction/rs_sync_transaction_controller.h"
#include "transaction/rs_transaction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSSyncTransactionControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSyncTransactionControllerTest::SetUpTestCase() {}
void RSSyncTransactionControllerTest::TearDownTestCase() {}
void RSSyncTransactionControllerTest::SetUp() {}
void RSSyncTransactionControllerTest::TearDown() {}

/**
 * @tc.name: GetRSTransaction001
 * @tc.desc: test results of GetRSTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSSyncTransactionControllerTest, GetRSTransaction001, TestSize.Level1)
{
    RSSyncTransactionController* instance = RSSyncTransactionController::GetInstance();
    std::shared_ptr<RSTransaction> ptr = instance->GetRSTransaction();
    EXPECT_TRUE(ptr == nullptr);

    instance->needCloseSync_ = true;
    ptr = instance->GetRSTransaction();
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: OpenSyncTransaction001
 * @tc.desc: test results of OpenSyncTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSSyncTransactionControllerTest, OpenSyncTransaction001, TestSize.Level1)
{
    RSSyncTransactionController* instance = RSSyncTransactionController::GetInstance();
    instance->needCloseSync_ = false;
    instance->OpenSyncTransaction();
    EXPECT_TRUE(instance->needCloseSync_);

    instance->OpenSyncTransaction();
    EXPECT_TRUE(instance->needCloseSync_);

    instance->rsTransaction_ = nullptr;
    instance->needCloseSync_ = false;
    instance->OpenSyncTransaction();
    EXPECT_TRUE(instance->rsTransaction_ == nullptr);
}

/**
 * @tc.name: OpenSyncTransaction002
 * @tc.desc: test results of OpenSyncTransaction
 * @tc.type: FUNC
 * @tc.require: issueICGEDM
 */
HWTEST_F(RSSyncTransactionControllerTest, OpenSyncTransaction002, TestSize.Level1)
{
    RSSyncTransactionController* instance = RSSyncTransactionController::GetInstance();
    instance->needCloseSync_ = false;
    instance->OpenSyncTransaction();
    EXPECT_TRUE(instance->needCloseSync_);
}

/**
 * @tc.name: CloseSyncTransaction001
 * @tc.desc: test results of CloseSyncTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSSyncTransactionControllerTest, CloseSyncTransaction001, TestSize.Level1)
{
    RSSyncTransactionController* instance = RSSyncTransactionController::GetInstance();
    EXPECT_TRUE(instance->needCloseSync_);
    instance->CloseSyncTransaction();
    EXPECT_TRUE(!instance->needCloseSync_);

    instance->CloseSyncTransaction();
    EXPECT_TRUE(!instance->needCloseSync_);

    instance->needCloseSync_ = true;
    instance->rsTransaction_ = std::make_shared<RSTransaction>();
    instance->CloseSyncTransaction();
    EXPECT_TRUE(instance->rsTransaction_ != nullptr);
    EXPECT_TRUE(!instance->needCloseSync_);
}

/**
 * @tc.name: CloseSyncTransaction002
 * @tc.desc: test results of CloseSyncTransaction
 * @tc.type: FUNC
 * @tc.require: issueICGEDM
 */
HWTEST_F(RSSyncTransactionControllerTest, CloseSyncTransaction002, TestSize.Level1)
{
    RSSyncTransactionController* instance = RSSyncTransactionController::GetInstance();
    EXPECT_FALSE(instance->needCloseSync_);
    instance->needCloseSync_ = true;
    instance->OpenSyncTransaction();
    EXPECT_FALSE(instance->needCloseSync_);
}
} // namespace OHOS::Rosen
