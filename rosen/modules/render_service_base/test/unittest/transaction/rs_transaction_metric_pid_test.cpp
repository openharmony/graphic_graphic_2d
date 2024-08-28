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

#include <gtest/gtest.h>

#include "transaction/rs_transaction_metric_collector.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSTransactionMetricPidTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionMetricPidTest::SetUpTestCase() {}
void RSTransactionMetricPidTest::TearDownTestCase() {}

void RSTransactionMetricPidTest::SetUp() {}
void RSTransactionMetricPidTest::TearDown() {}

/**
 * @tc.name: Update001
 * @tc.desc: Verify the Update
 * @tc.type:FUNC
 */
HWTEST_F(RSTransactionMetricPidTest, Update001, TestSize.Level1) {
    uint64_t testData = 100;
    RSTransactionMetricPid metric;
    metric.Update(testData, testData);
    metric.Update(testData, testData);
    ASSERT_EQ(metric.rsTxCount_, 2);
}
}
}
