/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "rs_layer_transaction_handler.h"
#define UnmarshallingFunc RSLayerParcelFactory_UnmarshallingFunc
#include "rs_surface_layer_parcel.h"
#undef UnmarshallingFunc
#include "rs_render_layer_cmd.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_render_to_composer_connection.h"
#include "rs_render_composer_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen{
class RSLayerTransactionHandlerTest : public Test {};

/**
 * Function: Commit_WithNull_Connection_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct handler with null connection
 *                  2. call CommitRSLayerTransaction
 *                  3. expect no crash and IsEmpty callable
 */
HWTEST(RSLayerTransactionHandlerTest, Commit_WithNull_Connection_NoCrash, TestSize.Level1)
{
    RSLayerTransactionHandler handler;
    ComposerInfo info;
    handler.CommitRSLayerTransaction(info, 0, "");
    EXPECT_TRUE(handler.IsEmpty());
}

/**
 * Function: AddRSLayerParcel_Null_Ignored
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call AddRSLayerParcel with nullptr
 *                  2. verify handler remains empty
 */
HWTEST(RSLayerTransactionHandlerTest, AddRSLayerParcel_Null_Ignored, TestSize.Level1)
{
    RSLayerTransactionHandler handler;
    std::shared_ptr<RSLayerParcel> nullParcel;
    handler.AddRSLayerParcel(nullParcel, 0);
    EXPECT_TRUE(handler.IsEmpty());
}

/**
 * Function: Commit_WithNonEmpty_WithoutConnection_Fails
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. add a valid RSLayerParcel
 *                  2. call CommitRSLayerTransaction without setting connection
 *                  3. expect false and handler remains non-empty
 */
HWTEST(RSLayerTransactionHandlerTest, Commit_WithNonEmpty_WithoutConnection_Fails, TestSize.Level1)
{
    RSLayerTransactionHandler handler;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(1);
    auto zCmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    RSLayerId id = static_cast<RSLayerId>(100u);
    std::shared_ptr<RSLayerParcel> parcel = std::make_shared<RSUpdateRSLayerCmd>(id, zCmd);
    handler.AddRSLayerParcel(parcel, id);
    EXPECT_FALSE(handler.IsEmpty());

    ComposerInfo info;
    bool ok = handler.CommitRSLayerTransaction(info, 0u, "");
    EXPECT_FALSE(ok);
    EXPECT_FALSE(handler.IsEmpty());
}

/**
 * Function: Commit_WithProxyToStub_FailsAndKeepsData
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set connection to proxy bound to stub with null agent
 *                  2. add a valid RSLayerParcel
 *                  3. call CommitRSLayerTransaction; expect false (agent returns false)
 *                  4. verify handler remains non-empty
 */
HWTEST(RSLayerTransactionHandlerTest, Commit_WithProxyToStub_FailsAndKeepsData, TestSize.Level1)
{
    // Bind Proxy to an in-process Stub (server) with agent that returns false
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> server = sptr<RSRenderToComposerConnection>::MakeSptr(
        "composer_conn", 0u, agent);
    sptr<RSRenderToComposerConnectionProxy> proxy = new RSRenderToComposerConnectionProxy(server);

    RSLayerTransactionHandler handler;
    handler.SetRSComposerConnectionProxy(proxy);

    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(2);
    auto zCmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    RSLayerId id = static_cast<RSLayerId>(200u);
    std::shared_ptr<RSLayerParcel> parcel = std::make_shared<RSUpdateRSLayerCmd>(id, zCmd);
    handler.AddRSLayerParcel(parcel, id);
    EXPECT_FALSE(handler.IsEmpty());

    ComposerInfo info;
    bool ok = handler.CommitRSLayerTransaction(info, 0u, "");
    EXPECT_FALSE(ok);
    EXPECT_FALSE(handler.IsEmpty());
}
}
