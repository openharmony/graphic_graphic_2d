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

#ifndef RS_TUNNEL_TEST_UTILS_H
#define RS_TUNNEL_TEST_UTILS_H

#include <memory>
#include <parameters.h>
#include <unordered_set>
#include <vector>

#include "common/rs_tunnel_layer_utils.h"
#include "consumer_surface.h"
#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "rs_composer_client.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_render_to_composer_connection_stub.h"
#include "rs_surface_layer.h"

namespace OHOS::Rosen::TunnelTest {
inline constexpr uint32_t TEST_BUFFER_SIZE = 0x100;
inline constexpr uint32_t TEST_STRIDE_ALIGNMENT = 0x8;
inline constexpr uint32_t TEST_TUNNEL_LAYER_PROPERTY = TUNNEL_PROP_BUFFER_ADDR;

inline std::vector<std::shared_ptr<RSLayer>>& GetRecordingComposerLayers()
{
    static std::vector<std::shared_ptr<RSLayer>> layers;
    return layers;
}

inline void ClearRecordingComposerLayers()
{
    GetRecordingComposerLayers().clear();
}

inline TunnelLayerInfo MakeTunnelLayerInfo(TunnelTypeMask tunnelType = TUNNEL_TYPE_STYLUS)
{
    TunnelLayerInfo info;
    info.tunnelTypeMask = tunnelType;
    return info;
}

inline bool SetTunnelInfoForConsumer(const sptr<IConsumerSurface>& consumer, TunnelLayerState& state,
    TunnelTypeMask tunnelType = TUNNEL_TYPE_STYLUS)
{
    if (consumer == nullptr || consumer->SetTunnelLayerInfo(MakeTunnelLayerInfo(tunnelType)) != GSERROR_OK) {
        return false;
    }
    return consumer->GetTunnelLayerInfo(state) == GSERROR_OK;
}

inline bool SetTunnelInfoForConsumer(const sptr<IConsumerSurface>& consumer,
    TunnelTypeMask tunnelType = TUNNEL_TYPE_STYLUS)
{
    TunnelLayerState state;
    return SetTunnelInfoForConsumer(consumer, state, tunnelType);
}

inline sptr<SurfaceBuffer> CreateTestSurfaceBuffer()
{
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    if (buffer == nullptr) {
        return nullptr;
    }
    BufferRequestConfig requestConfig = {
        .width = TEST_BUFFER_SIZE,
        .height = TEST_BUFFER_SIZE,
        .strideAlignment = TEST_STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    return buffer->Alloc(requestConfig) == GSERROR_OK ? buffer : nullptr;
}

inline RSSurfaceHandler::SurfaceBufferEntry CreateTestBufferEntry()
{
    RSSurfaceHandler::SurfaceBufferEntry entry;
    entry.buffer = CreateTestSurfaceBuffer();
    entry.acquireFence = SyncFence::InvalidFence();
    entry.timestamp = 0;
    entry.damageRect = { .x = 0, .y = 0, .w = TEST_BUFFER_SIZE, .h = TEST_BUFFER_SIZE };
    if (entry.buffer != nullptr) {
        entry.bufferOwnerCount_->bufferId_ = entry.buffer->GetBufferId();
    }
    entry.RegisterReleaseBufferListener([](uint64_t) {});
    return entry;
}

inline IConsumerSurface::AcquireBufferReturnValue CreateTestAcquireBufferReturnValue()
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    returnValue.buffer = CreateTestSurfaceBuffer();
    returnValue.fence = SyncFence::InvalidFence();
    returnValue.timestamp = 0;
    returnValue.damages = { { .x = 0, .y = 0, .w = TEST_BUFFER_SIZE, .h = TEST_BUFFER_SIZE } };
    return returnValue;
}

class ScopedNewTunnelSwitch {
public:
    explicit ScopedNewTunnelSwitch(bool enabled) : oldValue_(Rosen::IsNewTunnelEnabled())
    {
        system::SetParameter("rosen.debug.new_tunnel", enabled ? "true" : "false");
    }

    ~ScopedNewTunnelSwitch()
    {
        system::SetParameter("rosen.debug.new_tunnel", oldValue_ ? "true" : "false");
    }

private:
    bool oldValue_;
};

class RecordingRenderToComposerConnection final : public RSRenderToComposerConnectionStub {
public:
    bool CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData) override
    {
        (void)transactionData;
        return false;
    }

    void CleanLayerBufferBySurfaceId(uint64_t surfaceId) override
    {
        (void)surfaceId;
    }

    int32_t CommitTunnelLayerBySurfaceId(uint64_t surfaceId, uint64_t tunnelLayerId,
        const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, sptr<SyncFence>& releaseFence) override
    {
        (void)acquireFence;
        commitTunnelCalled = true;
        ++commitTunnelCallCount;
        lastSurfaceId = surfaceId;
        lastTunnelLayerId = tunnelLayerId;
        lastBufferId = buffer == nullptr ? 0 : buffer->GetBufferId();
        releaseFence = SyncFence::InvalidFence();
        return commitTunnelResult;
    }

