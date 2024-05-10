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

#include "recording/draw_cmd.h"
#include <cstdint>

#include "platform/common/rs_system_properties.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "recording/mem_allocator.h"
#include "recording/op_item.h"

#include "draw/brush.h"
#include "draw/path.h"
#include "draw/surface.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/log.h"
#include "utils/scalar.h"
#include "utils/system_properties.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr int TEXT_BLOB_CACHE_MARGIN = 10;
bool GetOffScreenSurfaceAndCanvas(const Canvas& canvas,
    std::shared_ptr<Drawing::Surface>& offScreenSurface, std::shared_ptr<Canvas>& offScreenCanvas)
{
    auto surface = canvas.GetSurface();
    if (!surface) {
        return false;
    }
    offScreenSurface = surface->MakeSurface(surface->Width(), surface->Height());
    if (!offScreenSurface) {
        return false;
    }
    offScreenCanvas = offScreenSurface->GetCanvas();
    offScreenCanvas->SetMatrix(canvas.GetTotalMatrix());
    return true;
}
}

std::function<void (std::shared_ptr<Drawing::Image> image)> DrawOpItem::holdDrawingImagefunc_ = nullptr;
void DrawOpItem::SetBaseCallback(
    std::function<void (std::shared_ptr<Drawing::Image> image)> holdDrawingImagefunc)
{
    holdDrawingImagefunc_ = holdDrawingImagefunc;
}

std::function<std::shared_ptr<Drawing::Typeface>(uint64_t)> DrawOpItem::customTypefaceQueryfunc_ = nullptr;
void DrawOpItem::SetTypefaceQueryCallBack(
    std::function<std::shared_ptr<Drawing::Typeface>(uint64_t)> customTypefaceQueryfunc)
{
    customTypefaceQueryfunc_ = customTypefaceQueryfunc;
}

void DrawOpItem::BrushHandleToBrush(const BrushHandle& brushHandle, const DrawCmdList& cmdList, Brush& brush)
{
    brush.SetBlendMode(brushHandle.mode);
    brush.SetAntiAlias(brushHandle.isAntiAlias);

    if (brushHandle.colorSpaceHandle.size) {
        auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(cmdList, brushHandle.colorSpaceHandle);
        const Color4f color4f = { brushHandle.color.GetRedF(), brushHandle.color.GetGreenF(),
                                  brushHandle.color.GetBlueF(), brushHandle.color.GetAlphaF() };
        brush.SetColor(color4f, colorSpace);
    } else {
        brush.SetColor(brushHandle.color);
    }

    if (brushHandle.shaderEffectHandle.size) {
        auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(cmdList, brushHandle.shaderEffectHandle);
        brush.SetShaderEffect(shaderEffect);
    }

    Filter filter;
    bool hasFilter = false;
    if (brushHandle.colorFilterHandle.size) {
        auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(cmdList, brushHandle.colorFilterHandle);
        filter.SetColorFilter(colorFilter);
        hasFilter = true;
    }
    if (brushHandle.imageFilterHandle.size) {
        auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(cmdList, brushHandle.imageFilterHandle);
        filter.SetImageFilter(imageFilter);
        hasFilter = true;
    }
    if (brushHandle.maskFilterHandle.size) {
        auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(cmdList, brushHandle.maskFilterHandle);
        filter.SetMaskFilter(maskFilter);
        hasFilter = true;
    }

    if (hasFilter) {
        filter.SetFilterQuality(brushHandle.filterQuality);
        brush.SetFilter(filter);
    }
}

void DrawOpItem::BrushToBrushHandle(const Brush& brush, DrawCmdList& cmdList, BrushHandle& brushHandle)
{
    const Filter& filter = brush.GetFilter();
    brushHandle.color = brush.GetColor();
    brushHandle.mode = brush.GetBlendMode();
    brushHandle.isAntiAlias = brush.IsAntiAlias();
    brushHandle.filterQuality = filter.GetFilterQuality();
    brushHandle.colorSpaceHandle = CmdListHelper::AddColorSpaceToCmdList(cmdList, brush.GetColorSpace());
    brushHandle.shaderEffectHandle = CmdListHelper::AddShaderEffectToCmdList(cmdList, brush.GetShaderEffect());
    brushHandle.colorFilterHandle = CmdListHelper::AddColorFilterToCmdList(cmdList, filter.GetColorFilter());
    brushHandle.imageFilterHandle = CmdListHelper::AddImageFilterToCmdList(cmdList, filter.GetImageFilter());
    brushHandle.maskFilterHandle = CmdListHelper::AddMaskFilterToCmdList(cmdList, filter.GetMaskFilter());
}

void DrawOpItem::GeneratePaintFromHandle(const PaintHandle& paintHandle, const DrawCmdList& cmdList, Paint& paint)
{
    paint.SetBlendMode(paintHandle.mode);
    paint.SetAntiAlias(paintHandle.isAntiAlias);
    paint.SetStyle(paintHandle.style);

    if (paintHandle.colorSpaceHandle.size) {
        auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(cmdList, paintHandle.colorSpaceHandle);
        const Color4f color4f = { paintHandle.color.GetRedF(), paintHandle.color.GetGreenF(),
                                  paintHandle.color.GetBlueF(), paintHandle.color.GetAlphaF() };
        paint.SetColor(color4f, colorSpace);
    } else {
        paint.SetColor(paintHandle.color);
    }

    if (paintHandle.shaderEffectHandle.size) {
        auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(cmdList, paintHandle.shaderEffectHandle);
        paint.SetShaderEffect(shaderEffect);
    }

    Filter filter;
    bool hasFilter = false;
    if (paintHandle.colorFilterHandle.size) {
        auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(cmdList, paintHandle.colorFilterHandle);
        filter.SetColorFilter(colorFilter);
        hasFilter = true;
    }
    if (paintHandle.imageFilterHandle.size) {
        auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(cmdList, paintHandle.imageFilterHandle);
        filter.SetImageFilter(imageFilter);
        hasFilter = true;
    }
    if (paintHandle.maskFilterHandle.size) {
        auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(cmdList, paintHandle.maskFilterHandle);
        filter.SetMaskFilter(maskFilter);
        hasFilter = true;
    }

    if (hasFilter) {
        filter.SetFilterQuality(paintHandle.filterQuality);
        paint.SetFilter(filter);
    }

    if (paintHandle.blurDrawLooperHandle.size) {
        auto blurDrawLooper = CmdListHelper::GetBlurDrawLooperFromCmdList(cmdList,
            paintHandle.blurDrawLooperHandle);
        paint.SetLooper(blurDrawLooper);
    }

    if (!paint.HasStrokeStyle()) {
        return;
    }

    paint.SetWidth(paintHandle.width);
    paint.SetMiterLimit(paintHandle.miterLimit);
    paint.SetCapStyle(paintHandle.capStyle);
    paint.SetJoinStyle(paintHandle.joinStyle);
    if (paintHandle.pathEffectHandle.size) {
        auto pathEffect = CmdListHelper::GetPathEffectFromCmdList(cmdList, paintHandle.pathEffectHandle);
        paint.SetPathEffect(pathEffect);
    }
}

