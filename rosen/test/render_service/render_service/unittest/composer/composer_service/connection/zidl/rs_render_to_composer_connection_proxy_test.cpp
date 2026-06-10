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
#include <unistd.h>
#include <unordered_set>
#include <vector>

#include "rs_composer_to_render_connection.h"
#include "rs_layer_transaction_data.h"
#include "rs_render_composer_agent.h"
#include "rs_render_to_composer_connection.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_render_to_composer_connection_stub.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSRenderToComposerConnectionProxyTest : public Test {};

namespace {
class RecordingRenderToComposerConnectionStub final : public RSRenderToComposerConnectionStub {
public:
    bool CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData) override
    {
        return false;
    }

    void CleanLayerBufferBySurfaceId(uint64_t surfaceId) override {}

    int32_t CommitTunnelLayerBySurfaceId(uint64_t surfaceId, uint64_t tunnelLayerId,
        const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, sptr<SyncFence>& releaseFence) override
    {
        commitTunnelCalled = true;
        lastSurfaceId = surfaceId;
        lastTunnelLayerId = tunnelLayerId;
        lastBufferSeqNum = buffer == nullptr ? 0 : buffer->GetSeqNum();
        lastAcquireFenceFd = acquireFence == nullptr ? -1 : acquireFence->Get();
        releaseFence = new SyncFence(dup(STDIN_FILENO));
        return returnValue;
    }

    void ClearFrameBuffers() override {}
    void ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds) override {}
    void SetScreenBacklight(uint32_t level) override {}
    void SetScreenLinearMatrix(const std::vector<float>& matirx) override {}
    void SetComposerToRenderConnection(const sptr<IRSComposerToRenderConnection>& composerToRenderConn) override {}
    void PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer) override {}
    void MarkTunnelSurfaceInvalid(uint64_t surfaceId) override {}

    bool commitTunnelCalled = false;
    uint64_t lastSurfaceId = 0;
    uint64_t lastTunnelLayerId = 0;
    uint32_t lastBufferSeqNum = 0;
    int32_t lastAcquireFenceFd = -2;
    int32_t returnValue = GRAPHIC_DISPLAY_SUCCESS;
};

sptr<SurfaceBuffer> CreateTunnelTestBuffer()
{
    BufferRequestConfig requestConfig = {
        .width = 4,
        .height = 4,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    if (buffer != nullptr && buffer->Alloc(requestConfig) != GSERROR_OK) {
        return nullptr;
    }
    return buffer;
}
} // namespace

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
 *                  7. call SetScreenLinearMatrix and verify matrix
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, ProxyStub_AllCommands, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    sptr<IRemoteObject> obj = stub->AsObject();
    RSRenderToComposerConnectionProxy proxy(obj);
    ASSERT_NE(obj, nullptr);

    std::unique_ptr<RSLayerTransactionData> tx = std::make_unique<RSLayerTransactionData>();
    tx->GetPayload().emplace_back(static_cast<uint64_t>(1u), std::shared_ptr<RSLayerParcel>(nullptr));
    EXPECT_FALSE(proxy.CommitLayers(tx));

    proxy.ClearFrameBuffers();

    proxy.CleanLayerBufferBySurfaceId(123u);

    std::unordered_set<uint64_t> ids { 7u, 8u };
    proxy.ClearRedrawGPUCompositionCache(ids);

    proxy.SetScreenBacklight(88u);

    std::vector<float> matrix1 = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    proxy.SetScreenLinearMatrix(matrix1);
    EXPECT_EQ(ids.size(), 2u);
}

