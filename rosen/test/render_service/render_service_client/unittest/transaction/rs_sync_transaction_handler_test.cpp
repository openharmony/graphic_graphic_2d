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

#include "gtest/gtest.h"

#include "transaction/rs_sync_transaction_handler.h"
#include "transaction/rs_transaction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSSyncTransactionHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSyncTransactionHandlerTest::SetUpTestCase() {}
void RSSyncTransactionHandlerTest::TearDownTestCase() {}
void RSSyncTransactionHandlerTest::SetUp() {}
void RSSyncTransactionHandlerTest::TearDown() {}

/**
 * @tc.name: GetRSTransaction001
 * @tc.desc: test results of GetRSTransaction
 * @tc.type: FUNC
 * @tc.require: issueIBRL62
 */
HWTEST_F(RSSyncTransactionHandlerTest, GetRSTransaction001, TestSize.Level1)
{
    auto syncTransaction = std::make_shared<RSSyncTransactionHandler>();
    std::shared_ptr<RSTransaction> ptr = syncTransaction->GetRSTransaction();
    EXPECT_TRUE(ptr == nullptr);

    auto transation = std::make_shared<RSTransactionHandler>();
    syncTransaction->SetTransactionHandler(transation);
    syncTransaction->needCloseSync_ = true;
    ptr = syncTransaction->GetRSTransaction();
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: OpenSyncTransaction001
 * @tc.desc: test results of OpenSyncTransaction
 * @tc.type: FUNC
 * @tc.require: issueIBRL62
 */
HWTEST_F(RSSyncTransactionHandlerTest, OpenSyncTransaction001, TestSize.Level1)
{
    auto syncTransaction = std::make_shared<RSSyncTransactionHandler>();
    syncTransaction->needCloseSync_ = false;
    syncTransaction->OpenSyncTransaction();
    EXPECT_TRUE(syncTransaction->needCloseSync_);

    syncTransaction->OpenSyncTransaction();
    EXPECT_TRUE(syncTransaction->needCloseSync_);

    syncTransaction->rsTransaction_ = nullptr;
    syncTransaction->needCloseSync_ = false;
    syncTransaction->OpenSyncTransaction();
}

/**
 * @tc.name: CloseSyncTransaction001
 * @tc.desc: test results of CloseSyncTransaction
 * @tc.type: FUNC
 * @tc.require: issueIBRL62
 */
HWTEST_F(RSSyncTransactionHandlerTest, CloseSyncTransaction001, TestSize.Level1)
{
    auto syncTransaction = std::make_shared<RSSyncTransactionHandler>();
    syncTransaction->needCloseSync_ = true;
    syncTransaction->CloseSyncTransaction();
    EXPECT_TRUE(!syncTransaction->needCloseSync_);

    syncTransaction->CloseSyncTransaction();
    EXPECT_TRUE(!syncTransaction->needCloseSync_);

    syncTransaction->needCloseSync_ = true;
    syncTransaction->rsTransaction_ = std::make_shared<RSTransaction>();
    syncTransaction->CloseSyncTransaction();
    EXPECT_TRUE(syncTransaction->rsTransaction_ != nullptr);
    EXPECT_TRUE(!syncTransaction->needCloseSync_);
}
} // namespace OHOS::Rosen