void DrawOpItem::GenerateHandleFromPaint(CmdList& cmdList, const Paint& paint, PaintHandle& paintHandle)
{
    paintHandle.isAntiAlias = paint.IsAntiAlias();
    paintHandle.style = paint.GetStyle();
    paintHandle.color = paint.GetColor();
    paintHandle.mode = paint.GetBlendMode();

    if (paint.HasFilter()) {
        const Filter& filter = paint.GetFilter();
        paintHandle.filterQuality = filter.GetFilterQuality();
        paintHandle.colorFilterHandle = CmdListHelper::AddColorFilterToCmdList(cmdList, filter.GetColorFilter());
        paintHandle.imageFilterHandle = CmdListHelper::AddImageFilterToCmdList(cmdList, filter.GetImageFilter());
        paintHandle.maskFilterHandle = CmdListHelper::AddMaskFilterToCmdList(cmdList, filter.GetMaskFilter());
    }

    if (paint.GetColorSpace()) {
        paintHandle.colorSpaceHandle = CmdListHelper::AddColorSpaceToCmdList(cmdList, paint.GetColorSpace());
    }

    if (paint.GetShaderEffect()) {
        paintHandle.shaderEffectHandle = CmdListHelper::AddShaderEffectToCmdList(cmdList, paint.GetShaderEffect());
    }

    if (paint.GetLooper()) {
        paintHandle.blurDrawLooperHandle = CmdListHelper::AddBlurDrawLooperToCmdList(cmdList,
            paint.GetLooper());
    }

    if (!paint.HasStrokeStyle()) {
        return;
    }

    paintHandle.width = paint.GetWidth();
    paintHandle.miterLimit = paint.GetMiterLimit();
    paintHandle.capStyle = paint.GetCapStyle();
    paintHandle.joinStyle = paint.GetJoinStyle();
    if (paint.GetPathEffect()) {
        paintHandle.pathEffectHandle = CmdListHelper::AddPathEffectToCmdList(cmdList, paint.GetPathEffect());
    }
}

GenerateCachedOpItemPlayer::GenerateCachedOpItemPlayer(DrawCmdList &cmdList, Canvas* canvas, const Rect* rect)
    : canvas_(canvas), rect_(rect), cmdList_(cmdList) {}

bool GenerateCachedOpItemPlayer::GenerateCachedOpItem(uint32_t type, void* handle)
{
    if (handle == nullptr) {
        return false;
    }

    if (type == DrawOpItem::TEXT_BLOB_OPITEM) {
        auto* op = static_cast<DrawTextBlobOpItem::ConstructorHandle*>(handle);
        return op->GenerateCachedOpItem(cmdList_, canvas_);
    }
    return false;
}

/* UnmarshallingPlayer */
std::unordered_map<uint32_t, UnmarshallingPlayer::UnmarshallingFunc>*
    UnmarshallingPlayer::opUnmarshallingFuncLUT_ = nullptr;

bool UnmarshallingPlayer::RegisterUnmarshallingFunc(uint32_t type, UnmarshallingPlayer::UnmarshallingFunc func)
{
    if (!opUnmarshallingFuncLUT_) {
        static std::unordered_map<uint32_t, UnmarshallingPlayer::UnmarshallingFunc> opUnmarshallingFuncLUT = {};
        opUnmarshallingFuncLUT_ = &opUnmarshallingFuncLUT;
    }
    return opUnmarshallingFuncLUT_->emplace(type, func).second;
}

UnmarshallingPlayer::UnmarshallingPlayer(const DrawCmdList& cmdList) : cmdList_(cmdList) {}

std::shared_ptr<DrawOpItem> UnmarshallingPlayer::Unmarshalling(uint32_t type, void* handle)
{
    if (type == DrawOpItem::OPITEM_HEAD || !opUnmarshallingFuncLUT_) {
        return nullptr;
    }

    auto it = opUnmarshallingFuncLUT_->find(type);
    if (it == opUnmarshallingFuncLUT_->end() || it->second == nullptr) {
        return nullptr;
    }

    auto func = it->second;
    return (*func)(this->cmdList_, handle);
}

/* DrawWithPaintOpItem */
DrawWithPaintOpItem::DrawWithPaintOpItem(const DrawCmdList& cmdList, const PaintHandle& paintHandle, uint32_t type)
    : DrawOpItem(type)
{
    GeneratePaintFromHandle(paintHandle, cmdList, paint_);
}

/* DrawPointOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawPoint, DrawOpItem::POINT_OPITEM, DrawPointOpItem::Unmarshalling);

DrawPointOpItem::DrawPointOpItem(const DrawCmdList& cmdList, DrawPointOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, POINT_OPITEM), point_(handle->point) {}

std::shared_ptr<DrawOpItem> DrawPointOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPointOpItem>(cmdList, static_cast<DrawPointOpItem::ConstructorHandle*>(handle));
}

void DrawPointOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(point_, paintHandle);
}

void DrawPointOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawPoint(point_);
}

/* DrawPointsOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawPoints, DrawOpItem::POINTS_OPITEM, DrawPointsOpItem::Unmarshalling);

DrawPointsOpItem::DrawPointsOpItem(const DrawCmdList& cmdList, DrawPointsOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, POINTS_OPITEM), mode_(handle->mode)
{
    pts_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->pts);
}

std::shared_ptr<DrawOpItem> DrawPointsOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPointsOpItem>(cmdList, static_cast<DrawPointsOpItem::ConstructorHandle*>(handle));
}

void DrawPointsOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto pointsData = CmdListHelper::AddVectorToCmdList<Point>(cmdList, pts_);
    cmdList.AddOp<ConstructorHandle>(mode_, pointsData, paintHandle);
}

void DrawPointsOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawPoints(mode_, pts_.size(), pts_.data());
}

/* DrawLineOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawLine, DrawOpItem::LINE_OPITEM, DrawLineOpItem::Unmarshalling);

DrawLineOpItem::DrawLineOpItem(const DrawCmdList& cmdList, DrawLineOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, LINE_OPITEM),
      startPt_(handle->startPt), endPt_(handle->endPt) {}

std::shared_ptr<DrawOpItem> DrawLineOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawLineOpItem>(cmdList, static_cast<DrawLineOpItem::ConstructorHandle*>(handle));
}

void DrawLineOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(startPt_, endPt_, paintHandle);
}

void DrawLineOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawLine(startPt_, endPt_);
}

/* DrawRectOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawRect, DrawOpItem::RECT_OPITEM, DrawRectOpItem::Unmarshalling);

DrawRectOpItem::DrawRectOpItem(const DrawCmdList& cmdList, DrawRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, RECT_OPITEM), rect_(handle->rect) {}

std::shared_ptr<DrawOpItem> DrawRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRectOpItem>(cmdList, static_cast<DrawRectOpItem::ConstructorHandle*>(handle));
}

void DrawRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rect_, paintHandle);
}

void DrawRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawRect(rect_);
}

/* DrawRoundRectOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawRoundRect, DrawOpItem::ROUND_RECT_OPITEM, DrawRoundRectOpItem::Unmarshalling);

DrawRoundRectOpItem::DrawRoundRectOpItem(const DrawCmdList& cmdList, DrawRoundRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, ROUND_RECT_OPITEM), rrect_(handle->rrect) {}

std::shared_ptr<DrawOpItem> DrawRoundRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRoundRectOpItem>(cmdList, static_cast<DrawRoundRectOpItem::ConstructorHandle*>(handle));
}

void DrawRoundRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rrect_, paintHandle);
}

void DrawRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawRoundRect(rrect_);
}

/* DrawNestedRoundRectOpItem */
REGISTER_UNMARSHALLING_FUNC(
    DrawNestedRoundRect, DrawOpItem::NESTED_ROUND_RECT_OPITEM, DrawNestedRoundRectOpItem::Unmarshalling);

