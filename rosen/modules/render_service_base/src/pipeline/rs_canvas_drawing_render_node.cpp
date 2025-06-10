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

#include "pipeline/rs_canvas_drawing_render_node.h"

#include <memory>

#include "include/core/SkCanvas.h"
#include "src/image/SkImage_Base.h"
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#else
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#endif

#include "rs_trace.h"

#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "params/rs_canvas_drawing_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
static std::mutex drawingMutex_;
namespace {
constexpr uint32_t DRAWCMDLIST_COUNT_LIMIT = 300;
constexpr uint32_t DRAWCMDLIST_OPSIZE_TOTAL_COUNT_LIMIT = 10000;
}
RSCanvasDrawingRenderNode::RSCanvasDrawingRenderNode(
    NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSCanvasRenderNode(id, context, isTextureExportNode)
{
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this) - sizeof(RSCanvasRenderNode));
}

RSCanvasDrawingRenderNode::~RSCanvasDrawingRenderNode()
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
#endif
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this) - sizeof(RSCanvasRenderNode));
}

void RSCanvasDrawingRenderNode::InitRenderContent()
{
    drawingNodeRenderID = UNI_RENDER_THREAD_INDEX;
}

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
bool RSCanvasDrawingRenderNode::ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas)
{
    if (canvas_ == nullptr) {
        RS_LOGE("canvas_ is nullptr");
        return false;
    }
    auto preMatrix = canvas_->GetTotalMatrix();
    auto preSurface = surface_;
    if (!ResetSurface(width, height, canvas)) {
        return false;
    }
    auto image = preSurface->GetImageSnapshot();
    if (!image) {
        return false;
    }
    Drawing::TextureOrigin origin = RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL
        ? Drawing::TextureOrigin::BOTTOM_LEFT : Drawing::TextureOrigin::TOP_LEFT;
    auto sharedBackendTexture = image->GetBackendTexture(false, &origin);
    if (!sharedBackendTexture.IsValid()) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedBackendTexture is nullptr");
        return false;
    }
    Drawing::BitmapFormat bitmapFormat = { image->GetColorType(), image->GetAlphaType() };
    auto sharedTexture = std::make_shared<Drawing::Image>();
    auto context = canvas.GetGPUContext();
    if (!context || !sharedTexture->BuildFromTexture(*context, sharedBackendTexture.GetTextureInfo(),
        origin, bitmapFormat, nullptr, SKResourceManager::DeleteSharedTextureContext,
        new SharedTextureContext(image))) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture is nullptr");
        return false;
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (sharedTexture->IsTextureBacked()) {
            RS_LOGI("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture from texture to raster image");
            sharedTexture = sharedTexture->MakeRasterImage();
            if (!sharedTexture) {
                RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture: MakeRasterImage failed");
                return false;
            }
        }
    }
    canvas_->DrawImage(*sharedTexture, 0.f, 0.f, Drawing::SamplingOptions());
    if (preThreadInfo_.second && preSurface) {
        preThreadInfo_.second(std::move(preSurface));
    }
    preThreadInfo_ = curThreadInfo_;
    canvas_->SetMatrix(preMatrix);
    canvas_->Flush();
    return true;
}
#endif

void RSCanvasDrawingRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    int width = 0;
    int height = 0;
    std::lock_guard<std::mutex> lockTask(taskMutex_);
    if (!GetSizeFromDrawCmdModifiers(width, height)) {
        return;
    }

    if (IsNeedResetSurface()) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        if (preThreadInfo_.second && surface_) {
            preThreadInfo_.second(std::move(surface_));
        }
        preThreadInfo_ = curThreadInfo_;
#endif
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    } else if ((isGpuSurface_) && (preThreadInfo_.first != curThreadInfo_.first)) {
        if (!ResetSurfaceWithTexture(width, height, canvas)) {
            return;
        }
    }
#else
    }
