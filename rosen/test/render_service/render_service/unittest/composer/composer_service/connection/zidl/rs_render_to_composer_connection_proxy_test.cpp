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
#include <set>
#include <vector>
#include "rs_render_to_composer_connection_stub.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_layer_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TestRenderToComposerStub : public RSRenderToComposerConnectionStub {
public:
    bool CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData) override
    {
        committed_ = (transactionData != nullptr);
        return true;
    }
    void ClearFrameBuffers() override { cleared_ = true; }
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId) override { cleanedSurfaceId_ = surfaceId; }
    void ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds) override { cacheIds_ = bufferIds; }
    void SetScreenBacklight(uint32_t level) override { backlight_ = level; }
    void SetComposerToRenderConnection(const sptr<RSIComposerToRenderConnection>& composerToRenderConn) override {}

    bool committed_ {false};
    bool cleared_ {false};
    uint64_t cleanedSurfaceId_ {0};
    std::set<uint64_t> cacheIds_ {};
    uint32_t backlight_ {0};
};

class RSRenderToComposerConnectionProxyTest : public Test {};

/**
 * Function: ProxyStub_AllCommands
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderToComposerConnection stub/proxy pair
 *                  2. call CommitLayers with minimal payload and verify committed
 *                  3. call ClearFrameBuffers and verify cleared
 *                  4. call CleanLayerBufferBySurfaceId and verify forwarded id
 *                  5. call ClearRedrawGPUCompositionCache and verify ids count
 *                  6. call SetScreenBacklight and verify level
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, ProxyStub_AllCommands, TestSize.Level1)
{
    sptr<TestRenderToComposerStub> stub = sptr<TestRenderToComposerStub>::MakeSptr();
    sptr<IRemoteObject> obj = stub->AsObject();
    RSRenderToComposerConnectionProxy proxy(obj);

    std::unique_ptr<RSLayerTransactionData> tx(new RSLayerTransactionData());
    tx->GetPayload().emplace_back(static_cast<uint64_t>(1u), std::shared_ptr<RSLayerParcel>(nullptr));
    proxy.CommitLayers(tx);
    EXPECT_TRUE(stub->committed_);

    proxy.ClearFrameBuffers();
    EXPECT_TRUE(stub->cleared_);

    proxy.CleanLayerBufferBySurfaceId(123u);
    EXPECT_EQ(stub->cleanedSurfaceId_, 123u);

    std::set<uint64_t> ids { 7u, 8u };
    proxy.ClearRedrawGPUCompositionCache(ids);
    EXPECT_EQ(stub->cacheIds_.size(), 2u);

    proxy.SetScreenBacklight(88u);
    EXPECT_EQ(stub->backlight_, 88u);
}

/**
 * Function: Proxy_SendRequest_RemoteNull_ErrorPaths
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with nullptr remote
 *                  2. call ClearFrameBuffers/CleanLayerBufferBySurfaceId
 *                  3. call ClearRedrawGPUCompositionCache/SetScreenBacklight
 *                  4. ensure functions execute without crash to hit SendRequest error branch
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_SendRequest_RemoteNull_ErrorPaths, TestSize.Level1)
{
    sptr<IRemoteObject> nullObj = nullptr;
    RSRenderToComposerConnectionProxy proxy(nullObj);
    proxy.ClearFrameBuffers();
    proxy.CleanLayerBufferBySurfaceId(1u);
    std::set<uint64_t> ids {1u};
    proxy.ClearRedrawGPUCompositionCache(ids);
    proxy.SetScreenBacklight(1u);
    SUCCEED();
}
} // namespace OHOS::Rosen
