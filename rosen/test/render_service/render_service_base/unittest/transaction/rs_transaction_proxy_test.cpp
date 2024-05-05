/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "command/rs_animation_command.h"
#include "command/rs_command.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderThreadClientTest : public RSIRenderClient {
public:
    RSRenderThreadClientTest() = default;
    ~RSRenderThreadClientTest() = default;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override {};
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override {};
};

class RSTransactionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::unique_ptr<RSIRenderClient> CreateRenderThreadClient()
    {
        return std::make_unique<RSRenderThreadClientTest>();
    }
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
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
}

/**
 * @tc.name: SetRenderThreadClient002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, SetRenderThreadClient002, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
}

/**
 * @tc.name: SetRenderServiceClient001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, SetRenderServiceClient001, TestSize.Level1)
{
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
}

/**
 * @tc.name: SetRenderServiceClient002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, SetRenderServiceClient002, TestSize.Level1)
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
 * @tc.name: FlushImplicitTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction002, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto rsTransactionProxy = RSTransactionProxy::GetInstance();
    ASSERT_NE(rsTransactionProxy, nullptr);
    rsTransactionProxy->StartSyncTransaction();
    rsTransactionProxy->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction003, TestSize.Level1)
{
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->StartSyncTransaction();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction004, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction005, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction006, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction007
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction007, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    FlushEmptyCallback callback = [](const uint64_t timestamp) -> bool {
        return true;
    };
    RSTransactionProxy::GetInstance()->SetFlushEmptyCallback(callback);
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
 * @tc.name: FlushImplicitTransactionFromRT002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransactionFromRT002, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->FlushImplicitTransactionFromRT(timestamp);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransactionFromRT003, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
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

/**
 * @tc.name: Commit002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, Commit002, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->Commit(timestamp);
}

/**
 * @tc.name: Commit003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, Commit003, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Commit(timestamp);
}

/**
 * @tc.name: Commit004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, Commit004, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->Commit(timestamp);
}

/**
 * @tc.name: CommitSyncTransaction001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CommitSyncTransaction001, TestSize.Level1)
{
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CommitSyncTransaction002, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CommitSyncTransaction003, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CommitSyncTransaction004, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CommitSyncTransaction005, TestSize.Level1)
{
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timestamp, "abilityName");
}

/**
 * @tc.name: MarkTransactionNeedSync001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, MarkTransactionNeedSync001, TestSize.Level1)
{
    auto rsTransactionProxy = RSTransactionProxy::GetInstance();
    ASSERT_NE(rsTransactionProxy, nullptr);
    rsTransactionProxy->MarkTransactionNeedSync();
}

/**
 * @tc.name: MarkTransactionNeedSync002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, MarkTransactionNeedSync002, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->MarkTransactionNeedSync();
}

/**
 * @tc.name: MarkTransactionNeedCloseSync001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, MarkTransactionNeedCloseSync001, TestSize.Level1)
{
    auto rsTransactionProxy = RSTransactionProxy::GetInstance();
    ASSERT_NE(rsTransactionProxy, nullptr);
    rsTransactionProxy->MarkTransactionNeedCloseSync(0);
}

/**
 * @tc.name: MarkTransactionNeedCloseSync002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, MarkTransactionNeedCloseSync002, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->MarkTransactionNeedCloseSync(0);
}

/**
 * @tc.name: AddCommand001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand001, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    command.reset();
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand002, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    command.reset();
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand003, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    command.reset();
    RSTransactionProxy::GetInstance()->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand004, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand005, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand006, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand007
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand007, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommandFromRT001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommandFromRT001, TestSize.Level1)
{
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
    command.reset();
    RSTransactionProxy::GetInstance()->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
}

/**
 * @tc.name: AddCommandFromRT002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommandFromRT002, TestSize.Level1)
{
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
    command.reset();
    RSTransactionProxy::GetInstance()->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
}

/**
 * @tc.name: ExecuteSynchronousTask001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, ExecuteSynchronousTask001, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(nullptr);
}

/**
 * @tc.name: ExecuteSynchronousTask002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, ExecuteSynchronousTask002, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(nullptr);
}

/**
 * @tc.name: ExecuteSynchronousTask003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, ExecuteSynchronousTask003, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task);
}

/**
 * @tc.name: ExecuteSynchronousTask004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, ExecuteSynchronousTask004, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, false);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, true);
}

/**
 * @tc.name: ExecuteSynchronousTask005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, ExecuteSynchronousTask005, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, false);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, true);
}

/**
 * @tc.name: ExecuteSynchronousTask006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, ExecuteSynchronousTask006, TestSize.Level1)
{
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, false);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, true);
}

/**
 * @tc.name: AddCommonCommandTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommonCommandTest, TestSize.Level1)
{
    RSTransactionProxy::GetInstance()->Begin();
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    command.reset();
    bool isRenderServiceCommand = false;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->AddCommand(command, isRenderServiceCommand, FollowType::FOLLOW_TO_PARENT, 1);
}
} // namespace Rosen
} // namespace OHOS