#endif
    if (!surface_) {
        return;
    }

    RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
    isNeedProcess_ = false;

    Rosen::Drawing::Matrix mat;
    if (RSPropertiesPainter::GetGravityMatrix(
            GetRenderProperties().GetFrameGravity(), GetRenderProperties().GetFrameRect(), width, height, mat)) {
        canvas.ConcatMatrix(mat);
    }
    if (!recordingCanvas_) {
        image_ = surface_->GetImageSnapshot();
        if (image_) {
            SKResourceManager::Instance().HoldResource(image_);
        }
    } else {
        auto cmds = recordingCanvas_->GetDrawCmdList();
        if (cmds && !cmds->IsEmpty()) {
            recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
        }
    }
    std::lock_guard<std::mutex> lock(imageMutex_);
    if (!image_) {
        return;
    }
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Paint paint;
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
    canvas.AttachPaint(paint);
    if (canvas.GetRecordingState()) {
        auto cpuImage = image_->MakeRasterImage();
        if (!cpuImage) {
            RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents: MakeRasterImage failed");
            canvas.DetachPaint();
            return;
        }
        canvas.DrawImage(*cpuImage, 0.f, 0.f, samplingOptions);
    } else {
        canvas.DrawImage(*image_, 0.f, 0.f, samplingOptions);
    }
    canvas.DetachPaint();
}

bool RSCanvasDrawingRenderNode::IsNeedProcess() const
{
    if (!renderDrawable_) {
        return false;
    }
    return renderDrawable_->IsNeedDraw();
}

void RSCanvasDrawingRenderNode::ContentStyleSlotUpdate()
{
    // update content_style when node not on tree, need check (waitSync_ false, not on tree, never on tree
    // not texture exportnode, unirender mode)
    // if canvas drawing node never on tree, should not update, it will lost renderParams->localDrawRect_
#ifdef RS_ENABLE_GPU
    if (IsWaitSync() || IsOnTheTree() || isNeverOnTree_ || !stagingRenderParams_ ||
        !RSUniRenderJudgement::IsUniRender() || GetIsTextureExportNode()) {
        return;
    }
#else
    if (IsWaitSync() || IsOnTheTree() || isNeverOnTree_ || !stagingRenderParams_ ||
        !RSUniRenderJudgement::IsUniRender() || GetIsTextureExportNode()) {
        return;
    }
#endif

    if (!dirtyTypes_.test(static_cast<size_t>(RSModifierType::CONTENT_STYLE))) {
        return;
    }
#ifdef RS_ENABLE_GPU
    auto surfaceParams = GetStagingRenderParams()->GetCanvasDrawingSurfaceParams();
    if (surfaceParams.width == 0 || surfaceParams.height == 0) {
        RS_LOGI_LIMIT("RSCanvasDrawingRenderNode::ContentStyleSlotUpdate Area Size Error, NodeId[%{public}" PRIu64 "]"
            "width[%{public}d], height[%{public}d]", GetId(), surfaceParams.width, surfaceParams.height);
        return;
    }
#endif

    //only update content_style dirtyType
    auto savedirtyTypes = dirtyTypes_;
    dirtyTypes_.reset();
    dirtyTypes_.set(static_cast<int>(RSModifierType::CONTENT_STYLE), true);
    isPostPlaybacked_ = true;

    UpdateDrawableVecV2();

    if (!IsWaitSync()) {
        RS_LOGE("RSCanvasDrawingRenderNode::ContentStyleSlotUpdate NodeId[%{public}" PRIu64
                "] UpdateDrawableVecV2 failed, dirtySlots empty", GetId());
        return;
    }

    RS_LOGI_LIMIT("RSCanvasDrawingRenderNode::ContentStyleSlotUpdate NodeId[%{public}" PRIu64 "]", GetId());
    RS_OPTIONAL_TRACE_NAME_FMT("canvas drawing node[%llu] ContentStyleSlotUpdate", GetId());

    // clear content_style drawcmdlist
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    auto contentCmdList = drawCmdLists_.find(RSModifierType::CONTENT_STYLE);
    if (contentCmdList != drawCmdLists_.end()) {
        contentCmdList->second.clear();
    }
    savedirtyTypes.set(static_cast<int>(RSModifierType::CONTENT_STYLE), false);
    dirtyTypes_ = savedirtyTypes;

    AddToPendingSyncList();
}

void RSCanvasDrawingRenderNode::SetNeedProcess(bool needProcess)
{
    if (!stagingRenderParams_) {
        return;
    }
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetNeedSync(needProcess);
    isNeedProcess_ = needProcess;
#else
    isNeedProcess_ = false;
#endif
}

