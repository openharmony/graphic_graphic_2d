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

#include "pipeline/rs_display_render_node.h"

#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "screen_manager/screen_types.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSDisplayRenderNode::RSDisplayRenderNode(NodeId id, const RSDisplayNodeConfig& config, std::weak_ptr<RSContext> context)
    : RSRenderNode(id, context), RSSurfaceHandler(id), screenId_(config.screenId), offsetX_(0), offsetY_(0),
      isMirroredDisplay_(config.isMirrored),
      dirtyManager_(std::make_shared<RSDirtyRegionManager>())
{}

RSDisplayRenderNode::~RSDisplayRenderNode() = default;

void RSDisplayRenderNode::CollectSurface(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender)
{
    for (auto& child : node->GetSortedChildren()) {
        child->CollectSurface(child, vec, isUniRender);
    }
}

void RSDisplayRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareDisplayRenderNode(*this);
}

void RSDisplayRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->ProcessDisplayRenderNode(*this);
}

RSDisplayRenderNode::CompositeType RSDisplayRenderNode::GetCompositeType() const
{
    return compositeType_;
}

void RSDisplayRenderNode::SetCompositeType(RSDisplayRenderNode::CompositeType type)
{
    compositeType_ = type;
}

void RSDisplayRenderNode::SetForceSoftComposite(bool flag)
{
    forceSoftComposite_ = flag;
}

bool RSDisplayRenderNode::IsForceSoftComposite() const
{
    return forceSoftComposite_;
}

void RSDisplayRenderNode::SetMirrorSource(SharedPtr node)
{
    if (!isMirroredDisplay_ || node == nullptr) {
        return;
    }
    mirrorSource_ = node;
}

void RSDisplayRenderNode::ResetMirrorSource()
{
    mirrorSource_.reset();
}

bool RSDisplayRenderNode::IsMirrorDisplay() const
{
    return isMirroredDisplay_;
}

void RSDisplayRenderNode::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
}

bool RSDisplayRenderNode::GetSecurityDisplay() const
{
    return isSecurityDisplay_;
}

void RSDisplayRenderNode::SetIsMirrorDisplay(bool isMirror)
{
    isMirroredDisplay_ = isMirror;
    RS_LOGD("RSDisplayRenderNode::SetIsMirrorDisplay, node id:[%" PRIu64 "], isMirrorDisplay: [%s]", GetId(),
        IsMirrorDisplay() ? "true" : "false");
}

bool RSDisplayRenderNode::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    if (consumer_ != nullptr && surface_ != nullptr) {
        RS_LOGI("RSDisplayRenderNode::CreateSurface already created, return");
        return true;
    }
    consumer_ = Surface::CreateSurfaceAsConsumer("DisplayNode");
    if (consumer_ == nullptr) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer_->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    consumerListener_ = listener;
    auto producer = consumer_->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);

#ifdef ACE_ENABLE_GL
    // GPU render
    surface_ = std::make_shared<RSSurfaceOhosGl>(surface);
#else
    // CPU render
    surface_ = std::make_shared<RSSurfaceOhosRaster>(surface);
#endif

    RS_LOGI("RSDisplayRenderNode::CreateSurface end");
    surfaceCreated_ = true;
    return true;
}

bool RSDisplayRenderNode::SkipFrame(uint32_t skipFrameInterval)
{
    frameCount_++;
    // ensure skipFrameInterval is not 0
    if (skipFrameInterval == 0) {
        return false;
    }
    if ((frameCount_ - 1) % skipFrameInterval == 0) {
        return false;
    }
    return true;
}

ScreenRotation RSDisplayRenderNode::GetRotation() const
{
    auto boundsGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr == nullptr) {
        return ScreenRotation::ROTATION_0;
    }
    // -90.0f: convert rotation degree to 4 enum values
    return static_cast<ScreenRotation>(static_cast<int32_t>(std::roundf(boundsGeoPtr->GetRotation() / -90.0f)) % 4);
}

void RSDisplayRenderNode::UpdateDisplayDirtyManager(uint32_t bufferage)
{
    for (auto iter = lastFrameSurfacePos_.cbegin(); iter != lastFrameSurfacePos_.cend(); iter++) {
        if (currentFrameSurfacePos_.find(iter->first) == currentFrameSurfacePos_.end()) {
            dirtyManager_->MergeDirtyRect(iter->second);
        }
    }
    dirtyManager_->SetBufferAge(bufferage);
    dirtyManager_->UpdateDirty();
    lastFrameSurfacePos_.clear();
    lastFrameSurfacePos_.swap(currentFrameSurfacePos_);
}
} // namespace Rosen
} // namespace OHOS
