/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "command/rs_animation_command.h"
#include "ui/rs_ui_context_manager.h"
#include "transaction/rs_transaction.h"
#include "transaction/rs_transaction_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSTransactionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionTest::SetUpTestCase() {}
void RSTransactionTest::TearDownTestCase() {}
void RSTransactionTest::SetUp() {}
void RSTransactionTest::TearDown() {}

/**
 * @tc.name: FlushImplicitTransaction001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, FlushImplicitTransaction001, TestSize.Level1)
{
    // RSRenderThreadClient cannot be created,
    //      which constructor is privated.
    //      Only use its static function.
    RSTransaction::FlushImplicitTransaction();
    EXPECT_NE(RSTransaction::FlushImplicitTransaction, nullptr);
}

/**
 * @tc.name: FlushImplicitTransaction002
 * @tc.desc: test results of FlushImplicitTransaction
 * @tc.type: FUNC
 * @tc.require: issueICGEDM
 */
HWTEST_F(RSTransactionTest, FlushImplicitTransaction002, TestSize.Level1)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    RSTransaction::FlushImplicitTransaction();
    EXPECT_NE(RSTransaction::FlushImplicitTransaction, nullptr);
}

/**
 * @tc.name: Marshalling001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, Marshalling001, TestSize.Level1)
{
    RSTransaction rsTransaction;
    rsTransaction.Begin();
    rsTransaction.OpenSyncTransaction();
    rsTransaction.CloseSyncTransaction();
    rsTransaction.Commit();
    Parcel parcel;
    EXPECT_TRUE(rsTransaction.Marshalling(parcel));
    EXPECT_NE(rsTransaction.Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: SetTransactionHandler
 * @tc.desc: test results of SetTransactionHandler
 * @tc.type: FUNC
 * @tc.require: issueICJ853
 */
HWTEST_F(RSTransactionTest, SetTransactionHandler, TestSize.Level1)
{
    RSTransaction rsTransaction;
    std::shared_ptr<RSTransactionHandler> rsTransactionHandler = std::make_shared<RSTransactionHandler>();
    rsTransaction.SetTransactionHandler(rsTransactionHandler);
    EXPECT_NE(rsTransaction.rsTransactionHandler_, nullptr);
}

