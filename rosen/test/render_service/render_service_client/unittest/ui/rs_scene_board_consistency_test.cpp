/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "command/rs_canvas_node_command.h"
#include "command/rs_command_factory.h"
#include "command/rs_node_command.h"
#include "command/rs_surface_node_command.h"
#include "command/rs_ui_director_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_transaction_data.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_ui_context_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSSceneBoardConsistencyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    // Marks the client context via the public RSUIDirector API (owner of the mark) and
    // sets up the server-side mirror: GoCreate + Mark for the given client context token.
    static std::shared_ptr<RSUIDirector> MarkClientInstance(const std::shared_ptr<RSUIContext>& uiContext);
    static void MarkServerInstance(RSContext& context, const std::shared_ptr<RSUIContext>& uiContext);
};

void RSSceneBoardConsistencyTest::SetUpTestCase() {}
void RSSceneBoardConsistencyTest::TearDownTestCase() {}
void RSSceneBoardConsistencyTest::SetUp() {}
void RSSceneBoardConsistencyTest::TearDown() {}

std::shared_ptr<RSUIDirector> RSSceneBoardConsistencyTest::MarkClientInstance(
    const std::shared_ptr<RSUIContext>& uiContext)
{
    OHOS::sptr<OHOS::IRemoteObject> remote;
    auto director = RSUIDirector::Create(remote, uiContext);
    if (director != nullptr) {
        director->MarkAsSceneBoard();
    }
    return director;
}

void RSSceneBoardConsistencyTest::MarkServerInstance(RSContext& context,
    const std::shared_ptr<RSUIContext>& uiContext)
{
    pid_t pid = getpid();
    uint64_t token = uiContext->GetToken();
    NodeId directorNodeId = (static_cast<NodeId>(pid) << 32) | 1;
    RSUIDirectorCommandHelper::GoCreate(context, directorNodeId, token);
    RSUIDirectorCommandHelper::MarkSceneBoard(context, directorNodeId, token);
}

namespace {
constexpr uint64_t CLIENT_NULL_TOKEN = 0;

// Client-side node ids must live in the same pid slot the server extracts via ExtractPid.
NodeId MakeClientNodeId(uint32_t seq)
{
    return (static_cast<NodeId>(getpid()) << 32) | seq;
}
} // namespace

/**
 * @tc.name: CreateConsistency001
 * @tc.desc: Client node creation maps to a server node with the same token; the SceneBoard
 *           verdict matches the client context flag on both instances.
 * @tc.type: FUNC
 */
HWTEST_F(RSSceneBoardConsistencyTest, CreateConsistency001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> remote;
    auto contextA = RSUIContextManager::MutableInstance().CreateRSUIContext(remote);
    auto contextB = RSUIContextManager::MutableInstance().CreateRSUIContext(remote);
    ASSERT_NE(contextA, nullptr);
    ASSERT_NE(contextB, nullptr);
    ASSERT_NE(contextA->GetToken(), contextB->GetToken());
    auto directorA = MarkClientInstance(contextA);
    ASSERT_NE(directorA, nullptr);
    EXPECT_TRUE(contextA->IsSceneBoard());
    EXPECT_FALSE(contextB->IsSceneBoard());

    RSContext serverContext;
    MarkServerInstance(serverContext, contextA);

    // Node created under the marked instance A: server mirror must be marked and token-registered.
    NodeId idA = MakeClientNodeId(1);
    RSCanvasNodeCommandHelper::Create(serverContext, idA, false, contextA->GetToken());
    auto nodeA = serverContext.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(idA);
    ASSERT_NE(nodeA, nullptr);
    EXPECT_EQ(nodeA->GetUIContextToken(), contextA->GetToken());
    EXPECT_TRUE(nodeA->IsSceneBoard());

    // Node created under the unmarked instance B (same process): process-level fallback
    // marks it too — SceneBoard is a process identity.
    NodeId idB = MakeClientNodeId(2);
    RSCanvasNodeCommandHelper::Create(serverContext, idB, false, contextB->GetToken());
    auto nodeB = serverContext.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(idB);
    ASSERT_NE(nodeB, nullptr);
    EXPECT_EQ(nodeB->GetUIContextToken(), contextB->GetToken());
    EXPECT_TRUE(nodeB->IsSceneBoard());

    // Context-less legacy path (token 0): instance registered as neither.
    NodeId idLegacy = MakeClientNodeId(3);
    RSCanvasNodeCommandHelper::Create(serverContext, idLegacy, false, CLIENT_NULL_TOKEN);
    auto nodeLegacy = serverContext.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(idLegacy);
    ASSERT_NE(nodeLegacy, nullptr);
    EXPECT_EQ(nodeLegacy->GetUIContextToken(), CLIENT_NULL_TOKEN);
    EXPECT_FALSE(nodeLegacy->IsSceneBoard());
}

