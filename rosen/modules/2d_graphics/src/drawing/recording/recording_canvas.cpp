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
#include "recording/recording_region.h"
#include "draw/color.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/filter.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/log.h"

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

void RecordingCanvas::Clear() const
{
    if (cmdList_ == nullptr) {
        return;
    }
    cmdList_->ClearOp();
}

void RecordingCanvas::DrawPoint(const Point& point)
{
    cmdList_->AddOp<DrawPointOpItem>(point);
}

void RecordingCanvas::DrawPoints(PointMode mode, size_t count, const Point pts[])
{
    std::vector<Point> points(pts, pts + count);
    auto pointsData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, points);
    cmdList_->AddOp<DrawPointsOpItem>(mode, pointsData);
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
    std::vector<Point> radiusXY;
    radiusXY.push_back(roundRect.GetCornerRadius(RoundRect::CornerPos::TOP_LEFT_POS));
    radiusXY.push_back(roundRect.GetCornerRadius(RoundRect::CornerPos::TOP_RIGHT_POS));
    radiusXY.push_back(roundRect.GetCornerRadius(RoundRect::CornerPos::BOTTOM_RIGHT_POS));
    radiusXY.push_back(roundRect.GetCornerRadius(RoundRect::CornerPos::BOTTOM_LEFT_POS));
    auto radiusXYData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, radiusXY);

    cmdList_->AddOp<DrawRoundRectOpItem>(radiusXYData, roundRect.GetRect());
}

void RecordingCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    std::vector<Point> outerRadiusXY;
    outerRadiusXY.push_back(outer.GetCornerRadius(RoundRect::CornerPos::TOP_LEFT_POS));
    outerRadiusXY.push_back(outer.GetCornerRadius(RoundRect::CornerPos::TOP_RIGHT_POS));
    outerRadiusXY.push_back(outer.GetCornerRadius(RoundRect::CornerPos::BOTTOM_RIGHT_POS));
    outerRadiusXY.push_back(outer.GetCornerRadius(RoundRect::CornerPos::BOTTOM_LEFT_POS));
    auto outerRadiusXYData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, outerRadiusXY);

    std::vector<Point> innerRadiusXY;
    innerRadiusXY.push_back(inner.GetCornerRadius(RoundRect::CornerPos::TOP_LEFT_POS));
    innerRadiusXY.push_back(inner.GetCornerRadius(RoundRect::CornerPos::TOP_RIGHT_POS));
    innerRadiusXY.push_back(inner.GetCornerRadius(RoundRect::CornerPos::BOTTOM_RIGHT_POS));
    innerRadiusXY.push_back(inner.GetCornerRadius(RoundRect::CornerPos::BOTTOM_LEFT_POS));
    auto innerRadiusXYData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, innerRadiusXY);

    cmdList_->AddOp<DrawNestedRoundRectOpItem>(outerRadiusXYData, outer.GetRect(), innerRadiusXYData, inner.GetRect());
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
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    cmdList_->AddOp<DrawPathOpItem>(pathHandle);
}

void RecordingCanvas::DrawBackground(const Brush& brush)
{
    Filter filter = brush.GetFilter();
    BrushHandle brushHandle = {
        brush.GetColor(),
        brush.GetBlendMode(),
        brush.IsAntiAlias(),
        filter.GetFilterQuality(),
        CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush.GetColorSpace()),
        CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush.GetShaderEffect()),
        CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
        CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
        CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
    };
    cmdList_->AddOp<DrawBackgroundOpItem>(brushHandle);
}

void RecordingCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    cmdList_->AddOp<DrawShadowOpItem>(pathHandle, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
}

void RecordingCanvas::DrawRegion(const Region& region)
{
    auto regionHandle = CmdListHelper::AddRecordedToCmdList<RecordingRegion>(*cmdList_, region);
    cmdList_->AddOp<DrawRegionOpItem>(regionHandle);
}

void RecordingCanvas::DrawPatch(const Point cubics[12], const ColorQuad colors[4],
    const Point texCoords[4], BlendMode mode)
{
    const size_t cubicsCount = 12;
    std::vector<Point> skiaCubics = {};
    if (cubics != nullptr) {
        skiaCubics = std::vector<Point>(cubics, cubics + cubicsCount);
    }

    const size_t colorsCount = 4;
    std::vector<ColorQuad> skiaColors = {};
    if (colors != nullptr) {
        skiaColors = std::vector<ColorQuad>(colors, colors + colorsCount);
    }

    const size_t texCoordsCount = 4;
    std::vector<Point> skiaTexCoords = {};
    if (texCoords != nullptr) {
        skiaTexCoords = std::vector<Point>(texCoords, texCoords + texCoordsCount);
    }

    auto cubicsData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, skiaCubics);
    auto colorsData = CmdListHelper::AddVectorToCmdList<ColorQuad>(*cmdList_, skiaColors);
    auto texCoordsData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, skiaTexCoords);
    cmdList_->AddOp<DrawPatchOpItem>(cubicsData, colorsData, texCoordsData, mode);
}

void RecordingCanvas::DrawEdgeAAQuad(const Rect& rect, const Point clip[4],
    QuadAAFlags aaFlags, ColorQuad color, BlendMode mode)
{
    const size_t clipCount = 4;
    std::vector<Point> clipData = {};
    if (clip != nullptr) {
        clipData = std::vector<Point>(clip, clip + clipCount);
    }

    auto clipDataPtr = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, clipData);
    cmdList_->AddOp<DrawEdgeAAQuadOpItem>(rect, clipDataPtr, aaFlags, color, mode);
}

void RecordingCanvas::DrawVertices(const Vertices& vertices, BlendMode mode)
{
    auto verticesHandle = CmdListHelper::AddVerticesToCmdList(*cmdList_, vertices);
    cmdList_->AddOp<DrawVerticesOpItem>(verticesHandle, mode);
}

void RecordingCanvas::DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
    FilterMode filterMode, const Brush* brush)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, *image);
    BrushHandle brushHandle;
    bool hasBrush = false;
    if (brush != nullptr) {
        hasBrush = true;
        Filter filter = brush->GetFilter();
        brushHandle = {
            brush->GetColor(),
            brush->GetBlendMode(),
            brush->IsAntiAlias(),
            filter.GetFilterQuality(),
            CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush->GetColorSpace()),
            CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush->GetShaderEffect()),
            CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
            CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
            CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
        };
    }

    cmdList_->AddOp<DrawImageNineOpItem>(imageHandle, center, dst, filterMode, brushHandle, hasBrush);
}

void RecordingCanvas::DrawAnnotation(const Rect& rect, const char* key, const Data* data)
{
    auto dataHandle = CmdListHelper::AddDataToCmdList(*cmdList_, data);
    cmdList_->AddOp<DrawAnnotationOpItem>(rect, key, dataHandle);
}

void RecordingCanvas::DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
    FilterMode filterMode, const Brush* brush)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, *image);
    BrushHandle brushHandle;
    bool hasBrush = false;
    if (brush != nullptr) {
        hasBrush = true;
        Filter filter = brush->GetFilter();
        brushHandle = {
            brush->GetColor(),
            brush->GetBlendMode(),
            brush->IsAntiAlias(),
            filter.GetFilterQuality(),
            CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush->GetColorSpace()),
            CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush->GetShaderEffect()),
            CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
            CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
            CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
        };
    }

    cmdList_->AddOp<DrawImageLatticeOpItem>(imageHandle, lattice, dst, filterMode, brushHandle, hasBrush);
}

