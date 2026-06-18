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

#include "command/rs_delegate_composite_command.h"

#include "buffer_extra_data_impl.h"

#include "common/rs_optional_trace.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
TransactionBufferCommand::Registrar TransactionBufferCommand::instance_;

void TransactionBufferCommand::Process(RSContext& context)
{
    RS_OPTIONAL_TRACE_FMT("TransactionBufferCommand::Process, size=%u", configList_.size());
    for (auto& config : configList_) {
        auto consumer = GetConsumerSurface(context, config.nodeId);
        auto producer = consumer != nullptr ? consumer->GetProducer() : nullptr;
        if (!producer) {
            continue;
        }
        if (!config.transaction) {
            continue;
        }
        auto buffer = config.transaction->GetBuffer();
        if (!buffer) {
            continue;
        }
        BufferRequestConfig bufferConfig = buffer->GetBufferRequestConfig();
        bufferConfig.usage = bufferConfig.usage | consumer->GetDefaultUsage();
        bufferConfig.usage &= ~BUFFER_USAGE_CPU_WRITE;
        buffer->SetBufferRequestConfig(bufferConfig);
        BufferFlushConfigWithDamages configDamages;
        configDamages.damages = config.transaction->GetDamages();
        configDamages.timestamp = config.transaction->GetUiTimestamp();
        configDamages.desiredPresentTimestamp = config.transaction->GetDesiredPresentTimestamp();
        sptr<BufferExtraData> bedata = new BufferExtraDataImpl();
        (void)producer->FlushBuffer(buffer->GetSeqNum(), bedata, config.transaction->GetFence(), configDamages);
    }
    configList_.clear();
}

sptr<IConsumerSurface> TransactionBufferCommand::GetConsumerSurface(RSContext& context, NodeId nodeId)
{
    auto node = context.GetNodeMap().GetRenderNode(nodeId);
    if (node == nullptr) {
        return nullptr;
    }
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = nullptr;
    if (auto canvasDrawingNode = RSBaseRenderNode::ReinterpretCast<RSCanvasDrawingRenderNode>(node)) {
#ifdef RS_MODIFIERS_DRAW_ENABLE
        surfaceHandler = canvasDrawingNode->GetSurfaceHandler();
#endif
    } else if (auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node)) {
        surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    }
    if (surfaceHandler == nullptr) {
        return nullptr;
    }
    return surfaceHandler->GetConsumer();
}

RSCommand* TransactionBufferCommand::Unmarshalling(Parcel& parcel)
{
    RS_OPTIONAL_TRACE_NAME_FMT("TransactionBufferCommand::Unmarshalling start");
    MessageParcel* messageParcel = static_cast<MessageParcel*>(&parcel);
    auto command = new TransactionBufferCommand();
    NodeId nodeId = 0;
    if (!messageParcel->ReadUint64(nodeId)) {
        RS_LOGE("TransactionBufferCommand::Marshalling: read nodeId fail.");
        return command;
    }
    command->SetNodeId(nodeId);
    uint32_t configCount = 0;
    if (!messageParcel->ReadUint32(configCount)) {
        RS_LOGE("TransactionBufferCommand::Marshalling: read configCount fail.");
        return command;
    }
    if (configCount == 0) {
        return command;
    }
    std::vector<RSTransactionConfig> configList;
    for (uint32_t i = 0; i < configCount; i++) {
        RSTransactionConfig config;
        if (RSBufferTransaction::ReadTransactionConfigFromMessageParcel(*messageParcel, config) != GSERROR_OK) {
            RS_LOGE("TransactionBufferCommand::Marshalling: read config fail.");
            return command;
        }
        configList.push_back(config);
    }
    command->SetRSTransactionConfigs(configList);
    RS_OPTIONAL_TRACE_NAME_FMT(
        "TransactionBufferCommand::Unmarshalling end, nodeId=%" PRIu64 ", configCount=%u", nodeId, configList.size());
    return command;
}

bool TransactionBufferCommand::Marshalling(Parcel& parcel) const
{
    uint32_t configCount = configList_.size();
    RS_OPTIONAL_TRACE_NAME_FMT(
        "TransactionBufferCommand::Marshalling start, nodeId=%" PRIu64 ", configCount=%u", nodeId_, configCount);
    if (!RSMarshallingHelper::Marshalling(parcel, GetType())) {
        RS_LOGE("TransactionBufferCommand::Marshalling: write command type fail.");
        return false;
    }
    if (!RSMarshallingHelper::Marshalling(parcel, GetSubType())) {
        RS_LOGE("TransactionBufferCommand::Marshalling: write command subType fail.");
        return false;
    }
    MessageParcel* messageParcel = static_cast<MessageParcel*>(&parcel);
    if (!messageParcel->WriteUint64(nodeId_)) {
        RS_LOGE("TransactionBufferCommand::Marshalling: write nodeId fail.");
        return false;
    }
    if (!messageParcel->WriteUint32(configCount)) {
        RS_LOGE("TransactionBufferCommand::Marshalling: write configCount fail.");
        return false;
    }
    for (auto& config : configList_) {
        if (RSBufferTransaction::WriteTransactionConfigToMessageParcel(*messageParcel, config)) {
            RS_LOGE("TransactionBufferCommand::Marshalling: write config fail.");
            return false;
        }
    }
    RS_OPTIONAL_TRACE_NAME_FMT(
        "TransactionBufferCommand::Marshalling end, nodeId=%" PRIu64 ", configCount=%u", nodeId_, configCount);
    return true;
}

void TransactionBufferCommand::SetNodeId(NodeId nodeId)
{
    nodeId_ = nodeId;
}

void TransactionBufferCommand::SetRSTransactionConfigs(std::vector<RSTransactionConfig>& configs)
{
    configList_.clear();
    configList_.swap(configs);
}

TransactionBufferCommand::TransactionBufferCommand(NodeId nodeId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("TransactionBufferCommand::TransactionBufferCommand in, no configs");
    nodeId_ = nodeId;
}

TransactionBufferCommand::TransactionBufferCommand(std::vector<RSTransactionConfig>& configs, NodeId nodeId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("TransactionBufferCommand::TransactionBufferCommand in, size=%u", configs.size());
    SetRSTransactionConfigs(configs);
    nodeId_ = nodeId;
}

TransactionBufferCommand::TransactionBufferCommand()
{
    RS_OPTIONAL_TRACE_NAME_FMT("TransactionBufferCommand::TransactionBufferCommand in");
}
} // namespace Rosen
} // namespace OHOS