DrawNestedRoundRectOpItem::DrawNestedRoundRectOpItem(
    const DrawCmdList& cmdList, DrawNestedRoundRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, NESTED_ROUND_RECT_OPITEM),
      outerRRect_(handle->outerRRect), innerRRect_(handle->innerRRect) {}

std::shared_ptr<DrawOpItem> DrawNestedRoundRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawNestedRoundRectOpItem>(
        cmdList, static_cast<DrawNestedRoundRectOpItem::ConstructorHandle*>(handle));
}

void DrawNestedRoundRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(outerRRect_, innerRRect_, paintHandle);
}

void DrawNestedRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawNestedRoundRect(outerRRect_, innerRRect_);
}

/* DrawArcOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawArc, DrawOpItem::ARC_OPITEM, DrawArcOpItem::Unmarshalling);

DrawArcOpItem::DrawArcOpItem(const DrawCmdList& cmdList, DrawArcOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, ARC_OPITEM), rect_(handle->rect),
      startAngle_(handle->startAngle), sweepAngle_(handle->sweepAngle) {}

std::shared_ptr<DrawOpItem> DrawArcOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawArcOpItem>(cmdList, static_cast<DrawArcOpItem::ConstructorHandle*>(handle));
}

void DrawArcOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rect_, startAngle_, sweepAngle_, paintHandle);
}

void DrawArcOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawArc(rect_, startAngle_, sweepAngle_);
}

/* DrawPieOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawPie, DrawOpItem::PIE_OPITEM, DrawPieOpItem::Unmarshalling);

DrawPieOpItem::DrawPieOpItem(const DrawCmdList& cmdList, DrawPieOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PIE_OPITEM), rect_(handle->rect),
      startAngle_(handle->startAngle), sweepAngle_(handle->sweepAngle) {}

std::shared_ptr<DrawOpItem> DrawPieOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPieOpItem>(cmdList, static_cast<DrawPieOpItem::ConstructorHandle*>(handle));
}

void DrawPieOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rect_, startAngle_, sweepAngle_, paintHandle);
}

void DrawPieOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawPie(rect_, startAngle_, sweepAngle_);
}

/* DrawOvalOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawOval, DrawOpItem::OVAL_OPITEM, DrawOvalOpItem::Unmarshalling);

DrawOvalOpItem::DrawOvalOpItem(const DrawCmdList& cmdList, DrawOvalOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, OVAL_OPITEM), rect_(handle->rect) {}

std::shared_ptr<DrawOpItem> DrawOvalOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawOvalOpItem>(cmdList, static_cast<DrawOvalOpItem::ConstructorHandle*>(handle));
}

void DrawOvalOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rect_, paintHandle);
}

void DrawOvalOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawOval(rect_);
}

/* DrawCircleOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawCircle, DrawOpItem::CIRCLE_OPITEM, DrawCircleOpItem::Unmarshalling);

DrawCircleOpItem::DrawCircleOpItem(const DrawCmdList& cmdList, DrawCircleOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, CIRCLE_OPITEM),
      centerPt_(handle->centerPt), radius_(handle->radius) {}

std::shared_ptr<DrawOpItem> DrawCircleOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawCircleOpItem>(cmdList, static_cast<DrawCircleOpItem::ConstructorHandle*>(handle));
}

void DrawCircleOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(centerPt_, radius_, paintHandle);
}

void DrawCircleOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawCircle(centerPt_, radius_);
}

/* DrawPathOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawPath, DrawOpItem::PATH_OPITEM, DrawPathOpItem::Unmarshalling);

DrawPathOpItem::DrawPathOpItem(const DrawCmdList& cmdList, DrawPathOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PATH_OPITEM)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> DrawPathOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPathOpItem>(cmdList, static_cast<DrawPathOpItem::ConstructorHandle*>(handle));
}

void DrawPathOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto pathHandle = CmdListHelper::AddPathToCmdList(cmdList, *path_);
    cmdList.AddOp<ConstructorHandle>(pathHandle, paintHandle);
}

void DrawPathOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGD("DrawPathOpItem path is null!");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawPath(*path_);
}

/* DrawBackgroundOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawBackground, DrawOpItem::BACKGROUND_OPITEM, DrawBackgroundOpItem::Unmarshalling);

DrawBackgroundOpItem::DrawBackgroundOpItem(const DrawCmdList& cmdList, DrawBackgroundOpItem::ConstructorHandle* handle)
    : DrawOpItem(BACKGROUND_OPITEM)
{
    BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
}

std::shared_ptr<DrawOpItem> DrawBackgroundOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawBackgroundOpItem>(
        cmdList, static_cast<DrawBackgroundOpItem::ConstructorHandle*>(handle));
}

void DrawBackgroundOpItem::Marshalling(DrawCmdList& cmdList)
{
    BrushHandle brushHandle;
    BrushToBrushHandle(brush_, cmdList, brushHandle);
    cmdList.AddOp<ConstructorHandle>(brushHandle);
}

void DrawBackgroundOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawBackground(brush_);
}

/* DrawShadowStyleOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawShadowStyle, DrawOpItem::SHADOW_STYLE_OPITEM, DrawShadowStyleOpItem::Unmarshalling);

DrawShadowStyleOpItem::DrawShadowStyleOpItem(
    const DrawCmdList& cmdList, DrawShadowStyleOpItem::ConstructorHandle* handle)
    : DrawOpItem(SHADOW_STYLE_OPITEM),
      planeParams_(handle->planeParams),
      devLightPos_(handle->devLightPos),
      lightRadius_(handle->lightRadius),
      ambientColor_(handle->ambientColor),
      spotColor_(handle->spotColor),
      flag_(handle->flag),
      isLimitElevation_(handle->isLimitElevation)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> DrawShadowStyleOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawShadowStyleOpItem>(
        cmdList, static_cast<DrawShadowStyleOpItem::ConstructorHandle*>(handle));
}

void DrawShadowStyleOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto pathHandle = CmdListHelper::AddPathToCmdList(cmdList, *path_);
    cmdList.AddOp<ConstructorHandle>(
        pathHandle, planeParams_, devLightPos_, lightRadius_, ambientColor_, spotColor_, flag_, isLimitElevation_);
}

void DrawShadowStyleOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGD("DrawShadowOpItem path is null!");
        return;
    }
    canvas->DrawShadowStyle(
        *path_, planeParams_, devLightPos_, lightRadius_, ambientColor_, spotColor_, flag_, isLimitElevation_);
}

/* DrawShadowOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawShadow, DrawOpItem::SHADOW_OPITEM, DrawShadowOpItem::Unmarshalling);

DrawShadowOpItem::DrawShadowOpItem(const DrawCmdList& cmdList, DrawShadowOpItem::ConstructorHandle* handle)
    : DrawOpItem(SHADOW_OPITEM), planeParams_(handle->planeParams), devLightPos_(handle->devLightPos),
    lightRadius_(handle->lightRadius), ambientColor_(handle->ambientColor),
    spotColor_(handle->spotColor), flag_(handle->flag)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> DrawShadowOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawShadowOpItem>(cmdList, static_cast<DrawShadowOpItem::ConstructorHandle*>(handle));
}

void DrawShadowOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto pathHandle = CmdListHelper::AddPathToCmdList(cmdList, *path_);
    cmdList.AddOp<ConstructorHandle>(
        pathHandle, planeParams_, devLightPos_, lightRadius_, ambientColor_, spotColor_, flag_);
}

void DrawShadowOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGD("DrawShadowOpItem path is null!");
        return;
    }
    canvas->DrawShadow(*path_, planeParams_, devLightPos_, lightRadius_,
                       ambientColor_, spotColor_, flag_);
}

/* DrawRegionOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawRegion, DrawOpItem::REGION_OPITEM, DrawRegionOpItem::Unmarshalling);

DrawRegionOpItem::DrawRegionOpItem(const DrawCmdList& cmdList, DrawRegionOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, REGION_OPITEM)
{
    region_ = CmdListHelper::GetRegionFromCmdList(cmdList, handle->region);
}

std::shared_ptr<DrawOpItem> DrawRegionOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRegionOpItem>(cmdList, static_cast<DrawRegionOpItem::ConstructorHandle*>(handle));
}

void DrawRegionOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto regionHandle = CmdListHelper::AddRegionToCmdList(cmdList, *region_);
    cmdList.AddOp<ConstructorHandle>(regionHandle, paintHandle);
}

void DrawRegionOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (region_ == nullptr) {
        LOGD("DrawRegionOpItem region is nullptr!");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawRegion(*region_);
}

/* DrawVerticesOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawVertices, DrawOpItem::VERTICES_OPITEM, DrawVerticesOpItem::Unmarshalling);

DrawVerticesOpItem::DrawVerticesOpItem(const DrawCmdList& cmdList, DrawVerticesOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, VERTICES_OPITEM), mode_(handle->mode)
{
    vertices_ = CmdListHelper::GetVerticesFromCmdList(cmdList, handle->vertices);
}

std::shared_ptr<DrawOpItem> DrawVerticesOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawVerticesOpItem>(cmdList, static_cast<DrawVerticesOpItem::ConstructorHandle*>(handle));
}

void DrawVerticesOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto opDataHandle = CmdListHelper::AddVerticesToCmdList(cmdList, *vertices_);
    cmdList.AddOp<ConstructorHandle>(opDataHandle, mode_, paintHandle);
}

void DrawVerticesOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (vertices_ == nullptr) {
        LOGD("DrawVerticesOpItem vertices is null");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawVertices(*vertices_, mode_);
}

/* DrawColorOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawColor, DrawOpItem::COLOR_OPITEM, DrawColorOpItem::Unmarshalling);

DrawColorOpItem::DrawColorOpItem(DrawColorOpItem::ConstructorHandle* handle)
    : DrawOpItem(COLOR_OPITEM), color_(handle->color), mode_(handle->mode) {}

std::shared_ptr<DrawOpItem> DrawColorOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawColorOpItem>(static_cast<DrawColorOpItem::ConstructorHandle*>(handle));
}

void DrawColorOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(color_, mode_);
}

void DrawColorOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawColor(color_, mode_);
}

/* DrawImageNineOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawImageNine, DrawOpItem::IMAGE_NINE_OPITEM, DrawImageNineOpItem::Unmarshalling);

DrawImageNineOpItem::DrawImageNineOpItem(const DrawCmdList& cmdList, DrawImageNineOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_NINE_OPITEM), center_(handle->center), dst_(handle->dst), filter_(handle->filter),
    hasBrush_(handle->hasBrush)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageNineOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageNineOpItem>(cmdList, static_cast<DrawImageNineOpItem::ConstructorHandle*>(handle));
}

void DrawImageNineOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *image_);
    BrushHandle brushHandle;
    if (hasBrush_) {
        BrushToBrushHandle(brush_, cmdList, brushHandle);
    }

    cmdList.AddOp<ConstructorHandle>(imageHandle, center_, dst_, filter_, brushHandle, hasBrush_);
}

void DrawImageNineOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGD("DrawImageNineOpItem image is null");
        return;
    }
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    canvas->DrawImageNine(image_.get(), center_, dst_, filter_, brushPtr);
}

/* DrawImageLatticeOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawImageLattice, DrawOpItem::IMAGE_LATTICE_OPITEM, DrawImageLatticeOpItem::Unmarshalling);

DrawImageLatticeOpItem::DrawImageLatticeOpItem(
    const DrawCmdList& cmdList, DrawImageLatticeOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_LATTICE_OPITEM), lattice_(handle->lattice), dst_(handle->dst), filter_(handle->filter),
    hasBrush_(handle->hasBrush)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageLatticeOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageLatticeOpItem>(
        cmdList, static_cast<DrawImageLatticeOpItem::ConstructorHandle*>(handle));
}

void DrawImageLatticeOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *image_);
    BrushHandle brushHandle;
    if (hasBrush_) {
        BrushToBrushHandle(brush_, cmdList, brushHandle);
    }

    cmdList.AddOp<ConstructorHandle>(imageHandle, lattice_, dst_, filter_, brushHandle, hasBrush_);
}

void DrawImageLatticeOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGD("DrawImageNineOpItem image is null");
        return;
    }
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    canvas->DrawImageLattice(image_.get(), lattice_, dst_, filter_, brushPtr);
}

/* DrawBitmapOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawBitmap, DrawOpItem::BITMAP_OPITEM, DrawBitmapOpItem::Unmarshalling);

DrawBitmapOpItem::DrawBitmapOpItem(const DrawCmdList& cmdList, DrawBitmapOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, BITMAP_OPITEM), px_(handle->px), py_(handle->py)
{
    bitmap_ = CmdListHelper::GetBitmapFromCmdList(cmdList, handle->bitmap);
}

std::shared_ptr<DrawOpItem> DrawBitmapOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawBitmapOpItem>(cmdList, static_cast<DrawBitmapOpItem::ConstructorHandle*>(handle));
}

void DrawBitmapOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto bitmapHandle = CmdListHelper::AddBitmapToCmdList(cmdList, *bitmap_);
    cmdList.AddOp<ConstructorHandle>(bitmapHandle, px_, py_, paintHandle);
}

void DrawBitmapOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (bitmap_ == nullptr) {
        LOGD("DrawBitmapOpItem bitmap is null");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawBitmap(*bitmap_, px_, py_);
}

/* DrawImageOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawImage, DrawOpItem::IMAGE_OPITEM, DrawImageOpItem::Unmarshalling);

DrawImageOpItem::DrawImageOpItem(const DrawCmdList& cmdList, DrawImageOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, IMAGE_OPITEM), px_(handle->px), py_(handle->py),
      samplingOptions_(handle->samplingOptions)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageOpItem>(cmdList, static_cast<DrawImageOpItem::ConstructorHandle*>(handle));
}

void DrawImageOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *image_);
    cmdList.AddOp<ConstructorHandle>(imageHandle, px_, py_, samplingOptions_, paintHandle);
}

void DrawImageOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGD("DrawImageOpItem image is null");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawImage(*image_, px_, py_, samplingOptions_);
}

/* DrawImageRectOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawImageRect, DrawOpItem::IMAGE_RECT_OPITEM, DrawImageRectOpItem::Unmarshalling);

DrawImageRectOpItem::DrawImageRectOpItem(const DrawCmdList& cmdList, DrawImageRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, IMAGE_RECT_OPITEM), src_(handle->src), dst_(handle->dst),
      sampling_(handle->sampling), constraint_(handle->constraint), isForeground_(handle->isForeground)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
}

DrawImageRectOpItem::DrawImageRectOpItem(const Image& image, const Rect& src,
    const Rect& dst, const SamplingOptions& sampling,
    SrcRectConstraint constraint, const Paint& paint, bool isForeground)
    : DrawWithPaintOpItem(paint, DrawOpItem::IMAGE_RECT_OPITEM), src_(src), dst_(dst), sampling_(sampling),
    constraint_(constraint), image_(std::make_shared<Image>(image)), isForeground_(isForeground)
{
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageRectOpItem>(cmdList, static_cast<DrawImageRectOpItem::ConstructorHandle*>(handle));
}

void DrawImageRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *image_);
    cmdList.AddOp<ConstructorHandle>(imageHandle, src_, dst_, sampling_, constraint_, paintHandle);
}

void DrawImageRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGD("DrawImageRectOpItem image is null");
        return;
    }
    if (isForeground_) {
        AutoCanvasRestore acr(*canvas, false);
        SaveLayerOps ops;
        canvas->SaveLayer(ops);
        canvas->AttachPaint(paint_);
        canvas->DrawImageRect(*image_, src_, dst_, sampling_, constraint_);
        Brush brush;
        brush.SetColor(canvas->GetEnvForegroundColor());
        brush.SetBlendMode(Drawing::BlendMode::SRC_IN);
        canvas->DrawBackground(brush);
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawImageRect(*image_, src_, dst_, sampling_, constraint_);
}

/* DrawPictureOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawPicture, DrawOpItem::PICTURE_OPITEM, DrawPictureOpItem::Unmarshalling);

DrawPictureOpItem::DrawPictureOpItem(const DrawCmdList& cmdList, DrawPictureOpItem::ConstructorHandle* handle)
    : DrawOpItem(PICTURE_OPITEM)
{
    picture_ = CmdListHelper::GetPictureFromCmdList(cmdList, handle->picture);
}

std::shared_ptr<DrawOpItem> DrawPictureOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPictureOpItem>(cmdList, static_cast<DrawPictureOpItem::ConstructorHandle*>(handle));
}

void DrawPictureOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto pictureHandle = CmdListHelper::AddPictureToCmdList(cmdList, *picture_);
    cmdList.AddOp<ConstructorHandle>(pictureHandle);
}

void DrawPictureOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (picture_ == nullptr) {
        LOGD("DrawPictureOpItem picture is null");
        return;
    }
    canvas->DrawPicture(*picture_);
}

/* DrawTextBlobOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawTextBlob, DrawOpItem::TEXT_BLOB_OPITEM, DrawTextBlobOpItem::Unmarshalling);

void SimplifyPaint(ColorQuad colorQuad, Paint& paint)
{
    Color color{colorQuad};
    paint.SetColor(color);
    paint.SetShaderEffect(nullptr);
    if (paint.HasFilter()) {
        Filter filter = paint.GetFilter();
        if (filter.GetColorFilter() != nullptr) {
            filter.SetColorFilter(nullptr);
            paint.SetFilter(filter);
        }
    }
    paint.SetWidth(1.04); // 1.04 is empirical value
    paint.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);
}

DrawTextBlobOpItem::DrawTextBlobOpItem(const DrawCmdList& cmdList, DrawTextBlobOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, TEXT_BLOB_OPITEM), x_(handle->x), y_(handle->y)
{
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;
    if (DrawOpItem::customTypefaceQueryfunc_) {
        typeface = DrawOpItem::customTypefaceQueryfunc_(handle->globalUniqueId);
    }

    TextBlob::Context ctx {typeface, false};
    if (typeface != nullptr) {
        ctx.SetIsCustomTypeface(true);
    }
    textBlob_ = CmdListHelper::GetTextBlobFromCmdList(cmdList, handle->textBlob, &ctx);
}

std::shared_ptr<DrawOpItem> DrawTextBlobOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawTextBlobOpItem>(cmdList, static_cast<DrawTextBlobOpItem::ConstructorHandle*>(handle));
}

void DrawTextBlobOpItem::Marshalling(DrawCmdList& cmdList)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsignd number shift
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    TextBlob::Context ctx {nullptr, false};
    auto textBlobHandle = CmdListHelper::AddTextBlobToCmdList(cmdList, textBlob_.get(), &ctx);
    uint64_t globalUniqueId = 0;
    if (ctx.GetTypeface() != nullptr) {
        uint32_t typefaceId = ctx.GetTypeface()->GetUniqueID();
        globalUniqueId = (shiftedPid | typefaceId);
    }

    cmdList.AddOp<ConstructorHandle>(textBlobHandle, globalUniqueId, x_, y_, paintHandle);
}

void DrawTextBlobOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (textBlob_ == nullptr) {
        LOGD("DrawTextBlobOpItem textBlob is null");
        return;
    }
    RectI globalClipBounds = canvas->GetDeviceClipBounds();
    bool saveFlag = false;
    if (globalClipBounds.GetWidth() == 1 || globalClipBounds.GetHeight() == 1) {
        // In this case, the clip area for textblob must have AA.
        Matrix m = canvas->GetTotalMatrix();
        canvas->Save();
        canvas->SetMatrix(m);
        auto bounds = textBlob_->Bounds();
        canvas->ClipRect(*bounds, ClipOp::INTERSECT, true);
        saveFlag = true;
    }
    if (canvas->isHighContrastEnabled()) {
        LOGD("DrawTextBlobOpItem::Playback highContrastEnabled, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        DrawHighContrastEnabled(canvas);
    } else {
        canvas->AttachPaint(paint_);
        canvas->DrawTextBlob(textBlob_.get(), x_, y_);
    }
    if (saveFlag) {
        canvas->Restore();
    }
}

void DrawTextBlobOpItem::DrawHighContrastEnabled(Canvas* canvas) const
{
    ColorQuad colorQuad = paint_.GetColor().CastToColorQuad();
    if (Color::ColorQuadGetA(colorQuad) == 0 || paint_.HasFilter()) {
        canvas->AttachPaint(paint_);
        canvas->DrawTextBlob(textBlob_.get(), x_, y_);
        return;
    }
    if (canvas->GetAlphaSaveCount() > 0 && canvas->GetAlpha() < 1.0f) {
        std::shared_ptr<Drawing::Surface> offScreenSurface;
        std::shared_ptr<Canvas> offScreenCanvas;
        if (GetOffScreenSurfaceAndCanvas(*canvas, offScreenSurface, offScreenCanvas)) {
            DrawHighContrast(offScreenCanvas.get());
            offScreenCanvas->Flush();
            Drawing::Brush paint;
            paint.SetAntiAlias(true);
            canvas->AttachBrush(paint);
            Drawing::SamplingOptions sampling =
                Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
            canvas->Save();
            canvas->ResetMatrix();
            canvas->DrawImage(*offScreenSurface->GetImageSnapshot().get(), 0, 0, sampling);
            canvas->DetachBrush();
            canvas->Restore();
            return;
        }
    }
    DrawHighContrast(canvas);
}

void DrawTextBlobOpItem::DrawHighContrast(Canvas* canvas) const
{
    ColorQuad colorQuad = paint_.GetColor().CastToColorQuad();
    uint32_t channelSum = Color::ColorQuadGetR(colorQuad) + Color::ColorQuadGetG(colorQuad) +
        Color::ColorQuadGetB(colorQuad);
    bool flag = channelSum < 594; // 594 is empirical value

    Paint outlinePaint(paint_);
    SimplifyPaint(flag ? Color::COLOR_WHITE : Color::COLOR_BLACK, outlinePaint);
    outlinePaint.SetStyle(Paint::PAINT_FILL_STROKE);
    canvas->AttachPaint(outlinePaint);
    canvas->DrawTextBlob(textBlob_.get(), x_, y_);

    Paint innerPaint(paint_);
    SimplifyPaint(flag ? Color::COLOR_BLACK : Color::COLOR_WHITE, innerPaint);
    innerPaint.SetStyle(Paint::PAINT_FILL);
    canvas->AttachPaint(innerPaint);
    canvas->DrawTextBlob(textBlob_.get(), x_, y_);
}

bool DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(
    DrawCmdList& cmdList, const TextBlob* textBlob, scalar x, scalar y, Paint& p)
{
    if (!textBlob) {
        LOGD("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return false;
    }

    auto bounds = textBlob->Bounds();
    if (!bounds || !bounds->IsValid()) {
        return false;
    }
    bounds->Offset(x, y);
    bounds->MakeOutset(TEXT_BLOB_CACHE_MARGIN, TEXT_BLOB_CACHE_MARGIN);
    // create CPU raster surface
    Drawing::ImageInfo offscreenInfo { bounds->GetWidth(), bounds->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL, nullptr};
    std::shared_ptr<Surface> offscreenSurface = Surface::MakeRaster(offscreenInfo);
    if (offscreenSurface == nullptr) {
        return false;
    }
    auto offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        return false;
    }
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    //OffscreenCanvas used once, detach is unnecessary, FakeBrush/Pen avoid affecting ImageRectOp, attach is necessary.
    bool isForeground = false;
    if (p.GetColor() == Drawing::Color::COLOR_FOREGROUND) {
        isForeground = true;
        p.SetColor(Drawing::Color::COLOR_BLACK);
    }
    offscreenCanvas->AttachPaint(p);
    offscreenCanvas->DrawTextBlob(textBlob, x, y);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(),
        bounds->GetLeft() + image->GetWidth(), bounds->GetTop() + image->GetHeight());
    SamplingOptions sampling(FilterMode::LINEAR, MipmapMode::LINEAR);
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, image);
    PaintHandle fakePaintHandle;
    fakePaintHandle.isAntiAlias = true;
    fakePaintHandle.style = Paint::PaintStyle::PAINT_FILL;
    cmdList.AddOp<DrawImageRectOpItem::ConstructorHandle>(
        imageHandle, src, dst, sampling, SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT, fakePaintHandle, isForeground);
    return true;
}

bool DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(DrawCmdList& cmdList, Canvas* canvas)
{
    std::shared_ptr<TextBlob> textBlob_ = CmdListHelper::GetTextBlobFromCmdList(cmdList, textBlob);
    if (!textBlob_) {
        LOGD("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return false;
    }

    auto bounds = textBlob_->Bounds();
    if (!bounds || !bounds->IsValid()) {
        return false;
    }
    bounds->Offset(x, y);

    std::shared_ptr<Surface> offscreenSurface = nullptr;

    if (auto surface = canvas != nullptr ? canvas->GetSurface() : nullptr) {
        // create GPU accelerated surface if possible
        offscreenSurface = surface->MakeSurface(bounds->GetWidth(), bounds->GetHeight());
    } else {
        // create CPU raster surface
        Drawing::ImageInfo offscreenInfo { bounds->GetWidth(), bounds->GetHeight(),
            Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL, nullptr};
        offscreenSurface = Surface::MakeRaster(offscreenInfo);
    }
    if (offscreenSurface == nullptr) {
        return false;
    }

    Canvas* offscreenCanvas = offscreenSurface->GetCanvas().get();

    // align draw op to [0, 0]
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    Paint p;
    GeneratePaintFromHandle(paintHandle, cmdList, p);
    offscreenCanvas->AttachPaint(p);
    offscreenCanvas->DrawTextBlob(textBlob_.get(), x, y);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(),
        bounds->GetLeft() + image->GetWidth(), bounds->GetTop() + image->GetHeight());
    SamplingOptions sampling;
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, image);
    PaintHandle fakePaintHandle;
    fakePaintHandle.isAntiAlias = true;
    fakePaintHandle.style = Paint::PaintStyle::PAINT_FILL;
    cmdList.AddOp<DrawImageRectOpItem::ConstructorHandle>(imageHandle, src, dst, sampling,
        SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT, fakePaintHandle);
    return true;
}

std::shared_ptr<DrawImageRectOpItem> DrawTextBlobOpItem::GenerateCachedOpItem(Canvas* canvas)
{
    if (!textBlob_) {
        LOGD("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }

    auto bounds = textBlob_->Bounds();
    if (!bounds || !bounds->IsValid()) {
        return nullptr;
    }
    bounds->Offset(x_, y_);

    std::shared_ptr<Surface> offscreenSurface = nullptr;

    if (auto surface = canvas != nullptr ? canvas->GetSurface() : nullptr) {
        // create GPU accelerated surface if possible
        offscreenSurface = surface->MakeSurface(bounds->GetWidth(), bounds->GetHeight());
    } else {
        // create CPU raster surface
        Drawing::ImageInfo offscreenInfo { bounds->GetWidth(), bounds->GetHeight(),
            Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL, nullptr};
        offscreenSurface = Surface::MakeRaster(offscreenInfo);
    }
    if (offscreenSurface == nullptr) {
        return nullptr;
    }

    Canvas* offscreenCanvas = offscreenSurface->GetCanvas().get();

    // align draw op to [0, 0]
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    Playback(offscreenCanvas, nullptr);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(), bounds->GetRight(), bounds->GetBottom());
    Paint fakePaint;
    fakePaint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    fakePaint.SetAntiAlias(true);
    return std::make_shared<DrawImageRectOpItem>(*image, src, dst, SamplingOptions(),
        SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT, fakePaint);
}

/* DrawSymbolOpItem */
REGISTER_UNMARSHALLING_FUNC(DrawSymbol, DrawOpItem::SYMBOL_OPITEM, DrawSymbolOpItem::Unmarshalling);

