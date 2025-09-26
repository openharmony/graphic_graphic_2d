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

#include <unistd.h>
#include <gtest/gtest.h>

#include "transaction/rs_transaction_data.h"
#include "transaction/rs_transaction_metric_collector.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSTransactionMetricCollectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionMetricCollectorTest::SetUpTestCase() {}
void RSTransactionMetricCollectorTest::TearDownTestCase() {}

void RSTransactionMetricCollectorTest::SetUp() {}
void RSTransactionMetricCollectorTest::TearDown() {}

/**
 * @tc.name: GetInstance001
 * @tc.desc: Verify the GetInstance
 * @tc.type:FUNC
 */
HWTEST_F(RSTransactionMetricCollectorTest, GetInstance001, TestSize.Level1) {
    auto& instance1 = RSTransactionMetricCollector::GetInstance();
    auto& instance2 = RSTransactionMetricCollector::GetInstance();
    ASSERT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: Collect001
 * @tc.desc: Verify the Collect
 * @tc.type:FUNC
 */
HWTEST_F(RSTransactionMetricCollectorTest, Collect001, TestSize.Level1) {
    std::unique_ptr<RSTransactionData> transactionData1 = std::make_unique<RSTransactionData>();
    transactionData1->SetSendingPid(1001);
    std::unique_ptr<RSTransactionData> transactionData2 = std::make_unique<RSTransactionData>();
    transactionData2->SetSendingPid(1002);
    RSTransactionMetricCollector& collector = RSTransactionMetricCollector::GetInstance();
    collector.Collect(transactionData1);
    sleep(6);
    collector.Collect(transactionData1);
    sleep(1);
    collector.Collect(transactionData2);
    if (!RSSystemProperties::IsBetaRelease()) {
        ASSERT_EQ(collector.pidMetrics_.size(), 0);
    } else {
        ASSERT_EQ(collector.pidMetrics_.size(), 2);
    }
}

/**
 * @tc.name: Reset001
 * @tc.desc: Verify the Reset
 * @tc.type:FUNC
 */
HWTEST_F(RSTransactionMetricCollectorTest, Reset001, TestSize.Level1) {
    RSTransactionMetricCollector& collector = RSTransactionMetricCollector::GetInstance();
    collector.Reset(0);
    ASSERT_TRUE(collector.pidMetrics_.empty());
}
}
}
