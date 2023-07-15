/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "recording/recording_canvas.h"

#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"
#include "recording/recording_path.h"
#include "recording/recording_color_filter.h"
#include "recording/recording_color_space.h"
#include "recording/recording_image_filter.h"
#include "recording/recording_mask_filter.h"
#include "recording/recording_path_effect.h"
#include "recording/recording_shader_effect.h"
#include "draw/color.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/filter.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingCanvas::RecordingCanvas(int width, int height) : Canvas(width, height)
{
    cmdList_ = std::make_shared<DrawCmdList>(width, height);
}

std::shared_ptr<DrawCmdList> RecordingCanvas::GetDrawCmdList() const
{
    return cmdList_;
}

void RecordingCanvas::DrawPoint(const Point& point)
{
    cmdList_->AddOp<DrawPointOpItem>(point);
}

void RecordingCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    cmdList_->AddOp<DrawLineOpItem>(startPt, endPt);
}

void RecordingCanvas::DrawRect(const Rect& rect)
{
    cmdList_->AddOp<DrawRectOpItem>(rect);
}

void RecordingCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    cmdList_->AddOp<DrawRoundRectOpItem>(roundRect);
}

void RecordingCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    cmdList_->AddOp<DrawNestedRoundRectOpItem>(outer, inner);
}

void RecordingCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    cmdList_->AddOp<DrawArcOpItem>(oval, startAngle, sweepAngle);
}

void RecordingCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    cmdList_->AddOp<DrawPieOpItem>(oval, startAngle, sweepAngle);
}

void RecordingCanvas::DrawOval(const Rect& oval)
{
    cmdList_->AddOp<DrawOvalOpItem>(oval);
}

void RecordingCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    cmdList_->AddOp<DrawCircleOpItem>(centerPt, radius);
}

void RecordingCanvas::DrawPath(const Path& path)
{
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, path);
    cmdList_->AddOp<DrawPathOpItem>(pathHandle);
}

void RecordingCanvas::DrawBackground(const Brush& brush)
{
    Filter filter = brush.GetFilter();
    BrushHandle brushHandle = {
        CmdListHelper::AddRecordedToCmdList<RecordingColorSpace>(*cmdList_, brush.GetColorSpace()),
        CmdListHelper::AddRecordedToCmdList<RecordingShaderEffect>(*cmdList_, brush.GetShaderEffect()),
        CmdListHelper::AddRecordedToCmdList<RecordingColorFilter>(*cmdList_, filter.GetColorFilter()),
        CmdListHelper::AddRecordedToCmdList<RecordingImageFilter>(*cmdList_, filter.GetImageFilter()),
        CmdListHelper::AddRecordedToCmdList<RecordingMaskFilter>(*cmdList_, filter.GetMaskFilter()),
    };
    cmdList_->AddOp<DrawBackgroundOpItem>(brush.GetColor(), brush.GetBlendMode(), brush.IsAntiAlias(),
        filter.GetFilterQuality(), brushHandle);
}

void RecordingCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, path);
    cmdList_->AddOp<DrawShadowOpItem>(pathHandle, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
}

void RecordingCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    auto bitmapHandle = CmdListHelper::AddBitmapToCmdList(*cmdList_, bitmap);
    cmdList_->AddOp<DrawBitmapOpItem>(bitmapHandle, px, py);
}

void RecordingCanvas::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    cmdList_->AddOp<DrawImageOpItem>(imageHandle, px, py, sampling);
}

void RecordingCanvas::DrawImageRect(
    const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    cmdList_->AddOp<DrawImageRectOpItem>(imageHandle, src, dst, sampling, constraint);
}

void RecordingCanvas::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    Rect src(0, 0, image.GetWidth(), image.GetHeight());
    cmdList_->AddOp<DrawImageRectOpItem>(imageHandle, src, dst, sampling, SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
}

