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

#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_draw_cmd.h"
#include "recording/cmd_list_helper.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {

ExtendRecordingCanvas::ExtendRecordingCanvas(int32_t width, int32_t height, bool addDrawOpImmediate)
    : Drawing::RecordingCanvas(width, height, addDrawOpImmediate) {}

std::unique_ptr<ExtendRecordingCanvas> ExtendRecordingCanvas::Obtain(int32_t width, int32_t height,
    bool addDrawOpImmediate)
{
    std::unique_ptr<ExtendRecordingCanvas> canvas = nullptr;
    {
        std::lock_guard<std::mutex> lock(canvasMutex_);
        if (canvasPool_.empty()) {
            return std::make_unique<ExtendRecordingCanvas>(width, height, addDrawOpImmediate);
        }
        canvas = std::move(canvasPool_.front());
        canvasPool_.pop();
    }

    if (!canvas) {
        canvas = std::make_unique<ExtendRecordingCanvas>(width, height, addDrawOpImmediate);
    } else {
        canvas->Reset(width, height, addDrawOpImmediate);
    }
    return canvas;
}

void ExtendRecordingCanvas::Recycle(std::unique_ptr<ExtendRecordingCanvas>& canvas)
{
    std::lock_guard<std::mutex> lock(canvasMutex_);
    if (canvasPool_.size() >= MAX_CANVAS_SIZE) {
        return;
    }
    canvasPool_.push(std::move(canvas));
}

void ExtendRecordingCanvas::DrawImageWithParm(
    const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Data>& data,
    const Drawing::AdaptiveImageInfo& rsImageInfo, const Drawing::SamplingOptions& sampling)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<Drawing::DrawImageWithParmOpItem>(image, data, rsImageInfo, sampling);
        return;
    }
    auto object = std::make_shared<RSExtendImageObject>(image, data, rsImageInfo);
    auto drawCallList = Drawing::RecordingCanvas::GetDrawCmdList();
    auto objectHandle =
        Drawing::CmdListHelper::AddImageObjectToCmdList(*drawCallList, object);
    AddDrawOpImmediate<Drawing::DrawImageWithParmOpItem::ConstructorHandle>(objectHandle, sampling);
}

void ExtendRecordingCanvas::DrawPixelMapWithParm(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::AdaptiveImageInfo& rsImageInfo, const Drawing::SamplingOptions& sampling)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<Drawing::DrawPixelMapWithParmOpItem>(pixelMap, rsImageInfo, sampling);
        return;
    }
    auto object = std::make_shared<RSExtendImageObject>(pixelMap, rsImageInfo);
    auto drawCallList = Drawing::RecordingCanvas::GetDrawCmdList();
    auto objectHandle =
        Drawing::CmdListHelper::AddImageObjectToCmdList(*drawCallList, object);
    AddDrawOpImmediate<Drawing::DrawPixelMapWithParmOpItem::ConstructorHandle>(objectHandle, sampling);
}

void ExtendRecordingCanvas::DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& src,
    const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling,
    Drawing::SrcRectConstraint constraint)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<Drawing::DrawPixelMapRectOpItem>(pixelMap, src, dst, sampling);
        return;
    }
    auto object = std::make_shared<RSExtendImageBaseObj>(pixelMap, src, dst);
    auto drawCallList = Drawing::RecordingCanvas::GetDrawCmdList();
    auto objectHandle =
        Drawing::CmdListHelper::AddImageBaseObjToCmdList(*drawCallList, object);
    AddDrawOpImmediate<Drawing::DrawPixelMapRectOpItem::ConstructorHandle>(objectHandle, sampling);
}

void ExtendRecordingCanvas::DrawDrawFunc(Drawing::RecordingCanvas::DrawFunc&& drawFunc)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<Drawing::DrawFuncOpItem>(std::move(drawFunc)));
        return;
    }
    auto object = std::make_shared<RSExtendDrawFuncObj>(std::move(drawFunc));
    auto drawCallList = Drawing::RecordingCanvas::GetDrawCmdList();
    auto objectHandle =
        Drawing::CmdListHelper::AddDrawFuncObjToCmdList(*drawCallList, object);
    cmdList_->AddOp<Drawing::DrawFuncOpItem::ConstructorHandle>(objectHandle);
}

#ifdef ROSEN_OHOS
void ExtendRecordingCanvas::DrawSurfaceBuffer(const DrawingSurfaceBufferInfo& surfaceBufferInfo)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<Drawing::DrawSurfaceBufferOpItem>(surfaceBufferInfo);
        return;
    }
    AddDrawOpImmediate<Drawing::DrawSurfaceBufferOpItem::ConstructorHandle>(
        Drawing::CmdListHelper::AddSurfaceBufferToCmdList(*cmdList_, surfaceBufferInfo.surfaceBuffer_),
        surfaceBufferInfo.offSetX_, surfaceBufferInfo.offSetY_,
        surfaceBufferInfo.width_, surfaceBufferInfo.height_);
}
#endif

template<typename T, typename... Args>
void ExtendRecordingCanvas::AddDrawOpImmediate(Args&&... args)
{
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        Drawing::PaintHandle paintHandle;
        paintHandle.isAntiAlias = true;
        paintHandle.style = Drawing::Paint::PaintStyle::PAINT_FILL;
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
        return;
    }
    if (brushValid && penValid && Drawing::Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        Drawing::PaintHandle paintHandle;
        paintPen_.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL_STROKE);
        Drawing::DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintPen_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
        paintPen_.SetStyle(Drawing::Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        Drawing::PaintHandle paintHandle;
        Drawing::DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintBrush_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
    }
    if (penValid) {
        Drawing::PaintHandle paintHandle;
        Drawing::DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintPen_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
    }
}

template<typename T, typename... Args>
void ExtendRecordingCanvas::AddDrawOpDeferred(Args&&... args)
{
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        Drawing::Paint paint;
        paint.SetAntiAlias(true);
        paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paint));
        return;
    }
    if (brushValid && penValid && Drawing::Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        paintPen_.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL_STROKE);
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintPen_));
        paintPen_.SetStyle(Drawing::Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintBrush_));
    }
    if (penValid) {
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintPen_));
    }
}

void ExtendRecordingCanvas::DrawImageNineWithPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap,
    const Drawing::RectI& center, const Drawing::Rect& dst, Drawing::FilterMode filter, const Drawing::Brush* brush)
{
    auto image = RSPixelMapUtil::ExtractDrawingImage(pixelmap);
    Drawing::RecordingCanvas::DrawImageNine(image.get(), center, dst, filter, brush);
}
} // namespace Rosen
} // namespace OHOS
