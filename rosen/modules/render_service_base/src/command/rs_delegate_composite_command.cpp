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
#ifndef ROSEN_CROSS_PLATFORM
#include "command/rs_delegate_composite_command.h"
#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"

#include "transaction/rs_marshalling_helper.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_surface_render_node.h"
#include "buffer_extra_data.h"
#include "buffer_extra_data_impl.h"

namespace OHOS {
namespace Rosen {

TransactionBufferCommand::Register TransactionBufferCommand::instance_;
SurfaceTransactionCommand::Register SurfaceTransactionCommand::instance_;

static void RsDelegateCompositeCommandPrintLog(const std::string& log)
{
    ROSEN_LOGE("DelegateModeDebugTag:%{public}s:%{public}s", __func__, log.c_str());
    RS_OPTIONAL_TRACE_NAME_FMT("DelegateModeDebugTag:%s:%s", __func__, log.c_str());
}

void TransactionBufferCommand::Process(RSContext& context)
{
    RS_TRACE_NAME_FMT("TransactionBufferCommand::Process, cmdtype=%u", cmdType_);
    switch (cmdType_) {
        case CmdType::SET_BUFFER:
            ProcessCmdTypeSetBuffer(context);
            break;
        case CmdType::SET_RECT:
            ProcessCmdTypeSetRect(context);
            break;
        default:
            RsDelegateCompositeCommandPrintLog("TransactionBufferCommand Process fail: invalid cmdtype");
            break;
    }
}

RSCommand* TransactionBufferCommand::Unmarshalling(Parcel& parcel)
{
    MessageParcel* messageParcel = static_cast<MessageParcel*>(&parcel);
    if (!messageParcel) {
        return nullptr;
    }

    int32_t cmdType = -1;
    if (!messageParcel->ReadInt32(cmdType)) {
        RsDelegateCompositeCommandPrintLog("TransactionBufferCommand Unmarshalling fail: read cmdtype fail");
        return nullptr;
    }
    RSCommand* cmd = nullptr;
    switch (static_cast<CmdType>(cmdType)) {
        case CmdType::SET_BUFFER:
            cmd = TransactionBufferCommand::UnmarshallingCmdTypeSetBuffer(messageParcel);
            break;
        case CmdType::SET_RECT:
            cmd = TransactionBufferCommand::UnmarshallingCmdTypeSetRect(messageParcel);
            break;
        default:
            RsDelegateCompositeCommandPrintLog("TransactionBufferCommand Unmarshalling fail: invalid cmdtype");
            break;
    }
    return cmd;
}

bool TransactionBufferCommand::Marshalling(Parcel& parcel) const
{
    if (cmdType_ == CmdType::INVALID) {
        RsDelegateCompositeCommandPrintLog("TransactionBufferCommand Marshalling fail: invalid cmd type");
        return false;
    }

    if (!RSMarshallingHelper::Marshalling(parcel, GetType()) ||
        !RSMarshallingHelper::Marshalling(parcel, GetSubType())) {
        RsDelegateCompositeCommandPrintLog("TransactionBufferCommand Marshalling fail: RSMarshallingHelper fail");
        return false;
    }

    MessageParcel* messageParcel = static_cast<MessageParcel*>(&parcel);
    if (!messageParcel || !messageParcel->WriteInt32(static_cast<int32_t>(cmdType_))) {
        RsDelegateCompositeCommandPrintLog(
            "TransactionBufferCommand Marshalling fail: messageParcel is nullptr or write cmdtype fail");
        return false;
    }

    bool ret = false;
    switch (cmdType_) {
        case CmdType::SET_BUFFER:
            ret = MarshallingCmdTypeSetBuffer(messageParcel);
            break;
        case CmdType::SET_RECT:
            ret = MarshallingCmdTypeSetRect(messageParcel);
            break;
        default:
            RsDelegateCompositeCommandPrintLog("TransactionBufferCommand Marshalling fail: invalid cmdtype");
            break;
    }
    return ret;
}

bool TransactionBufferCommand::MarshallingCmdTypeSetBuffer(MessageParcel* messageParcel) const
{
    if (cmdType_ != CmdType::SET_BUFFER) {
        ROSEN_LOGE("DelegateModeDebugTag:MarshallingCmdTypeSetBuffer fail:cmdType=%{public}d is error", cmdType_);
        return false;
    }

    uint32_t size = configList_.size();
    RS_OPTIONAL_TRACE_NAME_FMT("MarshallingCmdTypeSetBuffer::Marshalling, size=%u", configList_.size());
    if (!messageParcel->WriteUint32(size)) {
        RsDelegateCompositeCommandPrintLog("MarshallingCmdTypeSetBuffer Marshalling fail, write configList size fail");
        return false;
    }
    for (const auto& config : configList_) {
        if (Transaction::WriteTransactionConfigToMessageParcel(*messageParcel, config) != GSERROR_OK) {
            RsDelegateCompositeCommandPrintLog("MarshallingCmdTypeSetBuffer fail: writeTransactionConfig");
            return false;
        }
    }
    RS_OPTIONAL_TRACE_NAME_FMT("MarshallingCmdTypeSetBuffer::Marshalling, nodeId=%llu", nodeId_);
    if (!messageParcel->WriteUint64(nodeId_)) {
        RsDelegateCompositeCommandPrintLog("MarshallingCmdTypeSetBuffer Marshalling fail: Write nodeId fail");
        return false;
    }
    return true;
}

void TransactionBufferCommand::ProcessCmdTypeSetBufferwithAttach(sptr<Transaction> transaction,
    sptr<IConsumerSurface> consumer, const RSTransactionConfig& config)
{
    auto buffer = transaction->GetBuffer();
    if (!buffer) {
        return;
    }
    auto producer = consumer->GetProducer();
    BufferRequestConfig bufferConfig = buffer->GetBufferRequestConfig();
    bufferConfig.usage = bufferConfig.usage | consumer->GetDefaultUsage();
    bufferConfig.usage &= ~BUFFER_USAGE_CPU_WRITE;
    buffer->SetBufferRequestConfig(bufferConfig);
    BufferFlushConfigWithDamages configDamages;
    configDamages.damages = transaction->GetDamages();
    configDamages.timestamp = transaction->GetUiTimestamp();
    configDamages.desiredPresentTimestamp = transaction->GetDesiredPresentTimestamp();
    if (!consumer->IsCached(buffer->GetSeqNum())) {
        GSError ret = producer->AttachBufferToQueue(buffer);
        if (ret == SURFACE_ERROR_BUFFER_QUEUE_FULL) {
            producer->CleanProducerBySeqNum(config.delList);
            ret = producer->AttachBufferToQueue(buffer);
        }
        RS_OPTIONAL_TRACE_NAME_FMT("AttachBufferToQueue=%d", ret);
    }
    sptr<BufferExtraData> bedata = new BufferExtraDataImpl();
    GSError ret = producer->FlushBuffer(buffer->GetSeqNum(), bedata, transaction->GetFence(), configDamages);
    if (ret != GSERROR_OK) {
        RS_TRACE_NAME_FMT("FlushBuffer=%u fail, buffer sequence=%u", ret, buffer->GetSeqNum());
        ROSEN_LOGE("DelegateModeDebugTag: FlushBuffer=%{public}u fail, buffer sequence=%{public}u",
            ret, buffer->GetSeqNum());
    }
}

void TransactionBufferCommand::ProcessCmdTypeSetBuffer(RSContext& context)
{
    if (cmdType_ != CmdType::SET_BUFFER) {
        return;
    }
    RS_TRACE_NAME_FMT("ProcessCmdTypeSetBuffer, configList size=%u", configList_.size());
    for (const auto& config : configList_) {
        auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
            context.GetNodeMap().GetRenderNode(config.nodeId));
        if (!node || !node->GetRSSurfaceHandler()) {
            continue;
        }
        sptr<IConsumerSurface> consumer = node->GetRSSurfaceHandler()->GetConsumer();
        if (!consumer || !consumer->GetProducer()) {
            continue;
        }
        auto producer = consumer->GetProducer();
        if (!config.transaction) {
            RS_TRACE_NAME_FMT("only has info=sequence=%u", config.transactionInfo.bufferInfo.sequence);
            BufferFlushConfigWithDamages configDamages;
            configDamages.damages = config.transactionInfo.damages;
            configDamages.timestamp = config.transactionInfo.uiTimestamp;
            configDamages.desiredPresentTimestamp = config.transactionInfo.desiredPresentTimestamp;
            sptr<BufferExtraData> bedata = new BufferExtraDataImpl();
            GSError ret = producer->FlushBuffer(
                config.transactionInfo.bufferInfo.sequence, bedata, config.transactionInfo.fence, configDamages);
            RS_TRACE_NAME_FMT("flushbuffer=%u", ret);
            if (ret != GSERROR_OK) {
                RS_TRACE_NAME_FMT("FlushBuffer=%u fail, buffer sequence=%u", ret,
                    config.transactionInfo.bufferInfo.sequence);
                ROSEN_LOGE("DelegateModeDebugTag: FlushBuffer=%{public}u fail, buffer sequence=%{public}u",
                    ret, config.transactionInfo.bufferInfo.sequence);
            }
            continue;
        }
        ProcessCmdTypeSetBufferwithAttach(config.transaction, consumer, config);
    }
}

RSCommand* TransactionBufferCommand::UnmarshallingCmdTypeSetBuffer(MessageParcel* messageParcel)
{
    uint32_t size = 0;
    if (!messageParcel->ReadUint32(size)) {
        RsDelegateCompositeCommandPrintLog("UnmarshallingCmdTypeSetBuffer fail: size is 0");
        return nullptr;
    }
    std::vector<RSTransactionConfig> configList;
    for (uint32_t i = 0; i < size; i++) {
        RSTransactionConfig config;
        if (Transaction::ReadTransactionConfigFromMessageParcel(*messageParcel, config) != GSERROR_OK) {
            RsDelegateCompositeCommandPrintLog("UnmarshallingCmdTypeSetBuffer fail: ReadTransactionConfig");
            return nullptr;
        }
        configList.push_back(config);
    }
    NodeId nodeId;
    if (!messageParcel->ReadUint64(nodeId)) {
        RsDelegateCompositeCommandPrintLog("UnmarshallingCmdTypeSetBuffer fail: nodeId");
        return nullptr;
    }
    auto command = new TransactionBufferCommand(configList, nodeId);
    return command;
}

TransactionBufferCommand::TransactionBufferCommand(std::vector<RSTransactionConfig>& configs, NodeId nodeId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("TransactionBufferCommand::TransactionBufferCommand in, size=%u", configs.size());
    configList_.clear();
    configList_.swap(configs);
    nodeId_ = nodeId;
    cmdType_ = CmdType::SET_BUFFER;
}

TransactionBufferCommand::TransactionBufferCommand() {}

TransactionBufferCommand::TransactionBufferCommand(RectF rect, bool isSrcRect, NodeId nodeId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("TransactionBufferCommand::TransactionBufferCommand in,"
        "rect=%s, isSrcRect=%d, nodeId=%llu", rect.ToString().c_str(), isSrcRect, nodeId);
    nodeId_ = nodeId;
    cmdType_ = CmdType::SET_RECT;
    isSrcRect_ = isSrcRect;
    rect_ = rect;
}

void TransactionBufferCommand::ProcessCmdTypeSetRect(RSContext& context)
{
    if (cmdType_ != CmdType::SET_RECT) {
        ROSEN_LOGE("DelegateModeDebugTag: ProcessCmdTypeSetRect fail:cmdType=%{public}d is error", cmdType_);
        return;
    }
    auto& nodeMap = context.GetNodeMap();
    auto& webNodeIds = context.GetWebNodeMap();
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
        nodeMap.GetRenderNode<RSRenderNode>(nodeId_))) {
        if (isSrcRect_) {
            node->SetDelegateSrcRect(rect_.GetTop(), rect_.GetLeft(), rect_.GetWidth(), rect_.GetHeight());
        } else {
            node->SetDelegateDstRect(rect_.GetTop(), rect_.GetLeft(), rect_.GetWidth(), rect_.GetHeight());
        }
        webNodeIds.emplace(nodeId_);
    } else {
        ROSEN_LOGE("DelegateModeDebugTag: ProcessCmdTypeSetRect fail: node(id:%{public}" PRIu64 ") not find", nodeId_);
    }
}