void RSCanvasDrawingRenderNode::PlaybackInCorrespondThread()
{
    auto nodeId = GetId();
    auto ctx = GetContext().lock();
    if (ctx == nullptr) {
        return;
    }
    auto task = [nodeId, ctx, this]() {
        std::lock_guard<std::mutex> lockTask(taskMutex_);
        auto node = ctx->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId);
        if (!node || !surface_ || !canvas_) {
            return;
        }
        RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
        ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
        isNeedProcess_ = false;
    };
    RSTaskDispatcher::GetInstance().PostTask(threadId_, task, false);
}

bool RSCanvasDrawingRenderNode::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#if (defined(ROSEN_IOS))
    surface_ = Drawing::Surface::MakeRaster(info);
#else
    auto gpuContext = canvas.GetGPUContext();
    isGpuSurface_ = true;
    if (gpuContext == nullptr) {
        RS_LOGD("RSCanvasDrawingRenderNode::ResetSurface: gpuContext is nullptr");
        isGpuSurface_ = false;
        surface_ = Drawing::Surface::MakeRaster(info);
    } else {
        surface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
        if (!surface_) {
            isGpuSurface_ = false;
            surface_ = Drawing::Surface::MakeRaster(info);
            RS_LOGW("RSCanvasDrawingRenderNode::ResetSurface [%{public}d, %{public}d] %{public}" PRIu64 "",
                width, height, GetId());
            if (!surface_) {
                RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface surface is nullptr");
                return false;
            }
            recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
            return true;
        }
    }
#endif
#else
    surface_ = Drawing::Surface::MakeRaster(info);
#endif
    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface surface is nullptr");
        return false;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface_.get());
    return true;
}

void RSCanvasDrawingRenderNode::ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type)
{
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    auto it = drawCmdLists_.find(type);
    if (it == drawCmdLists_.end() || it->second.empty()) {
        return;
    }
    for (const auto& drawCmdList : it->second) {
        drawCmdList->Playback(*context.canvas_);
        drawCmdList->ClearOp();
    }
    context.canvas_->Flush();
    it->second.clear();
}

uint32_t RSCanvasDrawingRenderNode::GetTid() const
{
    if (UNI_RENDER_THREAD_INDEX == drawingNodeRenderID) {
        return drawingNodeRenderID;
    }
    return curThreadInfo_.first;
}

Drawing::Bitmap RSCanvasDrawingRenderNode::GetBitmap()
{
    return GetBitmap(UNI_MAIN_THREAD_INDEX);
}

bool WriteSkImageToPixelmap(std::shared_ptr<Drawing::Image> image, Drawing::ImageInfo info,
    std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect)
{
    return image->ReadPixels(
        info, pixelmap->GetWritablePixels(), pixelmap->GetRowStride(), rect->GetLeft(), rect->GetTop());
}

std::shared_ptr<Drawing::Image> RSCanvasDrawingRenderNode::GetImage(const uint64_t tid)
{
    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetImage: image_ used by multi threads");
        return nullptr;
    }
    if (!image_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetImage: image_ is nullptr");
    }
    return image_;
}

Drawing::Bitmap RSCanvasDrawingRenderNode::GetBitmap(const uint64_t tid)
{
    Drawing::Bitmap bitmap;
    std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!image_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: image_ is nullptr");
        return bitmap;
    }
    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: image_ used by multi threads");
        return bitmap;
    }
    if (!image_->AsLegacyBitmap(bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: asLegacyBitmap failed");
    }
    return bitmap;
}

