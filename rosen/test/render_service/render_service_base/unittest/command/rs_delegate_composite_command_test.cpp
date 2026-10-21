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

#include <cstdint>
#include <gtest/gtest.h>

#include "command/rs_command_factory.h"
#include "command/rs_delegate_composite_command.h"
#include "common/rs_common_def.h"
#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"
#include "iconsumer_surface.h"
#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"
#include "sync_fence.h"
#include "transaction/rs_marshalling_helper.h"
#include "transaction/transaction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
#ifndef ROSEN_CROSS_PLATFORM
namespace {
constexpr NodeId SET_BUFFER_NODE_ID = 601;
constexpr NodeId SET_RECT_NODE_ID = 602;
constexpr uint32_t INVALID_BUFFER_SEQ = 4242;
constexpr float SRC_RECT_TOP = 11.0f;
constexpr float SRC_RECT_LEFT = 12.0f;
constexpr float SRC_RECT_WIDTH = 13.0f;
constexpr float SRC_RECT_HEIGHT = 14.0f;
constexpr float DST_RECT_TOP = 21.0f;
constexpr float DST_RECT_LEFT = 22.0f;
constexpr float DST_RECT_WIDTH = 23.0f;
constexpr float DST_RECT_HEIGHT = 24.0f;
} // namespace

class MockSurfaceNodeBufferReleaseCallback : public IRemoteStub<RSISurfaceNodeBufferReleaseCallback> {
public:
    MockSurfaceNodeBufferReleaseCallback() = default;
    ~MockSurfaceNodeBufferReleaseCallback() noexcept override = default;
    GSError OnBufferComplete(std::queue<OnCompletedRet>& queue) override
    {
        callCount++;
        while (!queue.empty()) {
            received.push_back(queue.front());
            queue.pop();
        }
        return GSERROR_OK;
    }
    uint32_t callCount = 0;
    std::vector<OnCompletedRet> received;
};

// Consumer surface whose producer is null, used to hit the "!consumer->GetProducer()" branch.
class NullProducerConsumerSurface : public IConsumerSurface {
public:
    NullProducerConsumerSurface() = default;
    ~NullProducerConsumerSurface() override = default;

    bool IsConsumer() const override
    {
        return true;
    }

    sptr<IBufferProducer> GetProducer() const override
    {
        return nullptr;
    }

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer) override
    {
        (void)buffer;
        return GSERROR_NOT_SUPPORT;
    }

    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer) override
    {
        (void)buffer;
        return GSERROR_NOT_SUPPORT;
    }

    uint32_t GetQueueSize() override
    {
        return 0;
    }

    GSError SetQueueSize(uint32_t queueSize) override
    {
        (void)queueSize;
        return GSERROR_NOT_SUPPORT;
    }

    int32_t GetDefaultWidth() override
    {
        return 0;
    }

    int32_t GetDefaultHeight() override
    {
        return 0;
    }

    GSError SetDefaultUsage(uint64_t usage) override
    {
        (void)usage;
        return GSERROR_NOT_SUPPORT;
    }

    uint64_t GetDefaultUsage() override
    {
        return 0;
    }

    GSError SetUserData(const std::string& key, const std::string& val) override
    {
        (void)key;
        (void)val;
        return GSERROR_NOT_SUPPORT;
    }

    std::string GetUserData(const std::string& key) override
    {
        (void)key;
        return "";
    }

    const std::string& GetName() override
    {
        return name_;
    }

    uint64_t GetUniqueId() const override
    {
        return uniqueId_;
    }

    GSError SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData>& metaData) override
    {
        (void)sequence;
        (void)metaData;
        return GSERROR_NOT_SUPPORT;
    }

    GSError SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key,
        const std::vector<uint8_t>& metaData) override
    {
        (void)sequence;
        (void)key;
        (void)metaData;
        return GSERROR_NOT_SUPPORT;
    }

    GSError SetTunnelHandle(const GraphicExtDataHandle* handle) override
    {
        (void)handle;
        return GSERROR_NOT_SUPPORT;
    }

    void Dump(std::string& result) const override
    {
        (void)result;
    }

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer, int32_t timeOut) override
    {
        (void)buffer;
        (void)timeOut;
        return GSERROR_NOT_SUPPORT;
    }

    GSError RegisterSurfaceDelegator(sptr<IRemoteObject> client) override
    {
        (void)client;
        return GSERROR_NOT_SUPPORT;
    }

    GSError AttachBufferToQueue(sptr<SurfaceBuffer> buffer) override
    {
        (void)buffer;
        return GSERROR_NOT_SUPPORT;
    }

    GSError DetachBufferFromQueue(sptr<SurfaceBuffer> buffer, bool isReserveSlot = false) override
    {
        (void)buffer;
        (void)isReserveSlot;
        return GSERROR_NOT_SUPPORT;
    }

    GSError SetScalingMode(ScalingMode scalingMode) override
    {
        (void)scalingMode;
        return GSERROR_NOT_SUPPORT;
    }

    GSError SetTunnelLayerInfo(const TunnelLayerInfo& info) override
    {
        (void)info;
        return GSERROR_NOT_SUPPORT;
    }

