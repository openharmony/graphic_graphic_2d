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

#include "ui/rs_texture_node.h"

#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSNode> RSTextureNode::Create()
{
#ifdef USE_FLUTTER_TEXTURE
    std::shared_ptr<RSTextureNode> node(new RSTextureNode());
    RSNodeMap::Instance().RegisterNode(node);

    // RSMsgPtr msgPtr = std::make_unique<CreateTextureNodeMsg>(node->GetId());
    // auto transactionProxy = RSTransactionProxy::GetInstance();
    // if (transactionProxy != nullptr) {
    //     transactionProxy->AddCommand(command, IsRenderServiceNode());
    // }
    return node;
#else
    return RSCanvasNode::Create();
#endif
}

void RSTextureNode::SetTextureRegistry(std::shared_ptr<flutter::OHOS::TextureRegistry> registry)
{
    // RSMsgPtr msgPtr = std::make_unique<SetRegistryMsg>(registry);
    // auto transactionProxy = RSTransactionProxy::GetInstance();
    // if (transactionProxy != nullptr) {
    //     transactionProxy->AddCommand(command, IsRenderServiceNode());
    // }
}

RSTextureNode::RSTextureNode(bool isRenderServiceNode) : RSCanvasNode(isRenderServiceNode) {}

void RSTextureNode::UpdateTexture(int64_t textureId, bool freeze, RectF drawRect)
{
    // RSMsgPtr msgPtr = std::make_unique<UpdateTextureMsg>(GetId(), textureId, freeze, drawRect);
    // auto transactionProxy = RSTransactionProxy::GetInstance();
    // if (transactionProxy != nullptr) {
    //     transactionProxy->AddCommand(command, IsRenderServiceNode());
    // }
}
} // namespace Rosen
} // namespace OHOS