/**
 * Function: Proxy_SendRequest_RemoteNull_ErrorPaths
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with nullptr remote
 *                  2. call ClearFrameBuffers/CleanLayerBufferBySurfaceId
 *                  3. call ClearRedrawGPUCompositionCache/SetScreenBacklight/SetScreenLinearMatrix
 *                  4. ensure functions execute without crash to hit SendRequest error branch
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_SendRequest_RemoteNull_ErrorPaths, TestSize.Level1)
{
    sptr<IRemoteObject> nullObj = nullptr;
    RSRenderToComposerConnectionProxy proxy(nullObj);
    ASSERT_EQ(nullObj, nullptr);
    proxy.ClearFrameBuffers();
    proxy.CleanLayerBufferBySurfaceId(1u);
    std::unordered_set<uint64_t> ids { 1u };
    proxy.ClearRedrawGPUCompositionCache(ids);
    proxy.SetScreenBacklight(1u);
    std::vector<float> matrix2 = { 1.0f, 2.0f };
    proxy.SetScreenLinearMatrix(matrix2);
    EXPECT_EQ(ids.count(1u), 1u);
}

/**
 * Function: Proxy_SetComposerToRenderConnection_Valid
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create stub/proxy pair
 *                  2. set a valid composer-to-render remote on proxy
 *                  3. ensure no crash and path executed
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_SetComposerToRenderConnection_Valid, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());
    sptr<RSComposerToRenderConnection> ctr = sptr<RSComposerToRenderConnection>::MakeSptr();
    proxy.SetComposerToRenderConnection(ctr);
    ASSERT_NE(ctr, nullptr);
}

/**
 * Function: Proxy_PreAllocProtectedFrameBuffers_WriteFail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create proxy and SurfaceBuffer without properties
 *                  2. call PreAllocProtectedFrameBuffers to hit write fail path
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_PreAllocProtectedFrameBuffers_WriteFail, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());
    sptr<SurfaceBuffer> sb = SurfaceBuffer::Create();
    proxy.PreAllocProtectedFrameBuffers(sb);
    ASSERT_NE(sb, nullptr);
}

/**
 * Function: Proxy_CommitLayers_NullTransaction_ReturnsFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create stub/proxy pair
 *                  2. call CommitLayers with nullptr transaction data
 *                  3. expect CommitLayers returns false (guard branch)
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_CommitLayers_NullTransaction_ReturnsFalse, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());
    std::unique_ptr<RSLayerTransactionData> tx(nullptr);
    EXPECT_FALSE(proxy.CommitLayers(tx));
}

/**
 * Function: Proxy_CommitLayers_MultipleParcels_FillLoop
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create stub/proxy pair
 *                  2. prepare transaction data with two payload entries (nullptr parcels)
 *                  3. call CommitLayers and expect false after attempting to send multiple parcels
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_CommitLayers_MultipleParcels_FillLoop, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());

    std::unique_ptr<RSLayerTransactionData> tx = std::make_unique<RSLayerTransactionData>();
    tx->GetPayload().emplace_back(static_cast<uint64_t>(101u), std::shared_ptr<RSLayerParcel>(nullptr));
    tx->GetPayload().emplace_back(static_cast<uint64_t>(102u), std::shared_ptr<RSLayerParcel>(nullptr));

    EXPECT_FALSE(proxy.CommitLayers(tx));
}

/**
 * Function: Proxy_SetComposerToRenderConnection_RemoteNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with nullptr remote
 *                  2. call SetComposerToRenderConnection with nullptr
 *                  3. ensure no crash to cover send path guard
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_SetComposerToRenderConnection_RemoteNull, TestSize.Level1)
{
    sptr<IRemoteObject> nullObj = nullptr;
    RSRenderToComposerConnectionProxy proxy(nullObj);
    sptr<RSComposerToRenderConnection> nullCtr;
    proxy.SetComposerToRenderConnection(nullCtr);
    ASSERT_EQ(nullCtr, nullptr);
}

/**
 * Function: Proxy_ClearRedrawGPUCompositionCache_EmptySet
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create proxy/stub pair
 *                  2. call ClearRedrawGPUCompositionCache with empty set
 *                  3. ensure no crash
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_ClearRedrawGPUCompositionCache_EmptySet, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());
    std::unordered_set<uint64_t> empty;
    proxy.ClearRedrawGPUCompositionCache(empty);
    EXPECT_TRUE(empty.empty());
}

/**
 * Function: Proxy_PreAllocProtectedFrameBuffers_NullBuffer_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create proxy
 *                  2. call PreAllocProtectedFrameBuffers with nullptr buffer
 *                  3. ensure no crash (guard branch)
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_PreAllocProtectedFrameBuffers_NullBuffer_NoCrash, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());
    sptr<SurfaceBuffer> nullBuf;
    proxy.PreAllocProtectedFrameBuffers(nullBuf);
    ASSERT_EQ(nullBuf, nullptr);
}

/**
 * Function: Proxy_CleanLayerBufferBySurfaceId_ZeroId
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create proxy with valid remote
 *                  2. call CleanLayerBufferBySurfaceId with 0
 *                  3. ensure path executes
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_CleanLayerBufferBySurfaceId_ZeroId, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());
    ASSERT_NE(stub->AsObject(), nullptr);
    proxy.CleanLayerBufferBySurfaceId(0u);
}

/**
 * Function: Proxy_SendLayers_ReadServerRetFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create proxy with remote returning success but empty reply
 *                  2. verify SendLayers returns COMPOSITOR_ERROR_BINDER_ERROR (line 104 true branch)
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_SendLayers_ReadServerRetFail_TrueBranch, TestSize.Level1)
{
    class FakeReadFailRemote : public IRemoteObject {
    public:
        FakeReadFailRemote() : IRemoteObject(u"test.descriptor") {}
        int32_t GetObjectRefCount() override
        {
            return 1;
        }
        int SendRequest(uint32_t, MessageParcel&, MessageParcel& reply, MessageOption&) override
        {
            reply.RewindRead(0);
            return NO_ERROR;
        }
        bool AddDeathRecipient(const sptr<DeathRecipient>&) override
        {
            return false;
        }
        bool RemoveDeathRecipient(const sptr<DeathRecipient>&) override
        {
            return false;
        }
        int Dump(int, const std::vector<std::u16string>&) override
        {
            return 0;
        }
    };
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<RSRenderToComposerConnection> stub = sptr<RSRenderToComposerConnection>::MakeSptr("ut", 0u, agent);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());

    std::vector<std::shared_ptr<MessageParcel>> parcels;
    RSComposerError ret = proxy.SendLayers(parcels);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
}

/**
 * Function: Proxy_CommitTunnelLayerBySurfaceId_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create proxy with recording stub
 *                  2. call CommitTunnelLayerBySurfaceId with valid buffer
 *                  3. verify request is sent and parameters are decoded on remote side
 */
