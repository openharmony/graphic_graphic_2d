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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    auto hybridrenderEnable = system::GetParameter("const.graphics.hybridrenderenable", "0");
    system::SetParameter("const.graphics.hybridrenderenable", "0");
    transaction->FlushImplicitTransaction(timestamp);
    system::SetParameter("const.graphics.hybridrenderenable", hybridrenderEnable);
}

#ifdef RS_ENABLE_VK
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    CommitTransactionCallback callback =
        [] (std::shared_ptr<RSIRenderClient> &renderServiceClient,
        std::unique_ptr<RSTransactionData>&& rsTransactionData, uint32_t& transactionDataIndex) {};
    RSTransactionHandler::SetCommitTransactionCallback(callback);
    transaction->FlushImplicitTransaction(timestamp);
}
#endif

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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
    transaction->CommitSyncTransaction(timestamp, "abilityName");
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
    transaction->CommitSyncTransaction(timestamp, "abilityName");
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
    transaction->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(timestamp, "abilityName");
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(timestamp, "abilityName");
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
    transaction->AddCommand(command, true, FollowType::FOLLOW_TO_PARENT, 1);
    transaction->CommitSyncTransaction(timestamp, "abilityName");
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
        std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
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
 * @tc.name: StartCloseSyncTransactionFallbackTaskTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionHandlerTest, StartCloseSyncTransactionFallbackTaskTest, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("runner");
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    transaction->StartCloseSyncTransactionFallbackTask(nullptr, true);
    transaction->StartCloseSyncTransactionFallbackTask(nullptr, false);
    ASSERT_TRUE(transaction->taskNames_.empty());

    transaction->StartCloseSyncTransactionFallbackTask(handler, true);
    transaction->StartCloseSyncTransactionFallbackTask(handler, false);
    ASSERT_TRUE(transaction->taskNames_.empty());

    transaction->StartCloseSyncTransactionFallbackTask(handler, true);
    sleep(8);
    ASSERT_TRUE(transaction->taskNames_.empty());
}
} // namespace Rosen
} // namespace OHOS