/**
 * @tc.name: ProcessFallbackConsistency001
 * @tc.desc: Client process-level fallback: an unmarked context of a marked process reports
 *           isSceneBoard=true for the sync-IPC value (HasSceneBoardInstance), matching the
 *           server's process-level query.
 * @tc.type: FUNC
 */
HWTEST_F(RSSceneBoardConsistencyTest, ProcessFallbackConsistency001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> remote;
    auto contextA = RSUIContextManager::MutableInstance().CreateRSUIContext(remote);
    auto contextB = RSUIContextManager::MutableInstance().CreateRSUIContext(remote);
    ASSERT_NE(contextA, nullptr);
    ASSERT_NE(contextB, nullptr);

    // The context manager is a process-wide singleton, so earlier test cases may already
    // have marked a context; only the post-mark state is deterministic here.
    EXPECT_FALSE(contextB->IsSceneBoard());

    auto directorA = MarkClientInstance(contextA);
    ASSERT_NE(directorA, nullptr);

    // After marking A: process-level fallback becomes true even though B itself is unmarked.
    EXPECT_TRUE(RSUIContextManager::Instance().HasSceneBoardInstance());
    EXPECT_FALSE(contextB->IsSceneBoard());
}

/**
 * @tc.name: CreateConsistency002
 * @tc.desc: Surface creation (both Create variants) mirrors the same consistency rules.
 * @tc.type: FUNC
 */
HWTEST_F(RSSceneBoardConsistencyTest, CreateConsistency002, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> remote;
    auto contextA = RSUIContextManager::MutableInstance().CreateRSUIContext(remote);
    ASSERT_NE(contextA, nullptr);
    auto directorA = MarkClientInstance(contextA);
    ASSERT_NE(directorA, nullptr);
    EXPECT_TRUE(contextA->IsSceneBoard());

    RSContext serverContext;
    MarkServerInstance(serverContext, contextA);

    NodeId idCreate = MakeClientNodeId(4);
    SurfaceNodeCommandHelper::Create(serverContext, idCreate, RSSurfaceNodeType::DEFAULT, false,
        contextA->GetToken());
    auto surfaceCreate = serverContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(idCreate);
    ASSERT_NE(surfaceCreate, nullptr);
    EXPECT_TRUE(surfaceCreate->IsSceneBoard());
    EXPECT_EQ(surfaceCreate->GetUIContextToken(), contextA->GetToken());

    NodeId idConfig = MakeClientNodeId(5);
    SurfaceNodeCommandHelper::CreateWithConfig(serverContext, idConfig, "consistency", 1,
        SurfaceWindowType::DEFAULT_WINDOW, contextA->GetToken());
    auto surfaceConfig = serverContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(idConfig);
    ASSERT_NE(surfaceConfig, nullptr);
    EXPECT_TRUE(surfaceConfig->IsSceneBoard());
    EXPECT_EQ(surfaceConfig->GetUIContextToken(), contextA->GetToken());
}

/**
 * @tc.name: MigrationConsistency001
 * @tc.desc: Instance info changes on migration: server current-owner token follows the new
 *           context while the birth identity stays; history keeps both tokens.
 * @tc.type: FUNC
 */
