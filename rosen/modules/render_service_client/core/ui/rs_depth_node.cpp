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

#include "ui/rs_depth_node.h"

#include "command_modifier/rs_depth_node_command_modifier.h"
#include "command/rs_depth_node_command.h"
#include "modifier_ng/appearance/rs_depth_space_modifier.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSDepthNode::SharedPtr RSDepthNode::Create(bool isRenderServiceNode, bool isTextureExportNode,
    std::shared_ptr<RSUIContext> rsUIContext)
{
    RS_LOGD("RSDepthNode::Create");
    SharedPtr node(new RSDepthNode(isRenderServiceNode, isTextureExportNode, rsUIContext));
    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSDepthNodeCreate>(node->GetId(), isTextureExportNode);
    node->AddCommand(command, node->IsRenderServiceNode());
    node->SetUIContextToken();

    return node;
}

void RSDepthNode::CreateRenderNode()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSDepthNodeCreate>(GetId(), isTextureExportNode_);
    AddCommand(command, IsRenderServiceNode());
}

void RSDepthNode::SetDepthSpaceType(DepthSpaceType depthSpaceType)
{
    SetRSCmdProperty<DepthSpaceTypeCmdModifier>(DepthSpaceTypeCmdParam{depthSpaceType});
}

void RSDepthNode::SetDepthImage(const std::shared_ptr<RSImage>& depthImage)
{
    SetPropertyNG<ModifierNG::RSDepthSpaceModifier, &ModifierNG::RSDepthSpaceModifier::SetDepthImage>(depthImage);
}

void RSDepthNode::SetDepthCameraPara(const DepthCameraPara& cameraPara)
{
    SetPropertyNG<ModifierNG::RSDepthSpaceModifier, &ModifierNG::RSDepthSpaceModifier::SetDepthCameraPara>(cameraPara);
}

void RSDepthNode::SetDepthLightPara(const DepthLightPara& lightPara)
{
    SetPropertyNG<ModifierNG::RSDepthSpaceModifier, &ModifierNG::RSDepthSpaceModifier::SetDepthLightPara>(lightPara);
}

void RSDepthNode::SetDepthImageMatrix(const Matrix3f& imageMatrix)
{
    SetPropertyNG<ModifierNG::RSDepthSpaceModifier, &ModifierNG::RSDepthSpaceModifier::SetDepthImageMatrix>(
        imageMatrix);
}

RSDepthNode::RSDepthNode(bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, isTextureExportNode, rsUIContext) {}
} // namespace Rosen
} // namespace OHOS
