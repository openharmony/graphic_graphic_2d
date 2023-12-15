/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "platform/common/rs_log.h"
#include "ui/rs_texture_export.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {

RSTextureExport::RSTextureExport()
{
    rsUiDirector_ = RSUIDirector::Create();
}

RSTextureExport::~RSTextureExport()
{
    rsUiDirector_->Destroy(true);
}

bool RSTextureExport::DoTextureExport(std::shared_ptr<RSNode> rootNode, SurfaceId surfaceId)
{   
    rsUiDirector_->StartTextureExport();
    std::shared_ptr<RSNode> virtualRootNode = RSRootNode::Create(false, true);
    RSSurfaceNodeConfig config = {
        .SurfaceNodeName = "textureExportSurfaceNode",
        .additionalData = nullptr,
        .isTextureExportNode = true,
        .surfaceId = surfaceId
    };
    std::shared_ptr<RSSurfaceNode> virtualSurfaceNode = RSSurfaceNode::Create(config, false);
    if (!virtualSurfaceNode) {
        ROSEN_LOGE("RSTextureExport::DoTextureExport create surfaceNode failed");
        return false;
    }
    rsUiDirector_->SetRoot(virtualRootNode->GetId());
    rsUiDirector_->SetRSSurfaceNode(virtualSurfaceNode);
    virtualRootNode->AddChild(rootNode);
    rsUiDirector_->GoForeground(true);
    return true;
}

} // namespace Rosen
} // namespace OHOS