private:
    std::string name_ = "null_producer_consumer_surface";
    uint64_t uniqueId_ = 0;
};

class RSDelegateCompositeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSContext> CreateContextWithSurfaceNode(NodeId nodeId, bool enableDelegateMode);
    static RSTransactionConfig MakeInfoOnlyConfig(NodeId nodeId, uint32_t sequence);
    static void DrainManagerState(RSContext& context);

    static inline pid_t nodePid_ = 60100;
    static inline sptr<MockSurfaceNodeBufferReleaseCallback> listener_ = nullptr;
};

void RSDelegateCompositeCommandTest::SetUpTestCase() {}

void RSDelegateCompositeCommandTest::TearDownTestCase()
{
    listener_ = nullptr;
}

void RSDelegateCompositeCommandTest::SetUp()
{
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    listener_ = new MockSurfaceNodeBufferReleaseCallback();
    manager.RegisterSurfaceNodeBufferReleaseListener(nodePid_, listener_);
}

void RSDelegateCompositeCommandTest::TearDown()
{
    auto context = std::make_shared<RSContext>();
    DrainManagerState(*context);
    RsDelegateCompositeCallbackManager::GetInstance().UnRegisterSurfaceNodeBufferReleaseListener(nodePid_);
    listener_->callCount = 0;
    listener_->received.clear();
}

std::shared_ptr<RSContext> RSDelegateCompositeCommandTest::CreateContextWithSurfaceNode(
    NodeId nodeId, bool enableDelegateMode)
{
    auto context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId, context);
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    if (surfaceHandler != nullptr) {
        surfaceHandler->SetConsumer(IConsumerSurface::Create("DelegateCmdTestSurface"));
    }
    if (enableDelegateMode) {
        node->SetDelegateMode(true);
    }
    context->GetMutableNodeMap().RegisterRenderNode(node);
    return context;
}

RSTransactionConfig RSDelegateCompositeCommandTest::MakeInfoOnlyConfig(NodeId nodeId, uint32_t sequence)
{
    RSTransactionConfig config;
    config.nodeId = nodeId;
    config.transaction = nullptr;
    config.transactionInfo.bufferInfo.sequence = sequence;
    config.transactionInfo.fence = nullptr;
    config.delList = {};
    return config;
}

void RSDelegateCompositeCommandTest::DrainManagerState(RSContext& context)
{
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    while (manager.ProcessDelegateCompositeCommand(context)) {}
    manager.NotifyCurrentSurfaceNodeBufferReleaseCallback();
}

