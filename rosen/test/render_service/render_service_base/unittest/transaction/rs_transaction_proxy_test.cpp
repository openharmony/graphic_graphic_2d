/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "command/rs_command.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSTransactionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionProxyTest::SetUpTestCase() {}
void RSTransactionProxyTest::TearDownTestCase() {}
void RSTransactionProxyTest::SetUp() {}
void RSTransactionProxyTest::TearDown() {}

/**
 * @tc.name: SetRenderThreadClient001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, SetRenderThreadClient001, TestSize.Level1)
{
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
}

/**
 * @tc.name: SetRenderThreadClient002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, SetRenderThreadClient002, TestSize.Level1)
{
    auto renderServiceClient = nullptr;
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
}

/**
 * @tc.name: FlushImplicitTransaction001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction001, TestSize.Level1)
{
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransactionFromRT001, TestSize.Level1)
{
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->FlushImplicitTransactionFromRT(timestamp);
}

/**
 * @tc.name: Commit001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, Commit001, TestSize.Level1)
{
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->Commit(timestamp);
}
} // namespace Rosen
} // namespace OHOS