/**
 * @tc.name: OpenSyncTransactionTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, OpenSyncTransactionTest001, TestSize.Level1)
{
    auto rsTransaction = std::make_shared<RSTransaction>();
    rsTransaction->rsTransactionHandler_ = std::make_shared<RSTransactionHandler>();
    EXPECT_NE(rsTransaction->GetTransactionHandler(), nullptr);
    EXPECT_EQ(rsTransaction->GetSyncId(), 0);
    rsTransaction->OpenSyncTransaction();
    rsTransaction->syncId_ = 1;
    rsTransaction->OpenSyncTransaction();
}

/**
 * @tc.name: CloseSyncTransactionTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, CloseSyncTransactionTest001, TestSize.Level1)
{
    auto rsTransaction = std::make_shared<RSTransaction>();
    rsTransaction->OpenSyncTransaction();
    EXPECT_TRUE(rsTransaction->GetSyncId() > 0);
    rsTransaction->subSyncTransactions_[0] = 1;
    EXPECT_FALSE(rsTransaction->subSyncTransactions_.empty());
    rsTransaction->CloseSyncTransaction();

    rsTransaction->subSyncTransactions_.clear();
    rsTransaction->subSyncTransactions_[0] = 0;
    rsTransaction->OpenSyncTransaction();
    rsTransaction->CloseSyncTransaction();
    EXPECT_FALSE(rsTransaction->GetSyncId() > 0);
}

/**
 * @tc.name: InnerCloseSyncTransactionTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, InnerCloseSyncTransactionTest001, TestSize.Level1)
{
    auto rsTransaction = std::make_shared<RSTransaction>();
    rsTransaction->rsTransactionHandler_ = std::make_shared<RSTransactionHandler>();
    EXPECT_NE(rsTransaction->GetTransactionHandler(), nullptr);
    EXPECT_EQ(rsTransaction->GetSyncId(), 0);
    rsTransaction->InnerCloseSyncTransaction();
}

/**
 * @tc.name: BeginTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, BeginTest001, TestSize.Level1)
{
    auto rsTransaction = std::make_shared<RSTransaction>();
    rsTransaction->rsTransactionHandler_ = std::make_shared<RSTransactionHandler>();
    EXPECT_NE(rsTransaction->GetTransactionHandler(), nullptr);
    EXPECT_EQ(rsTransaction->GetSyncId(), 0);
    rsTransaction->Begin();
}

/**
 * @tc.name: CommitTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, CommitTest001, TestSize.Level1)
{
    auto rsTransaction = std::make_shared<RSTransaction>();
    rsTransaction->rsTransactionHandler_ = std::make_shared<RSTransactionHandler>();
    EXPECT_NE(rsTransaction->GetTransactionHandler(), nullptr);
    EXPECT_EQ(rsTransaction->GetSyncId(), 0);
    rsTransaction->Commit();
}

/**
 * @tc.name: StartSubTransactionTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, StartSubTransactionTest001, TestSize.Level1)
{
    const uint64_t syncId = 123;
    auto rsTransaction = std::make_shared<RSTransaction>();
    EXPECT_EQ(rsTransaction->GetTransactionHandler(), nullptr);
    rsTransaction->StartSubTransaction(syncId);

    rsTransaction->rsTransactionHandler_ = std::make_shared<RSTransactionHandler>();
    EXPECT_NE(rsTransaction->GetTransactionHandler(), nullptr);
    rsTransaction->StartSubTransaction(syncId);
}

/**
 * @tc.name: AddSubSyncTransactionTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, AddSubSyncTransactionTest001, TestSize.Level1)
{
    const uint64_t syncId = 123;
    const uint64_t token = 1;
    auto rsTransaction = std::make_shared<RSTransaction>();
    auto subRSTransaction = std::make_shared<RSTransaction>();
    EXPECT_EQ(rsTransaction->GetTransactionHandler(), nullptr);
    EXPECT_NE(rsTransaction->GetSyncId(), syncId);
    rsTransaction->AddSubSyncTransaction(subRSTransaction, token, syncId);

    rsTransaction->syncId_ = syncId;
    EXPECT_EQ(rsTransaction->GetSyncId(), syncId);
    rsTransaction->AddSubSyncTransaction(subRSTransaction, token, syncId);
}

/**
 * @tc.name: RemoveSubSyncTransactionTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, RemoveSubSyncTransactionTest001, TestSize.Level1)
{
    const uint64_t syncId = 123;
    const uint64_t token = 1;
    auto rsTransaction = std::make_shared<RSTransaction>();
    EXPECT_EQ(rsTransaction->GetTransactionHandler(), nullptr);
    EXPECT_NE(rsTransaction->GetSyncId(), syncId);
    rsTransaction->RemoveSubSyncTransaction(token, syncId);

    rsTransaction->syncId_ = syncId;
    EXPECT_EQ(rsTransaction->GetSyncId(), syncId);
    rsTransaction->RemoveSubSyncTransaction(token, syncId);

    rsTransaction->subSyncTransactions_.clear();
    rsTransaction->subSyncTransactions_[token]++;
    rsTransaction->isNeedCloseSyncTransaction_ = true;
    rsTransaction->RemoveSubSyncTransaction(token, syncId);
}

/**
 * @tc.name: RemoveSubSyncTransactionTest002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, RemoveSubSyncTransactionTest002, TestSize.Level1)
{
    const uint64_t syncId = 123;
    const uint64_t token = 1;
    auto rsTransaction = std::make_shared<RSTransaction>();

    rsTransaction->syncId_ = syncId;
    EXPECT_EQ(rsTransaction->GetSyncId(), syncId);

    rsTransaction->subSyncTransactions_.clear();
    rsTransaction->subSyncTransactions_[token]++;
    rsTransaction->isNeedCloseSyncTransaction_ = false;

    EXPECT_FALSE(rsTransaction->subSyncTransactions_.empty());
    rsTransaction->RemoveSubSyncTransaction(token, syncId);
    EXPECT_FALSE(rsTransaction->subSyncTransactions_.empty());
}

/**
 * @tc.name: ProcessAllSyncTransactionTest001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, ProcessAllSyncTransactionTest001, TestSize.Level1)
{
    const uint64_t token = 1;
    auto rsTransaction = std::make_shared<RSTransaction>();
    EXPECT_EQ(rsTransaction->GetTransactionHandler(), nullptr);
    rsTransaction->ProcessAllSyncTransaction();

    rsTransaction->rsTransactionHandler_ = std::make_shared<RSTransactionHandler>();
    EXPECT_NE(rsTransaction->GetTransactionHandler(), nullptr);
    rsTransaction->subSyncTransactions_[token] = 0;
    rsTransaction->ProcessAllSyncTransaction();

    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;
    auto newContext = std::make_shared<RSUIContext>(token);
    RSUIContextManager::MutableInstance().rsUIContextMap_[token] = newContext;
    EXPECT_NE(RSUIContextManager::Instance().GetRSUIContext(token), nullptr);
    rsTransaction->ProcessAllSyncTransaction();
}

/**
 * @tc.name: ProcessAllSyncTransactionTest002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, ProcessAllSyncTransactionTest002, TestSize.Level1)
{
    const uint64_t token = 1;
    auto rsTransaction = std::make_shared<RSTransaction>();

    rsTransaction->rsTransactionHandler_ = std::make_shared<RSTransactionHandler>();
    EXPECT_NE(rsTransaction->GetTransactionHandler(), nullptr);
    rsTransaction->subSyncTransactions_[token] = 0;

    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;
    auto newContext = std::make_shared<RSUIContext>(token);
    RSUIContextManager::MutableInstance().rsUIContextMap_[token] = newContext;
    EXPECT_NE(RSUIContextManager::Instance().GetRSUIContext(token), nullptr);
    rsTransaction->ProcessAllSyncTransaction();
}
} // namespace OHOS::Rosen