RSCommand* TransactionBufferCommand::UnmarshallingCmdTypeSetRect(MessageParcel* messageParcel)
{
    float top = 0;
    float left = 0;
    float width = 0;
    float height = 0;
    bool isSrcRect = false;
    NodeId nodeId = 0;

    bool ret =
        messageParcel->ReadFloat(top) &&
        messageParcel->ReadFloat(left) &&
        messageParcel->ReadFloat(width) &&
        messageParcel->ReadFloat(height) &&
        messageParcel->ReadBool(isSrcRect) &&
        messageParcel->ReadUint64(nodeId);
    if (!ret) {
        ROSEN_LOGE("DelegateModeDebugTag: UnmarshallingCmdTypeSetRect fail");
        return nullptr;
    }

    RectF rect(top, left, width, height);
    auto command = new TransactionBufferCommand(rect, isSrcRect, nodeId);
    return command;
}

bool TransactionBufferCommand::MarshallingCmdTypeSetRect(MessageParcel* messageParcel) const
{
    if (cmdType_ != CmdType::SET_RECT) {
        ROSEN_LOGE("DelegateModeDebugTag: MarshallingCmdTypeSetRect fail:cmdType=%{public}d is error", cmdType_);
        return false;
    }
    bool ret =
        messageParcel->WriteFloat(rect_.GetTop()) &&
        messageParcel->WriteFloat(rect_.GetLeft()) &&
        messageParcel->WriteFloat(rect_.GetWidth()) &&
        messageParcel->WriteFloat(rect_.GetHeight()) &&
        messageParcel->WriteBool(isSrcRect_) &&
        messageParcel->WriteUint64(nodeId_);
    if (!ret) {
        ROSEN_LOGE("DelegateModeDebugTag: MarshallingCmdTypeSetRect fail: "
            "rect=%{public}s, isSrcRect=%{public}d",
            rect_.ToString().c_str(), isSrcRect_);
        return false;
    }
    return true;
}

