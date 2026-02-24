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
#include "modifier_ng/rs_render_modifier_ng.h"
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
constexpr uint32_t DRAWCMDLIST_COUNT_LIMIT = 300; // limit of the drawcmdlists.
constexpr uint32_t DRAWCMDLIST_OPSIZE_TOTAL_COUNT_LIMIT = 10000;
constexpr uint32_t OP_COUNT_LIMIT_PER_FRAME = 10000;
constexpr uint32_t OP_COUNT_LIMIT_FOR_CACHE = 200000;
}
RSCanvasDrawingRenderNode::RSCanvasDrawingRenderNode(
    NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSCanvasRenderNode(id, context, isTextureExportNode)
{
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this) - sizeof(RSCanvasRenderNode));
}

RSCanvasDrawingRenderNode::~RSCanvasDrawingRenderNode()
{
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

    if (IsNeedResetSurface() && !ResetSurface(width, height, canvas)) {
        return;
    }
    if (!surface_) {
        return;
    }

    ModifierNG::RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
    ApplyDrawCmdModifierNG(context, ModifierNG::RSModifierType::CONTENT_STYLE);
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
    if (IsWaitSync() || IsOnTheTree() || isNeverOnTree_ || !stagingRenderParams_ ||
        !RSUniRenderJudgement::IsUniRender() || GetIsTextureExportNode()) {
        return;
    }

    if (!dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE))) {
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

    // only update content_style dirtyType
    auto saveDirtyTypesNG = dirtyTypesNG_;
    dirtyTypesNG_.reset();
    dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
    isPostPlaybacked_ = true;

    UpdateDrawableVecV2();

    if (!IsWaitSync()) {
        RS_LOGE("RSCanvasDrawingRenderNode::ContentStyleSlotUpdate NodeId[%{public}" PRIu64
                "] UpdateDrawableVecV2 failed, dirtySlots empty", GetId());
        return;
    }

    RS_LOGI_LIMIT("RSCanvasDrawingRenderNode::ContentStyleSlotUpdate NodeId[%{public}" PRIu64 "]", GetId());
    RS_OPTIONAL_TRACE_NAME_FMT("canvas drawing node[%llu] ContentStyleSlotUpdate", GetId());

    {
        // clear content_style drawcmdlist
        std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
        auto contentCmdList = drawCmdListsNG_.find(ModifierNG::RSModifierType::CONTENT_STYLE);
        if (contentCmdList != drawCmdListsNG_.end()) {
            contentCmdList->second.clear();
        }
    }
    saveDirtyTypesNG.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), false);
    dirtyTypesNG_ = saveDirtyTypesNG;
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

bool RSCanvasDrawingRenderNode::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    {
        std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
        outOfLimitCmdList_.clear();
        cachedOpCount_ = 0;
    }
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

void RSCanvasDrawingRenderNode::ApplyDrawCmdModifierNG(ModifierNG::RSModifierContext& context,
                                                       ModifierNG::RSModifierType type)
{
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    auto it = drawCmdListsNG_.find(type);
    if (it == drawCmdListsNG_.end() || it->second.empty()) {
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
    return UNI_MAIN_THREAD_INDEX;
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
    if (HasCachedOp()) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: has unused OP, bitmap may be incorrect.");
    }
    return bitmap;
}

bool RSCanvasDrawingRenderNode::GetPixelmap(std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect,
    const uint64_t tid, Drawing::DrawCmdListPtr drawCmdList, Drawing::DrawCmdListPtr lastDrawCmdList)
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

    if (HasCachedOp()) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: has unused OP, pixelMap may be incorrect.");
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
    if ((GetIsTextureExportNode() || !RSUniRenderJudgement::IsUniRender()) && lastDrawCmdList != nullptr &&
        !lastDrawCmdList->IsEmpty()) {
        lastDrawCmdList->Playback(*canvas, rect);
    }
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
    const auto& contentModifiers = GetModifiersNG(ModifierNG::RSModifierType::CONTENT_STYLE);
    if (contentModifiers.empty()) {
        return false;
    }
    for (const auto& modifier : contentModifiers) {
        if (auto cmd = modifier->Getter<Drawing::DrawCmdListPtr>(ModifierNG::RSPropertyType::CONTENT_STYLE, nullptr)) {
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
        HILOG_COMM_ERROR("RSCanvasDrawingRenderNode::InitRenderParams failed");
        return;
    }
#endif
}

CM_INLINE void RSCanvasDrawingRenderNode::ApplyModifiers()
{
    if (HasCachedOp() && !dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE))) {
        dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
        std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
        ApplyCachedCmdList();
        CheckDrawCmdListSizeNG(ModifierNG::RSModifierType::CONTENT_STYLE);
        SetNeedProcess(true);
    }
    RSRenderNode::ApplyModifiers();
}

