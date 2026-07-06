
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
#include "rs_split_surface.h"
#include "command/rs_surface_node_command.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "params/rs_screen_render_params.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSRenderNodeDrawable;
class RSScreenRenderNodeDrawable;
}

namespace {
constexpr uint32_t BUFFER_WIDTH_EXPAND = 2;
constexpr float BUFFER_HEIGHT_RATIO = 1.4f;
} // namespace

SplitSurface::SplitSurface(uint32_t screenWidth, uint32_t screenHeight)
{
    bufferWidth_ = screenWidth + BUFFER_WIDTH_EXPAND;
    bufferHeight_ = screenHeight * BUFFER_HEIGHT_RATIO;
}

void SplitSurface::Init(std::shared_ptr<RSRenderNode> opIncParentNode_)
{
    if (splitSurfaceNode_ == nullptr) {
        auto context = RSMainThread::Instance()->GetContext().shared_from_this();
        if (context) {
            NodeId splitSurfaceNodeId = GenerateUniqueNodeIdForRS();
            LAYER_SPLIT_LOGD("SplitSurface::Init splitSurfaceNodeId=%{public}" PRIu64, splitSurfaceNodeId);
            // Create splitSurfaceBuffer_
            splitSurfaceBuffer_ = std::make_unique<RSSplitSurfaceBuffer>(
                name_, splitSurfaceNodeId, bufferWidth_, bufferHeight_);
            splitSurfaceBuffer_->Init(true);
            splitSurfaceBuffer_->PreAllocateBuffer();
            // Create splitSurfaceNode_
            splitSurfaceNode_ = std::shared_ptr<RSSurfaceRenderNode>(
               new RSSurfaceRenderNode(RSSurfaceRenderNodeConfig {
                    .id = splitSurfaceNodeId,
                    .name = "splitSurfaceNode",
                    .isTextureExportNode = false
                },
                context,
                splitSurfaceBuffer_->GetSurfaceHandler()));
            if (splitSurfaceNode_ == nullptr) {
                return;
            }
            splitSurfaceNode_->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
            splitSurfaceNode_->SetHardwareEnabled(true);
            splitSurfaceNode_->SetSplitSurfaceNode(true);
            context->GetMutableNodeMap().RegisterRenderNode(splitSurfaceNode_);
            splitSurfaceDrawable_ = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
                splitSurfaceNode_->GetRenderDrawable());
        }
    }

    splitSurfaceNode_->GetMutableRenderProperties().SetBackgroundColor(RSColor(Drawing::Color::COLOR_TRANSPARENT));
    auto bounds = opIncParentNode_->GetRenderProperties().GetBounds();
    splitSurfaceNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, bounds.z_, bounds.w_));
    auto frame = opIncParentNode_->GetRenderProperties().GetFrame();
    splitSurfaceNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, frame.z_, frame.w_));

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(splitSurfaceNode_->GetRenderParams().get());
    surfaceParams->SetHardwareEnabled(true);
    surfaceParams->SetSplitLayerTag(true);
}

NodeId SplitSurface::GetSurfaceNodeId()
{
    if (splitSurfaceNode_ == nullptr) {
        return 0;
    }
    return splitSurfaceNode_->GetId();
}

bool SplitSurface::CheckParentNodeOnTheTree()
{
    if (splitSurfaceNode_) {
        auto parent = splitSurfaceNode_->GetParent().lock();
        if (parent == nullptr || !parent->IsOnTheTree()) {
            return false;
        }
    }
    return true;
}

void SplitSurface::UnregisterSplitSurfaceNode()
{
    surfaceStatus_ = SurfaceStatus::UNREGISTER;
    if (!splitSurfaceNode_) {
        return;
    }
    
    auto parent = splitSurfaceNode_->GetParent().lock();
    if (parent) {
        parent->RemoveChild(splitSurfaceNode_);
    }

    auto context = RSMainThread::Instance()->GetContext().shared_from_this();
    if (context) {
        context->GetMutableNodeMap().UnregisterRenderNode(splitSurfaceNode_->GetId());
    }
    splitSurfaceNode_.reset();
}

bool SplitSurface::IsBufferConsumed()
{
    if (splitSurfaceNode_ == nullptr) {
        return false;
    }

    auto surfaceHandler = splitSurfaceNode_->GetRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return false;
    }

    if (surfaceHandler->GetBuffer()) {
        return true;
    }

    return false;
}

bool SplitSurface::IsBufferReleased()
{
    if (splitSurfaceBuffer_ == nullptr) {
        LAYER_SPLIT_LOGD("SplitSurface::IsBufferReleased splitSurfaceBuffer_ == nullptr");
        return true;
    } else {
        if (!splitSurfaceBuffer_->IsSurfaceCreated()) {
            LAYER_SPLIT_LOGD("SplitSurface::IsBufferReleased !splitSurfaceBuffer_->IsSurfaceCreated()");
            return true;
        }
        auto consumer = splitSurfaceBuffer_->GetSurfaceHandler()->GetConsumer();
        if (!consumer || consumer->QueryIfBufferAvailable()) {
            LAYER_SPLIT_LOGD("SplitSurface::IsBufferReleased consumer->QueryIfBufferAvailable()");
            return true;
        }
    }
    LAYER_SPLIT_LOGD("SplitSurface::IsBufferReleased no available buffer");
    return false;
}

bool SplitSurface::RequestFrame(RSSurfaceRenderParams& params)
{
    if (!IsBufferReleased()) {
        return false;
    }

    if (splitRenderFrame_) {
        splitRenderFrame_->Reset();
    }
    splitRenderFrame_ = splitSurfaceBuffer_->RequestFrame(colorSpace_);
    if (!splitRenderFrame_) {
        LAYER_SPLIT_LOGE("SplitSurface::Request failed");
        return false;
    }
    splitCanvas_ = std::make_shared<RSPaintFilterCanvas>(splitRenderFrame_->GetFrame()->GetSurface().get());
    splitCanvas_->SetTargetColorGamut(colorSpace_);
    splitCanvas_->SetScreenId(params.GetScreenId());
    splitCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    return true;
}

void SplitSurface::SetBufferNull()
{
    auto surfaceHandler = splitSurfaceBuffer_->GetSurfaceHandler();
    if (!surfaceHandler) {
        LAYER_SPLIT_LOGD("SplitSurface::SetBufferNull !surfaceHandler");
        return;
    }

    splitSurfaceNode_->UpdateBuffer();
    isDfxDrawed_ = false;
}

void SplitSurface::DrawDfx(Drawing::Color color)
{
    if (splitCanvas_ == nullptr || isDfxDrawed_) {
        return;
    }
    Drawing::Brush rectBrush;
    rectBrush.SetColor(color);
    rectBrush.SetAntiAlias(true);
    rectBrush.SetAlphaF(0.2f);
    Drawing::RectF rect(0.0, 0.0, bufferWidth_, bufferHeight_);
    splitCanvas_->ResetMatrix();
    splitCanvas_->AttachBrush(rectBrush);
    splitCanvas_->DrawRect(rect);
    splitCanvas_->DetachBrush();
    isDfxDrawed_ = true;
}

void SplitSurface::FlushFrame()
{
    splitSurfaceBuffer_->FlushFrame();
}

void SplitSurface::SetColorSpace(GraphicColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

GraphicColorGamut SplitSurface::GetColorSpace() const
{
    return colorSpace_;
}
}