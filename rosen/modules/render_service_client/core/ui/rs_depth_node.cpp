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

#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSDepthNode::SharedPtr RSDepthNode::Create(bool isRenderServiceNode, bool isTextureExportNode,
    std::shared_ptr<RSUIContext> rsUIContext)
{
    SharedPtr node(new RSDepthNode(isRenderServiceNode, isTextureExportNode, rsUIContext));
    return node;
}

void RSDepthNode::SetDepthSpaceType(DepthSpaceType spaceType) {}

void RSDepthNode::SetDepthImage(const std::shared_ptr<RSImage>& depthImage) {}

void RSDepthNode::SetDepthCameraPara(const DepthCameraPara& cameraPara) {}

void RSDepthNode::SetDepthLightPara(const DepthLightPara& lightPara) {}

RSDepthNode::RSDepthNode(bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(isRenderServiceNode, isTextureExportNode, rsUIContext) {}
} // namespace Rosen
} // namespace OHOS
