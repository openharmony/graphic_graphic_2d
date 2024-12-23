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

#include "transaction/rs_transaction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsTransactionsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsTransactionsTest::SetUpTestCase() {}
void RsTransactionsTest::TearDownTestCase() {}
void RsTransactionsTest::SetUp() {}
void RsTransactionsTest::TearDown() {}

/**
 * @tc.name: Marshalling01
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RsTransactionsTest, Marshalling01, TestSize.Level1)
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
 * @tc.name: FlushImplicitTransaction01
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RsTransactionsTest, FlushImplicitTransaction01, TestSize.Level1)
{
    RSTransaction::FlushImplicitTransaction();
    EXPECT_NE(RSTransaction::FlushImplicitTransaction, nullptr);
}
} // namespace OHOS::Rosen