void RecordingCanvas::DrawPicture(const Picture& picture)
{
    auto pictureHandle = CmdListHelper::AddPictureToCmdList(*cmdList_, picture);
    cmdList_->AddOp<DrawPictureOpItem>(pictureHandle);
}

void RecordingCanvas::ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias)
{
    cmdList_->AddOp<ClipRectOpItem>(rect, op, doAntiAlias);
}

void RecordingCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
    cmdList_->AddOp<ClipRoundRectOpItem>(roundRect, op, doAntiAlias);
}

void RecordingCanvas::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, path);
    cmdList_->AddOp<ClipPathOpItem>(pathHandle, op, doAntiAlias);
}

void RecordingCanvas::SetMatrix(const Matrix& matrix)
{
    cmdList_->AddOp<SetMatrixOpItem>(matrix);
}

void RecordingCanvas::ResetMatrix()
{
    cmdList_->AddOp<ResetMatrixOpItem>();
}

void RecordingCanvas::ConcatMatrix(const Matrix& matrix)
{
    cmdList_->AddOp<ConcatMatrixOpItem>(matrix);
}

void RecordingCanvas::Translate(scalar dx, scalar dy)
{
    cmdList_->AddOp<TranslateOpItem>(dx, dy);
}

void RecordingCanvas::Scale(scalar sx, scalar sy)
{
    cmdList_->AddOp<ScaleOpItem>(sx, sy);
}

void RecordingCanvas::Rotate(scalar deg, scalar sx, scalar sy)
{
    cmdList_->AddOp<RotateOpItem>(deg, sx, sy);
}

void RecordingCanvas::Shear(scalar sx, scalar sy)
{
    cmdList_->AddOp<ShearOpItem>(sx, sy);
}

void RecordingCanvas::Flush()
{
    cmdList_->AddOp<FlushOpItem>();
}

void RecordingCanvas::Clear(ColorQuad color)
{
    cmdList_->AddOp<ClearOpItem>(color);
}

void RecordingCanvas::Save()
{
    cmdList_->AddOp<SaveOpItem>();
    saveCount_++;
}

void RecordingCanvas::SaveLayer(const SaveLayerOps& saveLayerOps)
{
    Rect rect;
    bool hasBrush = false;
    Color color;
    BlendMode mode;
    bool isAntiAlias = false;
    Filter::FilterQuality filterQuality;
    BrushHandle brushHandle;
    CmdListHandle imageFilterHandle;

    if (saveLayerOps.GetBounds() != nullptr) {
        rect = *saveLayerOps.GetBounds();
    }
    const Brush* brush = saveLayerOps.GetBrush();
    if (brush != nullptr) {
        hasBrush = true;
        color = brush->GetColor();
        mode = brush->GetBlendMode();
        isAntiAlias = brush->IsAntiAlias();
        filterQuality = brush->GetFilter().GetFilterQuality();
        Filter filter = brush->GetFilter();
        brushHandle = {
            CmdListHelper::AddRecordedToCmdList<RecordingColorSpace>(*cmdList_, brush->GetColorSpace()),
            CmdListHelper::AddRecordedToCmdList<RecordingShaderEffect>(*cmdList_, brush->GetShaderEffect()),
            CmdListHelper::AddRecordedToCmdList<RecordingColorFilter>(*cmdList_, filter.GetColorFilter()),
            CmdListHelper::AddRecordedToCmdList<RecordingImageFilter>(*cmdList_, filter.GetImageFilter()),
            CmdListHelper::AddRecordedToCmdList<RecordingMaskFilter>(*cmdList_, filter.GetMaskFilter()),
        };
    }
    imageFilterHandle = CmdListHelper::AddRecordedToCmdList<RecordingImageFilter>(
        *cmdList_, saveLayerOps.GetImageFilter());

    cmdList_->AddOp<SaveLayerOpItem>(rect, hasBrush, color, mode, isAntiAlias, filterQuality, brushHandle,
        imageFilterHandle, saveLayerOps.GetSaveLayerFlags());
    saveCount_++;
}

