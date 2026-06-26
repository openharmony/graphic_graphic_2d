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

#include "command_modifier/rs_canvas_node_command_modifier.h"

#include "command/rs_canvas_node_command.h"
#include "pipeline/rs_simple_draw_cmd_list.h"
#include "ui/rs_canvas_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

namespace {
// Convert the stored cmd list into a Drawing::DrawCmdListPtr ready to be pushed to render.
// Returns nullptr if neither drawingCmdList nor simpleDrawCmdList is available.
Drawing::DrawCmdListPtr PrepareDrawCmdList(Drawing::DrawCmdListPtr& drawingCmdList,
    SimpleDrawCmdListPtr& simpleDrawCmdList)
{
    if (drawingCmdList) {
        simpleDrawCmdList = RSSimpleDrawCmdList::CreateFromDrawCmdList(drawingCmdList);
        auto result = drawingCmdList;
        drawingCmdList = nullptr;
        return result;
    }
    if (simpleDrawCmdList) {
        return simpleDrawCmdList->ConvertToDrawCmdList();
    }
    return nullptr;
}
} // namespace

void HdrPresentCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSCanvasNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeSetHDRPresent>(
        node->GetId(), param_.hdrPresent_);
    if (AddCommand(command, true)) {
        ROSEN_LOGD("RSCanvasNode::SetHDRPresent HDRClient set hdr true");
    }
}

void ColorGamutCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSCanvasNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeSetColorGamut>(
        node->GetId(), param_.colorGamut_);
    AddCommand(command, true);
}

void IsFreezeCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSCanvasNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(
        node->GetId(), param_.isFreeze_, param_.isMarkedByUI_);
    AddCommand(command, true);
}

void ClearRecordingCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSCanvasNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeClearRecording>(
        node->GetId());
    AddCommand(command, node->IsRenderServiceNode());
}

void FinishRecordCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSCanvasNode>(GetNode());
    if (!node) return;

    auto drawCmdList = PrepareDrawCmdList(param_.drawingCmdList_, param_.simpleDrawCmdList_);
    if (!drawCmdList) {
        ROSEN_LOGE("FinishRecordCmdModifier::UpdateToRender() simpleDrawCmdList_ is nullptr");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeUpdateRecording>(
        node->GetId(), drawCmdList, param_.modifierType_);
    AddCommand(command, node->IsRenderServiceNode());
}

void DrawOnNodeCmdModifier::UpdateToRender() // only go foreground call this func
{
    auto node = std::static_pointer_cast<RSCanvasNode>(GetNode());
    if (!node) return;

    auto drawCmdList = PrepareDrawCmdList(param_.drawingCmdList_, param_.simpleDrawCmdList_);
    if (!drawCmdList) {
        ROSEN_LOGE("DrawOnNodeCmdModifier::UpdateToRender() simpleDrawCmdList_ is nullptr");
        return;
    }

    std::unique_ptr<RSCommand> clearRecordingCommand = std::make_unique<RSCanvasNodeClearRecording>(
        node->GetId());
    AddCommand(clearRecordingCommand, node->IsRenderServiceNode());

    std::unique_ptr<RSCommand> updateRecordingCommand = std::make_unique<RSCanvasNodeUpdateRecording>(
        node->GetId(), drawCmdList, param_.modifierType_);
    AddCommand(updateRecordingCommand, node->IsRenderServiceNode());
}

RSCmdModifier::UpdateResult DrawOnNodeCmdModifier::UpdateToRenderWithResult()
{
    auto node = std::static_pointer_cast<RSCanvasNode>(GetNode());
    if (!node) return false;

    auto drawCmdList = PrepareDrawCmdList(param_.drawingCmdList_, param_.simpleDrawCmdList_);
    if (!drawCmdList) {
        ROSEN_LOGE("DrawOnNodeCmdModifier::UpdateToRenderWithResult() simpleDrawCmdList_ is nullptr");
        return false;
    }

    std::unique_ptr<RSCommand> clearRecordingCommand = std::make_unique<RSCanvasNodeClearRecording>(
        node->GetId());
    AddCommand(clearRecordingCommand, node->IsRenderServiceNode());

    std::unique_ptr<RSCommand> updateRecordingCommand = std::make_unique<RSCanvasNodeUpdateRecording>(
        node->GetId(), drawCmdList, param_.modifierType_);
    return AddCommand(updateRecordingCommand, node->IsRenderServiceNode());
}

void HdrPresentCmdModifier::DumpParam(std::string& out) const
{
    out += "{hdrPresent:" + std::string(param_.hdrPresent_ ? "true" : "false") + "}";
}

void ColorGamutCmdModifier::DumpParam(std::string& out) const
{
    out += "{colorGamut:" + std::to_string(param_.colorGamut_) + "}";
}

void IsFreezeCmdModifier::DumpParam(std::string& out) const
{
    out += "{isFreeze:" + std::string(param_.isFreeze_ ? "true" : "false") +
           ", isMarkedByUI:" + std::string(param_.isMarkedByUI_ ? "true" : "false") + "}";
}

void ClearRecordingCmdModifier::DumpParam(std::string& out) const
{
    out += "{width:" + std::to_string(param_.width_) +
           ", height:" + std::to_string(param_.height_) + "}";
}

void FinishRecordCmdModifier::DumpParam(std::string& out) const
{
    out += "{drawCmdList:";
    if (param_.simpleDrawCmdList_) {
        out += "[simple size:" + std::to_string(param_.simpleDrawCmdList_->GetSize()) + "]";
    } else if (param_.drawingCmdList_) {
        out += "[drawing]";
    } else {
        out += "null";
    }
    out += ", modifierType:" + std::to_string(param_.modifierType_) + "}";
}

void DrawOnNodeCmdModifier::DumpParam(std::string& out) const
{
    out += "{drawCmdList:";
    if (param_.simpleDrawCmdList_) {
        out += "[simple size:" + std::to_string(param_.simpleDrawCmdList_->GetSize()) + "]";
    } else if (param_.drawingCmdList_) {
        out += "[drawing]";
    } else {
        out += "null";
    }
    out += ", modifierType:" + std::to_string(param_.modifierType_) + "}";
}

} // namespace Rosen
} // namespace OHOS
