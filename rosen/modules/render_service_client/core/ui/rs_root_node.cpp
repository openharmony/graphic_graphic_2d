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

#include "ui/rs_root_node.h"

#include "command/rs_root_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSNode> RSRootNode::Create(bool isRenderServiceNode, bool isTextureExportNode)
{
    static std::once_flag flag;
    std::shared_ptr<RSRootNode> node(new RSRootNode(isRenderServiceNode, isTextureExportNode));
    RSNodeMap::MutableInstance().RegisterNode(node);

    std::call_once(flag, []() {
        std::function<bool (std::shared_ptr<Drawing::Typeface>)> registerTypefaceFunc =
            [] (std::shared_ptr<Drawing::Typeface> typeface) -> bool {
                static Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
                return interface.RegisterTypeface(typeface);
            };
        Drawing::Typeface::RegisterCallBackFunc(registerTypefaceFunc);

        std::function<bool (std::shared_ptr<Drawing::Typeface>)> unregisterTypefaceFunc =
            [] (std::shared_ptr<Drawing::Typeface> typeface) -> bool {
                static Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
                return interface.UnRegisterTypeface(typeface);
            };
        Drawing::Typeface::UnRegisterCallBackFunc(unregisterTypefaceFunc);
    });

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return node;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeCreate>(node->GetId(), isTextureExportNode);
    transactionProxy->AddCommand(command, node->IsRenderServiceNode());
    return node;
}

RSRootNode::RSRootNode(bool isRenderServiceNode, bool isSamelayerRender)
    : RSCanvasNode(isRenderServiceNode, isSamelayerRender) {}

void RSRootNode::AttachRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode) const
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    if (!IsUniRenderEnabled() || isTextureExportNode_) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeAttachRSSurfaceNode>(GetId(),
            surfaceNode->GetId());
        transactionProxy->AddCommand(command, false);
    } else {
        std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeAttachToUniSurfaceNode>(GetId(),
            surfaceNode->GetId());
        transactionProxy->AddCommand(command, true);
    }
}

void RSRootNode::SetEnableRender(bool flag) const
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeSetEnableRender>(GetId(), flag);
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    transactionProxy->FlushImplicitTransaction();
}

void RSRootNode::OnBoundsSizeChanged() const
{
    if (IsUniRenderEnabled() && !isTextureExportNode_) {
        return;
    }
    // Planning: we should use frame size instead of bounds size to calculate the surface size.
    auto bounds = GetStagingProperties().GetBounds();
    // Set RootNode Surface Size with animation final value. NOTE: this logic is only used in RenderThreadVisitor
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSRootNodeUpdateSuggestedBufferSize>(GetId(), bounds.z_, bounds.w_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, false);
    }
}
} // namespace Rosen
} // namespace OHOS