/**
 * @tc.name: TransactionBufferCommandSubType001
 * @tc.desc: Verify SET_BUFFER constructor maps to TRANSACTION_BUFFER subtype and keeps node id.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandSubType001, TestSize.Level1)
{
    std::vector<RSTransactionConfig> configs = { MakeInfoOnlyConfig(SET_BUFFER_NODE_ID, INVALID_BUFFER_SEQ) };
    TransactionBufferCommand cmd(configs, SET_BUFFER_NODE_ID);
    EXPECT_EQ(cmd.GetType(), RSCommandType::DELEGATE_COMPOSITE);
    EXPECT_EQ(cmd.GetSubType(), RSDelegateCompositeCommandType::TRANSACTION_BUFFER);
    EXPECT_EQ(cmd.GetNodeId(), SET_BUFFER_NODE_ID);
}

/**
 * @tc.name: TransactionBufferCommandSubType002
 * @tc.desc: Verify SET_RECT constructor maps to SRC_RECT/DST_RECT subtype by isSrcRect flag.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandSubType002, TestSize.Level1)
{
    RectF srcRect(SRC_RECT_TOP, SRC_RECT_LEFT, SRC_RECT_WIDTH, SRC_RECT_HEIGHT);
    TransactionBufferCommand srcCmd(srcRect, true, SET_RECT_NODE_ID);
    EXPECT_EQ(srcCmd.GetType(), RSCommandType::DELEGATE_COMPOSITE);
    EXPECT_EQ(srcCmd.GetSubType(), RSDelegateCompositeCommandType::TRANSACTION_BUFFER_SRC_RECT);
    EXPECT_EQ(srcCmd.GetNodeId(), SET_RECT_NODE_ID);

    RectF dstRect(DST_RECT_TOP, DST_RECT_LEFT, DST_RECT_WIDTH, DST_RECT_HEIGHT);
    TransactionBufferCommand dstCmd(dstRect, false, SET_RECT_NODE_ID);
    EXPECT_EQ(dstCmd.GetSubType(), RSDelegateCompositeCommandType::TRANSACTION_BUFFER_DST_RECT);
}

/**
 * @tc.name: TransactionBufferCommandFactoryRegister001
 * @tc.desc: Verify all three new unmarshalling entries are registered in RSCommandFactory.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandFactoryRegister001, TestSize.Level1)
{
    auto& factory = RSCommandFactory::Instance();
    EXPECT_NE(factory.GetUnmarshallingFunc(RSCommandType::DELEGATE_COMPOSITE,
        RSDelegateCompositeCommandType::TRANSACTION_BUFFER), nullptr);
    EXPECT_NE(factory.GetUnmarshallingFunc(RSCommandType::DELEGATE_COMPOSITE,
        RSDelegateCompositeCommandType::TRANSACTION_BUFFER_SRC_RECT), nullptr);
    EXPECT_NE(factory.GetUnmarshallingFunc(RSCommandType::DELEGATE_COMPOSITE,
        RSDelegateCompositeCommandType::TRANSACTION_BUFFER_DST_RECT), nullptr);
    EXPECT_NE(factory.GetUnmarshallingFunc(RSCommandType::DELEGATE_COMPOSITE,
        RSDelegateCompositeCommandType::SURFACE_TRANSACTION_CMD), nullptr);
    // unregistered subtype must return nullptr
    EXPECT_EQ(factory.GetUnmarshallingFunc(RSCommandType::DELEGATE_COMPOSITE, 4), nullptr);
}

/**
 * @tc.name: TransactionBufferCommandMarshalling001
 * @tc.desc: Verify SET_BUFFER marshalling writes DELEGATE_COMPOSITE type and TRANSACTION_BUFFER
 *           subtype header; info-only config (null transaction) fails the payload write.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandMarshalling001, TestSize.Level1)
{
    std::vector<RSTransactionConfig> configs = { MakeInfoOnlyConfig(SET_BUFFER_NODE_ID, INVALID_BUFFER_SEQ) };
    TransactionBufferCommand cmd(configs, SET_BUFFER_NODE_ID);

    MessageParcel parcel;
    // null transaction fails at WriteTransactionConfigToMessageParcel
    EXPECT_FALSE(cmd.Marshalling(parcel));
    // type and subtype header is written before the payload failure
    uint16_t type = 0;
    uint16_t subtype = 0;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, type, subtype));
    EXPECT_EQ(type, RSCommandType::DELEGATE_COMPOSITE);
    EXPECT_EQ(subtype, RSDelegateCompositeCommandType::TRANSACTION_BUFFER);
}

/**
 * @tc.name: TransactionBufferCommandMarshalling002
 * @tc.desc: Verify SET_RECT src/dst commands round trip keeps their new subtypes.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandMarshalling002, TestSize.Level1)
{
    RectF srcRect(SRC_RECT_TOP, SRC_RECT_LEFT, SRC_RECT_WIDTH, SRC_RECT_HEIGHT);
    TransactionBufferCommand srcCmd(srcRect, true, SET_RECT_NODE_ID);
    MessageParcel srcParcel;
    ASSERT_TRUE(srcCmd.Marshalling(srcParcel));
    uint16_t type = 0;
    uint16_t subtype = 0;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(srcParcel, type, subtype));
    EXPECT_EQ(subtype, RSDelegateCompositeCommandType::TRANSACTION_BUFFER_SRC_RECT);
    auto* srcRaw = TransactionBufferCommand::Unmarshalling(srcParcel);
    ASSERT_NE(srcRaw, nullptr);
    std::unique_ptr<TransactionBufferCommand> srcRoundTrip(static_cast<TransactionBufferCommand*>(srcRaw));
    EXPECT_EQ(srcRoundTrip->GetSubType(), RSDelegateCompositeCommandType::TRANSACTION_BUFFER_SRC_RECT);

    RectF dstRect(DST_RECT_TOP, DST_RECT_LEFT, DST_RECT_WIDTH, DST_RECT_HEIGHT);
    TransactionBufferCommand dstCmd(dstRect, false, SET_RECT_NODE_ID);
    MessageParcel dstParcel;
    ASSERT_TRUE(dstCmd.Marshalling(dstParcel));
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(dstParcel, type, subtype));
    EXPECT_EQ(subtype, RSDelegateCompositeCommandType::TRANSACTION_BUFFER_DST_RECT);
    auto* dstRaw = TransactionBufferCommand::Unmarshalling(dstParcel);
    ASSERT_NE(dstRaw, nullptr);
    std::unique_ptr<TransactionBufferCommand> dstRoundTrip(static_cast<TransactionBufferCommand*>(dstRaw));
    EXPECT_EQ(dstRoundTrip->GetSubType(), RSDelegateCompositeCommandType::TRANSACTION_BUFFER_DST_RECT);
    EXPECT_EQ(dstRoundTrip->GetNodeId(), SET_RECT_NODE_ID);
}

/**
 * @tc.name: TransactionBufferCommandUnmarshalling001
 * @tc.desc: Verify unmarshalling fails on empty parcel and on invalid cmdType.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandUnmarshalling001, TestSize.Level1)
{
    // branch: ReadInt32(cmdType) fails on empty parcel
    MessageParcel emptyParcel;
    EXPECT_EQ(TransactionBufferCommand::Unmarshalling(emptyParcel), nullptr);

    // branch: invalid cmdType hits the default case and returns nullptr
    MessageParcel invalidParcel;
    ASSERT_TRUE(invalidParcel.WriteInt32(999));
    EXPECT_EQ(TransactionBufferCommand::Unmarshalling(invalidParcel), nullptr);
}

/**
 * @tc.name: TransactionBufferCommandUnmarshalling002
 * @tc.desc: Verify SET_BUFFER unmarshalling rejects truncated, oversized and missing-field parcels.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandUnmarshalling002, TestSize.Level1)
{
    // branch: ReadUint32(size) fails when payload is missing
    MessageParcel noSizeParcel;
    ASSERT_TRUE(noSizeParcel.WriteInt32(0)); // CmdType::SET_BUFFER
    EXPECT_EQ(TransactionBufferCommand::Unmarshalling(noSizeParcel), nullptr);

    // branch: size exceeds SURFACE_PARCEL_SIZE_LIMIT
    MessageParcel oversizeParcel;
    ASSERT_TRUE(oversizeParcel.WriteInt32(0)); // CmdType::SET_BUFFER
    ASSERT_TRUE(oversizeParcel.WriteUint32(UINT32_MAX));
    EXPECT_EQ(TransactionBufferCommand::Unmarshalling(oversizeParcel), nullptr);

    // branch: ReadTransactionConfigFromMessageParcel fails on truncated config payload
    MessageParcel truncatedParcel;
    ASSERT_TRUE(truncatedParcel.WriteInt32(0)); // CmdType::SET_BUFFER
    ASSERT_TRUE(truncatedParcel.WriteUint32(1)); // one config
    ASSERT_TRUE(truncatedParcel.WriteUint64(1)); // config.nodeId
    ASSERT_TRUE(truncatedParcel.WriteBool(false)); // config.hasBuffer
    // transaction info payload truncated: ReadBufferInfoFromMessageParcel fails
    EXPECT_EQ(TransactionBufferCommand::Unmarshalling(truncatedParcel), nullptr);

    // branch: ReadUint64(nodeId) fails when config list is empty and nodeId is missing
    MessageParcel noNodeIdParcel;
    ASSERT_TRUE(noNodeIdParcel.WriteInt32(0)); // CmdType::SET_BUFFER
    ASSERT_TRUE(noNodeIdParcel.WriteUint32(0)); // empty config list
    EXPECT_EQ(TransactionBufferCommand::Unmarshalling(noNodeIdParcel), nullptr);
}

/**
 * @tc.name: TransactionBufferCommandUnmarshalling003
 * @tc.desc: Verify SET_RECT unmarshalling rejects truncated field chains.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandUnmarshalling003, TestSize.Level1)
{
    // branch: rect field chain fails when nothing follows cmdType
    MessageParcel emptyRectParcel;
    ASSERT_TRUE(emptyRectParcel.WriteInt32(1)); // CmdType::SET_RECT
    EXPECT_EQ(TransactionBufferCommand::Unmarshalling(emptyRectParcel), nullptr);

    // branch: rect field chain fails when nodeId is missing
    MessageParcel noNodeIdParcel;
    ASSERT_TRUE(noNodeIdParcel.WriteInt32(1)); // CmdType::SET_RECT
    ASSERT_TRUE(noNodeIdParcel.WriteFloat(SRC_RECT_TOP));
    ASSERT_TRUE(noNodeIdParcel.WriteFloat(SRC_RECT_LEFT));
    ASSERT_TRUE(noNodeIdParcel.WriteFloat(SRC_RECT_WIDTH));
    ASSERT_TRUE(noNodeIdParcel.WriteFloat(SRC_RECT_HEIGHT));
    ASSERT_TRUE(noNodeIdParcel.WriteBool(true));
    EXPECT_EQ(TransactionBufferCommand::Unmarshalling(noNodeIdParcel), nullptr);
}

/**
 * @tc.name: TransactionBufferCommandProcess001
 * @tc.desc: Verify default constructed command hits the invalid cmdType branch without crash.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcess001, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    TransactionBufferCommand cmd;
    cmd.Process(*context);
}

/**
 * @tc.name: TransactionBufferCommandProcess002
 * @tc.desc: Verify SET_RECT command on a missing node only prints error and does not crash.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcess002, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    RectF rect(SRC_RECT_TOP, SRC_RECT_LEFT, SRC_RECT_WIDTH, SRC_RECT_HEIGHT);
    TransactionBufferCommand cmd(rect, true, SET_RECT_NODE_ID);
    cmd.Process(*context);
}

/**
 * @tc.name: TransactionBufferCommandProcess003
 * @tc.desc: Verify SET_RECT command applies src/dst rect on registered delegate node.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcess003, TestSize.Level1)
{
    auto context = CreateContextWithSurfaceNode(SET_RECT_NODE_ID, true);
    auto node = context->GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(SET_RECT_NODE_ID);
    ASSERT_NE(node, nullptr);
    node->SetDelegateSrcRect(RectF{0, 0, 0, 0});
    node->SetDelegateDstRect(RectF{0, 0, 0, 0});

    RectF srcRect(SRC_RECT_TOP, SRC_RECT_LEFT, SRC_RECT_WIDTH, SRC_RECT_HEIGHT);
    TransactionBufferCommand srcCmd(srcRect, true, SET_RECT_NODE_ID);
    srcCmd.Process(*context);
    // ProcessCmdTypeSetRect passes (GetTop, GetLeft, GetWidth, GetHeight) to the node
    EXPECT_EQ(node->GetDelegateSrcRect(),
        RectF(srcRect.GetTop(), srcRect.GetLeft(), srcRect.GetWidth(), srcRect.GetHeight()));

    RectF dstRect(DST_RECT_TOP, DST_RECT_LEFT, DST_RECT_WIDTH, DST_RECT_HEIGHT);
    TransactionBufferCommand dstCmd(dstRect, false, SET_RECT_NODE_ID);
    dstCmd.Process(*context);
    EXPECT_EQ(node->GetDelegateDstRect(),
        RectF(dstRect.GetTop(), dstRect.GetLeft(), dstRect.GetWidth(), dstRect.GetHeight()));
    context->GetMutableNodeMap().UnregisterRenderNode(SET_RECT_NODE_ID);
}

/**
 * @tc.name: TransactionBufferCommandProcessSetBuffer001
 * @tc.desc: Verify info-only flush failure in delegate mode adds release info with invalid fence
 *           and notifies the buffer release listener.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcessSetBuffer001, TestSize.Level1)
{
    NodeId nodeId = (static_cast<NodeId>(nodePid_) << 32) | SET_BUFFER_NODE_ID;
    auto context = CreateContextWithSurfaceNode(nodeId, true);
    std::vector<RSTransactionConfig> configs = { MakeInfoOnlyConfig(nodeId, INVALID_BUFFER_SEQ) };
    TransactionBufferCommand cmd(configs, nodeId);

    cmd.Process(*context);
    EXPECT_EQ(listener_->callCount, 1);
    ASSERT_EQ(listener_->received.size(), 1);
    EXPECT_EQ(listener_->received.front().nodeId, nodeId);
    EXPECT_EQ(listener_->received.front().bufferSeqNum, INVALID_BUFFER_SEQ);
    EXPECT_NE(listener_->received.front().releaseFence, nullptr);
    context->GetMutableNodeMap().UnregisterRenderNode(nodeId);
}

/**
 * @tc.name: TransactionBufferCommandProcessSetBuffer002
 * @tc.desc: Verify info-only flush failure without delegate mode does not add release info.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcessSetBuffer002, TestSize.Level1)
{
    NodeId nodeId = (static_cast<NodeId>(nodePid_) << 32) | 603;
    auto context = CreateContextWithSurfaceNode(nodeId, false);
    std::vector<RSTransactionConfig> configs = { MakeInfoOnlyConfig(nodeId, INVALID_BUFFER_SEQ) };
    TransactionBufferCommand cmd(configs, nodeId);

    cmd.Process(*context);
    EXPECT_EQ(listener_->callCount, 0);
    context->GetMutableNodeMap().UnregisterRenderNode(nodeId);
}

/**
 * @tc.name: TransactionBufferCommandProcessSetBuffer003
 * @tc.desc: Verify missing node config is skipped without crash or callback.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcessSetBuffer003, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    std::vector<RSTransactionConfig> configs = { MakeInfoOnlyConfig(SET_BUFFER_NODE_ID, INVALID_BUFFER_SEQ) };
    TransactionBufferCommand cmd(configs, SET_BUFFER_NODE_ID);
    cmd.Process(*context);
    EXPECT_EQ(listener_->callCount, 0);
}

/**
 * @tc.name: TransactionBufferCommandProcessSetBuffer004
 * @tc.desc: Verify node without consumer surface and consumer without producer are both skipped.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcessSetBuffer004, TestSize.Level1)
{
    // branch: node exists but has no consumer
    auto context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(SET_BUFFER_NODE_ID, context);
    node->SetDelegateMode(true);
    context->GetMutableNodeMap().RegisterRenderNode(node);
    std::vector<RSTransactionConfig> configs = { MakeInfoOnlyConfig(SET_BUFFER_NODE_ID, INVALID_BUFFER_SEQ) };
    TransactionBufferCommand cmd(configs, SET_BUFFER_NODE_ID);
    cmd.Process(*context);
    EXPECT_EQ(listener_->callCount, 0);
    context->GetMutableNodeMap().UnregisterRenderNode(SET_BUFFER_NODE_ID);

    // branch: consumer exists but producer is null
    NodeId nullProducerNodeId = (static_cast<NodeId>(nodePid_) << 32) | 604;
    auto context2 = std::make_shared<RSContext>();
    auto node2 = std::make_shared<RSSurfaceRenderNode>(nullProducerNodeId, context2);
    auto handler2 = node2->GetMutableRSSurfaceHandler();
    ASSERT_NE(handler2, nullptr);
    handler2->SetConsumer(new NullProducerConsumerSurface());
    node2->SetDelegateMode(true);
    context2->GetMutableNodeMap().RegisterRenderNode(node2);
    std::vector<RSTransactionConfig> configs2 = { MakeInfoOnlyConfig(nullProducerNodeId, INVALID_BUFFER_SEQ) };
    TransactionBufferCommand cmd2(configs2, nullProducerNodeId);
    cmd2.Process(*context2);
    EXPECT_EQ(listener_->callCount, 0);
    context2->GetMutableNodeMap().UnregisterRenderNode(nullProducerNodeId);
}

/**
 * @tc.name: TransactionBufferCommandProcessSetBuffer005
 * @tc.desc: Verify non-empty delList triggers CleanProducerBySeqNum before flush.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcessSetBuffer005, TestSize.Level1)
{
    NodeId nodeId = (static_cast<NodeId>(nodePid_) << 32) | 605;
    auto context = CreateContextWithSurfaceNode(nodeId, true);
    RSTransactionConfig config = MakeInfoOnlyConfig(nodeId, INVALID_BUFFER_SEQ);
    config.delList = { 1, 2, 3 };
    std::vector<RSTransactionConfig> configs = { config };
    TransactionBufferCommand cmd(configs, nodeId);

    cmd.Process(*context);
    // flush still fails on unknown sequence, delegate mode notifies release info
    EXPECT_EQ(listener_->callCount, 1);
    ASSERT_EQ(listener_->received.size(), 1);
    EXPECT_EQ(listener_->received.front().bufferSeqNum, INVALID_BUFFER_SEQ);
    context->GetMutableNodeMap().UnregisterRenderNode(nodeId);
}

/**
 * @tc.name: TransactionBufferCommandProcessSetBuffer006
 * @tc.desc: Verify attach path returns early when transaction carries no buffer.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcessSetBuffer006, TestSize.Level1)
{
    NodeId nodeId = (static_cast<NodeId>(nodePid_) << 32) | 606;
    auto context = CreateContextWithSurfaceNode(nodeId, true);
    RSTransactionConfig config;
    config.nodeId = nodeId;
    config.transaction = new Transaction(); // empty transaction without buffer
    config.delList = {};
    std::vector<RSTransactionConfig> configs = { config };
    TransactionBufferCommand cmd(configs, nodeId);

    cmd.Process(*context);
    EXPECT_EQ(listener_->callCount, 0);
    context->GetMutableNodeMap().UnregisterRenderNode(nodeId);
}

/**
 * @tc.name: TransactionBufferCommandProcessSetBuffer007
 * @tc.desc: Verify empty config list skips the loop but still triggers the notify callback path.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcessSetBuffer007, TestSize.Level1)
{
    NodeId nodeId = (static_cast<NodeId>(nodePid_) << 32) | 607;
    auto context = CreateContextWithSurfaceNode(nodeId, true);
    std::vector<RSTransactionConfig> emptyConfigs = {};
    TransactionBufferCommand cmd(emptyConfigs, nodeId);

    cmd.Process(*context);
    EXPECT_EQ(listener_->callCount, 0); // no release info, notify finds nothing
    context->GetMutableNodeMap().UnregisterRenderNode(nodeId);
}

/**
 * @tc.name: TransactionBufferCommandProcessSetBuffer008
 * @tc.desc: Verify info-only flush failure with a non-null fence passes the same fence
 *           through to the release callback instead of replacing it with invalid fence.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCommandTest, TransactionBufferCommandProcessSetBuffer008, TestSize.Level1)
{
    NodeId nodeId = (static_cast<NodeId>(nodePid_) << 32) | 608;
    auto context = CreateContextWithSurfaceNode(nodeId, true);
    RSTransactionConfig config = MakeInfoOnlyConfig(nodeId, INVALID_BUFFER_SEQ);
    sptr<SyncFence> fence = SyncFence::InvalidFence();
    config.transactionInfo.fence = fence; // non-null fence arm of the ternary
    std::vector<RSTransactionConfig> configs = { config };
    TransactionBufferCommand cmd(configs, nodeId);

    cmd.Process(*context);
    EXPECT_EQ(listener_->callCount, 1);
    ASSERT_EQ(listener_->received.size(), 1);
    EXPECT_EQ(listener_->received.front().releaseFence, fence); // same sptr passes through
    context->GetMutableNodeMap().UnregisterRenderNode(nodeId);
}
#endif // ROSEN_CROSS_PLATFORM
} // namespace Rosen
} // namespace OHOS