void SurfaceTransactionCommand::Process(RSContext& context)
{
    (void)context;
    RS_OPTIONAL_TRACE_NAME_FMT("SurfaceTransactionCommand::Process, srcId=%" PRIu64
        ", seqNum=%" PRIu64 ", pid=%d, tid=%d",
        commandSrcId_, commandSeqNum_, commandSendPid_, commandSendTid_);
    RsDelegateCompositeCallbackManager::GetInstance().AddSurfaceTransactionCmdInfo(
        commandSrcId_, commandSeqNum_, commandSendPid_, commandSendTid_);
}

RSCommand* SurfaceTransactionCommand::Unmarshalling(Parcel& parcel)
{
    uint64_t srcId = 0;
    uint64_t seqNum = 0;
    int32_t pid = 0;
    int32_t tid = 0;
    if (!parcel.ReadUint64(srcId) || !parcel.ReadUint64(seqNum) || !parcel.ReadInt32(pid) || !parcel.ReadInt32(tid)) {
        RsDelegateCompositeCommandPrintLog("SurfaceTransactionCommand::Unmarshalling fail");
        return nullptr;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("SurfaceTransactionCommand::Unmarshalling. srcId=%llu, seqNum=%llu, pid=%d, tid=%d",
        srcId, seqNum, pid, tid);
    return new SurfaceTransactionCommand(srcId, seqNum, pid, tid);
}

bool SurfaceTransactionCommand::Marshalling(Parcel& parcel) const
{
    RS_OPTIONAL_TRACE_NAME("SurfaceTransactionCommand::Marshalling begin");
    if (!RSMarshallingHelper::Marshalling(parcel, GetType()) ||
        !RSMarshallingHelper::Marshalling(parcel, GetSubType())) {
        RsDelegateCompositeCommandPrintLog("SurfaceTransactionCommand::Marshalling fail: RSMarshallingHelper fail");
        return false;
    }
    if (!parcel.WriteUint64(commandSrcId_) || !parcel.WriteUint64(commandSeqNum_) ||
        !parcel.WriteInt32(commandSendPid_) || !parcel.WriteInt32(commandSendTid_)) {
        RsDelegateCompositeCommandPrintLog("SurfaceTransactionCommand Marshalling fail: write command fail");
        return false;
    }
    return true;
}

SurfaceTransactionCommand::SurfaceTransactionCommand(uint64_t srcId, uint64_t seqNum, pid_t pid, pid_t tid)
{
    RS_OPTIONAL_TRACE_NAME_FMT("SurfaceTransactionCommand::SurfaceTransactionCommand: "
        "srcId=%llu, seqNum=%llu, pid=%d, tid=%d", srcId, seqNum, pid, tid);
    commandSrcId_ = srcId;
    commandSeqNum_ = seqNum;
    commandSendPid_ = pid;
    commandSendTid_ = tid;
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_CROSS_PLATFORM