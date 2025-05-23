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

#include "ui/rs_texture_export.h"

#include "platform/common/rs_log.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif

namespace OHOS {
namespace Rosen {
std::unordered_set<std::shared_ptr<RSSurfaceNode>> RSTextureExport::virtualSurfaceNodeSet_;
std::mutex RSTextureExport::virtualSurfaceNodeSetMutex_;
RSTextureExport::RSTextureExport(std::shared_ptr<RSNode> rootNode, SurfaceId surfaceId)
{
    if (rootNode == nullptr) {
        return;
    }
    rsUiDirector_ = RSUIDirector::Create();
    rootNode_ = rootNode;
    surfaceId_ = surfaceId;
    RSSurfaceNodeConfig config = {
        .SurfaceNodeName = "textureExportSurfaceNode",
        .additionalData = nullptr,
        .isTextureExportNode = true,
        .surfaceId = surfaceId_
    };
    virtualSurfaceNode_ = RSSurfaceNode::Create(config, false, rootNode_->GetRSUIContext());
    {
        std::lock_guard<std::mutex> lock(virtualSurfaceNodeSetMutex_);
        virtualSurfaceNodeSet_.insert(virtualSurfaceNode_);
    }
    rootNode_->SyncTextureExport(true);
}

RSTextureExport::~RSTextureExport()
{
    {
        std::lock_guard<std::mutex> lock(virtualSurfaceNodeSetMutex_);
        virtualSurfaceNodeSet_.erase(virtualSurfaceNode_);
    }
    rsUiDirector_->Destroy(true);
}

bool RSTextureExport::DoTextureExport()
{
    if (!rootNode_->IsTextureExportNode()) {
        rootNode_->SyncTextureExport(true);
    }
    rsUiDirector_->StartTextureExport();
    if (rootNode_->GetType() != RSUINodeType::ROOT_NODE) {
        virtualRootNode_ = RSRootNode::Create(false, true, rootNode_->GetRSUIContext());
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
        rsUiDirector_->SetRSRootNode(RSBaseNode::ReinterpretCast<RSRootNode>(rootNode_));
    } else {
        rsUiDirector_->SetRSRootNode(RSBaseNode::ReinterpretCast<RSRootNode>(virtualRootNode_));
        virtualRootNode_->AddChild(rootNode_);
    }
    rsUiDirector_->SetRSSurfaceNode(virtualSurfaceNode_);
    rsUiDirector_->GoForeground(true);
    return true;
}

void RSTextureExport::UpdateBufferInfo(float x, float y, float width, float height)
{
    virtualRootNode_->SetBounds({-x, -y, width, height});
    virtualRootNode_->SetFrame({-x, -y, width, height});
}

void RSTextureExport::StopTextureExport()
{
    rsUiDirector_->Destroy(true);
    rootNode_->RemoveFromTree();
}

#ifdef RS_ENABLE_VK
void RSTextureExport::ClearContext()
{
    if (!RSSystemProperties::IsUseVulkan()) {
        return;
    }

    for (auto& node : virtualSurfaceNodeSet_) {
        if (node == nullptr) {
            continue;
        }
        std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(node);
        if (rsSurface == nullptr) {
            continue;
        }
        auto rsSurfaceVulkan = std::static_pointer_cast<RSSurfaceOhosVulkan>(rsSurface);
        if (rsSurfaceVulkan == nullptr) {
            continue;
        }
        rsSurfaceVulkan->ClearSurfaceResource();
    }
}
#endif
} // namespace Rosen
} // namespace OHOS
