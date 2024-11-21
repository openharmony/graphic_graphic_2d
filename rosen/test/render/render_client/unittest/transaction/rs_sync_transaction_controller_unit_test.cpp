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

class RsSyncTransactionControllTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsSyncTransactionControllTest::SetUpTestCase() {}
void RsSyncTransactionControllTest::TearDownTestCase() {}
void RsSyncTransactionControllTest::SetUp() {}
void RsSyncTransactionControllTest::TearDown() {}

/**
 * @tc.name: CloseSyncTransaction01
 * @tc.desc: test results of CloseSyncTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RsSyncTransactionControllTest, CloseSyncTransaction01, TestSize.Level1)
{
    RSSyncTransactionController* instances = RSSyncTransactionController::GetInstance();
    EXPECT_TRUE(instances->needCloseSync_);
    instances->CloseSyncTransaction();
    EXPECT_TRUE(!instances->needCloseSync_);

    instances->CloseSyncTransaction();
    EXPECT_TRUE(!instances->needCloseSync_);

    instances->needCloseSync_ = true;
    instances->rsTransaction_ = std::make_shared<RSTransaction>();
    instances->CloseSyncTransaction();
    EXPECT_TRUE(instances->rsTransaction_ != nullptr);
    EXPECT_TRUE(!instances->needCloseSync_);
}

/**
 * @tc.name: GetRSTransaction01
 * @tc.desc: test results of GetRSTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RsSyncTransactionControllTest, GetRSTransaction01, TestSize.Level1)
{
    RSSyncTransactionController* instances = RSSyncTransactionController::GetInstance();
    std::shared_ptr<RSTransaction> ptr = instances->GetRSTransaction();
    EXPECT_TRUE(ptr == nullptr);

    instances->needCloseSync_ = true;
    ptr = instances->GetRSTransaction();
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: OpenSyncTransaction01
 * @tc.desc: test results of OpenSyncTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RsSyncTransactionControllTest, OpenSyncTransaction01, TestSize.Level1)
{
    RSSyncTransactionController* instances = RSSyncTransactionController::GetInstance();
    instances->needCloseSync_ = false;
    instances->OpenSyncTransaction();
    EXPECT_TRUE(instances->needCloseSync_);

    instances->OpenSyncTransaction();
    EXPECT_TRUE(instances->needCloseSync_);

    instances->rsTransaction_ = nullptr;
    instances->needCloseSync_ = false;
    instances->OpenSyncTransaction();
    EXPECT_TRUE(instances->rsTransaction_ == nullptr);
}
} // namespace OHOS::Rosen
