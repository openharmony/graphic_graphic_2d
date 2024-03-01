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
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "rs_driven_render_ext.h"
#include "rs_driven_render_visitor.h"

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
    if (!system::GetBoolParameter("persist.rosen.drivenrender.enabled", true)) {
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
    static bool drivenRenderEnabled = system::GetBoolParameter("persist.rosen.debug.drivenrender.enabled", true);
    return drivenRenderEnabled_ && drivenRenderEnabled;
}

const DrivenUniRenderMode& RSDrivenRenderManager::GetUniDrivenRenderMode() const
{
    return uniRenderMode_;
}

float RSDrivenRenderManager::GetUniRenderGlobalZOrder() const
{
    return uniRenderGlobalZOrder_;
}

RectI RSDrivenRenderManager::GetUniRenderSurfaceClipHoleRect() const
{
    return uniRenderSurfaceClipHoleRect_;
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
    canvas.Save();
    canvas.ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(absClipRRect),
        Drawing::ClipOp::INTERSECT, true);
    canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    canvas.Restore();
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

    if (OHOS::Rosen::RSSystemProperties::GetDebugTraceEnabled()) {
        RS_TRACE_NAME("RSDrivenRender:DoPrepareRenderTask backgroundDirty: " +
            std::to_string(static_cast<int>(backgroundDirty)) +
            ", contentDirty: " + std::to_string(static_cast<int>(contentDirty)) +
            ", nonContentDirty: " + std::to_string(static_cast<int>(nonContentDirty)) +
            ", dirtyType: " + std::to_string(static_cast<int>(dirtyType)) +
            ", hasInvalidScene: " + std::to_string(static_cast<int>(info.hasInvalidScene)) +
            ", hasDrivenNodeOnUniTree: " + std::to_string(static_cast<int>(info.hasDrivenNodeOnUniTree)) +
            ", isValidSurface: " + std::to_string(static_cast<int>(isValidSurface)));
    }

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
    std::string traceMsg = "";
    bool isReusableMode = false;
    auto currBackground = backgroundSurfaceNode_->GetDrivenCanvasNode();
    if (currBackground != nullptr && uniRenderMode_ == DrivenUniRenderMode::REUSE_WITH_CLIP_HOLE) {
        auto rsParent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(currBackground->GetParent().lock());
        if (rsParent != nullptr) {
            isReusableMode = true;
            traceMsg = "RSDrivenRender::ReusableProcess:[" + rsParent->GetName() + "]" +
                " " + rsParent->GetDstRect().ToString() +
                " Alpha: " + std::to_string(rsParent->GetGlobalAlpha()).substr(0, 4); // substr[0]-[4] is alpha value
        }
    }
    if (isReusableMode) {
        RS_TRACE_BEGIN(traceMsg);
    }

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
    uniRenderSurfaceClipHoleRect_.Clear();
    if (!backgroundSurfaceNode_->IsDisabledMode() || !contentSurfaceNode_->IsDisabledMode()) {
        isBufferCacheClear_ = false;
    }

    if (isReusableMode) {
        RS_TRACE_END();
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
    uniRenderSurfaceClipHoleRect_.Clear();
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
            uniRenderSurfaceClipHoleRect_ = CalcUniRenderSurfaceClipHoleRect();
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
    RS_LOGD("RSDrivenRenderManager: contentRenderMode = %{public}d, backgroundRenderMode"
        " = %{public}d, uniRenderMode = %{public}d",
        contentRenderMode, backgroundRenderMode, uniRenderMode_);
}

RectI RSDrivenRenderManager::CalcUniRenderSurfaceClipHoleRect()
{
    RectI rect;
    if (contentSurfaceNode_->GetDrivenCanvasNode() != nullptr) {
        auto canvasNode =
            RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(contentSurfaceNode_->GetDrivenCanvasNode());
        if (canvasNode == nullptr) {
            return rect;
        }
        auto& property = canvasNode->GetRenderProperties();
        Vector4f clipHoleRect = property.GetBounds();
        if (clipHoleRect.IsZero()) {
            clipHoleRect = property.GetFrame();
        }
        auto geoPtr = (property.GetBoundsGeometry());
        rect = geoPtr->MapAbsRect(RectF(clipHoleRect.x_, clipHoleRect.y_, clipHoleRect.z_, clipHoleRect.w_));
    }
    return rect;
}
} // namespace Rosen
} // namespace OHOS