DrawSymbolOpItem::DrawSymbolOpItem(const DrawCmdList& cmdList, DrawSymbolOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, SYMBOL_OPITEM), locate_(handle->locate)
{
    symbol_ = CmdListHelper::GetSymbolFromCmdList(cmdList, handle->symbolHandle);
}

std::shared_ptr<DrawOpItem> DrawSymbolOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawSymbolOpItem>(cmdList, static_cast<DrawSymbolOpItem::ConstructorHandle*>(handle));
}

void DrawSymbolOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto symbolHandle = CmdListHelper::AddSymbolToCmdList(cmdList, symbol_);
    cmdList.AddOp<ConstructorHandle>(symbolHandle, locate_, paintHandle);
}

void DrawSymbolOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (!canvas) {
        LOGD("SymbolOpItem::Playback failed cause by canvas is nullptr");
        return;
    }
    Path path(symbol_.path_);

    // 1.0 move path
    path.Offset(locate_.GetX(), locate_.GetY());

    // 2.0 split path
    std::vector<Path> paths;
    DrawingHMSymbol::PathOutlineDecompose(path, paths);
    std::vector<Path> pathLayers;
    DrawingHMSymbol::MultilayerPath(symbol_.symbolInfo_.layers, paths, pathLayers);

    // 3.0 set paint
    Paint paintCopy = paint_;
    paintCopy.SetAntiAlias(true);
    paintCopy.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
    paintCopy.SetWidth(0.0f);
    paintCopy.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);

    // draw path
    std::vector<DrawingRenderGroup> groups = symbol_.symbolInfo_.renderGroups;
    LOGD("SymbolOpItem::Draw RenderGroup size %{public}d", static_cast<int>(groups.size()));
    if (groups.size() == 0) {
        canvas->AttachPaint(paintCopy);
        canvas->DrawPath(path);
    }
    for (auto group : groups) {
        Path multPath;
        MergeDrawingPath(multPath, group, pathLayers);
        // color
        paintCopy.SetColor(Color::ColorQuadSetARGB(0xFF, group.color.r, group.color.g, group.color.b));
        paintCopy.SetAlphaF(group.color.a);
        canvas->AttachPaint(paintCopy);
        canvas->DrawPath(multPath);
    }
}

