/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "feature/capture/rs_surface_capture_task.h"

#include <memory>
#include <sys/mman.h>

#include "rs_trace.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "draw/color.h"
#include "draw/surface.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_composer_adapter.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/render_thread/rs_divided_render_util.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"
namespace OHOS {
namespace Rosen {
bool RSSurfaceCaptureTask::Run(sptr<RSISurfaceCaptureCallback> callback)
{
    if (ROSEN_EQ(captureConfig_.scaleX, 0.f) || ROSEN_EQ(captureConfig_.scaleY, 0.f) ||
        captureConfig_.scaleX < 0.f || captureConfig_.scaleY < 0.f) {
        RS_LOGE("RSSurfaceCaptureTask::Run: SurfaceCapture scale is invalid.");
        return false;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: node is nullptr");
        return false;
    }
    std::unique_ptr<Media::PixelMap> pixelmap;
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(captureConfig_, RSUniRenderJudgement::IsUniRender());
    std::string nodeName("RSSurfaceCaptureTask");
    std::shared_ptr<Drawing::ColorSpace> colorSpace = nullptr;
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        pixelmap = CreatePixelMapBySurfaceNode(surfaceNode, visitor_->IsUniRender());
        visitor_->IsDisplayNode(false);
        nodeName = surfaceNode->GetName();
    } else if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        GraphicColorGamut colorGamut = displayNode->GetColorSpace();
        if (colorGamut != GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB) {
            colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(colorGamut);
        }
        pixelmap = CreatePixelMapByDisplayNode(displayNode, visitor_->IsUniRender());
        visitor_->IsDisplayNode(true);
    } else {
        RS_LOGE("RSSurfaceCaptureTask::Run: Invalid RSRenderNodeType!");
        return false;
    }
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: pixelmap == nullptr!");
        return false;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
    auto grContext = renderContext != nullptr ? renderContext->GetSharedDrGPUContext() : nullptr;
    RSTagTracker tagTracker(grContext, node->GetId(), RSTagTracker::TAGTYPE::TAG_CAPTURE, nodeName);
#endif
    auto surface = CreateSurface(pixelmap, colorSpace);
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: surface is nullptr!");
        return false;
    }
    visitor_->SetSurface(surface.get());
    node->Process(visitor_);
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    std::shared_ptr<Drawing::Image> img(surface.get()->GetImageSnapshot());
    if (!img) {
        RS_LOGE("RSSurfaceCaptureTask::Run: img is nullptr");
        return false;
    }
    if (!CopyDataToPixelMap(img, pixelmap,
        captureConfig_, UniRenderEnabledType::UNI_RENDER_DISABLED, colorSpace)) {
            RS_LOGE("RSSurfaceCaptureTask::Run: CopyDataToPixelMap failed");
            return false;
    }
#endif
    // To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
    RSBaseRenderUtil::WritePixelMapToPng(*pixelmap);
    if (callback) {
        callback->OnSurfaceCapture(nodeId_, captureConfig_, pixelmap.get());
    }
    return true;
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode(
    std::shared_ptr<RSSurfaceRenderNode> node, bool isUniRender)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: node == nullptr");
        return nullptr;
    }
    if (!isUniRender && node->GetRSSurfaceHandler()->GetBuffer() == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: node GetBuffer == nullptr");
        return nullptr;
    }
    const auto& property = node->GetRenderProperties();
    int pixmapWidth = property.GetBoundsWidth();
    int pixmapHeight = property.GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * captureConfig_.scaleX);
    opts.size.height = ceil(pixmapHeight * captureConfig_.scaleY);
    RS_LOGD("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap width is [%{public}u], height is [%{public}u],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleY:[%{public}f], scaleY:[%{public}f]",
        node->GetId(), pixmapWidth, pixmapHeight, opts.size.width, opts.size.height,
        captureConfig_.scaleX, captureConfig_.scaleY);
    return Media::PixelMap::Create(opts);
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapByDisplayNode(
    std::shared_ptr<RSDisplayRenderNode> node, bool isUniRender)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: node is nullptr");
        return nullptr;
    }
    uint64_t screenId = node->GetScreenId();
    RSScreenModeInfo screenModeInfo;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: screenManager is nullptr!");
        return nullptr;
    }
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    uint32_t pixmapWidth = screenInfo.width;
    uint32_t pixmapHeight = screenInfo.height;
    if (!isUniRender) {
        auto rotation = node->GetRotation();
        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(pixmapWidth, pixmapHeight);
        }
    }
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * captureConfig_.scaleX);
    opts.size.height = ceil(pixmapHeight * captureConfig_.scaleY);
    RS_LOGI("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap width is [%{public}u], height is [%{public}u],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleY:[%{public}f], scaleY:[%{public}f],",
        node->GetId(), pixmapWidth, pixmapHeight, opts.size.width, opts.size.height,
        captureConfig_.scaleX, captureConfig_.scaleY);
    return Media::PixelMap::Create(opts);
}

bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img, const std::unique_ptr<Media::PixelMap>& pixelmap,
    const RSSurfaceCaptureConfig& captureConfig, const UniRenderEnabledType& uniRenderEnabledType,
    std::shared_ptr<Drawing::ColorSpace> colorSpace)
{
    auto captureType = captureConfig.captureType;
    // captureConfig.usedDma = ture, Entering This process is an abnormal situation(maybe dma-ccm not configed)
    // The memory needs to be reassigned
    bool isUsedClientPixelMap = (captureConfig.isClientPixelMap && !captureConfig.useDma);
    if (!isUsedClientPixelMap &&
        !RSCapturePixelMapManager::SetCapturePixelMapMem(pixelmap, captureConfig.captureType,
        uniRenderEnabledType, false)) {
        RS_LOGE("CopyDataToPixelMap::SetCapturePixelMapMem Fail");
        return false;
    }
    if (!RSCapturePixelMapManager::CopyDataToPixelMap(img, pixelmap, colorSpace)) {
        RS_LOGE("CopyDataToPixelMap::CopyData Fail");
        return false;
    }
#ifndef ROSEN_OHOS
    if (colorSpace != nullptr) {
        pixelmap->InnerSetColorSpace(colorSpace->IsSRGB()?
            OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::SRGB):
            OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::DISPLAY_P3));
    }
#endif
    return true;
}

std::shared_ptr<Drawing::Surface> RSSurfaceCaptureTask::CreateSurface(
    const std::unique_ptr<Media::PixelMap>& pixelmap, std::shared_ptr<Drawing::ColorSpace> colorSpace)
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: address == nullptr");
        return nullptr;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo{pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL, colorSpace};

#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
        if (renderContext == nullptr) {
            RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
            return nullptr;
        }
        renderContext->SetUpGpuContext(nullptr);
        return Drawing::Surface::MakeRenderTarget(renderContext->GetDrGPUContext(), false, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        return Drawing::Surface::MakeRenderTarget(
            RSMainThread::Instance()->GetRenderEngine()->GetSkContext().get(), false, info);
    }
#endif
    return Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

RSSurfaceCaptureVisitor::RSSurfaceCaptureVisitor(const RSSurfaceCaptureConfig& captureConfig, bool isUniRender)
    : captureConfig_(captureConfig), isUniRender_(isUniRender)
{
    renderEngine_ = RSMainThread::Instance()->GetRenderEngine();
}

void RSSurfaceCaptureVisitor::SetSurface(Drawing::Surface* surface)
{
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::SetSurface: surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->Scale(captureConfig_.scaleX, captureConfig_.scaleY);
    canvas_->SetDisableFilterCache(true);
}

void RSSurfaceCaptureVisitor::ProcessChildren(RSRenderNode &node)
{
    for (auto& child : *node.GetSortedChildren()) {
        if (!child) {
            continue;
        }
        child->Process(shared_from_this());
    }
}

void RSSurfaceCaptureVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode:" +
        std::to_string(node.GetId()));
    RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode child size:[%{public}d] total",
        node.GetChildrenCount());

    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: Canvas is null!");
        return;
    }

    ProcessChildren(node);
}

void RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni(RSSurfaceRenderNode& node)
{
    Drawing::Matrix translateMatrix;
    auto parentPtr = node.GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        // calculate the offset from this node's parent, and perform translate.
        auto parentNode = std::static_pointer_cast<RSSurfaceRenderNode>(parentPtr);
        const float parentNodeTranslateX = parentNode->GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X);
        const float parentNodeTranslateY = parentNode->GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y);
        const float thisNodetranslateX = node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X);
        const float thisNodetranslateY = node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y);
        translateMatrix.PreTranslate(
            thisNodetranslateX - parentNodeTranslateX, thisNodetranslateY - parentNodeTranslateY);
    }
    if (node.GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY) ||
        node.GetSpecialLayerMgr().Find(SpecialLayerType::SKIP)) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
        canvas_->ConcatMatrix(translateMatrix);
        canvas_->Clear(Drawing::Color::COLOR_WHITE);
        return;
    }

    if (node.GetChildrenCount() > 0) {
        canvas_->ConcatMatrix(translateMatrix);
        const auto saveCnt = canvas_->Save();
        ProcessChildren(node);
        canvas_->RestoreToCount(saveCnt);
        if (node.GetRSSurfaceHandler()->GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    } else {
        Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
        canvas_->ConcatMatrix(translateMatrix);
        if (node.GetRSSurfaceHandler()->GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    }
}

void RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni(RSSurfaceRenderNode& node)
{
    if (node.GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY) ||
        node.GetSpecialLayerMgr().Find(SpecialLayerType::SKIP)) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }
    ProcessChildren(node);
    if (node.GetRSSurfaceHandler()->GetBuffer() != nullptr) {
        // in display's coordinate.
        auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, false, false, false, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
}

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithoutUni(RSSurfaceRenderNode& node)
{
    if (isDisplayNode_) {
        CaptureSurfaceInDisplayWithoutUni(node);
    } else {
        CaptureSingleSurfaceNodeWithoutUni(node);
    }
}

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::Process:" + node.GetName());

    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode node: %{public}" PRIu64 " invisible", node.GetId());
        return;
    }

    ProcessSurfaceRenderNodeWithoutUni(node);
}
} // namespace Rosen
} // namespace OHOS