    void ClearFrameBuffers() override {}
    void ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds) override
    {
        (void)bufferIds;
    }

    void SetScreenBacklight(uint32_t level) override
    {
        (void)level;
    }

    void SetComposerToRenderConnection(const sptr<IRSComposerToRenderConnection>& composerToRenderConn) override
    {
        (void)composerToRenderConn;
    }

    void PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer) override
    {
        (void)buffer;
    }

    bool commitTunnelCalled = false;
    uint32_t commitTunnelCallCount = 0;
    int32_t commitTunnelResult = GRAPHIC_DISPLAY_SUCCESS;
    uint64_t lastSurfaceId = 0;
    uint64_t lastTunnelLayerId = 0;
    uint64_t lastBufferId = 0;
};

inline std::shared_ptr<RSComposerClientManager> CreateRecordingComposerManager(
    NodeId nodeId, const sptr<RecordingRenderToComposerConnection>& connection)
{
    if (connection == nullptr) {
        return nullptr;
    }
    sptr<IRSRenderToComposerConnection> proxy = new RSRenderToComposerConnectionProxy(connection->AsObject());
    auto client = RSComposerClient::Create(proxy, nullptr);
    if (client == nullptr) {
        return nullptr;
    }
    auto context = client->GetComposerContext();
    if (context == nullptr) {
        return nullptr;
    }
    auto layer = RSSurfaceLayer::Create(nodeId, context);
    if (layer == nullptr) {
        return nullptr;
    }
    layer->SetNodeId(nodeId);
    GetRecordingComposerLayers().emplace_back(layer);
    auto manager = std::make_shared<RSComposerClientManager>();
    manager->AddComposerClient(0, client);
    return manager;
}

struct TunnelTestContext {
    std::shared_ptr<RSSurfaceRenderNode> node;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler;
    sptr<IConsumerSurface> consumer;
    sptr<Surface> producer;

    bool IsBaseReady() const
    {
        return node != nullptr && surfaceHandler != nullptr && consumer != nullptr;
    }

    bool IsProducerReady() const
    {
        return IsBaseReady() && producer != nullptr;
    }
};

inline TunnelTestContext CreateTunnelTestContext(bool withProducer)
{
    TunnelTestContext context;
    context.node = RSTestUtil::CreateSurfaceNode();
    RSTestUtil::UnregisterConsumerListener();
    if (context.node == nullptr) {
        return context;
    }
    context.surfaceHandler = context.node->GetMutableRSSurfaceHandler();
    if (context.surfaceHandler == nullptr) {
        return context;
    }
    context.consumer = context.surfaceHandler->GetConsumer();
    if (context.consumer == nullptr || !withProducer) {
        return context;
    }
    sptr<IBufferProducer> producerToken = context.consumer->GetProducer();
    context.producer = Surface::CreateSurfaceAsProducer(producerToken);
    return context;
}

inline bool SetRuntimePendingBufferForTest(const TunnelTestContext& context)
{
    if (!context.IsBaseReady()) {
        return false;
    }
    auto entry = CreateTestBufferEntry();
    if (entry.buffer == nullptr) {
        return false;
    }
    context.node->GetTunnelRuntimeState().SetPendingBuffer(entry);
    return true;
}

class ScopedRegisteredSurfaceNode {
public:
    explicit ScopedRegisteredSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode>& node) : node_(node)
    {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr || node_ == nullptr) {
            return;
        }
        registered_ = mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node_);
        node_->SetIsOnTheTree(true);
        RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    }

    ~ScopedRegisteredSurfaceNode()
    {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr || node_ == nullptr || !registered_) {
            return;
        }
        node_->SetIsOnTheTree(false);
        mainThread->context_->GetMutableNodeMap().UnregisterRenderNode(node_->GetId());
        RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    }

    bool IsRegistered() const
    {
        return registered_;
    }

private:
    std::shared_ptr<RSSurfaceRenderNode> node_;
    bool registered_ = false;
};

inline bool MoveRuntimePendingToNormalHold(const std::shared_ptr<RSSurfaceRenderNode>& node,
    const std::shared_ptr<RSSurfaceHandler>& surfaceHandler, const sptr<IConsumerSurface>& consumer)
{
    if (node == nullptr || surfaceHandler == nullptr || consumer == nullptr) {
        return false;
    }
    RSSurfaceHandler::SurfaceBufferEntry entry;
    auto& runtime = node->GetTunnelRuntimeState();
    if (!runtime.TakePendingBuffer(entry) || entry.buffer == nullptr) {
        return false;
    }
    surfaceHandler->SetHoldBuffer(std::make_shared<RSSurfaceHandler::SurfaceBufferEntry>(entry));
    consumer->SetBufferHold(true);
    surfaceHandler->SetAvailableBufferCount(1);
    return true;
}

inline bool MoveRuntimePendingToNormalHold(const TunnelTestContext& context)
{
    if (!context.IsBaseReady()) {
        return false;
    }
    return MoveRuntimePendingToNormalHold(context.node, context.surfaceHandler, context.consumer);
}
} // namespace OHOS::Rosen::TunnelTest

#endif // RS_TUNNEL_TEST_UTILS_H
