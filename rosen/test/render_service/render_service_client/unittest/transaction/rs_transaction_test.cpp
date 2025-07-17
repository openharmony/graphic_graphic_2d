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
} // namespace OHOS::Rosen