void DrawSymbolOpItem::MergeDrawingPath(
    Drawing::Path& multPath, Drawing::DrawingRenderGroup& group, std::vector<Drawing::Path>& pathLayers)
{
    for (auto groupInfo : group.groupInfos) {
        Drawing::Path pathTemp;
        for (auto k : groupInfo.layerIndexes) {
            if (k >= pathLayers.size()) {
                continue;
            }
            pathTemp.AddPath(pathLayers[k]);
        }
        for (size_t h : groupInfo.maskIndexes) {
            if (h >= pathLayers.size()) {
                continue;
            }
            Drawing::Path outPath;
            auto isOk = outPath.Op(pathTemp, pathLayers[h], Drawing::PathOp::DIFFERENCE);
            if (isOk) {
                pathTemp = outPath;
            }
        }
        multPath.AddPath(pathTemp);
    }
}

/* ClipRectOpItem */
REGISTER_UNMARSHALLING_FUNC(ClipRect, DrawOpItem::CLIP_RECT_OPITEM, ClipRectOpItem::Unmarshalling);

ClipRectOpItem::ClipRectOpItem(ClipRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_RECT_OPITEM), rect_(handle->rect), clipOp_(handle->clipOp), doAntiAlias_(handle->doAntiAlias) {}

std::shared_ptr<DrawOpItem> ClipRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRectOpItem>(static_cast<ClipRectOpItem::ConstructorHandle*>(handle));
}

void ClipRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(rect_, clipOp_, doAntiAlias_);
}

void ClipRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipRect(rect_, clipOp_, doAntiAlias_);
}

/* ClipIRectOpItem */
REGISTER_UNMARSHALLING_FUNC(ClipIRect, DrawOpItem::CLIP_IRECT_OPITEM, ClipIRectOpItem::Unmarshalling);

ClipIRectOpItem::ClipIRectOpItem(ClipIRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_IRECT_OPITEM), rect_(handle->rect), clipOp_(handle->clipOp) {}

std::shared_ptr<DrawOpItem> ClipIRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipIRectOpItem>(static_cast<ClipIRectOpItem::ConstructorHandle*>(handle));
}

void ClipIRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(rect_, clipOp_);
}

void ClipIRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipIRect(rect_, clipOp_);
}

/* ClipRoundRectOpItem */
REGISTER_UNMARSHALLING_FUNC(ClipRoundRect, DrawOpItem::CLIP_ROUND_RECT_OPITEM, ClipRoundRectOpItem::Unmarshalling);

ClipRoundRectOpItem::ClipRoundRectOpItem(ClipRoundRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_ROUND_RECT_OPITEM), rrect_(handle->rrect), clipOp_(handle->clipOp),
    doAntiAlias_(handle->doAntiAlias) {}

