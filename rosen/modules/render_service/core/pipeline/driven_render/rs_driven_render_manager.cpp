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

#include "rs_driven_render_manager.h"

#include <parameters.h>
#include "platform/common/rs_log.h"
#include "rs_driven_render_ext.h"
#include "rs_driven_render_visitor.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
static std::unique_ptr<RSDrivenRenderManager> g_drivenRenderManagerInstance =
    std::make_unique<RSDrivenRenderManager>();

RSDrivenRenderManager& RSDrivenRenderManager::GetInstance()
{
    return *g_drivenRenderManagerInstance;
}

void RSDrivenRenderManager::InitInstance()
{
    if (!system::GetBoolParameter("persist.rosen.drivenrender.enabled", false)) {
        RS_LOGD("RSDrivenRenderManager: driven render not enabled.");
        return;
    }
    if (!RSDrivenRenderExt::Instance().OpenDrivenRenderExt()) {
        RS_LOGE("RSDrivenRenderManager: init instance failed!");
        return;
    }
    g_drivenRenderManagerInstance->drivenRenderEnabled_ = true;
}

bool RSDrivenRenderManager::GetDrivenRenderEnabled() const
{
    return drivenRenderEnabled_ && system::GetBoolParameter("rosen.debug.drivenrender.enabled", true);
}

const DrivenUniRenderMode& RSDrivenRenderManager::GetUniDrivenRenderMode() const
{
    return uniRenderMode_;
}

float RSDrivenRenderManager::GetUniRenderGlobalZOrder() const
{
    return uniRenderGlobalZOrder_;
}

bool RSDrivenRenderManager::ClipHoleForDrivenNode(RSPaintFilterCanvas& canvas, const RSCanvasRenderNode& node) const
{
    auto contentSurfaceNode = RSDrivenRenderManager::GetInstance().GetContentSurfaceNode();
    if (contentSurfaceNode->GetDrivenCanvasNode() == nullptr ||
        contentSurfaceNode->GetDrivenCanvasNode()->GetId() != node.GetId()) {
        return false;
    }
    auto& property = node.GetRenderProperties();
    Vector4f clipHoleRect = property.GetBounds();
    if (clipHoleRect.IsZero()) {
        clipHoleRect = property.GetFrame();
    }
    static int pixel = -1; // clip hole rect should large than content bounds
    auto x = std::ceil(clipHoleRect.x_ + pixel); // x decrease 1 pixel
    auto y = std::ceil(clipHoleRect.y_ + pixel); // y decrease 1 pixel
    auto width = std::floor(clipHoleRect.z_ - (2 * pixel)); // width increase 2 pixels
    auto height = std::floor(clipHoleRect.w_ - (2 * pixel)); // height increase 2 pixels
    RRect absClipRRect = RRect({x, y, width, height}, property.GetCornerRadius());

    // clip hole
    canvas.save();
    canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    canvas.clear(SK_ColorTRANSPARENT);
    canvas.restore();
    return true;
}

static bool IsValidSurfaceName(RSBaseRenderNode::SharedPtr backgroundNode)
{
    if (!backgroundNode) {
        return false;
    }
    auto rsParent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(backgroundNode->GetParent().lock());
    if (!rsParent) {
        return false;
    }
    return RSDrivenRenderExt::Instance().IsValidSurfaceName(rsParent->GetName());
}

void RSDrivenRenderManager::DoPrepareRenderTask(const DrivenPrepareInfo& info)
{
    bool backgroundDirty = info.dirtyInfo.backgroundDirty;
    bool contentDirty = info.dirtyInfo.contentDirty;
    bool nonContentDirty = info.dirtyInfo.nonContentDirty;
    bool isValidSurface = IsValidSurfaceName(info.backgroundNode);
    RSBaseRenderNode::SharedPtr currBackground = nullptr;
    RSBaseRenderNode::SharedPtr currContent = nullptr;
    DrivenDirtyType dirtyType = info.dirtyInfo.type;

    RS_LOGD("RSDrivenRenderManager: backgroundDirty = %d, contentDirty = %d, nonContentDirty = %d, dirtyType = %d, "
        "hasInvalidScene = %d, hasDrivenNodeOnUniTree = %d, isValidSurface = %d",
        backgroundDirty, contentDirty, nonContentDirty, dirtyType,
        info.hasInvalidScene, info.hasDrivenNodeOnUniTree, isValidSurface);

    if (!info.hasInvalidScene && info.hasDrivenNodeOnUniTree &&
        dirtyType != DrivenDirtyType::INVALID && isValidSurface) {
        currBackground = info.backgroundNode;
        currContent = info.contentNode;
    }

    bool shouldPrepare = false;
    if (currBackground != nullptr && currContent != nullptr) {
        shouldPrepare = true;
        if (contentCanvasNodeId_ != currContent->GetId() || backgroundCanvasNodeId_ != currBackground->GetId()) {
            contentSurfaceNode_->Reset();
            backgroundSurfaceNode_->Reset();
            contentSurfaceNode_->SetDrivenCanvasNode(currContent);
            backgroundSurfaceNode_->SetDrivenCanvasNode(currBackground);
            contentCanvasNodeId_ = currContent->GetId();
            backgroundCanvasNodeId_ = currBackground->GetId();
        }
    }

    if (shouldPrepare) {
        auto visitor = std::make_shared<RSDrivenRenderVisitor>();
        visitor->SetDirtyInfo(backgroundDirty, contentDirty, nonContentDirty);
        visitor->SetScreenRect(info.screenRect);
        contentSurfaceNode_->ResetCurrFrameState();
        backgroundSurfaceNode_->ResetCurrFrameState();
        visitor->PrepareDrivenSurfaceRenderNode(*backgroundSurfaceNode_);
        visitor->PrepareDrivenSurfaceRenderNode(*contentSurfaceNode_);
    } else {
        RS_LOGD("RSDrivenRenderManager: should not prepare and reset!");
        Reset();
    }
    UpdateUniDrivenRenderMode(dirtyType);

    if (!isBufferCacheClear_ && !info.hasDrivenNodeOnUniTree) {
        backgroundSurfaceNode_->ClearBufferCache();
        contentSurfaceNode_->ClearBufferCache();
        isBufferCacheClear_ = true;
    }
}

