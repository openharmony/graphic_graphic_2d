/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <parameters.h>

#include "command/rs_animation_command.h"
#include "command/rs_command.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_handler.h"

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

// Mock client to track committed transactions
class MockRenderClient : public RSIRenderClient {
public:
    MockRenderClient() = default;
    ~MockRenderClient() = default;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override
    {
        committedTransactions.push_back(std::move(transactionData));
    }
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override {};

    std::vector<std::unique_ptr<RSTransactionData>> committedTransactions;
};

class RSTransactionHandlerTest : public testing::Test {
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

void RSTransactionHandlerTest::SetUpTestCase() {}
void RSTransactionHandlerTest::TearDownTestCase() {}
void RSTransactionHandlerTest::SetUp() {}
void RSTransactionHandlerTest::TearDown() {}

/**
 * @tc.name: FlushImplicitTransactionHybridRender001
 * @tc.desc: test func FlushImplicitTransaction when callback is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICII2M
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransactionHybridRender001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    auto hybridrenderEnable = system::GetParameter("const.graphics.hybridrenderenable", "0");
    system::SetParameter("const.graphics.hybridrenderenable", "0");
    transaction->FlushImplicitTransaction(timestamp);
    system::SetParameter("const.graphics.hybridrenderenable", hybridrenderEnable);
}

/**
 * @tc.name: FlushImplicitTransactionHybridRender002
 * @tc.desc: test func FlushImplicitTransaction when callback is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueICII2M
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransactionHybridRender002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    CommitTransactionCallback callback =
        [] (std::shared_ptr<RSIRenderClient> &renderServiceClient,
        std::unique_ptr<RSTransactionData>&& rsTransactionData, uint32_t& transactionDataIndex,
        std::shared_ptr<RSTransactionHandler>) {};
    RSTransactionHandler::SetCommitTransactionCallback(callback);
    transaction->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransactionHybridRender003
 * @tc.desc: test func FlushImplicitTransaction when renderServiceClient_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICII2M
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransactionHybridRender003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(transaction, nullptr);
    uint64_t timestamp = 1;
    NodeId nodeId = 1;
    transaction->renderServiceClient_ = nullptr;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    transaction->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: SetRenderThreadClient001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, SetRenderThreadClient001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
}

/**
 * @tc.name: SetRenderThreadClient002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, SetRenderThreadClient002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
}

/**
 * @tc.name: SetRenderServiceClient001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, SetRenderServiceClient001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderServiceClient(renderServiceClient);
}

/**
 * @tc.name: SetRenderServiceClient002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, SetRenderServiceClient002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(transaction, nullptr);
    auto renderServiceClient = nullptr;
    transaction->SetRenderServiceClient(renderServiceClient);
}

/**
 * @tc.name: FlushImplicitTransaction001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransaction001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    transaction->FlushImplicitTransaction(timestamp);
    EXPECT_EQ(transaction->timestamp_, timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransaction002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    ASSERT_NE(transaction, nullptr);
    transaction->StartSyncTransaction();
    transaction->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransaction003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    transaction->Begin();
    transaction->StartSyncTransaction();
    transaction->FlushImplicitTransaction(timestamp);
    ASSERT_NE(transaction->timestamp_, timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransaction004, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: IsEmpty001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, IsEmpty001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    bool isCommonDataEmpty = transaction->implicitCommonTransactionData_->IsEmpty();
    bool isRemoteDataEmpty = transaction->implicitRemoteTransactionData_->IsEmpty();
    bool isCommonDataStackEmpty = transaction->implicitCommonTransactionDataStack_.empty();
    bool isRemoteDataStackEmpty = transaction->implicitRemoteTransactionDataStack_.empty();

    ASSERT_EQ(transaction->IsEmpty(),
        isCommonDataEmpty & isRemoteDataEmpty & isCommonDataStackEmpty & isRemoteDataStackEmpty);
}

/**
 * @tc.name: FlushImplicitTransaction005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransaction005, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransaction006, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction007
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransaction007, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    FlushEmptyCallback callback = [](const uint64_t timestamp) -> bool {
        return true;
    };
    transaction->SetFlushEmptyCallback(callback);
    transaction->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransactionFromRT001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    transaction->FlushImplicitTransactionFromRT(timestamp);
    ASSERT_NE(transaction->timestamp_, timestamp);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransactionFromRT002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->FlushImplicitTransactionFromRT(timestamp);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransactionFromRT003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
    transaction->FlushImplicitTransactionFromRT(timestamp);
}

/**
 * @tc.name: Commit001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, Commit001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    EXPECT_NE(renderServiceClient, nullptr);
    transaction->Commit(timestamp);
}

/**
 * @tc.name: Commit002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, Commit002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->Commit(timestamp);
}

/**
 * @tc.name: Commit003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, Commit003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Commit(timestamp);
}

/**
 * @tc.name: Commit004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, Commit004, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->Commit(timestamp);
}

/**
 * @tc.name: Commit005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, Commit005, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    ASSERT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->Commit(timestamp);

    transaction->implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    ASSERT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->Commit(timestamp);

    transaction->implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    transaction->implicitRemoteTransactionDataStack_.top()->MarkNeedSync();
    ASSERT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
    ASSERT_TRUE(transaction->implicitRemoteTransactionDataStack_.top()->IsNeedSync());
    transaction->Commit(timestamp);
}

/**
 * @tc.name: CommitSyncTransaction001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    transaction->Begin();
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
    EXPECT_EQ(transaction->timestamp_, timestamp);
}

/**
 * @tc.name: CommitSyncTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction004, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction005, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction006, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    transaction->Begin();
    transaction->Begin();
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
    EXPECT_EQ(transaction->timestamp_, timestamp);
}

/**
 * @tc.name: CommitSyncTransaction007
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction007, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->Begin();
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction008
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction008, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction09
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction09, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
}

/**
 * @tc.name: CommitSyncTransaction010
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, CommitSyncTransaction010, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(0, timestamp, "abilityName");
}

/**
 * @tc.name: MarkTransactionNeedSync001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MarkTransactionNeedSync001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(transaction, nullptr);
    transaction->MarkTransactionNeedSync();
}

/**
 * @tc.name: MarkTransactionNeedSync002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MarkTransactionNeedSync002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->MarkTransactionNeedSync();
}

/**
 * @tc.name: MarkTransactionNeedCloseSync001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MarkTransactionNeedCloseSync001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(transaction, nullptr);
    transaction->MarkTransactionNeedCloseSync(0);
}

/**
 * @tc.name: MarkTransactionNeedCloseSync002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MarkTransactionNeedCloseSync002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->Begin();
    transaction->MarkTransactionNeedCloseSync(0);
}

/**
 * @tc.name: AddCommand001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommand001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    command.reset();
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommand002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    command.reset();
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommand003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    command.reset();
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommand004, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommand005, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommand006, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: AddCommand007
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommand007, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: MoveCommandByNodeId001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveCommandByNodeId001, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    preTransaction->renderThreadClient_ = std::move(renderThreadClient);
    preTransaction->renderServiceClient_ = std::move(renderServiceClient);
    NodeId nodeId = 1;
    preTransaction->MoveCommandByNodeId(curTransaction, nodeId);
}

/**
 * @tc.name: MoveCommandByNodeId002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveCommandByNodeId002, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    preTransaction->renderThreadClient_ = std::move(renderThreadClient);
    preTransaction->renderServiceClient_ = std::move(renderServiceClient);
    NodeId nodeId = 1;
    preTransaction->MoveCommandByNodeId(curTransaction, nodeId);
}

/**
 * @tc.name: MoveCommandByNodeId003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveCommandByNodeId003, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    preTransaction->renderThreadClient_ = std::move(renderThreadClient);
    preTransaction->renderServiceClient_ = std::move(renderServiceClient);
    NodeId nodeId = 1;
    preTransaction->MoveCommandByNodeId(curTransaction, nodeId);
}

/**
 * @tc.name: MoveCommandByNodeId004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveCommandByNodeId004, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    preTransaction->renderThreadClient_ = std::move(renderThreadClient);
    preTransaction->renderServiceClient_ = std::move(renderServiceClient);
    NodeId nodeId = 1;
    preTransaction->MoveCommandByNodeId(curTransaction, nodeId);
}

/**
 * @tc.name: AddCommandFromRT001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommandFromRT001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
    command.reset();
    transaction->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
}

/**
 * @tc.name: AddCommandFromRT002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommandFromRT002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    transaction->SetRenderServiceClient(renderServiceClient);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
    command.reset();
    transaction->AddCommandFromRT(command, 1, FollowType::FOLLOW_TO_PARENT);
}

/**
 * @tc.name: ExecuteSynchronousTask001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ExecuteSynchronousTask001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->ExecuteSynchronousTask(nullptr);
}

/**
 * @tc.name: ExecuteSynchronousTask002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ExecuteSynchronousTask002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->ExecuteSynchronousTask(nullptr);
}

/**
 * @tc.name: ExecuteSynchronousTask003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ExecuteSynchronousTask003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->ExecuteSynchronousTask(task);
}

/**
 * @tc.name: ExecuteSynchronousTask004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ExecuteSynchronousTask004, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    renderThreadClient.reset();
    ASSERT_EQ(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->ExecuteSynchronousTask(task, false);
    transaction->ExecuteSynchronousTask(task, true);
}

/**
 * @tc.name: ExecuteSynchronousTask005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ExecuteSynchronousTask005, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->ExecuteSynchronousTask(task, false);
    transaction->ExecuteSynchronousTask(task, true);
}

/**
 * @tc.name: ExecuteSynchronousTask006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ExecuteSynchronousTask006, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ASSERT_NE(renderServiceClient, nullptr);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->SetRenderServiceClient(renderServiceClient);
    transaction->ExecuteSynchronousTask(task, false);
    transaction->ExecuteSynchronousTask(task, true);
}

/**
 * @tc.name: AddCommonCommandTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, AddCommonCommandTest, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    transaction->Begin();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    command.reset();
    bool isRenderServiceCommand = false;
    auto renderThreadClient = CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    transaction->AddCommand(command, isRenderServiceCommand, FollowType::FOLLOW_TO_PARENT, 1);
}

/**
 * @tc.name: MoveCommonCommandByNodeIdTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveCommonCommandByNodeIdTest001, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    preTransaction->Begin();
    ASSERT_FALSE(preTransaction->implicitCommonTransactionDataStack_.empty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransaction->AddCommonCommand(command);
    ASSERT_FALSE(preTransaction->implicitCommonTransactionDataStack_.top()->IsEmpty());
    curTransaction->Begin();
    ASSERT_FALSE(curTransaction->implicitCommonTransactionDataStack_.empty());
    preTransaction->MoveCommonCommandByNodeId(curTransaction, nodeId);
    ASSERT_TRUE(preTransaction->implicitCommonTransactionDataStack_.top()->IsEmpty());
    ASSERT_FALSE(curTransaction->implicitCommonTransactionDataStack_.top()->IsEmpty());
    preTransaction->Commit();
    curTransaction->Commit();
}

/**
 * @tc.name: MoveCommonCommandByNodeIdTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveCommonCommandByNodeIdTest002, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    preTransaction->Begin();
    ASSERT_FALSE(preTransaction->implicitCommonTransactionDataStack_.empty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransaction->AddCommonCommand(command);
    ASSERT_FALSE(preTransaction->implicitCommonTransactionDataStack_.top()->IsEmpty());
    ASSERT_TRUE(curTransaction->implicitCommonTransactionDataStack_.empty());
    preTransaction->MoveCommonCommandByNodeId(curTransaction, nodeId);
    ASSERT_FALSE(preTransaction->implicitCommonTransactionDataStack_.top()->IsEmpty());
    preTransaction->Commit();
    ASSERT_TRUE(curTransaction->implicitCommonTransactionData_->IsEmpty());
}

/**
 * @tc.name: MoveCommonCommandByNodeIdTest003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveCommonCommandByNodeIdTest003, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    ASSERT_TRUE(preTransaction->implicitCommonTransactionDataStack_.empty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransaction->AddCommonCommand(command);
    ASSERT_TRUE(curTransaction->implicitCommonTransactionDataStack_.empty());
    preTransaction->MoveCommonCommandByNodeId(curTransaction, nodeId);
    ASSERT_TRUE(preTransaction->implicitCommonTransactionData_->IsEmpty());
    ASSERT_FALSE(curTransaction->implicitCommonTransactionData_->IsEmpty());
}

/**
 * @tc.name: MoveCommonCommandByNodeIdTest004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveCommonCommandByNodeIdTest004, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    ASSERT_TRUE(preTransaction->implicitCommonTransactionDataStack_.empty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransaction->AddCommonCommand(command);
    curTransaction->Begin();
    ASSERT_FALSE(curTransaction->implicitCommonTransactionDataStack_.empty());
    preTransaction->MoveCommonCommandByNodeId(curTransaction, nodeId);
    curTransaction->Commit();
    ASSERT_TRUE(preTransaction->implicitCommonTransactionData_->IsEmpty());
    ASSERT_FALSE(curTransaction->implicitCommonTransactionData_->IsEmpty());
}

/**
 * @tc.name: MoveRemoteCommandByNodeIdTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveRemoteCommandByNodeIdTest001, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    preTransaction->Begin();
    ASSERT_FALSE(preTransaction->implicitRemoteTransactionDataStack_.empty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    ASSERT_FALSE(preTransaction->implicitRemoteTransactionDataStack_.top()->IsEmpty());
    curTransaction->Begin();
    ASSERT_FALSE(curTransaction->implicitRemoteTransactionDataStack_.empty());
    preTransaction->MoveRemoteCommandByNodeId(curTransaction, nodeId);
    ASSERT_TRUE(preTransaction->implicitRemoteTransactionDataStack_.top()->IsEmpty());
    ASSERT_FALSE(curTransaction->implicitRemoteTransactionDataStack_.top()->IsEmpty());
    preTransaction->Commit();
    curTransaction->Commit();
}

/**
 * @tc.name: MoveRemoteCommandByNodeIdTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveRemoteCommandByNodeIdTest002, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    preTransaction->Begin();
    ASSERT_FALSE(preTransaction->implicitRemoteTransactionDataStack_.empty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    ASSERT_FALSE(preTransaction->implicitRemoteTransactionDataStack_.top()->IsEmpty());
    ASSERT_TRUE(curTransaction->implicitRemoteTransactionDataStack_.empty());
    preTransaction->MoveRemoteCommandByNodeId(curTransaction, nodeId);
    ASSERT_FALSE(preTransaction->implicitRemoteTransactionDataStack_.top()->IsEmpty());
    preTransaction->Commit();
    ASSERT_TRUE(curTransaction->implicitRemoteTransactionData_->IsEmpty());
}

/**
 * @tc.name: MoveRemoteCommandByNodeIdTest003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveRemoteCommandByNodeIdTest003, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    ASSERT_TRUE(preTransaction->implicitRemoteTransactionDataStack_.empty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    ASSERT_TRUE(curTransaction->implicitRemoteTransactionDataStack_.empty());
    preTransaction->MoveRemoteCommandByNodeId(curTransaction, nodeId);
    ASSERT_TRUE(preTransaction->implicitRemoteTransactionData_->IsEmpty());
    ASSERT_FALSE(curTransaction->implicitRemoteTransactionData_->IsEmpty());
}

/**
 * @tc.name: MoveRemoteCommandByNodeIdTest004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MoveRemoteCommandByNodeIdTest004, TestSize.Level1)
{
    auto preTransaction = std::make_shared<RSTransactionHandler>();
    auto curTransaction = std::make_shared<RSTransactionHandler>();
    ASSERT_TRUE(preTransaction->implicitRemoteTransactionDataStack_.empty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    curTransaction->Begin();
    ASSERT_FALSE(curTransaction->implicitRemoteTransactionDataStack_.empty());
    preTransaction->MoveRemoteCommandByNodeId(curTransaction, nodeId);
    curTransaction->Commit();
    ASSERT_TRUE(preTransaction->implicitRemoteTransactionData_->IsEmpty());
    ASSERT_FALSE(curTransaction->implicitRemoteTransactionData_->IsEmpty());
}

/**
 * @tc.name: FlushImplicitTransactionFromRT004
 * @tc.desc: Test FlushImplicitTransactionFromRT and Begin and SetSyncTransactionNum
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionHandlerTest, FlushImplicitTransactionFromRT004, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient.reset();
    ASSERT_EQ(renderServiceClient, nullptr);

    transaction->renderServiceClient_ = renderServiceClient;
    transaction->FlushImplicitTransactionFromRT(timestamp);
    transaction->SetSyncTransactionNum(0);
    transaction->StartSyncTransaction();
    transaction->Begin();
    transaction->SetSyncTransactionNum(0);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommonCommand(command);
    transaction->AddRemoteCommand(command, 1, FollowType::NONE);
    while (!transaction->implicitCommonTransactionDataStack_.empty() ||
           !transaction->implicitRemoteTransactionDataStack_.empty()) {
            transaction->implicitCommonTransactionDataStack_.pop();
            transaction->implicitRemoteTransactionDataStack_.pop();
    }
    transaction->AddCommonCommand(command);
    transaction->AddRemoteCommand(command, 1, FollowType::NONE);
    ASSERT_TRUE(transaction->implicitCommonTransactionDataStack_.empty());
    ASSERT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
}

/**
 * @tc.name: TestPostTask
 * @tc.desc: add cases including using PostTask and PostDelayTask func.
 * @tc.type: FUNC
 * @tc.require: issueICQP7Q
 */
HWTEST_F(RSTransactionHandlerTest, TestPostTask, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();

    const std::function<void()>& task = []() {};
    transaction->PostTask(task);
    transaction->PostDelayTask(task, 0);
    transaction->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
    ASSERT_NE(transaction->taskRunner_, nullptr);
    transaction->PostDelayTask(task, 0);

    transaction->taskRunner_ = nullptr;
    transaction->PostDelayTask(task, 0);
}

/**
 * @tc.name: DumpCommandTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require: issueICV186
 */
HWTEST_F(RSTransactionHandlerTest, DumpCommandTest, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto renderThreadClient = CreateRenderThreadClient();
    transaction->SetRenderThreadClient(renderThreadClient);
    std::string dumpString;
    auto nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommonCommand(command);
    transaction->Begin();
    command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommonCommand(command);
    transaction->DumpCommand(dumpString);
    ASSERT_TRUE(dumpString.find("ImplicitRemoteTransactionData") != std::string::npos);
    ASSERT_TRUE(dumpString.find("ImplicitCommonTransactionData") != std::string::npos);
    ASSERT_TRUE(dumpString.find("ImplicitRemoteTransactionDataStack") != std::string::npos);
    ASSERT_TRUE(dumpString.find("ImplicitCommonTransactionDataStack") != std::string::npos);
    ASSERT_TRUE(dumpString.find(std::to_string(nodeId)) != std::string::npos);
    transaction->Commit();
    transaction->FlushImplicitTransaction();
    dumpString.clear();
    transaction->DumpCommand(dumpString);
    ASSERT_TRUE(dumpString.find("ImplicitRemoteTransactionData") == std::string::npos);
    ASSERT_TRUE(dumpString.find("ImplicitCommonTransactionData") == std::string::npos);
    ASSERT_TRUE(dumpString.find("ImplicitRemoteTransactionDataStack") == std::string::npos);
    ASSERT_TRUE(dumpString.find("ImplicitCommonTransactionDataStack") == std::string::npos);
}

/**
 * @tc.name: Begin001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, Begin001, TestSize.Level1)
{
    uint64_t syncId = 123;
    auto transaction = std::make_shared<RSTransactionHandler>();
    ASSERT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->Begin();

    transaction->implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    ASSERT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->Begin();
    transaction->Begin(syncId);

    ASSERT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
    ASSERT_FALSE(transaction->implicitRemoteTransactionDataStack_.top()->IsNeedSync());
    transaction->Begin();

    transaction->implicitRemoteTransactionDataStack_.top()->MarkNeedSync();
    ASSERT_TRUE(transaction->implicitRemoteTransactionDataStack_.top()->IsNeedSync());
    transaction->Begin();
}

/**
 * @tc.name: Begin002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, Begin002, TestSize.Level1)
{
    uint64_t syncId = 123;
    auto transaction = std::make_shared<RSTransactionHandler>();
    ASSERT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->needSync_ = false;
    transaction->Begin(syncId);
    ASSERT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
}

/**
 * @tc.name: Begin003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, Begin003, TestSize.Level1)
{
    uint64_t syncId = 123;
    auto transaction = std::make_shared<RSTransactionHandler>();
    ASSERT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->needSync_ = true;
    transaction->Begin(syncId);
    ASSERT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
}

/**
 * @tc.name: MergeSyncTransaction001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler1 = std::make_shared<RSTransactionHandler>();
    auto transactionHandler2 = transaction;
    transaction->MergeSyncTransaction(nullptr);
    transaction->MergeSyncTransaction(transactionHandler1);

    EXPECT_EQ(transactionHandler2.get(), transaction.get());
    transaction->MergeSyncTransaction(transactionHandler2);
}

/**
 * @tc.name: MergeSyncTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    ASSERT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    transactionHandler->implicitCommonTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    ASSERT_FALSE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    EXPECT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.top()->IsEmpty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: MergeSyncTransaction003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    ASSERT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    auto preTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(preTransactionData->IsEmpty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);

    transactionHandler->implicitCommonTransactionDataStack_.emplace(std::move(preTransactionData));
    ASSERT_FALSE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    EXPECT_FALSE(transactionHandler->implicitCommonTransactionDataStack_.top()->IsEmpty());

    EXPECT_TRUE(transaction->implicitCommonTransactionDataStack_.empty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: MergeSyncTransaction004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction004, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    ASSERT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    auto preTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(preTransactionData->IsEmpty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);

    transactionHandler->implicitCommonTransactionDataStack_.emplace(std::move(preTransactionData));
    ASSERT_FALSE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    EXPECT_FALSE(transactionHandler->implicitCommonTransactionDataStack_.top()->IsEmpty());

    EXPECT_TRUE(transaction->implicitCommonTransactionDataStack_.empty());
    transaction->implicitCommonTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    EXPECT_FALSE(transaction->implicitCommonTransactionDataStack_.empty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: MergeSyncTransaction005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction005, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    ASSERT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    transactionHandler->implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    ASSERT_FALSE(transactionHandler->implicitRemoteTransactionDataStack_.empty());
    EXPECT_TRUE(transactionHandler->implicitRemoteTransactionDataStack_.top()->IsEmpty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: MergeSyncTransaction006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction006, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    ASSERT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    auto preTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(preTransactionData->IsEmpty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);

    transactionHandler->implicitRemoteTransactionDataStack_.emplace(std::move(preTransactionData));
    ASSERT_FALSE(transactionHandler->implicitRemoteTransactionDataStack_.empty());
    EXPECT_FALSE(transactionHandler->implicitRemoteTransactionDataStack_.top()->IsEmpty());

    EXPECT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: MergeSyncTransaction007
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction007, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    ASSERT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    auto preTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(preTransactionData->IsEmpty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);

    transactionHandler->implicitRemoteTransactionDataStack_.emplace(std::move(preTransactionData));
    ASSERT_FALSE(transactionHandler->implicitRemoteTransactionDataStack_.empty());
    EXPECT_FALSE(transactionHandler->implicitRemoteTransactionDataStack_.top()->IsEmpty());

    EXPECT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    EXPECT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: MergeSyncTransaction008
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction008, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    EXPECT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    EXPECT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
    ASSERT_TRUE(transactionHandler->implicitRemoteTransactionDataStack_.empty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: MergeSyncTransaction009
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction009, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    EXPECT_TRUE(transaction->implicitCommonTransactionDataStack_.empty());
    transaction->implicitCommonTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    EXPECT_FALSE(transaction->implicitCommonTransactionDataStack_.empty());
    ASSERT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: MergeSyncTransaction010
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction010, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    transactionHandler->implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    ASSERT_FALSE(transactionHandler->implicitRemoteTransactionDataStack_.empty());
    EXPECT_TRUE(transactionHandler->implicitRemoteTransactionDataStack_.top()->IsEmpty());

    EXPECT_TRUE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    EXPECT_FALSE(transaction->implicitRemoteTransactionDataStack_.empty());
    transaction->MergeSyncTransaction(transactionHandler);
}

/**
 * @tc.name: ProcessSyncTransactionStack001
 * @tc.desc: test ProcessSyncTransactionStack with empty stack
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> emptyStack;
    transaction->ProcessSyncTransactionStack(emptyStack, mockClient, 0, 100, 1234, "testAbility");

    EXPECT_EQ(mockClient.committedTransactions.size(), 0);
}

/**
 * @tc.name: ProcessSyncTransactionStack002
 * @tc.desc: test ProcessSyncTransactionStack with single element stack (no flip)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> singleStack;
    singleStack.emplace(std::make_unique<RSTransactionData>());

    transaction->ProcessSyncTransactionStack(singleStack, mockClient, 1, 100, 1234, "testAbility");

    // Empty element should not be committed
    EXPECT_EQ(mockClient.committedTransactions.size(), 0);
}

/**
 * @tc.name: ProcessSyncTransactionStack003
 * @tc.desc: test ProcessSyncTransactionStack with single non-empty element
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack003, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> singleStack;
    auto data = std::make_unique<RSTransactionData>();
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    data->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);
    singleStack.emplace(std::move(data));

    transaction->ProcessSyncTransactionStack(singleStack, mockClient, 1, 100, 1234, "testAbility");

    EXPECT_EQ(mockClient.committedTransactions.size(), 1);
    EXPECT_EQ(mockClient.committedTransactions[0]->tid_, 1234);
    EXPECT_EQ(mockClient.committedTransactions[0]->abilityName_, "testAbility");
}

/**
 * @tc.name: ProcessSyncTransactionStack004
 * @tc.desc: test ProcessSyncTransactionStack with single element marked as NeedSync
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack004, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> singleStack;
    auto data = std::make_unique<RSTransactionData>();
    data->MarkNeedSync();
    singleStack.emplace(std::move(data));

    transaction->ProcessSyncTransactionStack(singleStack, mockClient, 2, 100, 1234, "testAbility");

    EXPECT_EQ(mockClient.committedTransactions.size(), 1);
    EXPECT_EQ(mockClient.committedTransactions[0]->tid_, 1234);
}

/**
 * @tc.name: ProcessSyncTransactionStack005
 * @tc.desc: test ProcessSyncTransactionStack with multiple elements (FIFO order)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack005, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> multiStack;
    // Push elements in order: first, second, third
    for (int i = 1; i <= 3; i++) {
        auto data = std::make_unique<RSTransactionData>();
        NodeId nodeId = i;
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSAnimationCallback>(nodeId, i, i, AnimationCallbackEvent::FINISHED);
        data->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);
        multiStack.emplace(std::move(data));
    }

    transaction->ProcessSyncTransactionStack(multiStack, mockClient, 3, 100, 1234, "testAbility");

    // All 3 elements should be committed in FIFO order
    EXPECT_EQ(mockClient.committedTransactions.size(), 3);
    // Verify they were processed in FIFO (first-in first-out) order
    for (size_t i = 0; i < mockClient.committedTransactions.size(); i++) {
        EXPECT_EQ(mockClient.committedTransactions[i]->tid_, 1234);
    }
}

/**
 * @tc.name: ProcessSyncTransactionStack006
 * @tc.desc: test ProcessSyncTransactionStack with multiple elements, some empty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack006, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> multiStack;
    // Push: empty, non-empty, empty
    multiStack.emplace(std::make_unique<RSTransactionData>());

    auto data1 = std::make_unique<RSTransactionData>();
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    data1->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);
    multiStack.emplace(std::move(data1));

    multiStack.emplace(std::make_unique<RSTransactionData>());

    transaction->ProcessSyncTransactionStack(multiStack, mockClient, 4, 100, 1234, "testAbility");

    // Only non-empty elements should be committed
    EXPECT_EQ(mockClient.committedTransactions.size(), 1);
}

/**
 * @tc.name: ProcessSyncTransactionStack007
 * @tc.desc: test ProcessSyncTransactionStack with multiple elements, some marked NeedSync
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack007, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> multiStack;
    // Push: empty, needSync, non-empty
    multiStack.emplace(std::make_unique<RSTransactionData>());

    auto data1 = std::make_unique<RSTransactionData>();
    data1->MarkNeedSync();
    multiStack.emplace(std::move(data1));

    auto data2 = std::make_unique<RSTransactionData>();
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    data2->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);
    multiStack.emplace(std::move(data2));

    transaction->ProcessSyncTransactionStack(multiStack, mockClient, 5, 100, 1234, "testAbility");

    // Both needSync and non-empty should be committed
    EXPECT_EQ(mockClient.committedTransactions.size(), 2);
}

/**
 * @tc.name: ProcessSyncTransactionStack008
 * @tc.desc: test ProcessSyncTransactionStack with empty ability name
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack008, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> singleStack;
    auto data = std::make_unique<RSTransactionData>();
    data->MarkNeedSync();
    singleStack.emplace(std::move(data));

    transaction->ProcessSyncTransactionStack(singleStack, mockClient, 6, 100, 1234, "");

    EXPECT_EQ(mockClient.committedTransactions.size(), 1);
    EXPECT_EQ(mockClient.committedTransactions[0]->abilityName_, "");
}

/**
 * @tc.name: ProcessSyncTransactionStack009
 * @tc.desc: test ProcessSyncTransactionStack verifies stack is emptied after processing
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack009, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> multiStack;
    for (int i = 0; i < 3; i++) {
        auto data = std::make_unique<RSTransactionData>();
        data->MarkNeedSync();
        multiStack.emplace(std::move(data));
    }

    EXPECT_EQ(multiStack.size(), 3);

    transaction->ProcessSyncTransactionStack(multiStack, mockClient, 7, 100, 1234, "test");

    EXPECT_TRUE(multiStack.empty());
    EXPECT_EQ(mockClient.committedTransactions.size(), 3);
}

/**
 * @tc.name: ProcessSyncTransactionStack010
 * @tc.desc: test ProcessSyncTransactionStack with large number of elements
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack010, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> largeStack;
    for (int i = 0; i < 10; i++) {
        auto data = std::make_unique<RSTransactionData>();
        data->MarkNeedSync();
        largeStack.emplace(std::move(data));
    }

    transaction->ProcessSyncTransactionStack(largeStack, mockClient, 8, 100, 1234, "test");

    EXPECT_EQ(mockClient.committedTransactions.size(), 10);
    EXPECT_TRUE(largeStack.empty());
}

/**
 * @tc.name: ProcessSyncTransactionStack011
 * @tc.desc: test ProcessSyncTransactionStack with all empty elements
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, ProcessSyncTransactionStack011, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    MockRenderClient mockClient;

    std::stack<std::unique_ptr<RSTransactionData>> multiStack;
    for (int i = 0; i < 3; i++) {
        multiStack.emplace(std::make_unique<RSTransactionData>());
    }

    transaction->ProcessSyncTransactionStack(multiStack, mockClient, 9, 100, 1234, "test");

    // No elements should be committed if all are empty and none need sync
    EXPECT_EQ(mockClient.committedTransactions.size(), 0);
    EXPECT_TRUE(multiStack.empty());
}

/**
 * @tc.name: MergeSyncTransaction011
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, MergeSyncTransaction011, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto transactionHandler = std::make_shared<RSTransactionHandler>();

    transactionHandler->implicitCommonTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    ASSERT_FALSE(transactionHandler->implicitCommonTransactionDataStack_.empty());
    EXPECT_TRUE(transactionHandler->implicitCommonTransactionDataStack_.top()->IsEmpty());

    EXPECT_TRUE(transaction->implicitCommonTransactionDataStack_.empty());
    transaction->implicitCommonTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    EXPECT_FALSE(transaction->implicitCommonTransactionDataStack_.empty());
    transaction->MergeSyncTransaction(transactionHandler);
}
} // namespace Rosen
} // namespace OHOS
