/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ui/rs_canvas_node.h"

#include <algorithm>
#include <string>

#ifdef ROSEN_OHOS
#include "common/rs_obj_abs_geometry.h"
#endif
#include "command/rs_canvas_node_command.h"
#include "platform/common/rs_log.h"
#include "common/rs_obj_geometry.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_node_map.h"

namespace OHOS {
namespace Rosen {
RSCanvasNode::SharedPtr RSCanvasNode::Create(bool isRenderServiceNode)
{
    SharedPtr node(new RSCanvasNode(isRenderServiceNode));
    RSNodeMap::MutableInstance().RegisterNode(node);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return node;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeCreate>(node->GetId());
    transactionProxy->AddCommand(command, node->IsRenderServiceNode());
    if (node->NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSCanvasNodeCreate>(node->GetId());
        transactionProxy->AddCommand(extraCommand, !node->IsRenderServiceNode());
    }
    return node;
}

RSCanvasNode::RSCanvasNode(bool isRenderServiceNode) : RSNode(isRenderServiceNode) {}

RSCanvasNode::~RSCanvasNode() {}

SkCanvas* RSCanvasNode::BeginRecording(int width, int height)
{
#ifdef ROSEN_OHOS
    recordingCanvas_ = new RSRecordingCanvas(width, height);
#endif
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return recordingCanvas_;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeClearRecording>(GetId());
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSCanvasNodeClearRecording>(GetId());
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode());
    }
    return recordingCanvas_;
}

bool RSCanvasNode::IsRecording() const
{
    return recordingCanvas_ != nullptr;
}

void RSCanvasNode::FinishRecording()
{
#ifdef ROSEN_OHOS
    if (!IsRecording()) {
        ROSEN_LOGW("RSCanvasNode::FinishRecording, IsRecording = false");
        return;
    }
    auto recording = static_cast<RSRecordingCanvas*>(recordingCanvas_)->GetDrawCmdList();
    delete recordingCanvas_;
    recordingCanvas_ = nullptr;
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording,
            drawContentLast_ ? RSModifierType::FOREGROUND_STYLE : RSModifierType::CONTENT_STYLE);
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording,
            drawContentLast_ ? RSModifierType::FOREGROUND_STYLE : RSModifierType::CONTENT_STYLE);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode());
    }
#endif
}

void RSCanvasNode::DrawOnNode(RSModifierType type, DrawFunc func)
{
    RSRecordingCanvas recordingCanvas(GetPaintWidth(), GetPaintHeight());
    func(&recordingCanvas);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    auto recording = recordingCanvas.GetDrawCmdList();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording, type);
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    if (NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand =
            std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording, type);
        transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode());
    }
}

float RSCanvasNode::GetPaintWidth() const
{
    auto frame = GetStagingProperties().GetFrame();
    return frame.z_ <= 0.f ? GetStagingProperties().GetBounds().z_ : frame.z_;
}

float RSCanvasNode::GetPaintHeight() const
{
    auto frame = GetStagingProperties().GetFrame();
    return frame.w_ <= 0.f ? GetStagingProperties().GetBounds().w_ : frame.w_;
}
} // namespace Rosen
} // namespace OHOS
