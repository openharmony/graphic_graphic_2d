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

#include <gtest/gtest.h>

#include "transaction/rs_transaction_data.h"
#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSTransactionDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionDataTest::SetUpTestCase() {}
void RSTransactionDataTest::TearDownTestCase() {}
void RSTransactionDataTest::SetUp() {}
void RSTransactionDataTest::TearDown() {}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionDataTest, ProcessTest, TestSize.Level1)
{
    RSContext context;
    RSTransactionData rsTransactionData;
    rsTransactionData.Process(context);
    ASSERT_TRUE(rsTransactionData.IsEmpty());

}

/**
 * @tc.name: ClearTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionDataTest, ClearTest, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    rsTransactionData.Clear();
    ASSERT_TRUE(rsTransactionData.IsEmpty());
}

/**
 * @tc.name: Unmarshalling
 * @tc.desc: Test Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, Unmarshalling, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    Parcel parcel;
    ASSERT_EQ(rsTransactionData.Unmarshalling(parcel), nullptr);
}
} // namespace Rosen
} // namespace OHOS