void RecordingCanvas::Restore()
{
    if (saveCount_ > 0) {
        cmdList_->AddOp<RestoreOpItem>();
        --saveCount_;
    }
}

void RecordingCanvas::ClipAdaptiveRoundRect(const std::vector<Point>& radius)
{
    auto radiusData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, radius);
    cmdList_->AddOp<ClipAdaptiveRoundRectOpItem>(radiusData);
}

void RecordingCanvas::DrawImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Data>& data,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling)
{
    ImageHandle imageHandle;
    if (data != nullptr) {
        imageHandle = CmdListHelper::AddCompressDataToCmdList(*cmdList_, data);
        cmdList_->AddOp<DrawAdaptiveImageOpItem>(imageHandle, rsImageInfo, smapling, false);
        return;
    }
    if (image != nullptr) {
        imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
        cmdList_->AddOp<DrawAdaptiveImageOpItem>(imageHandle, rsImageInfo, smapling, true);
    }
}

void RecordingCanvas::DrawPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling)
{
    auto pixelmapHandle = CmdListHelper::AddPixelMapToCmdList(*cmdList_, pixelMap);
    cmdList_->AddOp<DrawAdaptivePixelMapOpItem>(pixelmapHandle, rsImageInfo, smapling);
}

CoreCanvas& RecordingCanvas::AttachPen(const Pen& pen)
{
    Filter filter = pen.GetFilter();
    PenHandle penHandle = {
        CmdListHelper::AddRecordedToCmdList<RecordingPathEffect>(*cmdList_, pen.GetPathEffect()),
        CmdListHelper::AddRecordedToCmdList<RecordingColorSpace>(*cmdList_, pen.GetColorSpace()),
        CmdListHelper::AddRecordedToCmdList<RecordingShaderEffect>(*cmdList_, pen.GetShaderEffect()),
        CmdListHelper::AddRecordedToCmdList<RecordingColorFilter>(*cmdList_, filter.GetColorFilter()),
        CmdListHelper::AddRecordedToCmdList<RecordingImageFilter>(*cmdList_, filter.GetImageFilter()),
        CmdListHelper::AddRecordedToCmdList<RecordingMaskFilter>(*cmdList_, filter.GetMaskFilter()),
    };
    cmdList_->AddOp<AttachPenOpItem>(pen.GetColor(), pen.GetWidth(), pen.GetMiterLimit(), pen.GetCapStyle(),
        pen.GetJoinStyle(), pen.GetBlendMode(), pen.IsAntiAlias(), filter.GetFilterQuality(), penHandle);

    return *this;
}

CoreCanvas& RecordingCanvas::AttachBrush(const Brush& brush)
{
    Filter filter = brush.GetFilter();
    BrushHandle brushHandle = {
        CmdListHelper::AddRecordedToCmdList<RecordingColorSpace>(*cmdList_, brush.GetColorSpace()),
        CmdListHelper::AddRecordedToCmdList<RecordingShaderEffect>(*cmdList_, brush.GetShaderEffect()),
        CmdListHelper::AddRecordedToCmdList<RecordingColorFilter>(*cmdList_, filter.GetColorFilter()),
        CmdListHelper::AddRecordedToCmdList<RecordingImageFilter>(*cmdList_, filter.GetImageFilter()),
        CmdListHelper::AddRecordedToCmdList<RecordingMaskFilter>(*cmdList_, filter.GetMaskFilter()),
    };
    cmdList_->AddOp<AttachBrushOpItem>(brush.GetColor(), brush.GetBlendMode(), brush.IsAntiAlias(),
        filter.GetFilterQuality(), brushHandle);

    return *this;
}

CoreCanvas& RecordingCanvas::DetachPen()
{
    cmdList_->AddOp<DetachPenOpItem>();

    return *this;
}

CoreCanvas& RecordingCanvas::DetachBrush()
{
    cmdList_->AddOp<DetachBrushOpItem>();

    return *this;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
