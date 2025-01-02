/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
class RSRenderThreadUnitClientTest : public RSIRenderClient {
public:
    RSRenderThreadUnitClientTest() = default;
    ~RSRenderThreadUnitClientTest() = default;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override {};
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override {};
};

class RSTransactionProxyUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::unique_ptr<RSIRenderClient> CreateRenderThreadClient()
    {
        return std::make_unique<RSRenderThreadUnitClientTest>();
    }
};

void RSTransactionProxyUnitTest::SetUpTestCase() {}
void RSTransactionProxyUnitTest::TearDownTestCase() {}
void RSTransactionProxyUnitTest::SetUp() {}
void RSTransactionProxyUnitTest::TearDown() {}

/**
 * @tc.name: SetRenderThreadClient001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, SetRenderThreadClient001, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, SetRenderThreadClient002, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, SetRenderServiceClient001, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, SetRenderServiceClient002, TestSize.Level1)
{
    auto renderServiceClient = nullptr;
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
}

/**
 * @tc.name: FlushImplicitTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, FlushImplicitTransaction002, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto rsTransactionProxy = RSTransactionProxy::GetInstance();
    ASSERT_NE(rsTransactionProxy, nullptr);
    rsTransactionProxy->StartSyncTransaction();
    rsTransactionProxy->FlushImplicitTransaction(timeStamp);
}

/**
 * @tc.name: FlushImplicitTransaction004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, FlushImplicitTransaction004, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timeStamp);
}

/**
 * @tc.name: IsEmpty001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, IsEmpty001, TestSize.Level1)
{
    auto rsTransactionProxy = RSTransactionProxy::GetInstance();
    bool isCommonDataEmpty = rsTransactionProxy->implicitCommonTransactionData_->IsEmpty();
    bool isRemoteDataEmpty = rsTransactionProxy->implicitRemoteTransactionData_->IsEmpty();
    bool isCommonDataStackEmpty = rsTransactionProxy->implicitCommonTransactionDataStack_.empty();
    bool isRemoteDataStackEmpty = rsTransactionProxy->implicitRemoteTransactionDataStack_.empty();

    ASSERT_EQ(rsTransactionProxy->IsEmpty(),
        isCommonDataEmpty & isRemoteDataEmpty & isCommonDataStackEmpty & isRemoteDataStackEmpty);
}

/**
 * @tc.name: FlushImplicitTransaction005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, FlushImplicitTransaction005, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timeStamp);
}

/**
 * @tc.name: FlushImplicitTransaction006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, FlushImplicitTransaction006, TestSize.Level1)
{
    uint64_t timeStamp = 1;
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
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timeStamp);
}

/**
 * @tc.name: FlushImplicitTransaction007
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, FlushImplicitTransaction007, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    FlushEmptyCallback callback = [](const uint64_t timeStamp) -> bool {
        return true;
    };
    RSTransactionProxy::GetInstance()->SetFlushEmptyCallback(callback);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timeStamp);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, FlushImplicitTransactionFromRT002, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->FlushImplicitTransactionFromRT(timeStamp);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, FlushImplicitTransactionFromRT003, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    RSTransactionProxy::GetInstance()->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
    RSTransactionProxy::GetInstance()->FlushImplicitTransactionFromRT(timeStamp);
}

/**
 * @tc.name: Commit002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, Commit002, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->Commit(timeStamp);
}

/**
 * @tc.name: Commit003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, Commit003, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Commit(timeStamp);
}

/**
 * @tc.name: Commit004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, Commit004, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->Commit(timeStamp);
}

/**
 * @tc.name: CommitSyncTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, CommitSyncTransaction002, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
    RSTransactionProxy::GetInstance()->SetRenderServiceClient(renderServiceClient);
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timeStamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, CommitSyncTransaction003, TestSize.Level1)
{
    uint64_t timeStamp = 1;
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
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timeStamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, CommitSyncTransaction004, TestSize.Level1)
{
    uint64_t timeStamp = 1;
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
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timeStamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, CommitSyncTransaction005, TestSize.Level1)
{
    uint64_t timeStamp = 1;
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
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timeStamp, "abilityName");
}

/**
 * @tc.name: MarkTransactionNeedSync001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, MarkTransactionNeedSync001, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, MarkTransactionNeedSync002, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, MarkTransactionNeedCloseSync001, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, MarkTransactionNeedCloseSync002, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommand001, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommand002, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommand003, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommand004, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommand005, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommand006, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommand007, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommandFromRT001, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommandFromRT002, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, ExecuteSynchronousTask001, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, ExecuteSynchronousTask002, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, ExecuteSynchronousTask003, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, ExecuteSynchronousTask004, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, ExecuteSynchronousTask005, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, ExecuteSynchronousTask006, TestSize.Level1)
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
HWTEST_F(RSTransactionProxyUnitTest, AddCommonCommandTest, TestSize.Level1)
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

/**
 * @tc.name: FlushImplicitTransactionFromRT004
 * @tc.desc: Test FlushImplicitTransactionFromRT and Begin and SetSyncTransactionNum
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionProxyUnitTest, FlushImplicitTransactionFromRT004, TestSize.Level1)
{
    uint64_t timeStamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    RSTransactionProxy* instance = RSTransactionProxy::GetInstance();
    instance->renderServiceClient_ = renderServiceClient;
    instance->FlushImplicitTransactionFromRT(timeStamp);
    instance->SetSyncTransactionNum(0);
    instance->StartSyncTransaction();
    instance->Begin();
    instance->SetSyncTransactionNum(0);
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
    instance->AddCommonCommand(command);
    instance->AddRemoteCommand(command, 1, FollowType::NONE);
    while (!instance->implicitCommonTransactionDataStack_.empty() ||
           !instance->implicitRemoteTransactionDataStack_.empty()) {
        instance->implicitCommonTransactionDataStack_.pop();
        instance->implicitRemoteTransactionDataStack_.pop();
    }
    instance->AddCommonCommand(command);
    instance->AddRemoteCommand(command, 1, FollowType::NONE);
    ASSERT_TRUE(instance->implicitCommonTransactionDataStack_.empty());
    ASSERT_TRUE(instance->implicitRemoteTransactionDataStack_.empty());
}

/**
 * @tc.name: StartCloseSyncTransactionFallbackTaskTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyUnitTest, StartCloseSyncTransactionFallbackTaskTest, TestSize.Level1)
{
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("runner");
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    RSTransactionProxy* instance = RSTransactionProxy::GetInstance();

    instance->StartCloseSyncTransactionFallbackTask(nullptr, true);
    instance->StartCloseSyncTransactionFallbackTask(nullptr, false);
    ASSERT_TRUE(instance->taskNames_.empty());

    instance->StartCloseSyncTransactionFallbackTask(handler, true);
    instance->StartCloseSyncTransactionFallbackTask(handler, false);
    ASSERT_TRUE(instance->taskNames_.empty());

    instance->StartCloseSyncTransactionFallbackTask(handler, true);
    sleep(8);
    ASSERT_TRUE(instance->taskNames_.empty());
}
} // namespace Rosen
} // namespace OHOS
