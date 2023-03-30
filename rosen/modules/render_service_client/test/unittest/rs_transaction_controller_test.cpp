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

#include "transaction/rs_sync_transaction_controller.h"
#include "transaction/rs_transaction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSTransactionMock : public RSTransaction {
public:
    RSTransactionMock() = default;
    virtual ~RSTransactionMock() = default;
};

class RSTransactionControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionControllerTest::SetUpTestCase()
{}

void RSTransactionControllerTest::TearDownTestCase()
{}

void RSTransactionControllerTest::SetUp() {}
void RSTransactionControllerTest::TearDown() {}


/**
 * @tc.name: RSTransactionControllerTest001
 * @tc.desc: Verify process transaction controller
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionControllerTest, RSTransactionControllerTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransactionControllerTest RSTransactionControllerTest001 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSTransaction> transaction = std::make_shared<RSTransaction>();
    MessageParcel parcel;
    transaction->MarshallTransactionSyncController(parcel);
    EXPECT_TRUE(transaction != nullptr);
    GTEST_LOG_(INFO) << "RSTransactionControllerTest RSTransactionControllerTest001 end";
}

/**
 * @tc.name: RSTransactionControllerTest002
 * @tc.desc: Verify sync transaction controller
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionControllerTest, RSTransactionControllerTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransactionControllerTest RSTransactionControllerTest002 start";
    /**
     * @tc.steps: step1. init
     */
    auto controller = RSSyncTransactionController::GetInstance();
    controller->OpenSyncTransaction();
    auto transaction = controller->GetRSTransaction();
    EXPECT_TRUE(controller != nullptr);
    uint64_t count = 0;
    transaction->CloseSyncTransaction(count);
    GTEST_LOG_(INFO) << "RSTransactionControllerTest RSTransactionControllerTest002 end";
}

/**
 * @tc.name: RSTransactionControllerTest003
 * @tc.desc: Verify sync transaction controller
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionControllerTest, RSTransactionControllerTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransactionControllerTest RSTransactionControllerTest003 start";
    /**
     * @tc.steps: step1. init
     */
    auto transaction = std::make_shared<RSTransaction>();
    EXPECT_TRUE(transaction != nullptr);
    Parcel parcel;
    transaction->Marshalling(parcel);
    RSTransaction::Unmarshalling(parcel);
    MessageParcel messageParcel;
    transaction->MarshallTransactionSyncController(messageParcel);
    transaction->UnmarshallTransactionSyncController(messageParcel);
    transaction->OpenSyncTransaction();
    uint64_t count = 0;
    transaction->CloseSyncTransaction(count);
    transaction->Begin();
    transaction->Commit();
    GTEST_LOG_(INFO) << "RSTransactionControllerTest RSTransactionControllerTest003 end";
}

} // namespace Rosen
} // namespace OHOS