std::shared_ptr<DrawOpItem> ClipRoundRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRoundRectOpItem>(static_cast<ClipRoundRectOpItem::ConstructorHandle*>(handle));
}

void ClipRoundRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(rrect_, clipOp_, doAntiAlias_);
}

void ClipRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipRoundRect(rrect_, clipOp_, doAntiAlias_);
}

/* ClipPathOpItem */
REGISTER_UNMARSHALLING_FUNC(ClipPath, DrawOpItem::CLIP_PATH_OPITEM, ClipPathOpItem::Unmarshalling);

ClipPathOpItem::ClipPathOpItem(const DrawCmdList& cmdList, ClipPathOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_PATH_OPITEM), clipOp_(handle->clipOp), doAntiAlias_(handle->doAntiAlias)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> ClipPathOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipPathOpItem>(cmdList, static_cast<ClipPathOpItem::ConstructorHandle*>(handle));
}

void ClipPathOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto pathHandle = CmdListHelper::AddPathToCmdList(cmdList, *path_);
    cmdList.AddOp<ConstructorHandle>(pathHandle, clipOp_, doAntiAlias_);
}

void ClipPathOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGD("ClipPathOpItem path is null!");
        return;
    }
    canvas->ClipPath(*path_, clipOp_, doAntiAlias_);
}

/* ClipRegionOpItem */
REGISTER_UNMARSHALLING_FUNC(ClipRegion, DrawOpItem::CLIP_REGION_OPITEM, ClipRegionOpItem::Unmarshalling);

ClipRegionOpItem::ClipRegionOpItem(const DrawCmdList& cmdList, ClipRegionOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_REGION_OPITEM), clipOp_(handle->clipOp)
{
    region_ = CmdListHelper::GetRegionFromCmdList(cmdList, handle->region);
}

std::shared_ptr<DrawOpItem> ClipRegionOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRegionOpItem>(cmdList, static_cast<ClipRegionOpItem::ConstructorHandle*>(handle));
}

void ClipRegionOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto regionHandle = CmdListHelper::AddRegionToCmdList(cmdList, *region_);
    cmdList.AddOp<ConstructorHandle>(regionHandle, clipOp_);
}

void ClipRegionOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (region_ == nullptr) {
        LOGD("ClipRegionOpItem region is null!");
        return;
    }
    canvas->ClipRegion(*region_, clipOp_);
}

/* SetMatrixOpItem */
REGISTER_UNMARSHALLING_FUNC(SetMatrix, DrawOpItem::SET_MATRIX_OPITEM, SetMatrixOpItem::Unmarshalling);

SetMatrixOpItem::SetMatrixOpItem(SetMatrixOpItem::ConstructorHandle* handle) : DrawOpItem(SET_MATRIX_OPITEM)
{
    matrix_.SetAll(handle->matrixBuffer);
}

std::shared_ptr<DrawOpItem> SetMatrixOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<SetMatrixOpItem>(static_cast<SetMatrixOpItem::ConstructorHandle*>(handle));
}

void SetMatrixOpItem::Marshalling(DrawCmdList& cmdList)
{
    Matrix::Buffer matrixBuffer;
    matrix_.GetAll(matrixBuffer);
    cmdList.AddOp<ConstructorHandle>(matrixBuffer);
}

void SetMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->SetMatrix(matrix_);
}

/* ResetMatrixOpItem */
REGISTER_UNMARSHALLING_FUNC(ResetMatrix, DrawOpItem::RESET_MATRIX_OPITEM, ResetMatrixOpItem::Unmarshalling);

ResetMatrixOpItem::ResetMatrixOpItem() : DrawOpItem(RESET_MATRIX_OPITEM) {}

std::shared_ptr<DrawOpItem> ResetMatrixOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ResetMatrixOpItem>();
}

void ResetMatrixOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void ResetMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ResetMatrix();
}

/* ConcatMatrixOpItem */
REGISTER_UNMARSHALLING_FUNC(ConcatMatrix, DrawOpItem::CONCAT_MATRIX_OPITEM, ConcatMatrixOpItem::Unmarshalling);

ConcatMatrixOpItem::ConcatMatrixOpItem(ConcatMatrixOpItem::ConstructorHandle* handle) : DrawOpItem(CONCAT_MATRIX_OPITEM)
{
    matrix_.SetAll(handle->matrixBuffer);
}

