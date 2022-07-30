/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "modifier/rs_extended_modifier.h"

#include <memory>

#include "command/rs_node_command.h"
#include "modifier/rs_render_modifier.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_recording_canvas.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_node.h"

namespace OHOS {
namespace Rosen {
RSDrawingContext RSExtendedModifierHelper::CreateDrawingContext(NodeId nodeId)
{
    auto node = RSNodeMap::Instance().GetNode<RSCanvasNode>(nodeId);
    if (!node) {
        return { nullptr };
    }
    auto recordingCanvas = new RSRecordingCanvas(node->GetPaintWidth(), node->GetPaintHeight());
    return { recordingCanvas, node->GetPaintWidth(), node->GetPaintHeight() };
}

std::shared_ptr<RSRenderModifier> RSExtendedModifierHelper::CreateRenderModifier(
    RSDrawingContext& ctx, PropertyId id, RSModifierType type)
{
    auto renderProperty = std::make_shared<RSRenderProperty<DrawCmdListPtr>>(
        RSExtendedModifierHelper::FinishDrawing(ctx), id);
    auto renderModifier =  std::make_shared<RSDrawCmdListRenderModifier>(renderProperty);
    renderModifier->drawStyle_ = type;
    return renderModifier;
}

std::shared_ptr<DrawCmdList> RSExtendedModifierHelper::FinishDrawing(RSDrawingContext& ctx)
{
    auto recording = static_cast<RSRecordingCanvas*>(ctx.canvas)->GetDrawCmdList();
    delete ctx.canvas;
    ctx.canvas = nullptr;
    return recording;
}

template<typename T>
std::shared_ptr<RSRenderModifier> RSExtendedModifier<T>::CreateRenderModifier() const
{
    if (!this->property_->hasAddToNode_) {
        return nullptr;
    }
    RSDrawingContext ctx = RSExtendedModifierHelper::CreateDrawingContext(this->property_->nodeId_);
    Draw(ctx);
    return RSExtendedModifierHelper::CreateRenderModifier(ctx, this->property_->GetId(), GetModifierType());
}

template<typename T>
void RSExtendedModifier<T>::UpdateToRender()
{
    RSDrawingContext ctx = RSExtendedModifierHelper::CreateDrawingContext(this->property_->nodeId_);
    Draw(ctx);
    auto drawCmdList = RSExtendedModifierHelper::FinishDrawing(ctx);
    std::unique_ptr<RSCommand> command = std::make_unique<RSUpdatePropertyDrawCmdList>(
            this->property_->nodeId_, drawCmdList, this->property_->id_, false);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    auto node = RSNodeMap::Instance().GetNode<RSNode>(this->property_->nodeId_);
    if (transactionProxy && node) {
        transactionProxy->AddCommand(command, node->IsRenderServiceNode(), node->GetFollowType(), node->GetId());
        if (node->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSUpdatePropertyDrawCmdList>(
                this->property_->nodeId_, drawCmdList, this->property_->id_, false);
            transactionProxy->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());
        }
    }
}

template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<float>>;
template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<Color>>;
template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<Matrix3f>>;
template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<Quaternion>>;
template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<std::shared_ptr<RSFilter>>>;
template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<Vector2f>>;
template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<Vector4<Color>>>;
template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<Vector4f>>;
template class RS_EXPORT RSExtendedModifier<RSAnimatableProperty<std::shared_ptr<RSAnimatableBase>>>;
} // namespace Rosen
} // namespace OHOS
