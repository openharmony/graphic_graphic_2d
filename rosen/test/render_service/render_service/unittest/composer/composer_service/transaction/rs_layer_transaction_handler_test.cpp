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
    handler.AddRSLayerParcel(0, nullParcel);
    EXPECT_TRUE(handler.IsEmpty());
    handler.rsLayerTransactionData_ = std::make_unique<RSLayerTransactionData>();
    handler.AddRSLayerParcel(0, nullParcel);
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
    EXPECT_TRUE(handler.IsEmpty());
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(1);
    auto zCmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    RSLayerId id = static_cast<RSLayerId>(100u);
    std::shared_ptr<RSLayerParcel> parcel = std::make_shared<RSUpdateRSLayerCmd>(id, zCmd);
    handler.AddRSLayerParcel(id, parcel);
    EXPECT_FALSE(handler.IsEmpty());

    ComposerInfo info;
    bool ok = handler.CommitRSLayerTransaction(info, 0u, "");
    EXPECT_FALSE(ok);
    EXPECT_FALSE(handler.IsEmpty());
}

/**
 * Function: SetProxy_NonNull_CommitFail_KeepsData
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. add a valid RSLayerParcel
 *                  2. set a non-null proxy connection (remote=null)
 *                  3. commit and expect false; handler keeps data
 */
HWTEST(RSLayerTransactionHandlerTest, SetProxy_NonNull_CommitFail_KeepsData, TestSize.Level1)
{
    RSLayerTransactionHandler handler;
    RSLayerId id = static_cast<RSLayerId>(777u);
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(3);
    auto zCmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    std::shared_ptr<RSLayerParcel> parcel = std::make_shared<RSUpdateRSLayerCmd>(id, zCmd);
    handler.AddRSLayerParcel(id, parcel);
    ASSERT_FALSE(handler.IsEmpty());

    class MockRemoteLocal : public IRemoteObject {
    public:
        MockRemoteLocal() : IRemoteObject(IRSComposerToRenderConnection::GetDescriptor()) {}
        int32_t GetObjectRefCount() override { return 1; }
        int SendRequest(uint32_t, MessageParcel &, MessageParcel &, MessageOption &) override { return 1; }
        bool AddDeathRecipient(const sptr<DeathRecipient> &) override { return false; }
        bool RemoveDeathRecipient(const sptr<DeathRecipient> &) override { return false; }
        int Dump(int, const std::vector<std::u16string> &) override { return 0; }
    };
    sptr<IRemoteObject> nullRemote = sptr<MockRemoteLocal>::MakeSptr();
    auto proxy = sptr<RSRenderToComposerConnectionProxy>::MakeSptr(nullRemote);
    handler.SetRSComposerConnectionProxy(proxy);

    ComposerInfo info {};
    bool ok = handler.CommitRSLayerTransaction(info, 0u, "");
    EXPECT_FALSE(ok);
    EXPECT_FALSE(handler.IsEmpty());
}

/**
 * Function: SetProxy_Null_Ignored
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. call SetRSComposerConnectionProxy(nullptr)
 *                  2. expect no crash and connection remains unset
 */
HWTEST(RSLayerTransactionHandlerTest, SetProxy_Null_Ignored, TestSize.Level1)
{
    RSLayerTransactionHandler handler;
    sptr<IRSRenderToComposerConnection> nullConn;
    handler.SetRSComposerConnectionProxy(nullConn);
    ComposerInfo info {};
    bool ok = handler.CommitRSLayerTransaction(info, 0u, "");
    EXPECT_FALSE(ok);
}
}
