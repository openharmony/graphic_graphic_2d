/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_uni_render_visitor.h"

#include "common/rs_obj_abs_geometry.h"
#include "display_type.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_service_util.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_listener.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"
#include "rs_trace.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
namespace {
    const std::string SUB_TITLE_OF_LEASH_WINDOW = "leashWindow";
    const std::string SUB_TITLE_OF_STARTING_WINDOW = "startingWindow";
    const std::string NAME_OF_BOOT_ANIMATION_NODE = "main window0";
}
RSUniRenderVisitor::RSUniRenderVisitor() {}

RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSUniRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    isUniRenderForAll_ =
        RSSystemProperties::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    if (!isUniRenderForAll_) {
        RS_LOGI("RSUniRenderVisitor::PrepareDisplayRenderNode isUniRenderForAll_ false");
        uniRenderList_ = RSSystemProperties::GetUniRenderEnabledList();
    }
    PrepareBaseRenderNode(node);
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (isUniRenderForAll_ || uniRenderList_.find(node.GetName()) != uniRenderList_.end()) {
        isUniRender_ = true;
        hasUniRender_ = true;
    }
    if (IsChildOfDisplayNode(node) || IsChildOfLeashNode(node)) {
        auto currentGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
        if (currentGeoPtr != nullptr) {
            currentGeoPtr->UpdateByMatrixFromParent(nullptr);
            currentGeoPtr->UpdateByMatrixFromRenderThread(node.GetContextMatrix());
            currentGeoPtr->UpdateByMatrixFromSelf();
        }
        PrepareBaseRenderNode(node);
    } else {
        bool dirtyFlag = dirtyFlag_;
        dirtyFlag_ = node.Update(dirtyManager_, nullptr, dirtyFlag_);
        PrepareBaseRenderNode(node);
        dirtyFlag_ = dirtyFlag;
    }
    isUniRender_ = false;
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    if (!isUniRender_ || !IsChildOfSurfaceNode(node)) {
        return;
    }
    dirtyFlag_ = false;
    PrepareCanvasRenderNode(node);
}

void RSUniRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode &node)
{
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSUniRenderVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSUniRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    RS_LOGI("RSUniRenderVisitor::ProcessDisplayRenderNode node: %llu, child size:%u", node.GetChildrenCount(),
        node.GetId());
    globalZOrder_ = 0.0f;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    switch (screenInfo_.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ? RSDisplayRenderNode::CompositeType::COMPATIBLE_COMPOSITE
                : RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
            break;
        default:
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenState unsupported");
            return;
    }
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }
    processor_->Init(node.GetScreenId(), node.GetDisplayOffsetX(), node.GetDisplayOffsetY());
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();

    if (hasUniRender_) {
        std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
        auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
        if (!displayNodePtr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ReinterpretCastTo fail");
            return;
        }
        if (!node.IsSurfaceCreated()) {
            sptr<IBufferConsumerListener> listener = new RSUniRenderListener(displayNodePtr);
            if (!node.CreateSurface(listener)) {
                RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode CreateSurface failed");
                return;
            }
#ifdef RS_ENABLE_GL
            RS_LOGI("RSUniRenderVisitor::ProcessDisplayRenderNode SetRenderContext");
            node.GetRSSurface()->SetRenderContext(RSMainThread::Instance()->GetRenderContext().get());
#endif
        }

        auto rsSurface = node.GetRSSurface();
        if (rsSurface == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode No RSSurface found");
            return;
        }
        auto surfaceFrame = rsSurface->RequestFrame(screenInfo_.width, screenInfo_.height);
        if (surfaceFrame == nullptr) {
            RS_LOGE("RSUniRenderVisitor Request Frame Failed");
            return;
        }
        canvas_ = std::make_unique<RSPaintFilterCanvas>(surfaceFrame->GetCanvas());
        canvas_->clear(SK_ColorTRANSPARENT);

        ProcessBaseRenderNode(node);
        RS_TRACE_BEGIN("RSUniRender:FlushFrame");
        rsSurface->FlushFrame(surfaceFrame);
        RSMainThread::Instance()->WaitUtilUniRenderFinished();
        RS_TRACE_END();

        node.SetGlobalZOrder(uniZOrder_);
        processor_->ProcessSurface(node);
    } else {
        ScreenRotation rotation = screenManager->GetRotation(node.GetScreenId());
        uint32_t boundWidth = screenInfo_.width;
        uint32_t boundHeight = screenInfo_.height;
        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(boundWidth, boundHeight);
        }
        skCanvas_ = std::make_unique<SkCanvas>(boundWidth, boundHeight);
        canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas_.get());
        ProcessBaseRenderNode(node);
    }

    processor_->PostProcess();

    // We should release DisplayNode's surface buffer after PostProcess(),
    // since the buffer's releaseFence was set in PostProcess().
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(node);
    (void)RsRenderServiceUtil::ReleaseBuffer(surfaceHandler);
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
}

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %llu, child size:%u %s", node.GetId(),
        node.GetChildrenCount(), node.GetName().c_str());

    if (isUniRenderForAll_ || uniRenderList_.find(node.GetName()) != uniRenderList_.end()) {
        isUniRender_ = true;
    }
    if (isUniRender_) {
        if ((IsChildOfDisplayNode(node) && node.GetName() != NAME_OF_BOOT_ANIMATION_NODE) ||
            (IsChildOfLeashNode(node) && node.GetName().find(SUB_TITLE_OF_STARTING_WINDOW) == std::string::npos)) {
            if (!node.GetRenderProperties().GetVisible()) {
                RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %llu invisible", node.GetId());
                return;
            }

            if (!canvas_) {
                RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
                return;
            }
            auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
            if (!geoPtr) {
                RS_LOGI("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%llu, get geoPtr failed",
                    node.GetId());
                return;
            }
            RS_TRACE_BEGIN("RSUniRender::Process:" + node.GetName());
            uniZOrder_ = globalZOrder_++;
            canvas_->save();
            canvas_->SaveAlpha();
            canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha());
            canvas_->concat(geoPtr->GetAbsMatrix());
            auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
            RSPropertiesPainter::DrawTransitionProperties(transitionProperties, node.GetRenderProperties(), *canvas_);
            ProcessBaseRenderNode(node);
            canvas_->RestoreAlpha();
            canvas_->restore();
            RS_TRACE_END();
        } else {
            RS_TRACE_BEGIN("UniRender::Process:" + node.GetName());
            canvas_->save();
            canvas_->clipRect(SkRect::MakeXYWH(
                node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY(),
                node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight()));
            if (node.GetBuffer() == nullptr) {
                RS_LOGI("RSUniRenderVisitor::ProcessSurfaceRenderNode buffer is not available, set black");
                canvas_->clear(SK_ColorBLACK);
            } else {
                RS_LOGI("RSUniRenderVisitor::ProcessSurfaceRenderNode draw buffer on canvas");
                DrawBufferOnCanvas(node);
            }
            canvas_->restore();
            RS_TRACE_END();
        }
    } else {
        canvas_->save();
        node.SetOffset(offsetX_, offsetY_);
        node.ProcessRenderBeforeChildren(*canvas_);
        ProcessBaseRenderNode(node);
        node.SetGlobalZOrder(globalZOrder_++);
        processor_->ProcessSurface(node);
        node.ProcessRenderAfterChildren(*canvas_);
        canvas_->restore();
    }
    isUniRender_ = false;
}

void RSUniRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode node: %llu, child size:%u", node.GetId(),
        node.GetChildrenCount());
    if (!isUniRender_ || !node.GetRenderProperties().GetVisible() || !IsChildOfSurfaceNode(node)) {
        RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }

    canvas_->save();
    ProcessCanvasRenderNode(node);
    canvas_->restore();
}

void RSUniRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGD("RSUniRenderVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

bool RSUniRenderVisitor::IsChildOfDisplayNode(RSBaseRenderNode& node)
{
    auto parent = node.GetParent().lock();
    return parent && parent->IsInstanceOf<RSDisplayRenderNode>();
}

bool RSUniRenderVisitor::IsChildOfSurfaceNode(RSBaseRenderNode& node)
{
    auto parent = node.GetParent().lock();
    return parent && parent->IsInstanceOf<RSSurfaceRenderNode>();
}

bool RSUniRenderVisitor::IsChildOfLeashNode(RSBaseRenderNode& node)
{
    auto parent = node.GetParent().lock();
    if (parent && parent->IsInstanceOf<RSSurfaceRenderNode>()) {
        auto surfaceNode = parent->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode && surfaceNode->GetName().find(SUB_TITLE_OF_LEASH_WINDOW) != std::string::npos) {
            return true;
        }
    }
    RS_LOGD("RSUniRenderVisitor::IsChildOfLeashNode false");
    return false;
}

void RSUniRenderVisitor::DrawBufferOnCanvas(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::DrawBufferOnCanvas canvas is nullptr");
    }

    bool bitmapCreated = false;
    SkBitmap bitmap;
    std::vector<uint8_t> newTmpBuffer;
    auto buffer = node.GetBuffer();
    if (buffer->GetFormat() == PIXEL_FMT_YCRCB_420_SP || buffer->GetFormat() == PIXEL_FMT_YCBCR_420_SP) {
        bitmapCreated = RsRenderServiceUtil::CreateYuvToRGBABitMap(buffer, newTmpBuffer, bitmap);
    } else {
        SkColorType colorType = (buffer->GetFormat() == PIXEL_FMT_BGRA_8888) ?
            kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
        SkImageInfo imageInfo = SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(),
            colorType, kPremul_SkAlphaType);
        auto pixmap = SkPixmap(imageInfo, buffer->GetVirAddr(), buffer->GetStride());
        bitmapCreated = bitmap.installPixels(pixmap);
    }
    if (!bitmapCreated) {
        RS_LOGE("RSUniRenderVisitor::DrawBufferOnCanvas installPixels failed");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setAlphaf(node.GetContextAlpha() * node.GetRenderProperties().GetAlpha());

    canvas_->save();
    const RSProperties& property = node.GetRenderProperties();
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(node.GetRenderProperties().GetBoundsPositionX(),
            node.GetRenderProperties().GetBoundsPositionY(),
            node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr);
    }
    canvas_->drawBitmapRect(bitmap,
        SkRect::MakeXYWH(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight()),
        SkRect::MakeXYWH(node.GetRenderProperties().GetBoundsPositionX(),
            node.GetRenderProperties().GetBoundsPositionY(),
            node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight()), &paint);
    canvas_->restore();
}
} // namespace Rosen
} // namespace OHOS