HWTEST_F(RSSceneBoardConsistencyTest, MigrationConsistency001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> remote;
    auto contextA = RSUIContextManager::MutableInstance().CreateRSUIContext(remote);
    auto contextB = RSUIContextManager::MutableInstance().CreateRSUIContext(remote);
    ASSERT_NE(contextA, nullptr);
    ASSERT_NE(contextB, nullptr);
    auto directorA = MarkClientInstance(contextA);
    ASSERT_NE(directorA, nullptr);

    RSContext serverContext;
    MarkServerInstance(serverContext, contextA);

    // Born under A (marked).
    NodeId id = MakeClientNodeId(6);
    RSCanvasNodeCommandHelper::Create(serverContext, id, false, contextA->GetToken());
    auto node = serverContext.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
    ASSERT_TRUE(node->IsSceneBoard());
    ASSERT_EQ(node->GetUIContextToken(), contextA->GetToken());

    // Client migration A -> B mirrors the SetUIContextToken command the client re-sends.
    RSNodeCommandHelper::SetUIToken(serverContext, id, contextB->GetToken());

    // Current owner follows the migration; birth identity does not.
    EXPECT_EQ(node->GetUIContextToken(), contextB->GetToken());
    EXPECT_TRUE(node->IsSceneBoard());
    // History registry keeps both the birth token and the current token.
    auto tokens = node->GetUIContextTokenList();
    EXPECT_EQ(tokens.size(), 2u);
    EXPECT_NE(std::find(tokens.begin(), tokens.end(), contextA->GetToken()), tokens.end());
    EXPECT_NE(std::find(tokens.begin(), tokens.end(), contextB->GetToken()), tokens.end());

    // Client view (context flag) and server view (birth identity) legitimately differ here.
    EXPECT_FALSE(contextB->IsSceneBoard());
    EXPECT_TRUE(node->IsSceneBoard());
}

/**
 * @tc.name: CommandMarshallingConsistency001
 * @tc.desc: The Create command marshalling round-trip preserves the node id (token trails
 *           the parcel and is consumed by Unmarshalling), keeping the client-issued identity
 *           identical to what the server decodes.
 * @tc.type: FUNC
 */
HWTEST_F(RSSceneBoardConsistencyTest, CommandMarshallingConsistency001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> remote;
    auto context = RSUIContextManager::MutableInstance().CreateRSUIContext(remote);
    ASSERT_NE(context, nullptr);
    auto director = MarkClientInstance(context);
    ASSERT_NE(director, nullptr);
    const NodeId id = MakeClientNodeId(7);
    const uint64_t token = context->GetToken();

    Parcel parcel;
    RSCanvasNodeCreate command(id, false, token);
    ASSERT_TRUE(command.Marshalling(parcel));

    // Server-side decoding restores the command; the round-trip proves the trailing token
    // did not desync the stream (a size mismatch would fail decoding).
    // Marshalling writes the type/subType header before the params, so the direct
    // Unmarshalling entry must skip it exactly like the service does.
    parcel.RewindRead(0);
    uint16_t headerType = 0;
    uint16_t headerSubType = 0;
    ASSERT_TRUE(parcel.ReadUint16(headerType));
    ASSERT_TRUE(parcel.ReadUint16(headerSubType));
    EXPECT_EQ(headerType, static_cast<uint16_t>(RSCommandType::CANVAS_NODE));
    auto decoded = RSCanvasNodeCreate::Unmarshalling(parcel);
    ASSERT_NE(decoded, nullptr);
    EXPECT_EQ(decoded->GetNodeId(), id);

    // Decode the same parcel through the command factory path used by the real service.
    Parcel parcel2;
    RSCanvasNodeCreate command2(id, false, token);
    ASSERT_TRUE(command2.Marshalling(parcel2));
    parcel2.RewindRead(0);
    uint16_t type = 0;
    uint16_t subType = 0;
    ASSERT_TRUE(parcel2.ReadUint16(type));
    ASSERT_TRUE(parcel2.ReadUint16(subType));
    EXPECT_EQ(type, static_cast<uint16_t>(RSCommandType::CANVAS_NODE));
    auto factoryDecoded = RSCommandFactory::Instance().GetUnmarshallingFunc(type, subType);
    ASSERT_NE(factoryDecoded, nullptr);
    auto factoryCommand = factoryDecoded(parcel2);
    ASSERT_NE(factoryCommand, nullptr);
    EXPECT_EQ(factoryCommand->GetNodeId(), id);
}

} // namespace OHOS::Rosen