void RSCanvasDrawingRenderNode::CheckDrawCmdListSizeNG(ModifierNG::RSModifierType type)
{
    auto& drawCmdLists = drawCmdListsNG_[type];
    auto originCmdListSize = drawCmdLists.size();
    if (originCmdListSize == 0) {
        return;
    }
    size_t originOpCount = 0;
    size_t opCount = 0;
    size_t cmdListSize = 0;
    auto it = drawCmdLists.end();
    while (it != drawCmdLists.begin()) {
        it--;
        originOpCount += (*it)->GetOpItemSize();
        if (originOpCount > OP_COUNT_LIMIT_PER_FRAME || cmdListSize >= DRAWCMDLIST_COUNT_LIMIT) {
            it = drawCmdLists.erase(it);
            continue;
        }
        opCount = originOpCount;
        cmdListSize++;
    }
    bool overflow = originOpCount > OP_COUNT_LIMIT_PER_FRAME || originCmdListSize > DRAWCMDLIST_COUNT_LIMIT;
    if (overflow) {
        RS_OPTIONAL_TRACE_NAME_FMT("CheckDrawCmdListSizeNG nodeId:[%" PRIu64 "] modifierType:[%hd] stateOnTheTree:[%d]"
            " originCmdListSize:[%zu] cmdListSize:[%zu] originOpCount:[%zu] opCount:[%zu]", GetId(), type,
            IsOnTheTree(), originCmdListSize, cmdListSize, originOpCount, opCount);
        if (overflow != lastOverflowStatus_) {
            RS_LOGE("CheckDrawCmdListSizeNG: Cmdlist out of limit, nodeId:[%{public}" PRIu64 "] "
                "modifierType:[%{public}hd] stateOnTheTree:[%{public}d] originCmdListSize:[%{public}zu] "
                "cmdListSize:[%{public}zu] originOpCount:[%{public}zu] opCount:[%{public}zu]", GetId(), type,
                IsOnTheTree(), originCmdListSize, cmdListSize, originOpCount, opCount);
        }
    }
    lastOverflowStatus_ = overflow;
}

void RSCanvasDrawingRenderNode::AddDirtyType(ModifierNG::RSModifierType modifierType)
{
    dirtyTypesNG_.set(static_cast<int>(modifierType), true);
    if (modifierType != ModifierNG::RSModifierType::CONTENT_STYLE) {
        return;
    }
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    const auto& contentModifiers = GetModifiersNG(modifierType);
    bool hasContentModifiers = !contentModifiers.empty();
    if (!hasContentModifiers && outOfLimitCmdList_.empty()) {
        return;
    }

    if (hasContentModifiers) {
        ReportOpCount(drawCmdListsNG_[modifierType]);
    }
    size_t opCount = ApplyCachedCmdList();
    for (const auto& modifier : contentModifiers) {
        auto drawCmdList = modifier->Getter<Drawing::DrawCmdListPtr>(
            ModifierNG::ModifierTypeConvertor::GetPropertyType(modifierType), nullptr);
        if (drawCmdList == nullptr) {
            continue;
        }
        auto opItemSize = drawCmdList->GetOpItemSize();
        if (opCount > OP_COUNT_LIMIT_PER_FRAME) {
            outOfLimitCmdList_.emplace_back(drawCmdList);
            cachedOpCount_ += opItemSize;
        } else {
            auto lastOpCount = opCount;
            opCount += opItemSize;
            if (opCount > OP_COUNT_LIMIT_PER_FRAME) {
                SplitDrawCmdList(OP_COUNT_LIMIT_PER_FRAME - lastOpCount, drawCmdList, true);
            } else {
                drawCmdListsNG_[modifierType].emplace_back(drawCmdList);
                opCountAfterReset_ += opItemSize;
            }
        }
    }
    if (opCount > 0) {
        SetNeedProcess(true);
    }
    CheckDrawCmdListSizeNG(modifierType);
}

size_t RSCanvasDrawingRenderNode::ApplyCachedCmdList()
{
    size_t opCount = 0;
    while (!outOfLimitCmdList_.empty() && opCount < OP_COUNT_LIMIT_PER_FRAME) {
        auto drawCmdList = outOfLimitCmdList_.front();
        outOfLimitCmdList_.pop_front();
        auto opItemSize = drawCmdList->GetOpItemSize();
        auto lastOpCount = opCount;
        opCount += opItemSize;
        if (opCount > OP_COUNT_LIMIT_PER_FRAME) {
            SplitDrawCmdList(OP_COUNT_LIMIT_PER_FRAME - lastOpCount, drawCmdList, false);
            break;
        }
        drawCmdListsNG_[ModifierNG::RSModifierType::CONTENT_STYLE].emplace_back(drawCmdList);
        cachedOpCount_ -= opItemSize;
        opCountAfterReset_ += opItemSize;
    }
    return opCount;
}