void RecordingCanvas::DrawColor(ColorQuad color, BlendMode mode)
{
    cmdList_->AddOp<DrawColorOpItem>(color, mode);
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

void RecordingCanvas::DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y)
{
    if (!blob) {
        LOGE("blob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    
    if (IsCustomTextType()) {
        LOGD("RecordingCanvas::DrawTextBlob replace drawOpItem with cached one");
        ImageHandle imageHandle;
        DrawTextBlobOpItem textOp(imageHandle, x, y);
        textOp.GenerateCachedOpItem(cmdList_, blob);
    } else {
        auto textBlobHandle = CmdListHelper::AddTextBlobToCmdList(*cmdList_, blob);
        cmdList_->AddOp<DrawTextBlobOpItem>(textBlobHandle, x, y);
    }
}

void RecordingCanvas::ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias)
{
    cmdList_->AddOp<ClipRectOpItem>(rect, op, doAntiAlias);
}

void RecordingCanvas::ClipIRect(const RectI& rect, ClipOp op)
{
    cmdList_->AddOp<ClipIRectOpItem>(rect, op);
}

void RecordingCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
    std::vector<Point> radiusXY;
    radiusXY.push_back(roundRect.GetCornerRadius(RoundRect::CornerPos::TOP_LEFT_POS));
    radiusXY.push_back(roundRect.GetCornerRadius(RoundRect::CornerPos::TOP_RIGHT_POS));
    radiusXY.push_back(roundRect.GetCornerRadius(RoundRect::CornerPos::BOTTOM_RIGHT_POS));
    radiusXY.push_back(roundRect.GetCornerRadius(RoundRect::CornerPos::BOTTOM_LEFT_POS));
    auto radiusXYData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, radiusXY);

    cmdList_->AddOp<ClipRoundRectOpItem>(radiusXYData, roundRect.GetRect(), op, doAntiAlias);
}

void RecordingCanvas::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    cmdList_->AddOp<ClipPathOpItem>(pathHandle, op, doAntiAlias);
}

void RecordingCanvas::ClipRegion(const Region& region, ClipOp op)
{
    auto regionHandle = CmdListHelper::AddRecordedToCmdList<RecordingRegion>(*cmdList_, region);
    cmdList_->AddOp<ClipRegionOpItem>(regionHandle, op);
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
    BrushHandle brushHandle;

    if (saveLayerOps.GetBounds() != nullptr) {
        rect = *saveLayerOps.GetBounds();
    }
    const Brush* brush = saveLayerOps.GetBrush();
    if (brush != nullptr) {
        hasBrush = true;
        Filter filter = brush->GetFilter();
        brushHandle = {
            brush->GetColor(),
            brush->GetBlendMode(),
            brush->IsAntiAlias(),
            filter.GetFilterQuality(),
            CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush->GetColorSpace()),
            CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush->GetShaderEffect()),
            CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
            CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
            CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
        };
    }
    CmdListHandle imageFilterHandle = CmdListHelper::AddRecordedToCmdList<RecordingImageFilter>(
        *cmdList_, saveLayerOps.GetImageFilter());

    cmdList_->AddOp<SaveLayerOpItem>(rect, hasBrush, brushHandle,
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

uint32_t RecordingCanvas::GetSaveCount() const
{
    return saveCount_;
}

void RecordingCanvas::Discard()
{
    cmdList_->AddOp<DiscardOpItem>();
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
        pen.GetColor(),
        pen.GetWidth(),
        pen.GetMiterLimit(),
        pen.GetCapStyle(),
        pen.GetJoinStyle(),
        pen.GetBlendMode(),
        pen.IsAntiAlias(),
        filter.GetFilterQuality(),
        CmdListHelper::AddPathEffectToCmdList(*cmdList_, pen.GetPathEffect()),
        CmdListHelper::AddColorSpaceToCmdList(*cmdList_, pen.GetColorSpace()),
        CmdListHelper::AddShaderEffectToCmdList(*cmdList_, pen.GetShaderEffect()),
        CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
        CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
        CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
    };
    cmdList_->AddOp<AttachPenOpItem>(penHandle);

    return *this;
}

CoreCanvas& RecordingCanvas::AttachBrush(const Brush& brush)
{
    Filter filter = brush.GetFilter();
    BrushHandle brushHandle = {
        brush.GetColor(),
        brush.GetBlendMode(),
        brush.IsAntiAlias(),
        filter.GetFilterQuality(),
        CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush.GetColorSpace()),
        CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush.GetShaderEffect()),
        CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
        CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
        CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
    };
    cmdList_->AddOp<AttachBrushOpItem>(brushHandle);

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
void RecordingCanvas::SetIsCustomTextType(bool isCustomTextType)
{
    isCustomTextType_ = isCustomTextType;
}

bool RecordingCanvas::IsCustomTextType() const
{
    return isCustomTextType_;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