bool RSCanvasDrawingRenderNode::GetPixelmap(std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect,
    const uint64_t tid, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!pixelmap || !rect) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: pixelmap is nullptr");
        return false;
    }

    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: surface is nullptr");
        return false;
    }

    auto image = surface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: GetImageSnapshot failed");
        return false;
    }

    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: surface used by multi threads");
        return false;
    }

    Drawing::ImageInfo info = Drawing::ImageInfo { pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    if (!drawCmdList) {
        if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
            RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
            return false;
        }
        return true;
    }
    std::shared_ptr<Drawing::Surface> surface;
    std::unique_ptr<RSPaintFilterCanvas> canvas;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!canvas_) {
        return false;
    }
    auto gpuContext = canvas_->GetGPUContext();
    if (gpuContext == nullptr) {
        if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
            RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
        }
        return false;
    } else {
        Drawing::ImageInfo newInfo = Drawing::ImageInfo{ image->GetWidth(), image->GetHeight(),
            Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
        surface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, newInfo);
        if (!surface) {
            if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
                RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
            }
            return false;
        }
        canvas = std::make_unique<RSPaintFilterCanvas>(surface.get());
    }
#else
    if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
    }
    return false;
#endif
    canvas->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    drawCmdList->Playback(*canvas, rect);
    auto pixelmapImage = surface->GetImageSnapshot();
    if (!pixelmapImage) {
        return false;
    }
    if (!WriteSkImageToPixelmap(pixelmapImage, info, pixelmap, rect)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
        return false;
    }
    return true;
}

bool RSCanvasDrawingRenderNode::GetSizeFromDrawCmdModifiers(int& width, int& height)
{
    auto it = GetDrawCmdModifiers().find(RSModifierType::CONTENT_STYLE);
    if (it == GetDrawCmdModifiers().end() || it->second.empty()) {
        return false;
    }
    for (const auto& modifier : it->second) {
        auto prop = modifier->GetProperty();
        if (auto cmd = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop)->Get()) {
            width = std::max(width, cmd->GetWidth());
            height = std::max(height, cmd->GetHeight());
        }
    }
    if (width <= 0 || height <= 0) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetSizeFromDrawCmdModifiers: The width or height of the canvas is less "
                "than or equal to 0");
        return false;
    }
    return true;
}

bool RSCanvasDrawingRenderNode::IsNeedResetSurface() const
{
    if (!surface_ || !surface_->GetCanvas()) {
        return true;
    }
    return false;
}

void RSCanvasDrawingRenderNode::InitRenderParams()
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::InitRenderParams failed");
        return;
    }
#endif
}

void RSCanvasDrawingRenderNode::CheckDrawCmdListSize(RSModifierType type, size_t originCmdListSize)
{
    bool overflow = drawCmdLists_[type].size() > DRAWCMDLIST_COUNT_LIMIT;
    if (overflow) {
        RS_OPTIONAL_TRACE_NAME_FMT("AddDitryType id:[%llu] StateOnTheTree[%d] ModifierType[%d] ModifierCmdSize[%d]"
            "originCmdListSize[%d], CmdCount[%d]", GetId(), IsOnTheTree(), type, drawCmdLists_[type].size(),
            originCmdListSize, cmdCount_);
        if (overflow != lastOverflowStatus_) {
            RS_LOGE("AddDirtyType Out of Cmdlist Limit, This Node[%{public}" PRIu64 "] with Modifier[%{public}hd]"
                    " have drawcmdlist:%{public}zu, StateOnTheTree[%{public}d], originCmdListSize[%{public}zu],"
                    " CmdCount:[%{public}d]", GetId(), type, drawCmdLists_[type].size(), IsOnTheTree(),
                    originCmdListSize, cmdCount_);
        }
        // If such nodes are not drawn, The drawcmdlists don't clearOp during recording, As a result, there are
        // too many drawOp, so we need to add the limit of drawcmdlists.
        while ((GetOldDirtyInSurface().IsEmpty() || !IsDirty() || renderDrawable_) &&
                drawCmdLists_[type].size() > DRAWCMDLIST_COUNT_LIMIT) {
                drawCmdLists_[type].pop_front();
        }
        if (drawCmdLists_[type].size() > DRAWCMDLIST_COUNT_LIMIT) {
            RS_LOGE("AddDirtyType Cmdlist Protect Error, This Node[%{public}" PRIu64 "] with Modifier[%{public}hd]"
                    " have drawcmdlist:%{public}zu, StateOnTheTree[%{public}d], originCmdListSize[%{public}zu],"
                    " CmdCount:[%{public}d]", GetId(), type, drawCmdLists_[type].size(), IsOnTheTree(),
                    originCmdListSize, cmdCount_);
        }
    }
    lastOverflowStatus_ = overflow;
}

