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

#include "ui/rs_surface_node.h"

#include <algorithm>
#include <string>

#include "command/rs_surface_node_command.h"
#include "command/rs_base_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface_converter.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {

RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow)
{
    SharedPtr node(new RSSurfaceNode(surfaceNodeConfig, isWindow));

    RSNodeMap::Instance().RegisterNode(node);

    // create node in RS
    RSSurfaceRenderNodeConfig config = { .id = node->GetId() };
    if (!node->CreateNodeAndSurface(config)) {
        ROSEN_LOGE("RSSurfaceNode::Create, create node and surface is failed");
        return nullptr;
    }

    // create node in RT
    if (!isWindow) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(node->GetId());
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, isWindow);
        }
    }
    ROSEN_LOGD("RsDebug RSSurfaceNode::Create id:%llu", node->GetId());
    return node;
}

void RSSurfaceNode::SetBounds(const Vector4f& bounds)
{
    RSNode::SetBounds(bounds);
    UpdateSurfaceDefaultSize(bounds.z_, bounds.w_);
}

void RSSurfaceNode::SetBounds(float positionX, float positionY, float width, float height)
{
    RSNode::SetBounds(positionX, positionY, width, height);
    UpdateSurfaceDefaultSize(width, height);
}

void RSSurfaceNode::SetBoundsSize(const Vector2f& size)
{
    RSNode::SetBoundsSize(size);
    UpdateSurfaceDefaultSize(size.x_, size.y_);
}

void RSSurfaceNode::SetBoundsSize(float width, float height)
{
    RSNode::SetBoundsSize(width, height);
    UpdateSurfaceDefaultSize(width, height);
}

void RSSurfaceNode::SetBoundsWidth(float width)
{
    RSNode::SetBoundsWidth(width);
    UpdateSurfaceDefaultSize(width, GetStagingProperties().GetBoundsHeight());
}

void RSSurfaceNode::SetBoundsHeight(float height)
{
    RSNode::SetBoundsHeight(height);
    UpdateSurfaceDefaultSize(GetStagingProperties().GetBoundsWidth(), height);
}

void RSSurfaceNode::UpdateSurfaceDefaultSize(float width, float height)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSurfaceNodeUpdateSurfaceDefaultSize>(GetId(), width, height);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

bool RSSurfaceNode::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint64(GetId()) && parcel.WriteString(name_) && parcel.WriteBool(IsRenderServiceNode());
}

RSSurfaceNode* RSSurfaceNode::Unmarshalling(Parcel& parcel)
{
    uint64_t id = UINT64_MAX;
    std::string name;
    bool isRenderServiceNode = false;
    if (!(parcel.ReadUint64(id) && parcel.ReadString(name) && parcel.ReadBool(isRenderServiceNode))) {
        ROSEN_LOGE("RSSurfaceNode::Unmarshalling, read param is failed");
        return nullptr;
    }
    RSSurfaceNodeConfig config = { name };

    RSSurfaceNode* surfaceNode = new RSSurfaceNode(config, isRenderServiceNode);
    surfaceNode->SetId(id);

    return surfaceNode;
}

bool RSSurfaceNode::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    surface_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
                   ->CreateNodeAndSurface(config);
    return (surface_ != nullptr);
}

#ifdef ROSEN_OHOS
sptr<OHOS::Surface> RSSurfaceNode::GetSurface() const
{
    if (surface_ == nullptr) {
        ROSEN_LOGE("RSSurfaceNode::GetSurface, surface_ is nullptr");
        return nullptr;
    }
    auto ohosSurface = RSSurfaceConverter::ConvertToOhosSurface(surface_);
    return ohosSurface;
}
#endif

RSSurfaceNode::RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode)
    : RSNode(isRenderServiceNode), name_(config.SurfaceNodeName)
{}

RSSurfaceNode::~RSSurfaceNode() {
    if (!IsRenderServiceNode()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(GetId());
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, true);
        }
    }
}

} // namespace Rosen
} // namespace OHOS
