/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ui/rs_node.h"

#include <algorithm>
#include <string>

#ifdef ROSEN_OHOS
#include "common/rs_obj_abs_geometry.h"
#endif
#include "command/rs_node_command.h"
#include "platform/common/rs_log.h"
#include "common/rs_obj_geometry.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_node_map.h"

namespace OHOS {
namespace Rosen {
RSNode::SharedPtr RSNode::Create()
{
    SharedPtr node(new RSNode());
    RSNodeMap::Instance().RegisterNode(node);

    std::unique_ptr<RSCommand> command = std::make_unique<RSNodeCreate>(node->GetId());
    RSTransactionProxy::GetInstance().AddCommand(command);
    ROSEN_LOGI("RSNode::Create %llu", node->GetId());
    return node;
}

RSNode::RSNode(NodeId id)
{
    SetId(id);
}

RSNode::RSNode() {}

RSNode::~RSNode() {}

SkCanvas* RSNode::BeginRecording(int width, int height)
{
#ifdef ROSEN_OHOS
    ROSEN_LOGI("RSNode::BeginRecording [%d * %d]", width, height);
    recordingCanvas_ = new RSRecordingCanvas(width, height);
#endif
    return recordingCanvas_;
}

bool RSNode::IsRecording() const
{
    return recordingCanvas_ != nullptr;
}

void RSNode::OnUpdateRecording()
{
#ifdef ROSEN_OHOS
    if (!IsRecording()) {
        return;
    }
    ROSEN_LOGI("RSNode::Sync: node %llu stops recording", GetId());
    auto recording = static_cast<RSRecordingCanvas*>(recordingCanvas_)->GetDrawCmdList();
    delete recordingCanvas_;
    recordingCanvas_ = nullptr;
    std::unique_ptr<RSCommand> command = std::make_unique<RSNodeUpdateRecording>(GetId(), recording, drawContentLast_);
    RSTransactionProxy::GetInstance().AddCommand(command);
#endif
}

void RSNode::SetPaintOrder(bool drawContentLast)
{
    drawContentLast_ = drawContentLast;
}

void RSNode::UpdateRecording()
{
    OnUpdateRecording();
    for (auto child : GetChildren()) {
        auto childPtr = std::static_pointer_cast<RSNode>(RSNodeMap::Instance().GetNode(child).lock());
        if (childPtr != nullptr) {
            childPtr->UpdateRecording();
        }
    }
}

} // namespace Rosen
} // namespace OHOS
