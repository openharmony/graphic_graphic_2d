/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/composite_layer/rs_composite_layer_utils.h"

#include "platform/common/rs_log.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_texture_export.h"

namespace OHOS {
namespace Rosen {
namespace {
#ifndef ROSEN_CROSS_PLATFORM
constexpr uint32_t COMPOSITE_SURFACE_SIZE = 5;
#endif
}
RSCompositeLayerUtils::RSCompositeLayerUtils(std::shared_ptr<RSNode> rootNode, uint32_t zOrder)
{
    rootNode_ = rootNode;
    topLayerZOrder_ = zOrder;
    if (RSSystemProperties::GetRSClientMultiInstanceEnabled()) {
        rsUiDirector_ = RSUIDirector::Create();
        rsUiDirector_->Init(true, true);
    }
}

RSCompositeLayerUtils::~RSCompositeLayerUtils() {}

bool RSCompositeLayerUtils::CreateCompositeLayer()
{
    if (rootNode_ == nullptr) {
        return false;
    }

    if (auto surfaceNode = rootNode_->ReinterpretCastTo<RSSurfaceNode>()) {
        RSSurfaceNodeConfig config;
        config.SurfaceNodeName = surfaceNode->GetName();
        if (RSSystemProperties::GetRSClientMultiInstanceEnabled()) {
            compositeNode_ = RSSurfaceNode::Create(config, false, rsUiDirector_->GetRSUIContext());
            rsUiDirector_->SetRSSurfaceNode(compositeNode_);
        } else {
            compositeNode_ = RSSurfaceNode::Create(config, false);
        }
    }

    if (compositeNode_ == nullptr) {
        RS_LOGE("RSCompositeLayerUtils::CreateCompositeLayer compositeNode_ is null");
        return false;
    }

    auto parent = rootNode_->GetParent();
    if (parent == nullptr) {
        RS_LOGE("RSCompositeLayerUtils::CreateCompositeLayer rootNode hasn't parent");
        return false;
    }
    auto bounds = rootNode_->GetStagingProperties().GetBounds();
    compositeNode_->SetBounds({bounds.x_, bounds.y_, bounds.z_, bounds.w_});
    if (RSSystemProperties::GetRSClientMultiInstanceEnabled()) {
        rsUiDirector_->SendMessages();
    }

#ifndef ROSEN_CROSS_PLATFORM
    parent->AddChild(compositeNode_);
    rootNode_->RemoveFromTree();
    auto surface = compositeNode_->GetSurface();
    if (!surface) {
        return false;
    }
    auto surfaceId = surface->GetUniqueId();
    surface->SetQueueSize(COMPOSITE_SURFACE_SIZE);
    textureExport_ = std::make_shared<RSTextureExport>(rootNode_, surfaceId);
    textureExport_->DoTextureExport();
    RSInterfaces::GetInstance().SetLayerTopForHWC(compositeNode_->GetName(), true, topLayerZOrder_);
    return true;
#else
    return false;
#endif
}

void RSCompositeLayerUtils::UpdateVirtualNodeBounds(const Vector4f& bounds)
{
    if (textureExport_) {
        textureExport_->UpdateBufferInfo(bounds.x_, bounds.y_, bounds.z_, bounds.w_);
    }
    if (compositeNode_) {
        compositeNode_->SetBounds({bounds.x_, bounds.y_, bounds.z_, bounds.w_});
    }
    if (RSSystemProperties::GetRSClientMultiInstanceEnabled()) {
        rsUiDirector_->SendMessages();
    }
}

uint32_t RSCompositeLayerUtils::GetTopLayerZOrder()
{
    return topLayerZOrder_;
}

std::shared_ptr<RSSurfaceNode> RSCompositeLayerUtils::GetCompositeNode()
{
    return compositeNode_;
}

bool RSCompositeLayerUtils::DealWithSelfDrawCompositeNode(std::shared_ptr<RSNode> node, uint32_t zOrder)
{
    if (!node || node->GetChildren().size() != 1) {
        return false;
    }
    auto child = node->GetChildren()[0].lock();
    if (!child) {
        return false;
    }
    auto surfaceNode = child->ReinterpretCastTo<RSSurfaceNode>();
    if (surfaceNode && surfaceNode->IsSelfDrawingNode()) {
        RSInterfaces::GetInstance().SetLayerTopForHWC(surfaceNode->GetName(), true, zOrder);
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS