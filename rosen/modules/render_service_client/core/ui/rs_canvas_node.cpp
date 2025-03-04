/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "common/rs_obj_abs_geometry.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_node_command.h"
#include "platform/common/rs_log.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_node_map.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_ui_context.h"

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []() -> int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif
namespace OHOS {
namespace Rosen {
RSCanvasNode::SharedPtr RSCanvasNode::Create(
    bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
{
    SharedPtr node(new RSCanvasNode(isRenderServiceNode, isTextureExportNode, rsUIContext));
    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeCreate>(node->GetId(), isTextureExportNode);
    node->AddCommand(command, node->IsRenderServiceNode());
    return node;
}

RSCanvasNode::RSCanvasNode(bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, isTextureExportNode, rsUIContext)
{
    tid_ = gettid();
}

RSCanvasNode::~RSCanvasNode()
{
    CheckThread();
}

void RSCanvasNode::SetHDRPresent(bool hdrPresent)
{
    CheckThread();
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeSetHDRPresent>(GetId(), hdrPresent);
    if (AddCommand(command, true)) {
        ROSEN_LOGD("RSCanvasNode::SetHDRPresent HDRClient set hdr true");
    }
}

ExtendRecordingCanvas* RSCanvasNode::BeginRecording(int width, int height)
{
    CheckThread();
    recordingCanvas_ = new ExtendRecordingCanvas(width, height);
    recordingCanvas_->SetIsCustomTextType(isCustomTextType_);
    recordingCanvas_->SetIsCustomTypeface(isCustomTypeface_);
    if (auto recording = recordingCanvas_->GetDrawCmdList()) {
        recording->SetIsNeedUnmarshalOnDestruct(!IsRenderServiceNode());
    }
    if (!recordingUpdated_) {
        return recordingCanvas_;
    }
    recordingUpdated_ = false;
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeClearRecording>(GetId());
    AddCommand(command, IsRenderServiceNode());
    return recordingCanvas_;
}

bool RSCanvasNode::IsRecording() const
{
    return recordingCanvas_ != nullptr;
}

void RSCanvasNode::RegisterNodeMap()
{
    auto rsContext = GetRSUIContext();
    if (rsContext == nullptr) {
        return;
    }
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterNode(shared_from_this());
}

void RSCanvasNode::CreateRenderNodeForTextureExportSwitch()
{
    CheckThread();

    if (IsRenderServiceNode()) {
        hasCreateRenderNodeInRS_ = true;
    } else {
        hasCreateRenderNodeInRT_ = true;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeCreate>(GetId(), isTextureExportNode_);
    AddCommand(command, IsRenderServiceNode());
}

void RSCanvasNode::FinishRecording()
{
    CheckThread();
    if (!IsRecording()) {
        return;
    }
    auto recording = recordingCanvas_->GetDrawCmdList();
    delete recordingCanvas_;
    recordingCanvas_ = nullptr;
    if (recording && recording->IsEmpty()) {
        return;
    }
    if (recording != nullptr && RSSystemProperties::GetDrawTextAsBitmap()) {
        // replace drawOpItem with cached one (generated by CPU)
        recording->GenerateCache();
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording,
        drawContentLast_ ? static_cast<uint16_t>(RSModifierType::FOREGROUND_STYLE)
                         : static_cast<uint16_t>(RSModifierType::CONTENT_STYLE));
    AddCommand(command, IsRenderServiceNode());
    recordingUpdated_ = true;
}

void RSCanvasNode::DrawOnNode(RSModifierType type, DrawFunc func)
{
    CheckThread();
    auto recordingCanvas = std::make_shared<ExtendRecordingCanvas>(GetPaintWidth(), GetPaintHeight());
    recordingCanvas->SetIsCustomTextType(isCustomTextType_);
    recordingCanvas->SetIsCustomTypeface(isCustomTypeface_);
    func(recordingCanvas);

    auto recording = recordingCanvas->GetDrawCmdList();
    if (recording) {
        recording->SetIsNeedUnmarshalOnDestruct(!IsRenderServiceNode());
    }
    if (recording && recording->IsEmpty()) {
        return;
    }
    RSNode::SetDrawNode();
    if (recording != nullptr && RSSystemProperties::GetDrawTextAsBitmap()) {
        // replace drawOpItem with cached one (generated by CPU)
        recording->GenerateCache();
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording, static_cast<uint16_t>(type));
    if (AddCommand(command, IsRenderServiceNode())) {
        recordingUpdated_ = true;
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

void RSCanvasNode::SetFreeze(bool isFreeze)
{
    if (!IsUniRenderEnabled()) {
        ROSEN_LOGE("SetFreeze is not supported in separate render");
        return;
    }
    CheckThread();
    RSNode::SetDrawNode();
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(GetId(), isFreeze);
    AddCommand(command, true);
}

void RSCanvasNode::OnBoundsSizeChanged() const
{
    auto bounds = GetStagingProperties().GetBounds();
    std::lock_guard<std::mutex> lock(mutex_);
    if (boundsChangedCallback_) {
        boundsChangedCallback_(bounds);
    }
}

void RSCanvasNode::SetBoundsChangedCallback(BoundsChangedCallback callback)
{
  std::lock_guard<std::mutex> lock(mutex_);
  boundsChangedCallback_ = callback;
}

void RSCanvasNode::CheckThread()
{
    if (tid_ != gettid()) {
        ROSEN_LOGE("RSCanvasNode::CheckThread Must be called on same thread");
    }
}
} // namespace Rosen
} // namespace OHOS