void RSCanvasDrawingRenderNode::SplitDrawCmdList(
    size_t firstOpCount, Drawing::DrawCmdListPtr drawCmdList, bool splitOrigin)
{
    if (splitOrigin && cachedOpCount_ > OP_COUNT_LIMIT_FOR_CACHE) {
        drawCmdList->ClearOp();
        RS_LOGE("RSCanvasDrawingRenderNode::SplitDrawCmdList: OP count(%{public}zu) out of limit", cachedOpCount_);
        return;
    }
    auto firstCmdList = std::make_shared<Drawing::DrawCmdList>(
        drawCmdList->GetWidth(), drawCmdList->GetHeight(), Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    Drawing::DrawCmdListPtr cmdList = nullptr;
    auto drawOpItems = drawCmdList->GetDrawOpItems();
    auto opCount = drawOpItems.size();
    size_t splitCount = 0;
    for (size_t index = 0; index < opCount; index++) {
        if (index < firstOpCount) {
            firstCmdList->AddDrawOp(std::move(drawOpItems[index]));
            continue;
        }
        if (cmdList == nullptr) {
            cmdList = std::make_shared<Drawing::DrawCmdList>(
                drawCmdList->GetWidth(), drawCmdList->GetHeight(), Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
        }
        cmdList->AddDrawOp(std::move(drawOpItems[index]));
        if (((index - firstOpCount) % OP_COUNT_LIMIT_PER_FRAME == OP_COUNT_LIMIT_PER_FRAME - 1) ||
            index == opCount - 1) {
            if (splitOrigin) {
                outOfLimitCmdList_.emplace_back(cmdList);
                cachedOpCount_ += cmdList->GetOpItemSize();
            } else {
                auto it = outOfLimitCmdList_.begin();
                std::advance(it, splitCount);
                outOfLimitCmdList_.insert(it, cmdList);
                splitCount++;
            }
            cmdList = nullptr;
            if (splitOrigin && cachedOpCount_ > OP_COUNT_LIMIT_FOR_CACHE) {
                RS_LOGE(
                    "RSCanvasDrawingRenderNode::SplitDrawCmdList: OP count(%{public}zu) out of limit", cachedOpCount_);
                break;
            }
        }
    }
    if (!firstCmdList->IsEmpty()) {
        if (!splitOrigin) {
            cachedOpCount_ -= firstCmdList->GetOpItemSize();
        }
        drawCmdListsNG_[ModifierNG::RSModifierType::CONTENT_STYLE].emplace_back(firstCmdList);
        opCountAfterReset_ += firstCmdList->GetOpItemSize();
    }
    drawCmdList->ClearOp();
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
    drawCmdListsNG_.clear();
}

void RSCanvasDrawingRenderNode::ResetSurface(int width, int height, uint32_t resetSurfaceIndex)
{
    {
        std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
        outOfLimitCmdList_.clear();
        cachedOpCount_ = 0;
    }
    std::lock_guard<std::mutex> lockTask(taskMutex_);
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
    stagingRenderParams_->SetCanvasDrawingResetSurfaceIndex(resetSurfaceIndex);
    lastResetSurfaceTime_ = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    opCountAfterReset_ = 0;
}

const std::map<ModifierNG::RSModifierType, ModifierCmdList>& RSCanvasDrawingRenderNode::GetDrawCmdListsNG() const
{
    return drawCmdListsNG_;
}

void RSCanvasDrawingRenderNode::OnApplyModifiers()
{
    modifiersApplied_ = true;
}

void RSCanvasDrawingRenderNode::OnSync()
{
    RSRenderNode::OnSync();
    if (modifiersApplied_) {
        modifiersApplied_ = false;
        ClearResource();
    }
}

void RSCanvasDrawingRenderNode::AccumulateLastDirtyTypes()
{
    if (GetLastFrameSync() || !RSUniRenderJudgement::IsUniRender() || GetIsTextureExportNode()) {
        return;
    }
    dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
}

void RSCanvasDrawingRenderNode::ClearResource()
{
    if (RSUniRenderJudgement::IsUniRender() && !GetIsTextureExportNode()) {
        std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
        drawCmdListsNG_.clear();
    }
}

void RSCanvasDrawingRenderNode::ClearNeverOnTree()
{
    isNeverOnTree_ = false;
}

void RSCanvasDrawingRenderNode::CheckCanvasDrawingPostPlaybacked()
{
    if (!isPostPlaybacked_) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("canvas drawing node [%" PRIu64 "] CheckCanvasDrawingPostPlaybacked", GetId());
    // add empty drawop, only used in unirender mode
    dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
    auto contentCmdList = drawCmdListsNG_.find(ModifierNG::RSModifierType::CONTENT_STYLE);
    if (contentCmdList != drawCmdListsNG_.end()) {
        auto cmd = std::make_shared<Drawing::DrawCmdList>();
        contentCmdList->second.emplace_back(cmd);
    }
    isPostPlaybacked_ = false;
}

bool RSCanvasDrawingRenderNode::GetIsPostPlaybacked()
{
    return isPostPlaybacked_;
}

bool RSCanvasDrawingRenderNode::CheckCachedOp()
{
    if (!HasCachedOp()) {
        return false;
    }
    SetDirty(true);
    if (!IsOnTheTree()) {
        std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
        ApplyCachedCmdList();
        CheckDrawCmdListSizeNG(ModifierNG::RSModifierType::CONTENT_STYLE);
        dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
    }
    return true;
}

bool RSCanvasDrawingRenderNode::HasCachedOp() const
{
    return cachedOpCount_ > 0;
}
} // namespace Rosen
} // namespace OHOS