void RSCanvasDrawingRenderNode::AddDirtyType(RSModifierType modifierType)
{
    dirtyTypes_.set(static_cast<int>(modifierType), true);
    if (modifierType != RSModifierType::CONTENT_STYLE) {
        return;
    }
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    const auto itr = GetDrawCmdModifiers().find(modifierType);
    if (itr == GetDrawCmdModifiers().end()) {
        return;
    }

    size_t originCmdListSize = drawCmdLists_[modifierType].size();
    ReportOpCount(drawCmdLists_[modifierType]);
    for (const auto& modifier : itr->second) {
        if (modifier == nullptr) {
            continue;
        }
        auto prop = modifier->GetProperty();
        if (prop == nullptr) {
            continue;
        }
        auto cmd = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop)->Get();
        if (cmd == nullptr) {
            continue;
        }

        cmd->SetCanvasDrawingOpLimitEnable(true);
        drawCmdLists_[modifierType].emplace_back(cmd);
        ++cmdCount_;
        SetNeedProcess(true);
    }
    CheckDrawCmdListSize(modifierType, originCmdListSize);
}

void RSCanvasDrawingRenderNode::ReportOpCount(const std::list<Drawing::DrawCmdListPtr>& cmdLists) const
{
    size_t totalOpCount = 0;
    for (const auto& cmdList : cmdLists) {
        if (cmdList != nullptr) {
            totalOpCount += cmdList->GetOpItemSize();
        }
    }
    if (totalOpCount > DRAWCMDLIST_OPSIZE_TOTAL_COUNT_LIMIT) {
        RS_LOGI_LIMIT("CanvasDrawingNode OpCount oversize, NodeId[%{public}" PRIu64 "] totalOpCount[%{public}zu]",
            GetId(), totalOpCount);
    }
}

void RSCanvasDrawingRenderNode::ClearOp()
{
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    drawCmdLists_.clear();
}

void RSCanvasDrawingRenderNode::ResetSurface(int width, int height)
{
    std::lock_guard<std::mutex> lockTask(taskMutex_);
    cmdCount_ = 0;
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
    surface_ = nullptr;
    recordingCanvas_ = nullptr;
    GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
#ifndef ROSEN_CROSS_PLATFORM
    auto appSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetInstanceRootNode());
    if (appSurfaceNode) {
        colorSpace = appSurfaceNode->GetColorSpace();
    }
#endif
#ifdef RS_ENABLE_GPU
    stagingRenderParams_->SetCanvasDrawingSurfaceChanged(true);
    stagingRenderParams_->SetCanvasDrawingSurfaceParams(width, height, colorSpace);
#endif
}

const std::map<RSModifierType, std::list<Drawing::DrawCmdListPtr>>& RSCanvasDrawingRenderNode::GetDrawCmdLists() const
{
    return drawCmdLists_;
}

void RSCanvasDrawingRenderNode::ClearResource()
{
    if (RSUniRenderJudgement::IsUniRender() && !GetIsTextureExportNode()) {
        std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
        drawCmdLists_.clear();
    }
}

void RSCanvasDrawingRenderNode::ClearNeverOnTree()
{
    isNeverOnTree_ = false;
}

void RSCanvasDrawingRenderNode::CheckCanvasDrawingPostPlaybacked()
{
    if (isPostPlaybacked_) {
        RS_OPTIONAL_TRACE_NAME_FMT("canvas drawing node [%" PRIu64 "] CheckCanvasDrawingPostPlaybacked", GetId());
        // add empty drawop, only used in unirender mode
        dirtyTypes_.set(static_cast<int>(RSModifierType::CONTENT_STYLE), true);
        auto contentCmdList = drawCmdLists_.find(RSModifierType::CONTENT_STYLE);
        if (contentCmdList != drawCmdLists_.end()) {
            auto cmd = std::make_shared<Drawing::DrawCmdList>();
            contentCmdList->second.emplace_back(cmd);
        }
        isPostPlaybacked_ = false;
    }
}

bool RSCanvasDrawingRenderNode::GetIsPostPlaybacked()
{
    return isPostPlaybacked_;
}

} // namespace Rosen
} // namespace OHOS