std::shared_ptr<DrawOpItem> ConcatMatrixOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ConcatMatrixOpItem>(static_cast<ConcatMatrixOpItem::ConstructorHandle*>(handle));
}

void ConcatMatrixOpItem::Marshalling(DrawCmdList& cmdList)
{
    Matrix::Buffer matrixBuffer;
    matrix_.GetAll(matrixBuffer);
    cmdList.AddOp<ConstructorHandle>(matrixBuffer);
}

void ConcatMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ConcatMatrix(matrix_);
}

/* TranslateOpItem */
REGISTER_UNMARSHALLING_FUNC(Translate, DrawOpItem::TRANSLATE_OPITEM, TranslateOpItem::Unmarshalling);

TranslateOpItem::TranslateOpItem(TranslateOpItem::ConstructorHandle* handle)
    : DrawOpItem(TRANSLATE_OPITEM), dx_(handle->dx), dy_(handle->dy) {}

std::shared_ptr<DrawOpItem> TranslateOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<TranslateOpItem>(static_cast<TranslateOpItem::ConstructorHandle*>(handle));
}

void TranslateOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(dx_, dy_);
}

void TranslateOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Translate(dx_, dy_);
}

/* ScaleOpItem */
REGISTER_UNMARSHALLING_FUNC(Scale, DrawOpItem::SCALE_OPITEM, ScaleOpItem::Unmarshalling);

ScaleOpItem::ScaleOpItem(ScaleOpItem::ConstructorHandle* handle)
    : DrawOpItem(SCALE_OPITEM), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> ScaleOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ScaleOpItem>(static_cast<ScaleOpItem::ConstructorHandle*>(handle));
}

void ScaleOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(sx_, sy_);
}

void ScaleOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Scale(sx_, sy_);
}

/* RotateOpItem */
REGISTER_UNMARSHALLING_FUNC(Rotate, DrawOpItem::ROTATE_OPITEM, RotateOpItem::Unmarshalling);

RotateOpItem::RotateOpItem(RotateOpItem::ConstructorHandle* handle)
    : DrawOpItem(ROTATE_OPITEM), deg_(handle->deg), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> RotateOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<RotateOpItem>(static_cast<RotateOpItem::ConstructorHandle*>(handle));
}

void RotateOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(deg_, sx_, sy_);
}

void RotateOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Rotate(deg_, sx_, sy_);
}

/* ShearOpItem */
REGISTER_UNMARSHALLING_FUNC(Shear, DrawOpItem::SHEAR_OPITEM, ShearOpItem::Unmarshalling);

ShearOpItem::ShearOpItem(ShearOpItem::ConstructorHandle* handle)
    : DrawOpItem(SHEAR_OPITEM), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> ShearOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ShearOpItem>(static_cast<ShearOpItem::ConstructorHandle*>(handle));
}

void ShearOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(sx_, sy_);
}

void ShearOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Shear(sx_, sy_);
}

/* FlushOpItem */
REGISTER_UNMARSHALLING_FUNC(Flush, DrawOpItem::FLUSH_OPITEM, FlushOpItem::Unmarshalling);

FlushOpItem::FlushOpItem() : DrawOpItem(FLUSH_OPITEM) {}

std::shared_ptr<DrawOpItem> FlushOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<FlushOpItem>();
}

void FlushOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void FlushOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Flush();
}

/* ClearOpItem */
REGISTER_UNMARSHALLING_FUNC(Clear, DrawOpItem::CLEAR_OPITEM, ClearOpItem::Unmarshalling);

ClearOpItem::ClearOpItem(ClearOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLEAR_OPITEM), color_(handle->color) {}

std::shared_ptr<DrawOpItem> ClearOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClearOpItem>(static_cast<ClearOpItem::ConstructorHandle*>(handle));
}

void ClearOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(color_);
}

void ClearOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Clear(color_);
}

/* SaveOpItem */
REGISTER_UNMARSHALLING_FUNC(Save, DrawOpItem::SAVE_OPITEM, SaveOpItem::Unmarshalling);

SaveOpItem::SaveOpItem() : DrawOpItem(SAVE_OPITEM) {}

std::shared_ptr<DrawOpItem> SaveOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<SaveOpItem>();
}

void SaveOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void SaveOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Save();
}

/* SaveLayerOpItem */
REGISTER_UNMARSHALLING_FUNC(SaveLayer, DrawOpItem::SAVE_LAYER_OPITEM, SaveLayerOpItem::Unmarshalling);

SaveLayerOpItem::SaveLayerOpItem(const DrawCmdList& cmdList, SaveLayerOpItem::ConstructorHandle* handle)
    : DrawOpItem(SAVE_LAYER_OPITEM), saveLayerFlags_(handle->saveLayerFlags), rect_(handle->rect),
    hasBrush_(handle->hasBrush)
{
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }
}

std::shared_ptr<DrawOpItem> SaveLayerOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<SaveLayerOpItem>(cmdList, static_cast<SaveLayerOpItem::ConstructorHandle*>(handle));
}

void SaveLayerOpItem::Marshalling(DrawCmdList& cmdList)
{
    BrushHandle brushHandle;
    if (hasBrush_) {
        BrushToBrushHandle(brush_, cmdList, brushHandle);
    }
    cmdList.AddOp<ConstructorHandle>(rect_, hasBrush_, brushHandle, saveLayerFlags_);
}

void SaveLayerOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    const Rect* rectPtr = nullptr;
    if (rect_.IsValid()) {
        rectPtr = &rect_;
    }
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    SaveLayerOps slo(rectPtr, brushPtr, saveLayerFlags_);
    canvas->SaveLayer(slo);
}

/* RestoreOpItem */
REGISTER_UNMARSHALLING_FUNC(Restore, DrawOpItem::RESTORE_OPITEM, RestoreOpItem::Unmarshalling);

RestoreOpItem::RestoreOpItem() : DrawOpItem(RESTORE_OPITEM) {}

std::shared_ptr<DrawOpItem> RestoreOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<RestoreOpItem>();
}

void RestoreOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void RestoreOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Restore();
}

/* DiscardOpItem */
REGISTER_UNMARSHALLING_FUNC(Discard, DrawOpItem::DISCARD_OPITEM, DiscardOpItem::Unmarshalling);

DiscardOpItem::DiscardOpItem() : DrawOpItem(DISCARD_OPITEM) {}

std::shared_ptr<DrawOpItem> DiscardOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DiscardOpItem>();
}

void DiscardOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void DiscardOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Discard();
}

/* ClipAdaptiveRoundRectOpItem */
REGISTER_UNMARSHALLING_FUNC(
    ClipAdaptiveRoundRect, DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM, ClipAdaptiveRoundRectOpItem::Unmarshalling);

ClipAdaptiveRoundRectOpItem::ClipAdaptiveRoundRectOpItem(
    const DrawCmdList& cmdList, ClipAdaptiveRoundRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_ADAPTIVE_ROUND_RECT_OPITEM)
{
    radiusData_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->radiusData);
}

std::shared_ptr<DrawOpItem> ClipAdaptiveRoundRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipAdaptiveRoundRectOpItem>(
        cmdList, static_cast<ClipAdaptiveRoundRectOpItem::ConstructorHandle*>(handle));
}

void ClipAdaptiveRoundRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto radiusData = CmdListHelper::AddVectorToCmdList<Point>(cmdList, radiusData_);
    cmdList.AddOp<ConstructorHandle>(radiusData);
}

void ClipAdaptiveRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipRoundRect(*rect, radiusData_, true);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
