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

#include "command/rs_canvas_node_command.h"
#include "command/rs_node_command.h"
#include "command_modifier/rs_canvas_node_command_modifier.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_obj_geometry.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_ui_context.h"

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
    if (RSSystemProperties::GetRenderNodeLazyLoadEnabled()) {
        node->lazyLoad_ = true;
        node->nodeState_ = RSNodeState::LAZY_LOAD;
        node->AddLazyLoadCommand(std::move(command), node->IsRenderServiceNode());
        if (node->GetRSUIContext()) {
            node->AddLazyLoadCommand(
                std::make_unique<RSSetUIContextToken>(node->GetId(), node->GetRSUIContext()->GetToken()),
                node->IsRenderServiceNode(), node->GetFollowType(), node->GetId());
        }
    } else {
        node->AddCommand(command, node->IsRenderServiceNode());
        node->SetUIContextToken();
    }
    return node;
}

RSCanvasNode::RSCanvasNode(bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, isTextureExportNode, rsUIContext)
{}

RSCanvasNode::RSCanvasNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode,
    std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, id, isTextureExportNode, rsUIContext)
{
    shadowNodeFlag_ = true;
}

RSCanvasNode::~RSCanvasNode()
{}

void RSCanvasNode::CreateRenderNode()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeCreate>(GetId(), IsTextureExportNode());
    AddCommand(command, IsRenderServiceNode());
}

void RSCanvasNode::SetHDRPresent(bool hdrPresent)
{
    SetRSCmdProperty<HdrPresentCmdModifier>(HdrPresentCmdParam{
        hdrPresent
    });
}

void RSCanvasNode::SetColorGamut(uint32_t colorGamut)
{
    SetRSCmdProperty<ColorGamutCmdModifier>(ColorGamutCmdParam{
        colorGamut
    });
}

ExtendRecordingCanvas* RSCanvasNode::BeginRecording(int width, int height)
{
    if (recordingCanvas_) {
        delete recordingCanvas_;
        recordingCanvas_ = nullptr;
        RS_LOGE("RSCanvasNode::BeginRecording last beginRecording without finishRecording");
    }

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
    SetRSCmdProperty<ClearRecordingCmdModifier>(ClearRecordingCmdParam{
        width, height
    });
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

    auto modifierType =
        drawContentLast_ ? ModifierNG::RSModifierType::FOREGROUND_STYLE : ModifierNG::RSModifierType::CONTENT_STYLE;
    OnFinishRecording(recording, modifierType);
    recordingUpdated_ = true;
    cmdListImages_ = RSCmdListImageCollector::CollectCmdListImage(recording);
}

void RSCanvasNode::OnFinishRecording(Drawing::DrawCmdListPtr& drawCmdList, ModifierNG::RSModifierType modifierType)
{
    SetRSCmdProperty<FinishRecordCmdModifier>(FinishRecordCmdParam{
        drawCmdList, static_cast<uint16_t>(modifierType)
    });
}

void RSCanvasNode::DrawOnNode(ModifierNG::RSModifierType type, DrawFunc func)
{
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
    auto modifierType = static_cast<uint16_t>(type);
    auto result = SetRSCmdPropertyWithResult<DrawOnNodeCmdModifier>(DrawOnNodeCmdParam{
        recording, modifierType
    });
    if (std::holds_alternative<bool>(result) && std::get<bool>(result)) {
        recordingUpdated_ = true;
        cmdListImages_ = RSCmdListImageCollector::CollectCmdListImage(recording);
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

void RSCanvasNode::SetFreeze(bool isFreeze, bool isMarkedByUI)
{
    if (!IsUniRenderEnabled()) {
        ROSEN_LOGE("SetFreeze is not supported in separate render");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSCanvasNode::SetFreeze id:%llu", GetId());
    RSNode::SetDrawNode();
    SetRSCmdProperty<IsFreezeCmdModifier>(IsFreezeCmdParam{
        isFreeze, isMarkedByUI
    });
}

void RSCanvasNode::OnBoundsSizeChanged()
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

void RSCanvasNode::SetPixelmap(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (!pixelMap) {
        return;
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasNodeSetPixelmap>(GetId(), pixelMap);
    AddCommand(command, true);
}
} // namespace Rosen
} // namespace OHOS