void RSDrivenRenderManager::DoProcessRenderTask(const DrivenProcessInfo& info)
{
    auto visitor = std::make_shared<RSDrivenRenderVisitor>();
    visitor->SetUniProcessor(info.uniProcessor);
    visitor->SetUniColorSpace(info.uniColorSpace);
    visitor->SetUniGlobalZOrder(info.uniGlobalZOrder);

    RS_TRACE_BEGIN("RSUniRender:DrivenRenderBackGround");
    visitor->ProcessDrivenSurfaceRenderNode(*backgroundSurfaceNode_);
    RS_TRACE_END();
    RS_TRACE_BEGIN("RSUniRender:DrivenRenderContent");
    visitor->ProcessDrivenSurfaceRenderNode(*contentSurfaceNode_);
    RS_TRACE_END();

    uniRenderMode_ = DrivenUniRenderMode::RENDER_WITH_NORMAL;
    uniRenderGlobalZOrder_ = 0.0;
    if (!backgroundSurfaceNode_->IsDisabledMode() || !contentSurfaceNode_->IsDisabledMode()) {
        isBufferCacheClear_ = false;
    }
}

void RSDrivenRenderManager::Reset()
{
    contentSurfaceNode_->Reset();
    backgroundSurfaceNode_->Reset();
    contentCanvasNodeId_ = 0;
    backgroundCanvasNodeId_ = 0;
    uniRenderMode_ = DrivenUniRenderMode::RENDER_WITH_NORMAL;
    uniRenderGlobalZOrder_ = 0.0;
}

void RSDrivenRenderManager::UpdateUniDrivenRenderMode(DrivenDirtyType dirtyType)
{
    if (dirtyType == DrivenDirtyType::MARK_DRIVEN) {
        if (backgroundSurfaceNode_->IsExpandedMode()) {
            backgroundSurfaceNode_->DisabledRenderMode();
        }
        if (contentSurfaceNode_->IsExpandedMode()) {
            contentSurfaceNode_->DisabledRenderMode();
        }
        uniRenderMode_ = DrivenUniRenderMode::RENDER_WITH_NORMAL;
    } else {
        // adjust content render mode to load balancing
        if (backgroundSurfaceNode_->IsExpandedMode() && contentSurfaceNode_->IsExpandedMode()) {
            contentSurfaceNode_->DisabledRenderMode();
        }

        // uni-render mode should follow contentSurfaceNode render mode
        if (contentSurfaceNode_->IsExpandedMode()) {
            uniRenderMode_ = DrivenUniRenderMode::RENDER_WITH_CLIP_HOLE;
        } else if (contentSurfaceNode_->IsReusableMode()) {
            uniRenderMode_ = DrivenUniRenderMode::REUSE_WITH_CLIP_HOLE;
        } else {
            uniRenderMode_ = DrivenUniRenderMode::RENDER_WITH_NORMAL;
        }
    }

    if (backgroundSurfaceNode_->IsDisabledMode() && contentSurfaceNode_->IsDisabledMode()) {
        uniRenderGlobalZOrder_ = 0.0;
    } else {
        uniRenderGlobalZOrder_ = 3.0; // uni-layer z-order
    }

    auto contentRenderMode = contentSurfaceNode_->GetDrivenSurfaceRenderMode();
    auto backgroundRenderMode = backgroundSurfaceNode_->GetDrivenSurfaceRenderMode();
    RS_LOGI("RSDrivenRenderManager: contentRenderMode = %d, backgroundRenderMode = %d, uniRenderMode = %d",
        contentRenderMode, backgroundRenderMode, uniRenderMode_);
}
} // namespace Rosen
} // namespace OHOS