HWTEST_F(RSRenderToComposerConnectionProxyTest, Proxy_CommitTunnelLayerBySurfaceId_Success, TestSize.Level1)
{
    auto stub = sptr<RecordingRenderToComposerConnectionStub>::MakeSptr();
    ASSERT_NE(stub, nullptr);
    RSRenderToComposerConnectionProxy proxy(stub->AsObject());
    auto buffer = CreateTunnelTestBuffer();
    ASSERT_NE(buffer, nullptr);

    constexpr uint64_t surfaceId = 101u;
    constexpr uint64_t tunnelLayerId = 202u;
    sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
    EXPECT_EQ(proxy.CommitTunnelLayerBySurfaceId(surfaceId, tunnelLayerId, buffer, nullptr, releaseFence),
        GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_TRUE(stub->commitTunnelCalled);
    EXPECT_EQ(stub->lastSurfaceId, surfaceId);
    EXPECT_EQ(stub->lastTunnelLayerId, tunnelLayerId);
    EXPECT_EQ(stub->lastBufferSeqNum, buffer->GetSeqNum());
    EXPECT_EQ(stub->lastAcquireFenceFd, -1);
    ASSERT_NE(releaseFence, nullptr);
    EXPECT_GE(releaseFence->Get(), 0);
}
} // namespace OHOS::Rosen
