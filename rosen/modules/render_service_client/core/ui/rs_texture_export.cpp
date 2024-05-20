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

RSTextureExport::RSTextureExport(std::shared_ptr<RSNode> rootNode, SurfaceId surfaceId)
{
    rsUiDirector_ = RSUIDirector::Create();
    rootNode_ = rootNode;
    surfaceId_ = surfaceId;
    RSSurfaceNodeConfig config = {
        .SurfaceNodeName = "textureExportSurfaceNode",
        .additionalData = nullptr,
        .isTextureExportNode = true,
        .surfaceId = surfaceId_
    };
    virtualSurfaceNode_ = RSSurfaceNode::Create(config, false);
    rootNode_->SyncTextureExport(true);
}

RSTextureExport::~RSTextureExport()
{
    rsUiDirector_->Destroy(true);
}

bool RSTextureExport::DoTextureExport()
{
    if (!rootNode_->IsTextureExportNode()) {
        RS_LOGE("RSTextureExport::DoTextureExport rootNode is not textureExportNode");
        return false;
    }
    rsUiDirector_->StartTextureExport();
    if (rootNode_->GetType() != RSUINodeType::ROOT_NODE) {
        virtualRootNode_ = RSRootNode::Create(false, true);
        auto bounds = rootNode_->GetStagingProperties().GetBounds();
        virtualRootNode_->SetBounds({-bounds.x_, -bounds.y_, bounds.z_, bounds.w_});
        auto frame = rootNode_->GetStagingProperties().GetFrame();
        virtualRootNode_->SetFrame({-frame.x_, -frame.y_, frame.z_, frame.w_});
    }
    if (!virtualSurfaceNode_) {
        ROSEN_LOGE("RSTextureExport::DoTextureExport create surfaceNode failed");
        return false;
    }
    if (rootNode_->GetType() == RSUINodeType::ROOT_NODE) {
        rsUiDirector_->SetRoot(rootNode_->GetId());
    } else {
        rsUiDirector_->SetRoot(virtualRootNode_->GetId());
        virtualRootNode_->AddChild(rootNode_);
    }
    rsUiDirector_->SetRSSurfaceNode(virtualSurfaceNode_);
    rsUiDirector_->GoForeground(true);
    return true;
}

void RSTextureExport::StopTextureExport()
{
    rsUiDirector_->Destroy(true);
    rootNode_->RemoveFromTree();
}

} // namespace Rosen
} // namespace OHOS
